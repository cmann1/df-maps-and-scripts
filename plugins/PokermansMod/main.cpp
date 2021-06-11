#include "../common/utils.cpp"
#include "../common/Fx.cpp"
#include "../common/math.cpp"
#include "Pokerball.cpp"

const int MAX_PLAYERS = 4;
const float DT = 1.0 / 60;
const bool BALL_BALL_COLLISION = true;

class script
{
	
	scene@ g;
	bool is_running = true;
	
	textfield@ catch_text;
	float catch_text_timer;
	
	[hidden] array<Player> players(MAX_PLAYERS);
	
	script()
	{
		@g = get_scene();
		
		const int level_type = g.level_type();
		is_running = level_type != LT_NEXUS and level_type != LT_NEXUS_MP;
		
		if(is_running)
		{
			@catch_text = create_textfield();
			catch_text.align_horizontal(0);
			catch_text.align_vertical(-1);
			
			for(int i = 0; i < MAX_PLAYERS; i++)
			{
				players[i].player_index = i;
			}
		}
	}
	
	string get_article(string word)
	{
		string ch = word.substr(0, 1);
		
		if("aeiouAEIOU".findFirst(ch) >= 0)
			return "an";
		
		return "a";
	}
	
	string get_pokermans_name(string entity_name)
	{
		string result = "";
		bool first_underscore = true;
		string previous_ch = "";
		for(uint i = 0; i < entity_name.length(); i++)
		{
			string ch = entity_name.substr(i, 1);
			
			if(!first_underscore)
			{
				if(result.length == 0 || previous_ch == "_")
				{
					int c = ch[0];
					if(c >= 97 and c <= 122)
						ch[0] = c - 32;
				}
				
				result += ch == "_" ? " " : ch;
				previous_ch = ch;
			}
			else if(ch == "_")
				first_underscore = false;
		}
		
		return result;
	}
	
	bool catch_pokerman(int player_index, entity@ e)
	{
		const string name = e.type_name();
		if(name == "score_book" or name == "custom_score_book") return false;
		
		Player@ player = players[player_index];
		if(@player.self != null)
		{
			player.add_combo(e);
			player.self.dash(player.self.dash_max());
		}
		
		string nice_name = get_pokermans_name(e.type_name());
		catch_text.text("You caught " + get_article(nice_name) + " " + nice_name + "!");
		catch_text_timer = 60;
		catch_text.colour(0xFFFFFFFF);
		
//		const int remove_id = e.id();
//		for(int a = 0; a < entities; a++)
//		{
//			entity@ e2 = entity_by_index(a);
//			if(e2.type_name() == "AI_controller" and e2.vars().get_var("puppet_id").get_int32() == remove_id)
//			{
//				g.remove_entity(e2);
//				break;
//			}
//		}
		
		g.remove_entity(e);
		return true;
	}
	
	void checkpoint_load()
	{
		if(!is_running) return;
		
		for(int i = 0; i < MAX_PLAYERS; i++)
		{
			@players[i].self = null;
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
					player.init(e);
				}
			}
			else
			{
				player.step(entities);
			}
		}
		
		if(catch_text_timer > 1)
		{
			catch_text_timer--;
		}
		else if(catch_text_timer > 0)
		{
			catch_text_timer -= 0.05;
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
	
		if(catch_text_timer > 0)
		{
			if(catch_text_timer < 1)
			{
				catch_text.colour((uint(0xFF * catch_text_timer) << 24) | 0x00FFFFFF);
			}
			catch_text.draw_hud(19, 19, 0, -420, 1, 1, 0);
		}
	}
	
}

class Player
{
	
	scene@ g;
	dustman@ self = null;
	int player_index = -1;
	
	rectangle@ rect;
	float centre_x;
	float centre_y;
	
	float throw_power = 1500;
	float throw_power_variation = 200;
	float throw_angle_variation = 3;
	float throw_velocity_factor = 0.25;
	
	Player()
	{
		@g = get_scene();
	}
	
	void init(entity@ dm)
	{
		@self = dm.as_dustman();
		
		@rect = self.collision_rect();
		centre_x = (rect.left() + rect.right()) * 0.5;
		centre_y = (rect.top() + rect.bottom()) * 0.5;
	}
	
	void add_combo(entity@ e=null)
	{
		int amount = 1;
		
		if(@e != null)
		{
			controllable@ c = e.as_controllable();
			if(@c != null)
			{
				amount = c.life();
			}
		}
		
		self.combo_count(self.combo_count() + amount);
		self.combo_timer(1);
	}
	
	void step(int entities)
	{
		const float x = self.x();
		const float y = self.y();
		
		self.heavy_intent(0);
		
		if(self.light_intent() != 0)
		{
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
			
			Pokerball@ pokerball = Pokerball();
			pokerball.vel_x = self.x_speed() * throw_velocity_factor + vel_x;
			pokerball.vel_y = self.y_speed() * throw_velocity_factor + vel_y;
			pokerball.player_index = player_index;
			scriptenemy@ pokerball_enemy = create_scriptenemy(@pokerball);
			pokerball_enemy.x(x + centre_x);
			pokerball_enemy.y(y + centre_y);
			g.add_entity(pokerball_enemy.as_entity(), true);
			
			self.light_intent(0);
		}
	}
	
	void draw(float sub_frame)
	{
//		if(get_active_player() == player_index)
//		{
//			const float x = -800;
//			const float y = -450;
//			const float w = 200;
//			const float h = 20;
//			const float b = 5;
//			const float p = 15;
//			float t = max(0, min(1, ability_timer / ability_timer_max));
//			g.draw_rectangle_hud(2, 2, x-b+p, y-b+p, x+w+b+p, y+h+b+p, 0, 0xFFFFFFFF);
//			g.draw_rectangle_hud(2, 3, x+p, y+p, x+p+w*t, y+p + h, 0, char_colour);
//		}
	}
	
}