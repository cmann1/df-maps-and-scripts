#include '../lib/easing/sine.cpp';
#include '../lib/entities/player_head_data.cpp';
#include '../lib/entities/utils.cpp';
#include '../lib/enums/AttackType.cpp';
#include '../lib/enums/Character.cpp';
#include '../lib/enums/ColType.cpp';
#include '../lib/enums/ControllerMode.cpp';
#include '../lib/enums/InputKeyState.cpp';
#include '../lib/math/math.cpp';

#include 'Chirp.cpp';
#include 'PlayerData.cpp';
#include 'settings/PlayerSettings.cpp';

const int RUN_DUST_EMITTER_ID = EmitterId::LeafGroundCreate;
const int WALL_DUST_EMITTER_ID = EmitterId::LeafRoofCreate;
const int SKID_DUST_EMITTER_ID = EmitterId::TrashGroundCreate;

class Player
{
	
	script@ script;
	scene@ g;
	canvas@ canvas;
	int player_index;
	controllable@ player;
	dustman@ dm;
	camera@ cam;
	int character = Character::None;
	bool is_virtual;
	float rect_left, rect_top, rect_right, rect_bottom;
	float rect_w, rect_h;
	int previous_state = EntityState::Idle;
	float prev_y_speed;
	
	float x, y;
	float cx, cy;
	float prev_x;
	float prev_y;
	int face;
	int state;
	int attack_state;
	float state_timer;
	float seed = 0;
	
	float cam_x1, cam_y1;
	float cam_x2, cam_y2;
	int chunk_x1, chunk_y1;
	int chunk_x2, chunk_y2;

	float last_ground_dust = 0;
	float last_roof_dust = 0;
	float last_wall_dust = 0;
	
	PlayerData data;
	PlayerData checkpoint_data;
	
	Ribbon@ scarf;
	bool draw_scarf = true;
	bool pending_scarf_create;
	int size_scarf_grow_constraints = 8;
	int num_scarf_grow_constraints;
	array<DistanceConstraint@> scarf_grow_constraints(size_scarf_grow_constraints);
	array<float> scarf_positions;
	
	float chirp_timer = -1;
	bool chirp_released = true;
	int chirp_symbol = -1;
	
	int char_data_index = -1;
	float offset_x, offset_y;
	bool has_head_angle = false;
	float head_offset_x, head_offset_y;
	float head_offset_rotation;
	float head_rotation;
	float head_x, head_y;
	float head_top_x, head_top_y;
	bool has_head_data;
	
	float combo_timer = -1;
	bool calculate_head;
	bool pending_special;
	
	array<Chirp> chirps;
	int chirp_count;
	int chirp_rand_index;
	
	void init(script@ script, const int player_index)
	{
		@this.script = script;
		@g = script.g;
		@canvas = create_canvas(false, Player::SignLayer, Player::SignSubLayer);
		@cam = get_camera(player_index);
		this.player_index = player_index;
	}

	void checkpoint_save()
	{
		checkpoint_data = data;
		
		if(@scarf != null)
		{
			scarf.save_positions(scarf_positions);
		}
	}

	void checkpoint_load()
	{
		@dm = null;
		previous_state = EntityState::Idle;
		prev_y_speed = 0;
		character = Character::None;
		is_virtual = false;
		
		if(data.scarf_length > 0)
		{
			cam.controller_mode(ControllerMode::Advanced);
		}
		
		data = checkpoint_data;
		pending_scarf_create = false;
		num_scarf_grow_constraints = 0;
		combo_timer = -1;
		calculate_head = data.scarf_length > 0;
		
		chirps.resize(0);
		chirp_count = 0;
		pending_special = false;
	}

	void step(const int num_entities)
	{
		if(@dm == null)
		{
			@player = controller_controllable(player_index);
			@dm = @player != null ? player.as_dustman() : null;
			
			if(@dm == null)
				return;
			
			character = Character::get_character(dm.character(), is_virtual);
			
			rectangle@ rect = dm.base_rectangle();
			rect_left = rect.left();
			rect_top = rect.top();
			rect_right = rect.right();
			rect_bottom = rect.bottom();
			rect_w = rect_right - rect_left;
			rect_h = rect_bottom - rect_top;
			
			// Just so we can add a bit more dust in the level.
			dm.skill_combo_max(106);
			
			x = dm.x();
			y = dm.y();
			dm.centre(cx, cy);
			state = dm.state();
			state_timer = dm.state_timer();
			
			if(data.scarf_length > 0)
			{
				init_scarf();
				pending_scarf_create = true;
			}
			
			//if(data.scarf_length == 0)
			//{
			//	scarf_get(script.chirp_big_scarf_length);
			//}
		}
		
		if(data.locked)
		{
			dm.x_intent(0);
			dm.y_intent(0);
			dm.taunt_intent(0);
			dm.heavy_intent(0);
			dm.light_intent(0);
			dm.dash_intent(0);
			dm.jump_intent(0);
			dm.fall_intent(0);
		}
	}

	void step_post(const int num_entities)
	{
		if(@dm == null)
			return;
		
		prev_x = dm.prev_x();
		prev_y = dm.prev_y();
		x = dm.x();
		y = dm.y();
		dm.centre(cx, cy);
		state = dm.state();
		state_timer = dm.state_timer();
		attack_state = dm.attack_state();
		face = dm.face();
		
		if(attack_state == AttackType::Light || attack_state == AttackType::Heavy)
		{
			if(dm.attack_timer() < 1)
			{
				seed += x % 100 + y % 120;
			}
		}
		
		if(dm.attack_state() == AttackType::Special)
		{
			pending_special = true;
		}
		else if(pending_special)
		{
			super_ribbons();
			pending_special = false;
		}
		
		if(calculate_head)
		{
			calculate_head_base();
		}
		
		if(@scarf != null)
		{
			update_scarf();
		}
		
		create_surface_particles();

		if(state != previous_state)
		{
			previous_state = state;
		}
		
		prev_y_speed = dm.y_speed();
		
		if(combo_timer != -1)
		{
			if(combo_timer == 0)
			{
				combo_timer = dm.combo_timer();
			}
			else
			{
				dm.combo_timer(combo_timer);
			}
		}
		
		for(int i = chirp_count - 1; i >= 0; i--)
		{
			Chirp@ chirp = @chirps[i];
			chirp.timer += DT * 60;
			
			if(chirp.timer >= chirp.timer_max)
			{
				chirps.removeAt(i);
				chirp_count--;
			}
		}
		
		try_chirp();
		
		cam_x1 = cam.x() - cam.screen_width() * 0.5;
		cam_y1 = cam.y() - cam.screen_height() * 0.5;
		cam_x2 = cam.x() + cam.screen_width() * 0.5;
		cam_y2 = cam.y() + cam.screen_height() * 0.5;
		chunk_x1 = int(floor((cam_x1 - 48 * 8) / (48 * 16)));
		chunk_y1 = int(floor((cam_y1 - 48 * 22) / (48 * 16)));
		chunk_x2 = int(floor((cam_x2 + 48 * 8) / (48 * 16)));
		chunk_y2 = int(floor((cam_y2 + 48 * 22) / (48 * 16)));
		
		if(script.debug_view)
		{
			script.debug.rect(
				22, 22,
				cam_x1, cam_y1, cam_x2, cam_y2,
				0, 5, 0xffffffff, true, 1);
			script.debug.rect(
				22, 22,
				chunk_x1 * 48 * 16, chunk_y1 * 48 * 16,
				chunk_x2 * 48 * 16 + 48 * 16, chunk_y2 * 48 * 16 + 48 * 16,
				0, 5, 0xffffffff, true, 1);
		}
	}
	
	void try_chirp()
	{
		if(data.locked || data.scarf_length <= 0)
			return;
		if(attack_state == AttackType::Special)
			return;
		
		if(chirp_timer < 0)
		{
			if(!chirp_released && !InputKeyState::is_down(cam.input_taunt()))
			{
				chirp_released = true;
			}
			if(chirp_released && InputKeyState::is_down(cam.input_taunt()))
			{
				chirp_timer = script.chirp_big_time;
			}
		}
		else
		{
			chirp_released = !InputKeyState::is_down(cam.input_taunt());
			chirp_timer = max(chirp_timer - DT * 60, 0.0);
			
			if(chirp_released || chirp_timer == 0)
			{
				chirps.resize(++chirp_count);
				Chirp@ chirp = @chirps[chirp_count - 1];
				chirp.sign_x = head_top_x;
				chirp.sign_y = head_top_y;
				chirp.x = head_x;
				chirp.y = head_y;
				const bool is_big = data.scarf_length >= script.chirp_big_scarf_length && chirp_timer == 0;
				const bool is_medium = !is_big && data.scarf_length >= script.chirp_med_scarf_length && chirp_timer < script.chirp_big_time * 0.4;
				chirp.size_t = is_big ? 1.0 : is_medium ? 0.5 : 0.0;
				chirp.timer_max = is_big ? Player::ChirpBigTime : is_medium ? Player::ChirpSmallTime : Player::ChirpSmallTime;
				chirp.ring_size = is_big ? Player::ChirpBigRingSize : is_medium ? Player::ChirpMedRingSize : Player::ChirpSmallRingSize;
				chirp.sign_size = lerp(Player::ChirpSignSizeMax, Player::ChirpBigSignSizeMax, chirp.size_t);
				chirp.alpha = is_big ? 0.85 : is_medium ? 0.65 : 0.35;
				chirp_timer = -1;
				
				const int rand_max = is_big ? 2 : is_medium ? 4 : 3;
				const string type = is_big ? 'big' : is_medium ? 'med' : 'small';
				chirp_rand_index = (chirp_rand_index + 1 + (rand() % (rand_max - 1))) % rand_max;
				g.play_script_stream('chirp_' + type + '_' + (chirp_rand_index + 1), 0, chirp.x, chirp.y, false, rand_range(0.9, 1.0));
				
				if(is_medium || is_big)
				{
					int i = g.get_entity_collision(
						y - chirp.ring_size, y + chirp.ring_size, x - chirp.ring_size, x + chirp.ring_size,
						ColType::Trigger);
					while(--i >= 0)
					{
						scripttrigger@ st = g.get_scripttrigger_collision_index(i);
						if(@st == null)
							continue;
						JumpPad@ jump_pad = cast<JumpPad@>(st.get_object());
						if(@jump_pad == null)
							continue;
						
						if(jump_pad.activate_jump(this, chirp.ring_size, chirp.size_t))
						{
							lock(true, @dm != null && dm.combo_timer() > 0 ? 1 : -1);
							break;
						}
					}
				}
			}
		}
	}
	
	void lock(const bool locked=true, const float force_combo_timer=-1)
	{
		data.locked = locked;
		
		if(data.locked)
		{
			combo_timer = force_combo_timer < 0
				? @dm != null ? dm.combo_timer() : -1
				: force_combo_timer;
		}
		else
		{
			combo_timer = -1;
		}
	}
	
	void draw(float sub_frame)
	{
		const float sub_dx = lerp(prev_x, x, sub_frame) - prev_x;
		const float sub_dy = lerp(prev_y, y, sub_frame) - prev_y;
		
		for(int i = 0; i < chirp_count; i++)
		{
			Chirp@ chirp = @chirps[i];
			
			const float t = chirp.timer / chirp.timer_max;
			const float alpha = chirp.timer < Player::ChirpFadeTime
				? ease_in_out_sine(chirp.timer / Player::ChirpFadeTime)
				: chirp.timer > chirp.timer_max - Player::ChirpFadeTime
					? ease_in_out_sine((chirp.timer_max - chirp.timer) / Player::ChirpFadeTime)
					: 1;
			const uint clr = multiply_alpha(0xffffffff, chirp.alpha * alpha);
			const float size = chirp.sign_size + chirp.sign_size * t * Player::ChirpSignGrow;
			const float offset_x = cos(head_rotation * DEG2RAD) * (size - Player::ChirpSignSizeMax) * 1.25;
			const float offset_y = sin(head_rotation * DEG2RAD) * (size - Player::ChirpSignSizeMax) * 1.25;
			
			draw_sign(canvas, null, chirp_symbol, chirp.sign_x + offset_x, chirp.sign_y + offset_y, size, 0, clr);
			const float ring_scale = chirp.ring_size / 128 * lerp(Player::ChirpRingMin, Player::ChirpRingMax, t);
			canvas.draw_sprite(script.spr, 'chirp_ring', 0, 0, chirp.x, chirp.y, 0, ring_scale, ring_scale, clr);
		}
	}
	
	void update_scarf()
	{
		scarf.set_xy(head_x, head_y);
		
		if(pending_scarf_create && scarf.ready)
		{
			scarf.load_positions(scarf_positions);
			
			array<Constraint@>@ constraints = scarf.constraints;
			
			if(num_scarf_grow_constraints < 0)
			{
				num_scarf_grow_constraints = 0;
				
				while(num_scarf_grow_constraints + constraints.length >= size_scarf_grow_constraints)
				{
					scarf_grow_constraints.resize(size_scarf_grow_constraints *= 2);
				}
				
				for(uint i = 0; i < constraints.length; i++)
				{
					DistanceConstraint@ dc = cast<DistanceConstraint@>(constraints[i]);
					
					if(@dc != null)
					{
						@scarf_grow_constraints[num_scarf_grow_constraints++] = dc;
					}
				}
			}
			
			pending_scarf_create = false;
		}
		
		scarf.visible = draw_scarf;
		
		if(num_scarf_grow_constraints > 0)
		{
			for(int i = num_scarf_grow_constraints - 1; i >= 0; i--)
			{
				DistanceConstraint@ dc = scarf_grow_constraints[i];
				
				dc.rest_length = min(dc.rest_length + DT * Player::ScarfGrowRate, 24.0);
				
				if(dc.rest_length == 24)
				{
					@scarf_grow_constraints[i] = scarf_grow_constraints[--num_scarf_grow_constraints];
				}
			}
			
			if(scarf.glow_alpha < 1)
			{
				scarf.glow_alpha = min(scarf.glow_alpha + DT * 2, 1.0);
			}
		}
		else
		{
			if(scarf.glow_alpha > 0)
			{
				scarf.glow_alpha = max(scarf.glow_alpha - DT * 1.5, 0.0);
			}
		}
	}
	
	void create_surface_particles()
	{
		switch(state)
		{
			case EntityState::Run:
			{
				if(previous_state != EntityState::Run || check_ground_step_timer(state_timer))
				{
					add_ground_dust(dm.face(), RUN_DUST_EMITTER_ID);
				}
				break;
			}
			case EntityState::Dash:
			{
				if(previous_state != EntityState::Run && dm.ground())
				{
					add_ground_dust(dm.face(), RUN_DUST_EMITTER_ID);
				}
				break;
			}
			case EntityState::Land:
			{
				if(previous_state != EntityState::Land)
				{
					add_ground_dust(-1, RUN_DUST_EMITTER_ID, 0);
					add_ground_dust( 1, RUN_DUST_EMITTER_ID, 0);
				}
				break;
			}
			case EntityState::Skid:
			case EntityState::Superskid:
			case EntityState::Idle:
			{
				if(state != EntityState::Idle || dm.x_speed() > 10)
				{
					add_ground_dust(-dm.face() * 2, SKID_DUST_EMITTER_ID, 7);
				}
				break;
			}
			case EntityState::SlopeSlide:
			{
				add_ground_dust((dm.x_speed() < 0 ? 1 : -1), SKID_DUST_EMITTER_ID, 5);
				break;
			}
			case EntityState::RoofRun:
			{
				if(previous_state != EntityState::RoofRun || check_roof_step_timer(state_timer))
				{
					add_roof_dust(dm.face(), WALL_DUST_EMITTER_ID);
				}
				break;
			}
			case EntityState::RoofGrab:
			case EntityState::RoofGrabIdle:
			{
				if(abs(dm.speed()) > 1)
				{
					add_roof_dust(-dm.face() * 2, WALL_DUST_EMITTER_ID, 7);
				}
				break;
			}
			case EntityState::WallRun:
			{
				if(previous_state != EntityState::RoofRun || check_wall_step_timer(state_timer))
				{
					add_wall_dust(WALL_DUST_EMITTER_ID);
				}
				break;
			}
			case EntityState::WallGrab:
			case EntityState::WallGrabIdle:
			{
				if(abs(dm.speed()) > 1)
				{
					add_wall_dust(WALL_DUST_EMITTER_ID, 7);
				}
				break;
			}
		}
	}
	
	void update_character_head()
	{
		switch(character)
		{
			case Character::DustMan:
			case Character::DustWraith:
				char_data_index = 1;
				head_offset_x = -4;
				head_offset_y = 12;
				head_offset_rotation = 5;
				break;
			case Character::DustGirl:
			case Character::LeafSprite:
				char_data_index = 2;
				head_offset_x = -4;
				head_offset_y = 12;
				head_offset_rotation = 0;
				break;
			case Character::DustKid:
			case Character::TrashKing:
				char_data_index = 3;
				head_offset_x = -8;
				head_offset_y = 12;
				head_offset_rotation = 25;
				break;
			case Character::DustWorth:
			case Character::SlimeBoss:
				char_data_index = 0;
				head_offset_x = -4;
				head_offset_y = 12;
				head_offset_rotation = 5;
				break;
			default:
				char_data_index = -1;
				head_offset_x = 0;
				head_offset_y = 0;
				head_offset_rotation = 0;
				break;
		}
		
		calculate_head_base();
	}
	
	void calculate_head_base()
	{
		float ox = offset_x;
		float oy = offset_y;
		
		string sprite_name;
		uint frame;
		get_entity_sprite_info(player, sprite_name, frame, face);
		has_head_data = false;
		draw_scarf = sprite_name != 'none';
		
		if(char_data_index != -1)
		{
			const int anim_index = ANIM_INDEX.exists(sprite_name)
				? int(ANIM_INDEX[sprite_name]) : -1;
			
			if(anim_index != -1)
			{
				const array<array<float>>@ head_data = @HEAD_DATA[char_data_index];
				const array<float>@ anim_data = @head_data[anim_index];
				head_x = anim_data[frame * 3] * face;
				head_y = anim_data[frame * 3 + 1];
				head_top_x = head_x;
				head_top_y = head_y;
				head_rotation = anim_data[frame * 3 + 2] * face * RAD2DEG;
				has_head_data = true;
			}
		}
		
		if(!has_head_data)
		{
			head_x = ox * face;
			head_y = oy;
			head_top_x = head_x;
			head_top_y = head_y;
			
			head_rotation = 
				player.state() >= EntityState::RoofGrab and
				player.state() <= EntityState::RoofRun
					? 90 : -90;
			head_rotation += player.rotation();
		}
		else
		{
			if(head_offset_x != 0 or head_offset_y != 0)
			{
				float dx = head_offset_x * face;
				float dy = head_offset_y;
				
				switch(character)
				{
					case Character::DustKid:
					case Character::TrashKing:
						switch(state)
						{
							case EntityState::WallRun:
								if(state_timer >= 6.0)
								{
									dx += 18 * face;
									dy += 0;
								}
								break;
							case EntityState::WallGrab:
								if(state_timer >= 1.1)
								{
									dx += 10 * face;
									dy += 5;
								}
								break;
							case EntityState::WallGrabIdle:
								dx += 15 * face;
								dy += 0;
								break;
						}
						break;
				}
				
				float odx, ody;
				rotate(dx, dy, head_rotation * DEG2RAD, odx, ody);
				head_x += odx;
				head_y += ody;
				
				rotate(0, -40, head_rotation * DEG2RAD, odx, ody);
				head_top_x += odx;
				head_top_y += ody;
			}
			
			head_rotation += -90 + player.rotation();
		}
		
		rotate(head_x, head_y, player.rotation() * DEG2RAD, head_x, head_y);
		head_x += x;
		head_y += y;
		
		rotate(head_top_x, head_top_y, player.rotation() * DEG2RAD, head_top_x, head_top_y);
		head_top_x += x;
		head_top_y += y;
		
		if(player.attack_state() == AttackType::Idle)
		{
			head_x += player.draw_offset_x();
			head_y += player.draw_offset_y();
		}
	}
	
	bool check_ground_step_timer(const float state_timer)
	{
		switch(character)
		{
			case Character::DustMan:
			case Character::DustWraith:
			case Character::DustGirl:
			case Character::LeafSprite:
			case Character::DustWorth:
			case Character::SlimeBoss:
				return check_timer(state_timer, 4, 16) || check_timer(state_timer, 9, 16);
			case Character::DustKid:
			case Character::TrashKing:
				return check_timer(state_timer, 1, 16) || check_timer(state_timer, 5, 16);
		}

		return false;
	}
	
	bool check_roof_step_timer(const float state_timer)
	{
		switch(character)
		{
			case Character::DustMan:
			case Character::DustWraith:
			case Character::DustWorth:
			case Character::SlimeBoss:
			case Character::DustKid:
			case Character::TrashKing:
				return check_timer(state_timer, 1, 16) || check_timer(state_timer, 3, 16);
			case Character::DustGirl:
			case Character::LeafSprite:
				return check_timer(state_timer, 1, 16);
		}

		return false;
	}
	
	bool check_wall_step_timer(const float state_timer)
	{
		switch(character)
		{
			case Character::DustMan:
			case Character::DustWraith:
			case Character::DustWorth:
			case Character::SlimeBoss:
				return check_timer(state_timer, 0, 16) || check_timer(state_timer, 4, 16) || check_timer(state_timer, 7, 16);
			case Character::DustGirl:
			case Character::LeafSprite:
				return check_timer(state_timer, 0, 16) || check_timer(state_timer, 2, 16) || check_timer(state_timer, 4, 16);
			case Character::DustKid:
			case Character::TrashKing:
				return check_timer(state_timer, 1, 16) || check_timer(state_timer, 4, 16) || check_timer(state_timer, 6, 16);
		}

		return false;
	}
	
	void give_super()
	{
		if(@dm != null)
		{
			dm.skill_combo(dm.skill_combo_max());
		}
	}
	
	float add_dust(
		const float face, const int surface_angle, const int emitter_id,
		const float ox, const float oy, float px, float py,
		const float size_x, const float size_y, float rotation,
		float timer, const float check_timer=14)
	{
		if(check_timer > 0 && script.frame - timer < check_timer)
			return timer;

		rotation += surface_angle;

		px *= abs(face);
		rotate(px, py, surface_angle * DEG2RAD, px, py);
		script.add_emitter_burst(
			emitter_id,
			18, 12,
			dm.x() + ox + px,
			dm.y() + oy + py,
			size_x, size_y, rotation);
		
		if(check_timer > 0)
		{
			timer = script.frame;
		}

		return timer;
	}
	
	void add_ground_dust(const float face, const int emitter_id, const float check_timer=14)
	{
		last_ground_dust = add_dust(
			face, dm.ground_surface_angle(), emitter_id,
			0, rect_bottom,
			dm.x_speed() < 0 ? rect_right : rect_left, -5,
			10, 10,
			dm.x_speed() > 0 ? 10 : 170,
			last_ground_dust, check_timer);
	}
	
	void add_roof_dust(const float face, const int emitter_id, const float check_timer=14)
	{
		last_roof_dust = add_dust(
			face, dm.roof_surface_angle(), emitter_id,
			0, rect_top,
			dm.x_speed() > 0 ? rect_right : rect_left, -10,
			10, 10,
			dm.x_speed() > 0 ? -10 : 190,
			last_roof_dust, check_timer);
	}
	
	void add_wall_dust(const int emitter_id, const float check_timer=14)
	{
		const int face = dm.wall_left() ? -1 : 1;
		const int surface_angle = face == -1
				? dm.left_surface_angle() - 90
				: dm.right_surface_angle() + 90;
		last_wall_dust = add_dust(
			face,
			surface_angle,
			emitter_id,
			face > 0 ? rect_right : rect_left, -rect_h * 0.5,
			0, rect_h * 0.5,
			10, 10,
			face > 0 ? 0 : 0,
			last_wall_dust, check_timer);
	}

	bool check_timer(const float state_timer, const float frame, const float threshold)
	{
		return state_timer >= frame && state_timer < frame + DT * threshold;
	}
	
	void init_head()
	{
		if(char_data_index == -1)
		{
			update_character_head();
		}
		
		calculate_head = true;
	}
	
	void scarf_get(int amount)
	{
		if(amount < 1 || amount > 20)
			return;
		
		if(data.scarf_length == 0)
		{
			cam.controller_mode(ControllerMode::Advanced);
			data.scarf_length = 2 + amount;
			init_scarf();
			scarf.segment_length = 2;
			pending_scarf_create = true;
			num_scarf_grow_constraints = -1;
			chirp_symbol = int(abs(sin(x * 12.9898 + y * 78.233 + script.frame + seed)) * 43758.5453) % 11;
			return;
		}
		
		while(num_scarf_grow_constraints + amount >= size_scarf_grow_constraints)
		{
			scarf_grow_constraints.resize(size_scarf_grow_constraints *= 2);
		}
		
		data.scarf_length += amount;
		
		while(--amount >= 0)
		{
			@scarf_grow_constraints[num_scarf_grow_constraints++] = scarf.extend(2);
		}
		
		scarf.draw_end = data.scarf_length >= script.chirp_big_scarf_length;
	}
	
	void update_outside(const bool outside)
	{
		data.outside = outside;
		
		if(@scarf != null)
		{
			if(outside)
			{
				scarf.gravity_multiplier = 0.6;
				scarf.wind_multiplier = 0.65;
			}
			else
			{
				scarf.gravity_multiplier = 0.95;
				scarf.wind_multiplier = 0.2;
			}
		}
	}
	
	void super_ribbons()
	{
		hitbox@ hb = null;
		const float scale = player.scale();
		const float size = 600 * scale;
		
		for(uint i = 0; i < script.ribbons.length; i++)
		{
			Ribbon@ ribbon = script.ribbons[i];
			
			if(
				ribbon.x1 - ribbon.thickness > x + size || ribbon.x2 + ribbon.thickness < x - size ||
				ribbon.y1 - ribbon.thickness > y + size || ribbon.y2 + ribbon.thickness < y - size
			)
				continue;
			
			if(@hb == null)
			{
				@hb = create_hitbox(player, 1, x, y, -size, size, -size, size);
				hb.hit_outcome(1);
				hb.damage(10000);
				hb.can_parry(false);
				hb.attack_ff_strength(0);
				hb.attack_strength(60000 * scale);
			}
			
			const float ribbon_cx = (ribbon.x2 + ribbon.x1) * 0.5;
			const float ribbon_cy = (ribbon.y2 + ribbon.y1) * 0.5;
			hb.base_rectangle(-size - ribbon.thickness, size + ribbon.thickness, -size - ribbon.thickness, size + ribbon.thickness);
			hb.attack_dir(int(atan2(ribbon_cy - y, ribbon_cx - x) * RAD2DEG) + 90);
			ribbon.apply_attack_force(hb);
		}
	}
	
	private void init_scarf()
	{
		init_head();
		calculate_head_base();

		@scarf = Ribbon();
		scarf.segments = data.scarf_length;
		scarf.segment_length = 24;
		scarf.thickness = 10;
		scarf.layer = 18;
		scarf.sublayer = 4;
		scarf.glow_layer = Player::ScarfGrowLayer;
		scarf.glow_sublayer = Player::ScarfGrowSubLayer;
		scarf.tile_collisions = true;
		scarf.self_collisions = true;
		scarf.air_friction = 20;
		scarf.trim_space = 0;
		scarf.trim_size = 3;
		scarf.base_thickness = 0.45;
		scarf.create_sin_size = 0;
		scarf.glow_clr = Player::ScarfGlowClr;
		scarf.draw_end = data.scarf_length >= script.chirp_big_scarf_length;
		
		update_outside(data.outside);
		
		scripttrigger@ st = create_scripttrigger(scarf);
		scarf.init_xy(head_x, head_y);
		script.g.add_entity(st.as_entity(), false);
	}
	
}
