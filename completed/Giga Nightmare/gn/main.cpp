#include "../lib/std.cpp"
#include "../lib/colour.cpp"
#include "../lib/emitters.cpp"
#include "../lib/math/math.cpp"
#include "../lib/enums/EntityState.cpp"
#include "../lib/tiles/TileShape.cpp"
#include "../lib/drawing/Sprite.cpp"
#include "GameState.cpp"
#include "DeathText.cpp"

const string EMBED_dummy	= 'gn/dummy.png';

class script : callback_base
{
	
	scene@ g;
	controllable@ player = null;
	dustman@ dm = null;
	camera@ cam;
	entity@ apple;
	float apply_y_offset;
	float apply_end_y;
	
	[position,mode:world,layer:19,y:start_tile_y] float start_tile_x;
	[hidden] float start_tile_y;
	
	GameState state = Starting;
	uint seed = 3124;
	int giga_start_side;
	int giga_end_side;
	int prev_wall_x;
	int current_wall_x;
	int current_wall_y;
	int prev_wall_facing;
	int current_wall_facing = 0;
	int current_wall_facing_real = 0;
	bool current_wall_is_double;
	bool current_wall_is_jump;
	int jump_count;
	bool failed = false;
	bool next_wall_is_jump;
	int wall_count = 0;
	int phase1_wall_count = 30; // 30
	int phase2_wall_count = 18; // 20
	int target_wall_count = phase1_wall_count;
	int phase = 1;
	entity@ wall_emitter;
	
	float wall_jump_chance = 0.3;
	float double_wall_chance = 0.25;
	
	int phase1_distance_min = 5;
	int phase1_distance_max = 7;
	int phase2_distance_min = 25;
	int phase2_distance_max = 35;
	
	float state_timer = 0;
	
	controllable@ prism;
	
	Sprite arrow_spr('props5', 'symbol_1');
	
	float jorf_timer = 0;
	float jorf_tolerance = 0.4;
	textfield@ death_textfield;
	array<DeathText> death_text;
	array<string> death_strings = {"No!", "No!", "No!", "No.", "Stop!", "Why!", "Don't!"};
//	array<string> death_sounds = {"damage", "bgargoyle_attack", "damage_spikes", "impact_heavy_1", "wolf_attack"};
//	array<string> death_sounds = {"trashbeast_attack"};
	array<string> death_sounds = {"damage_spikes"};
	
	float death_time = 10;
	
	float death_text_interval;
	float death_text_interval_decay = 0.75;
	float death_text_max_interval = 0.9;
	float death_text_min_interval = 0.025;
	
	float death_sound_interval;
	float death_sound_interval_decay = 0.75;
	float death_sound_max_interval = 0.5;
	float death_sound_min_interval = 0.01;
	uint death_colour;
	
	float death_cam_min_height = 100;
	float death_cam_zoom = 3;
	
	float end_game_wait = 10;
	float fall_timer = 0;
	
	[entity] int apple_id;
	
	script()
	{
		@g = get_scene();
		@death_textfield = create_textfield();
		death_textfield.set_font("ProximaNovaReg", 100);
		death_textfield.align_horizontal(0);
		death_textfield.align_vertical(0);
	}
	
	void build_sprites(message@ msg)
	{
		msg.set_string("fall", "dummy");
	}

	void checkpoint_load()
	{
		@player = null;
		@dm = null;
		@cam = null;
	}
	
	int player_x_prev, player_y_prev;
	int player_jump_prev;
	int player_dash_prev;
	int player_fall_prev;
	int player_heavy_prev;
	int player_light_prev;
	float player_xs_prev;
	float player_ys_prev;
	
	void update_seed()
	{
		int x_prev = player.x_intent() + (player_x_prev == 0 ? 2 : 0);
		int y_prev = player.y_intent() + (player_y_prev == 0 ? 2 : 0);
		int jump_prev = player.jump_intent() + (player_jump_prev == 0 ? 2 : 0);
		int dash_prev = player.dash_intent() + (player_dash_prev == 0 ? 2 : 0);
		int fall_prev = player.fall_intent() + (player_fall_prev == 0 ? 2 : 0);
		int heavy_prev = player.heavy_intent() + (player_heavy_prev == 0 ? 2 : 0);
		int light_prev = player.light_intent() + (player_light_prev == 0 ? 2 : 0);
		int xs_prev = round_int(player.x_speed() + (player_xs_prev == 0 ? 2 : 0));
		int ys_prev = round_int(player.y_speed() + (player_ys_prev == 0 ? 2 : 0));
		
		seed += x_prev + y_prev + jump_prev + dash_prev + fall_prev + heavy_prev + light_prev + xs_prev + ys_prev;
		
		player_x_prev = player.x_intent();
		player_y_prev = player.y_intent();
		player_jump_prev = player.jump_intent();
		player_dash_prev = player.dash_intent();
		player_fall_prev = player.fall_intent();
		player_heavy_prev = player.heavy_intent();
		player_light_prev = player.light_intent();
		player_xs_prev = player.x_speed();
		player_ys_prev = player.y_speed();
	}
	
	void step(int entities)
	{
		if(player is null)
		{
			@player = controller_controllable(0);
			@dm = player.as_dustman();
			@cam = get_camera(0);
			@apple = entity_by_id(apple_id);
			
			apple.as_controllable().on_hurt_callback(this, "on_apple_hurt", 0);
			apply_y_offset = apple.y() - player.y();
			
			cam.screen_height(980);
			
			restart();
			
			return;
		}
		
		EntityState player_state = EntityState(player.state());
		
		if(player_state == EntityState::Fall)
		{
			fall_timer += DT;
			
			if(current_wall_facing != 0 && fall_timer > 1.25)
			{
				clear_after_fall();
			}
			
			float max = phase == 2 && state != EndGame ? 1.75 : 4;
			if(fall_timer > max)
			{
				restart();
			}
		}
		else
		{
			fall_timer = 0;
		}

		if(state != Starting && state < DeathStart && state != EndGame)
		{
			if(player.ground())
			{
				jorf_timer += DT;
				
				if(jorf_timer >= jorf_tolerance)
				{
					state = DeathStart;
				}
			}
		}
		
		if(state >= DeathStart)
		{
			player.x_intent(0);
			player.y_intent(0);
			player.jump_intent(0);
			player.dash_intent(0);
			player.fall_intent(0);
			player.taunt_intent(0);
			player.heavy_intent(0);
			player.light_intent(0);
			player.set_speed_xy(0, player.y_speed());
		}
		
		switch(state)
		{
			case Restarting:
			{
				update_seed();
				
				if(player.ground())
				{
					state = Starting;
				}
			}
				break;
				
			case Starting:
			{
				update_seed();
				
				if(!player.ground())
				{
					state = BeforeGiga;
					dm.dash(0);
					dm.dash_max(0);
					
					srand(seed);
					
					int start_tile_x = tile_coord(this.start_tile_x);
					int start_tile_y = tile_coord(this.start_tile_y);
					
					g.set_tile_filth(start_tile_x, start_tile_y, 0, 0, 0, 0, true, true);
					g.set_tile(start_tile_x, start_tile_y, 16, false, 0, 0, 0, 0);
					g.set_tile(start_tile_x, start_tile_y, 19, false, 0, 0, 0, 0);
					
					remove_wall_emitter();
					
					current_wall_x = tile_coord(player.x());
					current_wall_y = tile_coord(player.y()) - 2;
					spawn_wall();
				}
			}
				break;
				
			case BeforeGiga:
			{
				update_seed();
				
				if(player_state == EntityState::WallRun)
				{
					remove_prism();
					state = StartGiga;
					giga_start_side = player.wall_right() ? 1 : -1;
				}
			}
				break;
				
			case StartGiga:
			{
				if(player_state != EntityState::WallRun)
				{
					spike_wall();
					state = current_wall_is_double ? DoubleGigaTransition : DuringGiga;
				}
			}
				break;
				
			case DoubleGigaTransition:
			{
				if(player_state == EntityState::WallRun)
				{
					state = DoubleGigaBack;
					unspike_wall();
				}
			}
				break;
				
			case DoubleGigaBack:
			{
				if(player_state != EntityState::WallRun)
				{
					state = DuringGiga;
					giga_start_side = -current_wall_facing;
					current_wall_facing = -current_wall_facing;
					spike_wall();
				}
			}
				break;
				
			case DuringGiga:
			{
				if(player_state == EntityState::WallDash || player_state == EntityState::CrouchJump)
				{
					giga_end_side = player.wall_right() ? 1 : -1;
				}
				
				if(player_state == EntityState::WallDash || player_state == EntityState::Jump)
				{
					if(
						giga_end_side != giga_start_side &&
						(
							next_wall_is_jump && player_state == EntityState::Jump ||
							!next_wall_is_jump && player_state == EntityState::WallDash
						)
					)
					{
						state = EndGiga;
					}
					else
					{
						failed = true;
						current_wall_facing = 0;
						state = EndGiga;
					}
				}
			}
				break;
				
			case EndGiga:
			{
				if(player_state != EntityState::WallDash || player_state == EntityState::Jump)
				{
					remove_wall();
					
					if(!failed)
					{
						wall_count++;
						
						if(wall_count >= target_wall_count)
						{
							current_wall_facing = 0;
							
							if(phase == 1)
							{
								state = TransitionToPhase2;
							}
							else if(phase == 2)
							{
								state = BeforeEndGame;
								current_wall_is_jump = false;
								prev_wall_x = current_wall_x;
								prev_wall_facing = current_wall_facing_real;
								state_timer = 0;
								spawn_prism();
								set_apple_end_y();
							}
						}
						else
						{
							spawn_wall(giga_start_side);
							
							if(phase == 2)
							{
								spawn_prism();
							}
							
							state = BeforeGiga;
						}
					}
					else
					{
						state = FailedGiga;
					}
					
					jorf_timer = 0;
				}
			}
				break;
				
			case BeforeEndGame:
			{
				state_timer += DT;
				
				if(state_timer >= end_game_wait)
				{
					reset_dash_max();
					
					entity@ end_prism  = create_entity("enemy_tutorial_hexagon");
					end_prism.set_xy(
						prism.x() + prev_wall_facing * 15 * TILE2PIXEL,
						player.y() - 9.5 * TILE2PIXEL
					);
					g.add_entity(end_prism);
					state = EndGame;
					
					entity@ end_trigger  = create_entity("level_end");
					end_trigger.set_xy(end_prism.x(), end_prism.y());
					varstruct@ vars = end_trigger.vars();
					varvalue@ ent_list = vars.get_var("ent_list");
					vararray@ ent_list_arr = ent_list.get_array();
					ent_list_arr.resize(1);
					varvalue@ ent0 = ent_list_arr.at(0);
					ent0.set_int32(end_prism.id());
					g.add_entity(end_trigger);
					
					g.remove_entity(apple);
				}
			}
				break;
				
			case DeathStart:
			{
				state_timer += DT;
				if(state_timer >= 1.5)
				{
					state_timer = 0;
					state = DeathText;
					death_text_interval = death_text_max_interval;
					add_death_text();
				}
				
				death_zoom();
			}
				break;
				
			case DeathText:
			{
				state_timer += DT;
				death_text_interval -= DT;
				
				if(death_text_interval <= 0)
				{
					death_text_max_interval *= death_text_interval_decay;
					if(death_text_max_interval < death_text_min_interval)
					{
						death_text_max_interval = death_text_min_interval;
					}
					death_text_interval = death_text_max_interval;
					
					add_death_text();
				}
				
				float t = clamp01(state_timer / death_time);
				
				death_sound_interval -= DT;
				
				if(death_sound_interval <= 0)
				{
					death_sound_max_interval *= death_sound_interval_decay;
					if(death_sound_max_interval < death_sound_min_interval)
					{
						death_sound_max_interval = death_sound_min_interval;
					}
					death_sound_interval = death_sound_max_interval;
					
					g.play_sound("sfx_" + death_sounds[uint(rand_range(0, death_sounds.length() - 1))], player.x(), player.y(), t, false, false);
				}
				
				death_colour = rgba(1.0, 1.0, 1.0, t);
				
				death_zoom();
				cam.add_screen_shake(player.x(), player.y(), rand_rotation(), 1 + state_timer * 5);
				
				if(t >= 0.95)
				{
					crash();
				}
			}
				break;
		}
		
		if(prism !is null)
		{
			prism.x(prism.x() + prev_wall_facing * 315 * DT);
		}
		
		if(state == BeforeEndGame || state == EndGame)
		{
			float t = clamp01(state_timer / (end_game_wait * 0.5));
			apple.set_xy(player.x() + current_wall_facing_real * 70, lerp(player.y() + apply_y_offset, apply_end_y, t));
		}
		else
		{
			apple.set_xy(player.x() + current_wall_facing_real * 70, player.y() + apply_y_offset);
		}
	}
	
	void draw(float sub_frame)
	{
		switch(state)
		{
			case DeathText:
				if(state_timer > death_time * 0.5)
				{
					uint fade_clr = set_alpha(death_colour, clamp01((state_timer - death_time * 0.5) / (death_time * 0.5)));
					g.draw_rectangle_hud(22, 23, SCREEN_LEFT, SCREEN_TOP, SCREEN_RIGHT, SCREEN_BOTTOM, 0, fade_clr);
				}
				draw_death_texts();
				break;
		}
		
		if(state == Starting)
		{
			int start_tile_x = tile_coord(this.start_tile_x);
			int start_tile_y = tile_coord(this.start_tile_y);
			
			arrow_spr.draw(17, 19, 0, 0, start_tile_x * TILE2PIXEL + 24 - 96, (start_tile_y - 1) * TILE2PIXEL, 180);
			arrow_spr.draw(17, 19, 0, 0, start_tile_x * TILE2PIXEL + 24 + 96, (start_tile_y - 1) * TILE2PIXEL, 0);
		}
		else if(current_wall_facing != 0)
		{
//			int facing = current_wall_is_double ? : current_wall_facing;
			arrow_spr.draw(17, 19, 0, 0,
				current_wall_x * TILE2PIXEL + 24 + current_wall_facing_real * 1.5 * TILE2PIXEL,
				current_wall_y * TILE2PIXEL + 1 * TILE2PIXEL,
				current_wall_facing_real == 1
					? (next_wall_is_jump ? -45 : 0)
					: (next_wall_is_jump ? -135 : -180));
		}
	}
	
	void set_apple_end_y()
	{
		apply_end_y = prism.y() + 16 * TILE2PIXEL;;
	}
	
	void on_apple_hurt(controllable@ attacker, controllable@ attacked, hitbox@ attack_hitbox, int arg)
	{
		player.set_xy(
			prism.x() - current_wall_facing_real * 60,
			prism.y()
		);
	}
	
	
	void create_wall_emitter(int x, int y, int w, int h, int emitter_padding = 4)
	{
		remove_wall_emitter();
		
		@wall_emitter = create_emitter(EmitterId::DustGround,
			(x + w * 0.5) * TILE2PIXEL,
			(y + h * 0.5) * TILE2PIXEL,
			int(w * TILE2PIXEL + emitter_padding * 2),
			int(h * TILE2PIXEL + emitter_padding * 2),
			15, 12);
		g.add_entity(wall_emitter);
	}
	
	void remove_wall_emitter()
	{
		if(wall_emitter is null) return;
		
		g.remove_entity(wall_emitter);
		@wall_emitter = null;
	}
	
	void clear_after_fall()
	{
		remove_wall();
		remove_prism();
		current_wall_facing = 0;
		
		if(state == BeforeEndGame)
		{
			state = Restarting;
		}
	}
	
	void restart()
	{
		player.set_speed_xy(0, player.y_speed());
		
		this.start_tile_x = player.x();
		this.start_tile_y = player.y();
		int start_tile_x = tile_coord(this.start_tile_x);
		int start_tile_y = tile_coord(this.start_tile_y);
		
		g.set_tile(start_tile_x, start_tile_y, 16, true, TileShape::Full, 4, 1, 1);
		g.set_tile(start_tile_x, start_tile_y, 19, true, TileShape::Full, 5, 1, 1);
		g.set_tile_filth(start_tile_x, start_tile_y, 0, 13, 13, 13, true, true);
		
		create_wall_emitter(start_tile_x, start_tile_y, 1, 1);
		
		next_wall_is_jump = false;
		failed = false;
		wall_count = 0;
		state_timer = 0;
		reset_dash_max();
		
		if(state == TransitionToPhase2)
		{
			target_wall_count = phase2_wall_count;
			phase++;
		}
		
		if(state != EndGame && state < DeathStart)
		{
			state = Restarting;
		}
	}
	
	void reset_dash_max()
	{
		dm.dash_max(dm.type_name() == "dust_kid" ? 2 : 1);
	}
	
	void death_zoom()
	{
		if(cam.screen_height() > death_cam_min_height)
		{
			cam.screen_height(cam.screen_height() - death_cam_zoom);
		}
	}
	
	void draw_death_texts()
	{
		for(uint i = 0; i < death_text.length(); i++)
		{
			death_text[i].draw(death_textfield, death_colour | 0xFF000000);
			
//			float x = rand_range(SCREEN_LEFT, SCREEN_RIGHT);
//			g.draw_rectangle_hud(22, 23,
//				x + rand_range(-40,  -5), -2000,
//				x + rand_range(  5,  40),  2000,
//				rand_rotation(), death_colour);
		}
	}
	
	void add_death_text()
	{
		if(death_text.length() >= 300)
		{
			return;
		}
		
		death_text.insertLast(DeathText(
			death_strings[uint(rand_range(0, death_strings.length() - 1))],
			// Pos
			rand_range(SCREEN_LEFT * 0.8, SCREEN_RIGHT * 0.8), rand_range(SCREEN_TOP * 0.8, SCREEN_BOTTOM * 0.8),
			// Scale
			rand_range(0.75, 1.8), rand_range(0.75, 1.8),
			// Rotation
			rand_range(-10, 10)
		));
		
//		g.play_sound("sfx_" + death_sounds[uint(rand_range(0, death_sounds.length() - 1))], player.x(), player.y(), 1, false, false);
	}
	
	void crash()
	{
//		puts("CRASHING");
//		return;
		
//		sprites@ spr = create_sprites();
//		entity@ apple = entity_by_id(apple_id);
//		spr.add_sprite_set("doors");
//		apple.set_sprites(spr);
		
		float x = player.x();
		float y = player.y();
		entity@ test_entity = create_entity("enemy_gargoyle_small");
		entity@ test_AI = create_entity("AI_controller");
		test_entity.x(x);
		test_entity.y(y);
		test_AI.x(x);
		test_AI.y(y);
		g.add_entity(test_entity);
		g.add_entity(test_AI);
		
		test_AI.vars().get_var("puppet_id").set_int32(test_entity.id());
		
		dm.kill(true);
	}
	
	void spawn_prism()
	{
		remove_prism();
		
		if(current_wall_is_jump) return;
		
		@prism  = create_entity("enemy_tutorial_hexagon").as_controllable();
		prism.set_xy(
			(prev_wall_x + prev_wall_facing * 3.5) * TILE2PIXEL + 24,
			(current_wall_y + 4) * TILE2PIXEL
		);
		g.add_entity(prism.as_entity());
		prism.on_hurt_callback(this, "on_prism_hurt", 0);
	}
	
	void on_prism_hurt(controllable@ attacker, controllable@ attacked, hitbox@ attack_hitbox, int arg)
	{
		if(attack_hitbox.damage() == 3 && state != EndGame)
		{
			state = DeathStart;
		}
	}
	
	void remove_prism()
	{
		if(prism is null) return;
		
		g.remove_entity(prism.as_entity());
		@prism = null;
	}
	
	void remove_wall()
	{
		remove_wall_emitter();
		unspike_wall(0);
		
		tileinfo@ tile = create_tileinfo();
		tile.type(TileShape::Full);
		tile.solid(false);
		
		g.set_tile(current_wall_x, current_wall_y, 19, tile, true);
		g.set_tile(current_wall_x, current_wall_y + 1, 19, tile, true);
		g.set_tile(current_wall_x, current_wall_y + 2, 19, tile, true);
		g.set_tile(current_wall_x, current_wall_y + 3, 19, tile, true);
		
		g.set_tile(current_wall_x, current_wall_y, 16, tile, true);
		g.set_tile(current_wall_x, current_wall_y + 1, 16, tile, true);
		g.set_tile(current_wall_x, current_wall_y + 2, 16, tile, true);
		g.set_tile(current_wall_x, current_wall_y + 3, 16, tile, true);
	}
	
	void spawn_wall(int facing = 0)
	{
		float x = player.x();
		float y = player.y();
		
		if(facing == 0)
		{
			facing = player_facing();
		}
		
		prev_wall_x = current_wall_x;
		prev_wall_facing = current_wall_facing_real;
		
//		current_wall_is_double = true;
		current_wall_is_double = frand() < double_wall_chance;
		current_wall_facing_real = current_wall_is_double ? -facing : facing;
		
		bool is_phase1 = (phase == 1 || next_wall_is_jump || wall_count == 0);
		int wall_distance_min = is_phase1 ? phase1_distance_min : phase2_distance_min;
		int wall_distance_max = is_phase1 ? phase1_distance_max : phase2_distance_max;
		
		current_wall_facing = facing;
		current_wall_x += facing * rand_range(wall_distance_min, wall_distance_max);
		
		if(next_wall_is_jump)
		{
			current_wall_y -= 3;
		}
		
		tileinfo@ tile = create_tileinfo();
		tile.type(TileShape::Full);
		
		g.set_tile(current_wall_x, current_wall_y, 19, tile, true);
		g.set_tile(current_wall_x, current_wall_y + 1, 19, tile, true);
		g.set_tile(current_wall_x, current_wall_y + 2, 19, tile, true);
		
		g.set_tile(current_wall_x, current_wall_y, 16, true, TileShape::Full, 4, 1, 1);
		g.set_tile(current_wall_x, current_wall_y + 1, 16, true, TileShape::Full, 4, 1, 1);
		g.set_tile(current_wall_x, current_wall_y + 2, 16, true, TileShape::Full, 4, 1, 1);
		
		int tile_shape = facing > 0 ? TileShape::HalfC : TileShape::HalfB;
		tile.type(tile_shape);
		g.set_tile(current_wall_x, current_wall_y + 3, 19, tile, true);
		g.set_tile(current_wall_x, current_wall_y + 3, 16, true, tile_shape, 4, 1, 1);
		
		unspike_wall();
		create_wall_emitter(current_wall_x, current_wall_y, 1, 4);
		
		current_wall_is_jump = next_wall_is_jump;
		next_wall_is_jump = frand() < wall_jump_chance;
//		next_wall_is_jump = true;
		
		// Make sure the last wall is a dash wall so we get a prism
		if(phase == 2 && wall_count == target_wall_count - 1)
		{
			next_wall_is_jump = false;
		}
		
		if(next_wall_is_jump)
		{
			if(current_wall_is_double)
			{
				jump_count++;
			}
			
			if(jump_count++ >= 2)
			{
				next_wall_is_jump = false;
				jump_count = 0;
			}
		}
		else
		{
			jump_count = 0;
		}
		
		srand(seed);
		seed -= 314325;
	}
	
	void spike_wall()
	{
		uint left = current_wall_facing == 1 ? 13 : 0;
		uint right = current_wall_facing == -1 ? 13 : 0;
		
		spike_wall(left, right);
	}
	
	void unspike_wall(uint top_bottom = 13)
	{
		spike_wall(0, 0, top_bottom);
	}
	
	void spike_wall(uint left, uint right, uint top_bottom = 13)
	{
		// Top
		g.set_tile_filth(current_wall_x, current_wall_y, top_bottom, 0, left, right, true, true);
		// Middle
		g.set_tile_filth(current_wall_x, current_wall_y + 1, 0, 0, left, right, true, true);
		g.set_tile_filth(current_wall_x, current_wall_y + 2, 0, 0, left, right, true, true);
		// Bottom
		g.set_tile_filth(current_wall_x, current_wall_y + 3, 0, top_bottom, left, right, true, true);
	}
	
	int player_facing()
	{
		return player.x_speed() != 0
				? (player.x_speed() < 0 ? -1 : 1)
				: player.face();
	}
	
}