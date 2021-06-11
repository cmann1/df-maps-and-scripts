#include "../common/utils.cpp"
#include "../common/ColType.cpp"
#include "../common/Sprite.cpp"
#include "Potato.cpp"

const int MAX_PLAYERS = 4;

class script : callback_base
{
	
	scene@ g;
	array<Player> players(MAX_PLAYERS);
	Potato@ potato = null;
	Sprite warning("props5", "symbol_2");
	
	bool is_running;
	
	script()
	{
		@g = get_scene();
		
		const int level_type = g.level_type();
		is_running = level_type != LT_NEXUS and level_type != LT_NEXUS_MP;
		
		if(is_running)
		{
			add_broadcast_receiver("ball_dead", this, "ball_dead");
		}
	}
	
	void checkpoint_load()
	{
		if(!is_running) return;
		
		for(int i = 0; i < MAX_PLAYERS; i++)
		{
			@players[i].self = null;
		}
	}
	
	void ball_dead(string id, message@ msg)
	{
		for(int player_index = 0; player_index < MAX_PLAYERS; player_index++)
		{
			Player@ player = players[player_index];
			if(@player.self != null)
			{
				player.self.run_max(player.self.run_max() * 0.5);
				player.self.run_start(player.self.run_start() * 0.5);
				player.self.run_accel (player.self.run_accel() * 0.5);
				player.self.run_accel_over (player.self.run_accel_over() * 0.5);
				player.self.dash_speed (player.self.dash_speed() * 0.5);
				player.self.slope_slide_speed (player.self.slope_slide_speed() * 0.5);
				player.self.slope_max (player.self.slope_max() * 0.5);
				player.self.jump_a (player.self.jump_a()*0.5);
				player.self.hop_a (player.self.hop_a()*0.5);
			}
		}
	}
	
	void step(int entities)
	{
		if(!is_running) return;
		
		for(int player_index = 0; player_index < MAX_PLAYERS; player_index++)
		{
			Player@ player = players[player_index];
			
			if(@player.self == null)
			{
				entity@e = controller_entity(player_index);
				if(@e != null)
				{
					player.init(e.as_dustman());
				}
			}
			else
			{
				player.step();
				
				if(@potato == null)
				{
					@potato = Potato(player.self.x(), player.self.y() - 48);
					scriptenemy@ se = create_scriptenemy(potato);
					g.add_entity(se.as_entity());
				}
			}
		}
	}
	
	void draw(float sub_frame)
	{
		if(!is_running) return;
		
		for(int player_index = 0; player_index < MAX_PLAYERS; player_index++)
		{
			Player@ player = players[player_index];
			
			if(@player.self != null)
			{
				player.draw(sub_frame);
			}
		}
		
		if(@potato != null)
		{
			const float x = 0;
			const float y = 400;
			const float hw = 100;
			const float hh = 10;
			const float b = 5;
			float t = potato.is_dead ? 0 : min(1, potato.life / potato.max_life);
			g.draw_rectangle_hud(2, 2, x-hw-b, y-hh-b, x+hw+b, y+hh+b, 0, 0xFFFFFFFF);
			g.draw_rectangle_hud(2, 3, x-hw, y-hh, x-hw+hw*2*t, y+hh, 0, potato._in_world ? 0xFFEE2222 : 0xFF2222EE);
			
			if(potato.is_dead)
			{
				warning.draw_hud(2, 4, 0, 0, x, y - 60, 0, 0.75, 0.75);
			}
		}
	}
	
}

class Player
{
	
	scene@ g;
	dustman@ self = null;
	rectangle@ rect;
	
	Potato@ potato = null;
	int pickup_cooldown = 0;
	int pickup_cooldown_time = 45;
	
	float heat_max = 225;
	float heat = heat_max;
	float first_heat = 55;
	
	float centre_x;
	float centre_y;
	
	float throw_power = 1500;
	float throw_power_variation = 200;
	float throw_angle_variation = 5;
	float throw_velocity_factor = 0.35;
	
	Player()
	{
		@g = get_scene();
	}
	
	void init(dustman@ dm)
	{
		@self = dm;
		
		@rect = self.collision_rect();
		centre_x = (rect.left() + rect.right()) * 0.5;
		centre_y = (rect.top() + rect.bottom()) * 0.5;
	}
	
	void reset_combo()
	{
		if(self.combo_count() > 0 || self.combo_timer() > 0)
		{
			self.combo_count(self.combo_count() + 1);
			self.combo_timer(1);
		}
	}
	
	void throw_potato()
	{
		if(@potato == null) return;
		
		float throw_dir = 0;
		
		int intent_x = self.x_intent();
		int face = intent_x == 0 ? self.face() : intent_x;
		int intent_y = self.y_intent();
		
		if(intent_y == -1)
		{
			throw_dir = intent_x == 0 ? 0 : 45;
		}
		else if(intent_y == 1)
		{
			throw_dir = intent_x == 0 ? 180 : 135;
		}
		else
		{
			throw_dir = 80;
		}
		
		throw_dir += -throw_angle_variation + frand() * (throw_angle_variation * 2);
		throw_dir = throw_dir * face * DEG2RAD;
		
		float throw_speed = throw_power - throw_power_variation + frand() * throw_power_variation;
		float vel_x = sin(throw_dir) * throw_speed;
		float vel_y = -cos(throw_dir) * throw_speed;
		potato.drop(
			self.x() + centre_x, self.y() + centre_y,
			vel_x + self.x_speed() * throw_velocity_factor,
			vel_y + self.y_speed() * throw_velocity_factor
		);
		@potato = null;
		pickup_cooldown = pickup_cooldown_time;
		
		reset_combo();
	}
	
	void drop_potato()
	{
		if(@potato == null) return;
		
		potato.drop(self.x() + centre_x, self.y() + centre_y, self.x_speed() * 0.25, self.y_speed() * 0.25);
		@potato = null;
		pickup_cooldown = pickup_cooldown_time;
		
		reset_combo();
	}
	
	void step()
	{
		float x = self.x();
		float y = self.y();
		
		if(@potato == null)
		{
			if(pickup_cooldown == 0)
			{
				int collision_count = g.get_entity_collision(y + rect.top(), y + rect.bottom(), x + rect.left(), x + rect.right(), COL_TYPE_ENEMY);
				for(int i = 0; i < collision_count; i++)
				{
					entity@ e = g.get_entity_collision_index(i);
					scriptenemy@ se = @e != null ? e.as_scriptenemy() : null;
					Potato@ potato = @se != null ? cast<Potato>(se.get_object()) : null;
					if(@potato != null and potato.pickup(this))
					{
						@this.potato = @potato;
						heat = heat_max + first_heat;
						first_heat = 0;
						reset_combo();
					}
				}
			}
		}
		else
		{
			if(heat-- <= 0)
			{
				self.stun(0, -50);
				drop_potato();
				g.combo_break_count(g.combo_break_count() + 1);
			}
			
			// Drop the ball
			if(self.light_intent() != 0)
			{
				drop_potato();
			}
			
			// Throw the ball
			else if(self.heavy_intent() != 0)
			{
				throw_potato();
			}
			
			self.light_intent(0);
			self.heavy_intent(0);
		}
		
		if(pickup_cooldown > 0)
		{
			pickup_cooldown--;
		}
	}
	
	void draw(float sub_frame)
	{
		float x = self.x();
		float y = self.y();
		
		if(@potato != null)
		{
			const float bx = x;
			const float by = y - 110;
			const float hw = 50;
			const float hh = 6;
			const float b = 2;
			float t = 1 - min(1, heat / heat_max);
			g.draw_rectangle_world(20, 24, bx-hw-b, by-hh-b, bx+hw+b, by+hh+b, 0, 0xFFFFFFFF);
			g.draw_rectangle_world(20, 24, bx-hw, by-hh, bx-hw+hw*2*t, by+hh, 0, lerp_colour(0xFF22EE22, 0xFFEE2222, t));
		}
	}
	
	uint lerp_colour(uint c1, uint c2, float t)
	{
		const float a1 = (c1 >> 24) & 0xFF;
		const float a2 = (c2 >> 24) & 0xFF;
		const float r1 = (c1 >> 16) & 0xFF;
		const float r2 = (c2 >> 16) & 0xFF;
		const float g1 = (c1 >> 8) & 0xFF;
		const float g2 = (c2 >> 8) & 0xFF;
		const float b1 = (c1) & 0xFF;
		const float b2 = (c2) & 0xFF;
		
		const uint a = uint(a1 * (1 - t) + a2 * t);
		const uint r = uint(r1 * (1 - t) + r2 * t);
		const uint g = uint(g1 * (1 - t) + g2 * t);
		const uint b = uint(b1 * (1 - t) + b2 * t);
		
		return (a << 24) + (r << 16) + (g << 8) + b;
	}
	
}