#include "../common/Fx.cpp"
#include "../common/States.cpp"
#include "../common/utils.cpp"
#include "../common/math.cpp"

const int MAX_PLAYERS = 4;
const int FX_LAYER = 18;
const int FX_SUB_LAYER = 14;

class script
{
	
	scene@ g;
	[hidden] array<Player> players(MAX_PLAYERS);
	
	script()
	{
		for(int i = 0; i < MAX_PLAYERS; i++)
		{
			players[i].player_index = i;
		}
	}
	
	void checkpoint_load()
	{
		for(int i = 0; i < MAX_PLAYERS; i++)
		{
			@players[i].self = null;
		}
	}
	
	void step(int entities)
	{
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
	}
	
	void draw(float sub_frame)
	{
		for(int player_index = 0; player_index < MAX_PLAYERS; player_index++)
		{
			Player@ player = players[player_index];
			
			if(@player.self != null)
			{
				player.draw(sub_frame);
			}
		}
	}
}

const int ABILITY_NONE = 0;
const int ABILITY_SUPER_JUMP = 1;
const int ABILITY_SUPER_DASH = 2;
const int ABILITY_GROUND_SLAM = 3;

class Player
{
	
	scene@ g;
	dustman@ self = null;
	int player_index = -1;
	
	string character;
	string character_full;
	string character_prefix = "dm";
	uint char_colour = 0xFF000000;
	
	rectangle@ rect;
	float centre_x;
	float centre_y;
	float hitbox_left;
	float hitbox_right;
	
	[hidden] int ability_state = ABILITY_NONE;
	
	[hidden] float ability_timer_max = 100;
	[hidden] float ability_timer = ability_timer_max;
	float ability_timer_delay = -50;
	
	[hidden] int previous_state = -1;
	
	float shrink_size = 0.6;
	float feet_height = 20;
	float head_height = 30;
	
	[hidden] float super_jump_wait = 0;
	[hidden] float super_jump_wait_max = 0.5;
	[hidden] float super_jump_boost = 0;
	[hidden] float super_jump_boost_max = -2500;
	float super_jump_angle = 0;

	[hidden] int super_dash_timer = 0;
	[hidden] float super_dash_wait = 0;
	[hidden] float super_dash_wait_max = 0.5;
	float super_dash_boost = 2000;
	
	float ground_slam_fall_factor = 1.5;
	
	int down_double_tap_timeout = 15;
	int down_double_tap_wait = 0;
	bool down_double_tap = false;
	
	[hidden] float wall_run_speed;
	[hidden] float fall_max = -1;
	
	float roof_run_max = 3.5;
	float roof_run_min = 0.1;
	
	float wall_run_min = 1;
	float wall_run_max = 4.5;
	
	Player()
	{
		@g = get_scene();
	}
	
	void create_fx(float x, float y, string sprite_set, string sprite_name, uint palette=0, int fps=15, float rotation=0, float scale_x=1, float scale_y=1, uint colour=0xFFFFFFFF)
	{
		spawn_fx(x, y, sprite_set, sprite_name, palette, fps, rotation, scale_x, scale_y, colour)
			.set_layer(FX_LAYER, FX_SUB_LAYER);
	}
	
	void init(entity@ dm)
	{
		@self = dm.as_dustman();
		
		@rect = self.collision_rect();
		centre_x = (rect.left() + rect.right()) * 0.5;
		centre_y = (rect.top() + rect.bottom()) * 0.5;
		character_full = character = self.character();
		if(character.substr(0, 1) == "v")
			character = character.substr(1);
			
		if(character == "dustman")
		{
			wall_run_min = 1;
			wall_run_max = 4.9;
			character_prefix = "dm";
			char_colour = 0xFF9cb9ff;
		}
		else if(character == "dustgirl")
		{
			wall_run_min = 1;
			wall_run_max = 3.9;
			character_prefix = "dg";
			char_colour = 0xFFed7366;
		}
		else if(character == "dustworth")
		{
			wall_run_min = 0.1;
			wall_run_max = 4.5;
			character_prefix = "do";
			char_colour = 0xFFc3e082;
		}
		else if(character == "dustkid")
		{
			wall_run_min = 1;
			wall_run_max = 4.5;
			character_prefix = "dk";
			char_colour = 0xFFd699ff;
		}
	}
	
	void step(int entities)
	{
		const float x = self.x();
		const float y = self.y();
		int state = self.state();
		
		if(ability_state == ABILITY_NONE && ability_timer < ability_timer_max)
		{
			ability_timer++;
		}
		
		if(self.y_intent() > 0)
		{
			if(down_double_tap_wait > 0 and down_double_tap_wait < down_double_tap_timeout)
				down_double_tap = true;
			else if(!down_double_tap)
				down_double_tap_wait = down_double_tap_timeout;
		}
		else
		{
			if(down_double_tap_wait > 0) down_double_tap_wait--;
			down_double_tap = false;
		}
		
		if(ability_state == ABILITY_NONE)
		{
			if(ability_timer >= ability_timer_max)
			{
				// Shrink/grow
				if(self.taunt_intent() != 0){
					self.scale(self.scale() == 1 ? shrink_size : 1, false);
					ability_timer = ability_timer_delay;
					g.play_sound("sfx_respawn", x, y, 1, false, true);
					create_fx(x, y, "editor", "respawnteam1");
					self.taunt_intent(0);
				}
				
				// Start a super dash/jump
				else if(state == ST_IDLE or state == ST_SKID or state == ST_LAND or state == ST_SLOPE_SLIDE or state == ST_CROUCH_JUMP)
				{
					if(down_double_tap or (state == ST_IDLE and self.y_intent() > 0))
					{
						// Start super jump (down+jump during idle)
						if(self.jump_intent() != 0){
							self.jump_intent(0);
							ability_state = ABILITY_SUPER_JUMP;
							ability_timer = ability_timer_delay;
							self.state(state = ST_CROUCH_JUMP);
						}
						// Start super dash (down+dash during idle)
						else if(state != ST_CROUCH_JUMP and self.dash_intent() != 0){
							self.dash_intent(0);
							ability_state = ABILITY_SUPER_DASH;
							ability_timer = ability_timer_delay;
							// Put the player into the slide state, this looks goods as a dash windup
							self.state(state = ST_SKID);
						}
					}
				}
				
				// Ground slam
				if(state == ST_FALL or state == ST_HOVER){
					if(down_double_tap and self.jump_intent() == 1){
						if(fall_max == -1) fall_max = self.fall_max();
						self.fall_max(fall_max * ground_slam_fall_factor);
						self.set_speed_xy(self.x_speed(), self.fall_max());
						self.jump_intent(0);
						ability_timer = ability_timer_delay;
						ability_state = ABILITY_GROUND_SLAM;
						create_fx(x, y, character_full, character_prefix + "fastfall", 0, 15, 0, 1.5, 1.5);
						audio@ sfx = g.play_sound("sfx_bear_attack", x, y, 1, false, true);
					}
				}
			}
			
			// Enemy jump
			if(self.jump_intent() == 1 and (state == ST_FALL or state == ST_HOVER or state == ST_JUMP or state == ST_HOP)){
				float feet_x1 = x + hitbox_left;
				float feet_x2 = x + hitbox_right;
				float feet_y1 = y - feet_height;
				float feet_y2 = y;
				
				for (int i = 0; i < entities; i++) {
					entity@ e = entity_by_index(i);
					string type = e.type_name();
					if(type.substr(0, 5) == "enemy"){
						float e_x = e.x();
						float e_y = e.y();
							
						rectangle@ r = e.as_controllable().collision_rect();
						float left = e_x + r.left();
						float right = e_x + r.right();
						float top = e_y + r.top();
						float bottom = top + head_height;
						
						if(feet_x1 <= right and feet_x2 >= left and feet_y1 <= bottom and feet_y2 >= top){
							self.state(ST_JUMP);
							self.dash(self.dash_max());
							
							controllable@ ec = e.as_controllable();
							
							ec.set_speed_xy(ec.x_speed(), ec.y_speed() + 200);
							ec.y(ec.y() + 20);
//							create_fx(x, y, character_full, character_prefix + "land");
//							create_fx(x, y, character_full, character_prefix + "jump");
							g.play_sound("sfx_" + character_prefix + "_jump_wall_1", x, y, 1, false, true);
							break;
						}
					}
				}
			}
		}
		
		/*
		// Disabled these because they don't look so good and invalidate vanilla mechanics
		// Infinite ceiling run
		if(state == ST_ROOF_RUN){
			if(self.state_timer() >= roof_run_max)
				self.state_timer(roof_run_min);
			if(
				(self.x_intent() == -1 and self.x_speed() > 0) or
				(self.x_intent() == 1 and self.x_speed() < 0)
			){
				self.face(self.x_intent());
				self.set_speed_xy(-self.x_speed(), -self.y_speed());
			}
		}
		
		// Infinite wall run
		else if(state >= ST_WALL_RUN and state <= ST_WALL_GRAB_RELEASE){
			if(state == ST_WALL_RUN and previous_state != ST_WALL_RUN){
				wall_run_speed = self.y_speed();
			}
			
			if(self.y_intent() < 0){
				if(state != 11)
					self.state(state = 11);
				
				self.set_speed_xy(self.x_speed(), wall_run_speed);
				
				if(self.state_timer() >= wall_run_max and self.y_intent() < 0)
					self.state_timer(wall_run_min);
			}
		}
		*/
		
		// Super jump windup
		if(ability_state == ABILITY_SUPER_JUMP){
			// In case the player is attacked or something, cancel the jump
			if(state != ST_CROUCH_JUMP){
				ability_state = ABILITY_NONE;
				super_jump_wait = 0;
			}
			
			// The player can't do anything during jump windup
			disable_intents();
			
			// Hold at the end of the crouch anim
			self.sprite_index("crouch");
			if(self.state_timer() > 2.5){
				self.state_timer(2.5);
				super_jump_wait += 0.016;
				if(super_jump_wait >= super_jump_wait_max){
					ability_state = ABILITY_NONE;
					super_jump_wait = 0;
					super_jump_boost = super_jump_boost_max;
					super_jump_angle = self.rotation();
					self.state(ST_JUMP);
				}
			}
		}
		// Execute super jump
		else if(state == ST_JUMP){
			if(super_jump_boost != 0){
				const float angle = (super_jump_angle + 90) * DEG2RAD;
				self.set_speed_xy(
					self.x_speed() + cos(angle) * super_jump_boost,
					self.y_speed() + sin(angle) * super_jump_boost);
				super_jump_boost = 0;
				
				audio@ sfx = g.play_sound("sfx_bear_attack", x, y, 1, false, true);
				if(@sfx != null) sfx.time_scale(0.8);
				create_fx(x, y, character, character_prefix + "heavyland", 0, 15, super_jump_angle, 2.5, 1.5);
				create_fx(x, y, character, character_prefix + "jump", 0, 15, super_jump_angle, 1, 2.5);
			}
		}
	
		// Super dash wind up
		if(ability_state == ABILITY_SUPER_DASH){
			// Execute the super dash
			if(super_dash_wait >= super_dash_wait_max){
				const float angle = atan2(self.y_speed(), self.x_speed());
				ability_state = ABILITY_NONE;
				super_dash_wait = 0;
				self.set_speed_xy(
					self.x_speed() + cos(angle) * super_dash_boost,
					self.y_speed() + sin(angle) * super_dash_boost);
				create_fx(x, y, character, character_prefix + "dash", 0, 15, 0, 2.5 * self.face(), 2.5);
				audio@ sfx = g.play_sound("sfx_bear_attack", x, y, 1, false, true);
				if(@sfx != null) sfx.time_scale(1.2);
				super_dash_timer = 52;
			}
			
			// In case the player is attacked or something, cancel the dash
			if(state != ST_SKID){
				ability_state = ABILITY_NONE;
				super_dash_wait = 0;
			}
			
			// The player can't do anything during dash windup
			disable_intents();
			
			// Hold at the end of the anim
			if(self.state_timer() > 1.5){
				self.state_timer(1.5);
				super_dash_wait += 0.016;
				if(super_dash_wait >= super_dash_wait_max){
					// Initiate a dash. but wait till the next frame so that the player's velocity can be used
					// to calculate the correct direction for the super dash
					self.state(ST_DASH);
				}
			}
		}
		
		if(ability_state == ABILITY_GROUND_SLAM){
			if(self.ground() or state == ST_LAND)
			{
				hitbox@ hb = create_hitbox(
					self.as_controllable(), 0,
					x, y, -96, 40, -48 * 5, 48 * 5);
				hb.team(self.team());
				hb.aoe(true);
				hb.damage(9);
				hb.attack_strength(1200);
				hb.timer_speed(9);
				g.add_entity(hb.as_entity());
				
				g.play_sound("sfx_chest_land", x, y, 1, false, true);
				audio@ sfx = g.play_sound("sfx_chest_land", x, y, 1, false, true);
				create_fx(x, y - 20, character, character_prefix + "cleanseready", 0, 15, 90, 1.5, 2.0);
				create_fx(x, y, character, character_prefix + "heavyland", 0, 15, 0, 3, 2.0);
				if(@sfx != null) sfx.time_scale(0.7);
				
				self.fall_max(fall_max);
				ability_state = ABILITY_NONE;
			}
			
			else{
				// Cancel
				if(state != ST_FALL && state != ST_HOVER){
					ability_state = ABILITY_NONE;
					self.fall_max(fall_max);
				}
				
				// The player can't do anything during a ground slam
				disable_intents();
			}
		}
		
		// Super dash fx
		if(super_dash_timer > 0)
		{
			if(state != ST_DASH || (super_dash_timer < 0))
			{
				super_dash_timer = 0;
			}
			else{
				if((super_dash_timer / 10) * 10 == super_dash_timer)
				{
					float s = (1 + 1.5 * (super_dash_timer / 50.0)) * self.face();
					create_fx(x, y, character, character_prefix + "dash", 0, 15, 0, s, abs(s));
				}
				
				super_dash_timer -= 2;
				if(super_dash_timer < 0)
					super_dash_timer = 0;
			}
		}
		
		previous_state = state;
	}
	
	void disable_intents()
	{
		self.jump_intent(0);
		self.dash_intent(0);
		self.heavy_intent(0);
		self.light_intent(0);
		self.x_intent(0);
		self.y_intent(0);
	}
	
	void draw(float sub_frame)
	{
		if(get_active_player() == player_index)
		{
			const float x = -800;
			const float y = -450;
			const float w = 200;
			const float h = 20;
			const float b = 5;
			const float p = 15;
			float t = max(0, min(1, ability_timer / ability_timer_max));
			g.draw_rectangle_hud(2, 2, x-b+p, y-b+p, x+w+b+p, y+h+b+p, 0, 0xFFFFFFFF);
			g.draw_rectangle_hud(2, 3, x+p, y+p, x+p+w*t, y+p + h, 0, char_colour);
		}
	}
	
}