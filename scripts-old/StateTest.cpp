class Fx{
	sprites@ sprite;
	string sprite_name;
	float frame = 0;
	float frame_rate;
	uint palette;
	float x;
	float y;
	float rotation;
	float scale_x = 1;
	float scale_y = 1;
	uint colour;
	
	Fx(float x, float y, string sprite_set, string sprite_name, uint palette=0, float rotation=0, float scale_x=1, float scale_y=1, uint fps=10, uint colour=0xFFFFFFFF)
	{
		this.x = x;
		this.y = y;
		this.rotation = rotation;
		this.scale_x = scale_x;
		this.scale_y = scale_y;
		this.colour = colour;
		
		this.sprite_name = sprite_name;
		@sprite = create_sprites();
		sprite.add_sprite_set(sprite_set);
		
		frame_rate = fps / 60.0;
		this.palette = palette;
	}
	
	bool draw()
	{
		sprite.draw_world(19, 9, sprite_name,
				uint(frame), palette, x, y, rotation,
				scale_x, scale_y, colour);
		
		frame += frame_rate;
		return int(frame) < sprite.get_animation_length(sprite_name);
	}
}

class script{
	scene@ g;
	controllable@ player;
	dustman@ dm;

	float r = 0;
	float t = 0;
	
	bool ground_slam = false;

	bool super_jump = false;
	float super_jump_wait = 0;
	float super_jump_wait_max = 0.5;
	float jump_boost = 0;
	float jump_boost_max = -2500;

	bool super_dash = false;
	int super_dash_timer = 0;
	float super_dash_wait = 0;
	float super_dash_wait_max = 0.5;
	float super_dash_boost = 2000;

	int previous_state = -1;
	float wall_run_speed;

	float skill = 0;
	
	array<Fx@> effects_current = {};
	array<Fx@> effects_checkpoint = {};
	uint effects_current_count = 0;
	uint effects_checkpoint_count = 0;
	
	textfield@ text;

	bool can_use_skill(){
		return skill >= 99;
	}

	void update_skill(float sk){
		dm.skill_combo(int(skill = sk));
	}
	
	float frand(){
		return float(rand())/float(0x3fffffff);
	}
	
	Fx@ add_fx(Fx@ fx)
	{
		if(effects_current_count >= effects_current.length())
		{
			effects_current.resize(effects_current_count + 10);
		}
		@effects_current[effects_current_count++] = fx;
		return fx;
	}

	// Game callbacks

	script(){
		puts("INIT");
		@g = get_scene();
		
		@text = create_textfield();
		text.text("Abilities:\n" +
			"  Super Jump: Down+Jump\n" +
			"  Super Dash: Down+Dash\n" +
			"  Grounds Slam: Down+Jump (mid-air)\n" +
			"  Shrink: Taunt\n" +
			"  Enemy Step: Jump on an enemy's head\n" +
			"  Infinite Wall/Ceiling Run");
		text.align_horizontal(-1);
		text.align_vertical(-1);
		text.set_font("ProximaNovaReg", 36);
	}

	void checkpoint_save() {
		puts("CHECKPOINT SAVE");
		effects_checkpoint = effects_current;
		effects_checkpoint_count = effects_current_count;
	}

	void checkpoint_load() {
		puts("CHECKPOINT LOAD");
		@player = null;
		@dm = null;
		
		effects_current = effects_checkpoint;
		effects_current_count = effects_checkpoint_count;
	}

	void entity_on_add(entity@ e) {
		if(e.type_name().findFirst("entity_cleansed") == 0){
			g.remove_entity(e);
		}
		// puts("HELLO " + e.type_name());
	}

	void entity_on_remove(entity@ e) {
		// puts("BYE " + e.type_name());
		// if(e.type_name() == "effect")
		// {
			// varstruct@ vars = e.vars();
			// for(uint i = 0; i < vars.num_vars(); i++)
			// {
				// string name = vars.var_name(i);
				// varvalue@ value = vars.get_var(i);
				// puts(" "+name+":"+value.type_id());
			// }
		// }
	}

	void step(int entities){
		if(@player == null){
			entity@e = controller_entity(0);
			if(@e != null){
				@player = e.as_controllable();
				@dm = e.as_dustman();
				update_skill(99);
			}
		}
		else{
			int state = player.state();
			
			// Shrink/grow
			if(player.taunt_intent() != 0 and can_use_skill()){
				dm.scale(dm.scale() == 1 ? 0.5 : 1, false);
				update_skill(0);
			}
			
			// Infinite ceiling run
			// st_roof_run
			if(state == 17){
				if(player.state_timer() >= 3)
					player.state_timer(0.1);
				if(player.x_intent() != 0){
					float x_speed = abs(player.x_speed());
					float y_speed = player.y_speed();
					player.face(player.x_intent());
					player.set_speed_xy(x_speed * player.face(), y_speed);
				}
			}
			
			// Infinite wall run
			// st_wall_run, st_wall_grab, st_wall_grab_idle, st_wall_grab_release
			else if(state >= 11 and state <= 14){
				if(state == 11 and previous_state != 11){
					wall_run_speed = player.y_speed();
				}
				
				if(player.y_intent() < 0){
					if(state != 11){
						player.state(state = 11);
					}
					float x_speed = abs(player.x_speed());
					player.set_speed_xy(x_speed, wall_run_speed);
					if(player.state_timer() >= 4.5 and player.y_intent() < 0)
						player.state_timer(1);
				}
			}
			
			// Enemy jump
			// st_fall, st_hover, st_jump
			if(!ground_slam and player.jump_intent() == 1 and (state == 5 or state == 7 or state == 8)){
				float player_x = player.x();
				float player_y = player.y();
				// float head_width = 30;
				// float head_offset = 0;
				float feet_height = 20;
				float head_height = 30;
				
				rectangle@ player_rect = player.collision_rect();
				float feet_x1 = player_x + player_rect.left();
				float feet_x2 = player_x + player_rect.right();
				float feet_y1 = player_y - feet_height;
				float feet_y2 = player_y;
				
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
							player.state(8);
							dm.dash(dm.dash_max());
							
							controllable@ ec = e.as_controllable();
							
							ec.set_speed_xy(ec.x_speed(), ec.y_speed() + 200);
							ec.y(ec.y() + 20);
							add_fx(Fx(player_x, player_y, "dustman", "dmland", 0, 0, 1, 1, 15));
							add_fx(Fx(player_x, player_y, "dustman", "dmjump", 0, 0, 1, 1, 15));
							break;
						}
					}
				}
			}
			
			// Ground slam
			// st_fall, st_hover
			if(state == 5 or state ==7){
				if(player.y_intent() > 0 and player.jump_intent() == 1 and can_use_skill()){
					dm.set_speed_xy(dm.x_speed(), dm.fall_max());
					player.jump_intent(0);
					update_skill(-50);
					ground_slam = true;
					add_fx(Fx(dm.x(), dm.y(), "dustman", "dmfastfall", 0, 0, 1.5, 1.5, 15));
				}
			}
			else if(ground_slam){
				if(state == 6){ // st_land
					const float slam_width = 5 * 48;
					const float slam_height = 4 * 48;
					float px = player.x();
					float py = player.y();
					
					for(int i = entities - 1; i >= 0; i--){
						entity@ e = entity_by_index(i);
						if(e.type_name().substr(0, 5) == "enemy"){
							if(abs(e.x() - px) < slam_width and abs(e.y() - py) < slam_height){
								g.remove_entity(e);
							}
						}
					}
					
					// entity@ my_entity = create_entity("effect");
					// g.add_entity(my_entity);
					// puts((@my_entity==null)+"")
					if(effects_current_count >= effects_current.length())
					{
						effects_current.resize(effects_current_count + 10);
					}
					add_fx(Fx(px, py - 20, "dustman", "dmcleanseready", 0, 90, 1.5, 2.0, 15));
					add_fx(Fx(px, py, "dustman", "dmheavyland", 0, 0, 2.5, 2.0, 15));
				}
				
				ground_slam = false;
			}
			
			// Super dash and jump
			// st_idle
			if(state == 0 and can_use_skill()){
				if(player.y_intent() > 0){
				// Start super jump (down+jump during idle)
					if(player.jump_intent() != 0){
						player.jump_intent(0);
						super_jump = true;
						update_skill(-50);
						player.state(state = 10);
					}
				// Start super dash (down+dash during idle)
					else if(player.dash_intent() != 0){
						player.dash_intent(0);
						super_dash = true;
						update_skill(-50);
						// Put the player into the slide state, this looks goods as a dash windup
						player.state(state = 3);
					}
				}
			}
			
			// Execute the super jump
			if(super_jump){
				// In case the player is attacked or something, cancel the jump
				if(state != 10){
					super_jump = false;
					super_jump_wait = 0;
				}
				
				// The player can't do anything during jump windup
				player.jump_intent(0);
				player.dash_intent(0);
				player.heavy_intent(0);
				player.light_intent(0);
				player.x_intent(0);
				player.y_intent(0);
				
				// Hold at the end of the crouch anim
				player.sprite_index("crouch");
				if(player.state_timer() > 2.5){
					player.state_timer(2.5);
					super_jump_wait += 0.016;
					if(super_jump_wait >= super_jump_wait_max){
						super_jump = false;
						super_jump_wait = 0;
						jump_boost = jump_boost_max;
						player.state(8);
					}
				}
			}
			
			// Execute super dash
			else if(super_dash){
				// In case the player is attacked or something, cancel the dash
				if(state != 3){
					super_dash = false;
					super_dash_wait = 0;
				}
				
				// The player can't do anything during jump windup
				player.jump_intent(0);
				player.dash_intent(0);
				player.heavy_intent(0);
				player.light_intent(0);
				player.x_intent(0);
				player.y_intent(0);
				
				// Hold at the end of the anim
				if(player.state_timer() > 1.5){
					player.state_timer(1.5);
					super_dash_wait += 0.016;
					if(super_dash_wait >= super_dash_wait_max){
						super_dash = false;
						super_dash_wait = 0;
						player.state(9);
						float x_speed = player.x_speed();
						float y_speed = player.y_speed();
						player.set_speed_xy(x_speed + super_dash_boost * player.face(), y_speed);
						
						add_fx(Fx(dm.x(), dm.y(), "dustman", "dmdash", 0, 0, 2.5 * dm.face(), 2.5, 15));
						super_dash_timer = 52;
					}
				}
			}
			
			if(super_dash_timer > 0)
			{
				if((super_dash_timer / 10) * 10 == super_dash_timer)
				{
					float s = (1 + 1.5 * (super_dash_timer / 50.0)) * dm.face();
					add_fx(Fx(dm.x(), dm.y(), "dustman", "dmdash", 0, 0, s, abs(s), 15));
				}
				
				super_dash_timer -= 2;
				if(super_dash_timer < 0)
					super_dash_timer = 0;
			}
			
			// st_jump
			if(state == 8){
				if(jump_boost != 0){
					float x_speed = player.x_speed();
					float y_speed = player.y_speed();
					player.set_speed_xy(x_speed, y_speed + jump_boost);
					jump_boost = 0;
					
					add_fx(Fx(dm.x(), dm.y(), "dustman", "dmheavyland", 0, 0, 2.5, 1.5, 15));
					add_fx(Fx(dm.x(), dm.y(), "dustman", "dmjump", 0, 0, 1, 2.5, 15));
				}
			}
			
			if(skill < 99 and !super_jump and ! super_dash and !ground_slam){
				update_skill(skill + 1);
			}
			
			previous_state = state;
		}
	}
	
	void draw(float sub_frame)
	{
		for(uint i = 0; i < effects_current_count; i++)
		{
			Fx@ fx = effects_current[i];
			if(!effects_current[i].draw())
			{
				@effects_current[i] = @effects_current[effects_current_count - 1];
				@effects_current[effects_current_count--] = null;
			}
		}
		
		text.draw_hud(19, 19, -800 + 10, -450 + 10, 0.5, 0.5, 0);
	}

	// void step_post(int entities){
		// if(@player != null){
			// int state = player.state();
		// }
	// }

}