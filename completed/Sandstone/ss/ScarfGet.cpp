#include '../lib/drawing/Sprite.cpp';
#include '../lib/easing/quint.cpp';
#include '../lib/easing/sine.cpp';
#include '../lib/triggers/EnterExitTrigger.cpp';

#include 'Flock.cpp';

const int PARTICLES_EMITTER_ID = EmitterId::SlimeWall;

class ScarfGet : trigger_base, callback_base, LifecycleEntity, EnterExitTrigger
{
	
	[entity] int apple_id;
	[persist] int length = 1;
	
	[hidden] array<bool> players_activated;
	[hidden] float activated = -1;
	
	script@ script;
	scripttrigger@ self;
	controllable@ apple;
	
	float target_x;
	float target_y;
	
	canvas@ canvas;
	Sprite spr('props3', 'backdrops_3');
	entity@ emitter;
	
	float alpha = 1;
	float sign_size = Player::SignSizeMin;
	int sign_base = 0;
	uint sign_clr = 0xffffffff;
	int waiting;
	int prev_sign = -1;
	
	Flock flock;
	Player@ player;
	bool scarf_getted;
	
	string get_identifier() const override { return 'ScarfGet' + self.id(); }
	
	audio@ sound_amb;
	float sound_amb_vol;
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.self = self;
		@this.script = script;
		trigger_view_events = true;
		
		self.radius(180);
		self.square(true);
		
		@apple = controllable_by_id(apple_id);
		@canvas = create_canvas(false, Player::SignLayer, Player::SignSubLayer);
		
		if(!script.is_playing)
			return;
		
		if(players_activated.length == 0)
		{
			players_activated.resize(script.num_players);
		}
		
		@emitter = emitter_by_id(script.g, self.x() - 12, self.y() - 12, self.x() + 12, self.y() + 12, PARTICLES_EMITTER_ID);
		
		if(@apple != null)
		{
			apple.on_hurt_callback(this, 'on_apple_hurt', 0);
		}
		
		target_x = self.x();
		target_y = self.y();
		
		if(activated >= 0)
		{
			//alpha = 0;
		}
		
		@flock.script = script;
		flock.x = target_x;
		flock.y = target_y - 6;
		flock.add_birds(round_int(map(sin(self.x() * 12345 + self.y() * 8323), -1, 1, 4, 6)));
	}
	
	void on_remove()
	{
		if(@sound_amb != null)
		{
			sound_amb.stop();
			@sound_amb = null;
		}
	}
	
	void step()
	{
		if(!script.is_playing)
			return;
		
		init_lifecycle();
		step_enter_exit();
		
		const float x = @apple != null ? apple.x() : self.x();
		const float y = @apple != null ? apple.y() : self.y();
		
		if(@apple != null)
		{
			self.set_xy(x, y);
			
			// Pinned to target
			//if(!activated)
			{
				apple.set_xy(target_x, target_y);
				apple.set_speed_xy(0, 0);
				//apple.state(EntityState::Idle);
			}
			
			//float x_speed = apple.x_speed();
			//float y_speed = apple.y_speed();
			//
			//// Float above the ground
			////{
			//const float dx = target_x - x;
			////x_speed += (dx < 0 ? min(dx, -75.0) : max(dx, 75.0)) * 0.25;
			//x_speed += dx * 0.25;
			//
			//const float dist = 96;
			//tilecollision@ tc = script.g.collision_ground(x - 5, y, x + 5, y + dist);
			//
			//if(tc.hit())
			//{
			//	y_speed *= 0.92;
			//	y_speed -= (dist - abs(y -tc.hit_y())) * 4;
			//}
			////}
			//
			//apple.set_speed_xy(x_speed, y_speed);
		}
		
		if(@emitter != null)
		{
			emitter.set_xy(x, y);
		}
		
		//if(activated >= 0 && alpha > 0)
		//{
		//	alpha = max(alpha - DT, 0.0);
		//	
		//	if(alpha <= 0)
		//	{
		//		script.g.remove_entity(self.as_entity());
		//	}
		//}
		
		if(_in_view)
		{
			flock.step();
		}
		
		if(@sound_amb != null)
		{
			// Fade in
			if(sound_amb_vol >= 0 && sound_amb_vol < 1)
			{
				sound_amb_vol = min(sound_amb_vol + DT, 1.0);
				sound_amb.volume(sound_amb_vol);
			}
			else if(sound_amb_vol < 0 && sound_amb_vol >= -1)
			{
				sound_amb_vol = min(sound_amb_vol + DT * 0.35, 0.0);
				sound_amb.volume(-sound_amb_vol);
				
				if(sound_amb_vol == 0)
				{
					sound_amb.stop();
					@sound_amb = null;
				}
			}
		}
		
		if(@player != null)
		{
			if(flock.completed_count >= 1 && !scarf_getted)
			{
				player.give_super();
				player.scarf_get(length);
				scarf_getted = true;
			}
			
			if(flock.completed_count == flock.bird_count)
			{
				player.lock(false);
				@player = null;
				flock.targeted_count = flock.completed_count = 0;
				sound_amb_vol = -abs(sound_amb_vol);
			}
		}
		
		if(activated >= 0 && activated < ceil(activated / Player::SignPulsePeriod) * Player::SignPulsePeriod)
		{
			activated++;
		}
		
		update_signs();
	}
	
	void editor_step()
	{
		update_signs();
	}
	
	private void update_signs()
	{
		const float x = @apple != null ? apple.x() : self.x();
		const float y = @apple != null ? apple.y() : self.y();
		
		const float mid = Player::SignPulseInTime;
		const float t = ((activated >= 0 ? activated : script.frame) + abs(x)) / Player::SignPulsePeriod;
		float t2 = fraction(t);
		t2 = t2 <= mid ? ease_in_out_quint(t2 / mid) : ease_in_out_sine(1 - ((t2 - mid) / (1 - mid)));
		const float rand = fraction(abs(sin(ceil(t - mid * 0.5) * 1230 + x)));
		
		sign_size = map(t2, 0, 1, Player::SignSizeMin, Player::SignSizeMax);
		sign_clr = 0xffffff | (round_int(alpha * 255) << 24);
		const int next_sign = round_int(rand * 10);
		
		if(activated < 0 && waiting == 0 && @player == null && next_sign != prev_sign)
		{
			prev_sign = next_sign;
			sign_base = next_sign;
		}
	}
	
	bool can_trigger_enter_exit(controllable@ c)
	{
		return c.player_index() != -1;
	}
	
	void activate(controllable@ c)
	{
		activate_enter_exit(c);
	}
	
	void on_trigger_enter(controllable@ c)
	{
		waiting++;
	}
	
	void on_trigger_exit(controllable@ c)
	{
		waiting--;
	}
	
	void on_apple_hurt(controllable@ attacked, controllable@ attacker, hitbox@ attack_hitbox, int arg)
	{
		if(@attacker == null || attacker.player_index() == -1)
			return;
		if(@player != null)
			return;
		if(players_activated[attacker.player_index()])
		{
			script.players[attacker.player_index()].give_super();
			return;
		}
		if(attack_hitbox.damage() != 1 && attack_hitbox.damage() != 3)
			return;
		if(!attacker.ground())
			return;
		if(dist_sqr(attacker.x(), attacker.y(), self.x(), self.y()) > 240 * 240)
			return;
		
		players_activated[attacker.player_index()] = true;
		
		script.add_emitter_burst(
			EmitterId::SlimeGround,
			Player::SignFxLayer, Player::SignFxSubLayer,
			self.x(), self.y(),
			48, 48, 0, 0.5);
		
		for(int i = 0; i < script.num_players; i++)
		{
			if(!players_activated[i])
				return;
		}
		
		activated = script.frame;
		if(@emitter != null)
		{
			script.g.remove_entity(emitter);
			@emitter = null;
		}
		
		//if(@apple != null)
		//{
		//	script.g.remove_entity(apple.as_entity());
		//	@apple = null;
		//}
		
		@sound_amb = script.g.play_script_stream('scarf_get_amb', 0, self.x(), self.y(), true, 0);
		sound_amb_vol = 0;
		sound_amb.positional(true);
		
		@player = @script.players[attacker.player_index()];
		player.lock(true, 1);
		player.init_head();
		scarf_getted = false;
		flock.give_scarf(player);
	}
	
	void draw(float sub_frame)
	{
		if(!_in_view && script.is_playing)
			return;
		
		flock.draw(sub_frame);
		
		if(alpha <= 0)
			return;
		
		const float x = @apple != null ? lerp(apple.prev_x(), apple.x(), sub_frame) : self.x();
		const float y = @apple != null ? lerp(apple.prev_y(), apple.y(), sub_frame) : self.y();
		
		float ox = 0;
		float oy = 0;
		
		switch(@apple != null ? apple.state() : 0)
		{
			case EntityState::Fall:
			default:
				oy = 16;
				break;
		}
		
		const float s = sign_size;
		draw_sign(canvas, @spr, sign_base, x - 2*s + ox, y - 2*s + oy, s, 0, sign_clr);
		draw_sign(canvas, @spr, (sign_base + 155) % 11, x + 2*s + ox, y - 2*s + oy, s, 0, sign_clr);
		draw_sign(canvas, @spr, (sign_base + 456) % 11, x + 2*s + ox, y + 2*s + oy, s, 0, sign_clr);
		draw_sign(canvas, @spr, (sign_base + 992) % 11, x - 2*s + ox, y + 2*s + oy, s, 0, sign_clr);
		
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}
