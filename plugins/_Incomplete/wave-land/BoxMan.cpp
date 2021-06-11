#include "../common/math.cpp"
#include "../common/ColType.cpp"
#include "../common/Fx.cpp"
#include "Fx.cpp"

const int EDGE_TOP = 1;
const int EDGE_BOTTOM = 2;
const int EDGE_LEFT = 4;
const int EDGE_RIGHT = 8;

enum BoxState
{
	box_state_stand,
	box_state_jump,
	box_state_air,
	box_state_dash,
	box_state_attack
}

class BoxMan : enemy_base, callback_base
{
	
	scene@ g;
	script@ script;
	scriptenemy@ self;
	
	collision@ hit_box;
	int player_index = 0;
	camera@ player_camera = null;
	
	// ---------------------------------------------------
	// State
	// ---------------------------------------------------
	
	float x;
	float y;
	float prev_x;
	float prev_y;
	float vel_x = 0;
	float vel_y = 0;
	float pos_vel_x = 0;
	float pos_vel_y = 0;
	float pos_dx = 0;
	float pos_dy = 0;
	float run_vel = 0;
	float rotation = 0;
	float prev_rotation = 0;
	
	float inside_rotation = 0;
	
	BoxState state = box_state_stand;
	BoxState prev_state = state;
	float state_timer = 0;
	bool ignore_gravity = false;
	float ground_angle = 0;
	bool ground = false;
	bool wall_collision = false;
	float wall_angle = 0;
	float roof_angle = 0;
	float dash_angle = 0;
	float dash_vel = 0;
	float dash_ground_timer = 0;
	float current_dash_speed = 0;
	bool dash_slam = false;
	bool dash_fall = false;
	bool jump_charge = false;
	bool dash_charge = false;
	bool buffer_dash = false;
	
	bool attack_charge = true;
	float attack_cooldown = 0;
	
	bool prev_ground = false;
	bool prev_wall = false;
	bool prev_roof = false;
	
	int prev_x_intent = 0;
	int prev_y_intent = 0;
	
	// ---------------------------------------------------
	// Physics
	// ---------------------------------------------------
	
	float radius = 32;
	float collision_ray_grouping = 3;
	int collision_ray_count = 2;
	
	float gravity = 2500;
	float ground_move_force = 100;
	float air_move_force = 30;
	float ground_friction = 0.75;
	float landing_friction = 0.80;
	
	float run_speed = 550;
	float hop_speed = 800;
	float jump_speed = 1200;
	float dash_speed = 1200;
	float dash_ground_speed = 1080;
	float max_speed = 1200;
	float wall_hop_speed_x = 350; // Jump velocity when holding towards the wall
	float wall_jump_speed_x = 700; // Jump velocity when neutral or holding away from wall
	float wall_jump_speed_y = 1000; // Wall jump upward velocity
	float terminal_velocity = 3000;
	
	float jump_startup = 5;
	float jump_boost_factor = 0.95833; // Multiplier when jumping out of a dash
	float dash_duration = 11; // Number of frames of at full dash speed. Total dash time = dash_duration + dash_cooldown + dash_pause
	float dash_cooldown = 6; // Frames to go from dash_speed -> 0
	float dash_pause = 3; // Number of stationary frames at the end of dash
	float dash_slam_shake = 20;
	float corner_boost_window = 8; // Number of frames after starting a dash in which you can get a corner boost
	float corner_boost = 0.19792;
	
	float fast_fall_speed = 750;
	float fast_fall_tolerance = -300;
	
	float attack_duration = 6;
	float attack_cooldown_max = 60;
	float attack_radius = 64;
	float attack_diagonal_speed_multiplier = 1.2;
	int attack_damage = 4;
	float attack_shake_amount = 15;
	bool attack_kill = false;
	
	// ---------------------------------------------------
	// Animation, Drawing
	// ---------------------------------------------------
	
	float run_skew_amount = 25;
	float air_spin_speed = 10;
	float jump_squash_factor = 0.75;
	float jump_feet_stretch_factor = 0.4;
	float jump_head_pinch_factor = 0.4;
	
	float inside_size = 15;
	float inside_dot_size = 5;
	float inside_rotation_speed = 0.1;
	
	float attack_expansion = 0.15;
	
	
	////////////////////////////
	
	BoxMan()
	{
		@g = get_scene();
	}
	
	void set_position(float x, float y)
	{
		y -= radius;
		self.x(this.x = prev_x = x);
		self.y(this.y = prev_y = y);
	}
	
	void init(script@ sc, scriptenemy@ self)
	{
		@this.self = self;
		@this.script = sc;
		@hit_box = g.add_collision(@self.as_entity(), -radius, radius, -radius, radius, COL_TYPE_PLAYER);
		self.on_hit_callback(@this, "on_hit", 0);
		self.on_hurt_callback(@this, "on_hurt", 0);
		
		prev_x = self.x();
		prev_y = self.y();
		
		self.base_rectangle(-radius, radius, -radius, radius);
		self.hit_rectangle(-radius, radius, -radius, radius);
	}
	
	void on_add()
	{
		for(uint i = 0; i < num_cameras(); i++)
		{
			if(self.is_same(controller_entity(i))){
				player_index = i;
				break;
			}
		}
		
		@player_camera = get_camera(player_index);
		script.on_player_add(player_index, this);
	}
	
	void checkpoint_load()
	{
		
	}

	void on_hit(controllable@ attacker, controllable@ attacked, hitbox@ hb, int)
	{
		player_camera.add_screen_shake(x, y,
		atan2(attacked.y() - y, attacked.x() - x) * RAD2DEG,
		attack_shake_amount);
		
		if(attacked.life() - hb.damage() <= 0)
		{
			dash_charge = true;
			attack_kill = true;
		}
	}

	void on_hurt(controllable@ attacker, controllable@ attacked, hitbox@ hb, int)
	{
		puts("on_hurt");
	}
	
	// ---------------------------------------------------
	
	void check_tile(int x, int y, int side)
	{
		tilefilth@ tile = g.get_tile_filth (x, y);
		uint filth;
		
		if(side == 0)
			filth = tile.top();
		else if(side == 1)
			filth = tile.bottom();
		else if(side == 2)
			filth = tile.left();
		else if(side == 3)
			filth = tile.right();
		
		if(filth >= 9)
		{
			g.remove_entity(self.as_entity());
		}
	}
	
	raycast@ check_collisions(float dir_x, float dir_y, int tile_edge)
	{
		raycast@ ray = null;
		const float dx = dir_x * (radius + 2);
		const float dy = dir_y * (radius + 2);
		const float r = collision_ray_grouping;
		const int num_rays = collision_ray_count;
		
		for(int i = 0; i < num_rays; i++)
		{
			const float t = float(i) / (num_rays - 1);
			const float ox = dir_y * t * r;
			const float oy = dir_x * t * r;
			
			@ray = g.ray_cast_tiles(
				x + ox, y + oy,
				x + ox + dx, y + oy + dy,
				tile_edge);
//			script.debug.line(
//				x + ox, y + oy,
//				x + ox + dx, y + oy + dy,
//				20, 20, 0, 2, ray.hit() ? 0xFFFF0000 : 0xaaFFFFFF);
			
			if(ray.hit()) break;
			
			if(i != 0)
			{
				@ray = g.ray_cast_tiles(
					x - ox, y - oy,
					x - ox + dx, y - oy + dy,
					tile_edge);
//				script.debug.line(
//					x - ox, y - oy,
//					x - ox + dx, y - oy + dy,
//					20, 20, 0, 2, ray.hit() ? 0xFFFF0000 : 0xaaFFFFFF);
				
				if(ray.hit()) break;
			}
		}
		
		return ray;
	}
	
	string get_state_string(BoxState state)
	{
		if(state == box_state_stand)
			return "State_Stand";
		else if(state == box_state_jump)
			return "State_Jump";
		else if(state == box_state_air)
			return "State_Air";
		else if(state == box_state_dash)
			return "State_Dash";
		else if(state == box_state_attack)
			return "State_Attack";
		
		return "State_None";
	}
	
	// ---------------------------------------------------
	// States
	// ---------------------------------------------------
	
	void state_stand_enter()
	{
		ignore_gravity = true;
		dash_charge = true;
		jump_charge = false;
		run_vel = (vel_x > max_speed ? max_speed : vel_x) * landing_friction;
	}
	void state_stand()
	{
		if(self.light_intent() != 0 and attack_cooldown == 0)
		{
			state = box_state_attack;
			return;
		}
		
		if(!self.ground())
		{
			state = box_state_air;
			dash_charge = true;
			return;
		}
		
		if(self.jump_intent() != 0)
		{
			state = box_state_jump;
			return;
		}
		
		if(self.x_intent() != 0)
		{
			if(run_vel < 0 and self.x_intent() > 0 or run_vel > 0 and self.x_intent() < 0)
			{
				run_vel *= ground_friction;
			}
			
			if(run_vel > -run_speed and run_vel < run_speed)
			{
				run_vel += ground_move_force * self.x_intent();
				if(run_vel < -run_speed) run_vel = -run_speed;
				else if(run_vel > run_speed) run_vel = run_speed;
			}
		}
		else
		{
			run_vel *= ground_friction;
		}
		
		vel_x = cos(ground_angle * DEG2RAD) * run_vel;
		vel_y = sin(ground_angle * DEG2RAD) * run_vel;
		vel_y += abs(vel_y) * 0.025; // Otherwise the box can sometimes be pushed slightly too high on slopes and therefore no longer contact the ground causing problems
		
		self.rotation(ground_angle);
	}
	
	void state_jump_enter()
	{
		attack_cooldown = 0;
		dash_charge = true;
		ignore_gravity = true;
		buffer_dash = false;
		
		self.rotation(rotation = prev_rotation = 0);
		
		if(prev_state == box_state_air)
		{
			vel_y = 0;
		}
		
		else if(prev_state == box_state_dash)
		{
			// Instantly boost/change direction out of a dash
			if(self.x_intent() != 0)
			{
				vel_x = min(dash_ground_speed, max(abs(vel_x), abs(vel_y))) * self.x_intent();
			}
		}

		else if(prev_state == box_state_stand and abs(vel_x) > dash_ground_speed)
		{
			vel_x *= landing_friction;
		}
	}
	void state_jump()
	{
		if(self.dash_intent() != 0 or self.fall_intent() != 0)
		{
			buffer_dash = true;
		}


		if(prev_state == box_state_dash)
		{
			if(self.ground() or ground)
			{
				dash_ground_timer++;
			}
			else if(prev_ground)
			{
				do_corner_boost();
			}
		}
		
		if(state_timer >= jump_startup)
		{
			if(prev_state == box_state_dash)
				vel_x *= jump_boost_factor;
			vel_y = self.jump_intent() != 0 ? -jump_speed : -hop_speed;
			
			state = box_state_air;
			self.jump_intent(0);
			
			spawn_fx(x, y + radius, "dustworth", "dojump", 0, 15, 0, 2, 1);
		}
	}

	void state_air_enter()
	{
		self.rotation(0);
	}
	void state_air()
	{
		if(buffer_dash)
		{
			buffer_dash = false;
			
			if(self.x_intent() != 0 or self.y_intent() != 0)
			{
				state = box_state_dash;
				return;
			}
		}
		
		if(self.ground())
		{
			state = box_state_stand;
			rotation = prev_rotation = 0;
			return;
		}
		
		if(self.light_intent() != 0 and attack_cooldown == 0)
		{
			state = box_state_attack;
			return;
		}
		
		if(jump_charge and self.jump_intent() != 0)
		{
			state = box_state_jump;
			jump_charge = false;
			return;
		}
		
		if(dash_charge and (self.dash_intent() != 0 or self.fall_intent() != 0) and (self.x_intent() != 0 or self.y_intent() != 0))
		{
			state = box_state_dash;
			return;
		}
		
		if(self.y_intent() > 0)
		{
			if(prev_x_intent == 0 and prev_y_intent == 0 and vel_y > fast_fall_tolerance and vel_y < fast_fall_speed)
			{
				if(vel_y < fast_fall_speed * 0.75)
				{
					FastFallFx@ fx = FastFallFx();
					scripttrigger@ st = create_scripttrigger(fx);
					st.set_xy(x, y);
					g.add_entity(st.as_entity(), false);
				}
				
				vel_y = fast_fall_speed;
			}
		}
		
		
		
		if(self.x_intent() > 0)
		{
			if(vel_x < run_speed)
				run_vel = (vel_x += min(air_move_force, run_speed - vel_x));
		}
		else if(self.x_intent() < 0)
		{
			if(vel_x > -run_speed)
				run_vel = (vel_x -= min(air_move_force, run_speed + vel_x));
		}
		
		if(self.x_intent() != 0)
		{
			self.face(self.x_intent());
		}
		
		if(max(abs(run_vel), abs(pos_vel_x)) > 150)
		{
			rotation += air_spin_speed * (pos_vel_x > 0 ? 1 : pos_vel_x < 0 ? -1 : run_vel > 0 ? 1 : -1);
		}
		else
		{
			rotation = 0;
		}
	}
	
	void state_dash_enter()
	{
		float dx = self.x_intent();
		float dy = self.y_intent();
		dash_angle = atan2(dy, dx);
		vel_x = cos(dash_angle) * dash_speed;
		vel_y = sin(dash_angle) * dash_speed;
		dash_fall = dx == 0 and dy > 0;
		dash_vel = dash_speed;
		current_dash_speed = dash_speed;
		
		attack_cooldown = 0;
		ignore_gravity = !dash_fall;
		rotation = prev_rotation = 0;
		dash_charge = false;
		dash_ground_timer = 0;
		dash_slam = false;
		
		spawn_fx(x, y, "dustworth", "dodbljump", 0, 15, dash_angle * RAD2DEG + 90, 0.75, 1.5);
	}
	void state_dash()
	{
		float current_dash_angle = dash_angle;
		
		if(self.ground() or ground)
			current_dash_angle = ground_angle * DEG2RAD;
		else if(self.roof())
			current_dash_angle = roof_angle * DEG2RAD;
		else if(wall_collision)
			current_dash_angle = wall_angle * DEG2RAD;
		
		// Dashing into a surface - change direction
		if(current_dash_angle != dash_angle)
		{
			float surface_x = cos(current_dash_angle);
			float surface_y = sin(current_dash_angle);
			project(cos(dash_angle), sin(dash_angle), surface_x, surface_y, surface_x, surface_y);
			
			// Don't change direction when dashing straight into a surface
			current_dash_angle = abs(surface_x) > EPSILON or abs(surface_y) > EPSILON
				? atan2(surface_y, surface_x)
				: dash_angle;
		}
		
		// Transfer the whole magnitude of your velocity when dashing into a surface
		if(self.ground() and !prev_ground or self.roof() and !prev_roof or wall_collision and !prev_wall)
		{
			project(vel_x, vel_y, cos(current_dash_angle), sin(current_dash_angle), vel_x, vel_y);
			normalize(vel_x, vel_y, vel_x, vel_y);
			vel_x *= dash_vel;
			vel_y *= dash_vel;
		}
		
		if(self.ground() or ground)
		{
			if(self.jump_intent() != 0 and (dash_angle % PI) != 0)
			{
				state = box_state_jump;
				jump_charge = false;
				return;
			}
			
			if(dash_fall)
			{
				state = box_state_stand;
				return;
			}
			
			if(!prev_ground)
			{
				jump_charge = true;
			}
			
			current_dash_speed = dash_ground_speed;
			dash_ground_timer++;
			dash_fall = false;
		}
		else if(prev_ground)
		{
			do_corner_boost();
			state = box_state_air;
			return;
		}
		
		// Wall jump
		if(wall_collision and self.jump_intent() != 0)
		{
			float jump_angle = (wall_angle - 90) * DEG2RAD;
			float wall_x = cos(jump_angle);
			float jump_x = self.x_intent() == 0 or self.x_intent() == sgn(wall_x) ? wall_jump_speed_x : wall_hop_speed_x;
			float jump_y = wall_jump_speed_y * -sgn(wall_x);
			rotate(jump_x, jump_y, jump_angle, jump_x, jump_y);
			vel_x = jump_x;
			vel_y = jump_y;
			dash_charge = true;
			
			spawn_fx(x - cos(jump_angle) * radius, y - sin(jump_angle) * radius, "dustworth", "dojump", 0, 15, wall_angle, 2, 1);
			state = box_state_air;
			return;
		}
		
		if(self.light_intent() != 0 and attack_cooldown == 0)
		{
			state = box_state_attack;
			return;
		}
		
		if(!dash_fall && state_timer > dash_duration + dash_cooldown + dash_pause)
		{
			state = box_state_air;
			return;
		}
		
		if(dash_fall and vel_y > current_dash_speed)
		{
			current_dash_speed = vel_y;
		}
		
		const float t = jump_charge and !ground or dash_fall
			? 1.0
			: max( 0, min(1, 1 - (state_timer - dash_duration + 1) / (dash_cooldown + 1)) );
		dash_vel = t * current_dash_speed;
		vel_x = cos(current_dash_angle) * dash_vel;
		vel_y = sin(current_dash_angle) * dash_vel;
		
		if(self.roof())
		{
			self.rotation(roof_angle);
		}
		else if(wall_collision)
		{
			self.rotation(wall_angle);
		}
		
		if(state_timer % 2 == 0)
		{
			BoxAfterImage@ fx = BoxAfterImage();
			fx.size = radius;
			fx.rotation = self.rotation() + this.rotation;
			scripttrigger@ st = create_scripttrigger(fx);
			fx.prev_x = self.prev_x();
			fx.prev_y = self.prev_y();
			st.set_xy(x, y);
			g.add_entity(st.as_entity(), false);
		}
		
	}
	
	void do_corner_boost()
	{
//		vel_y = 0;
		const float boost_factor = min(1,max(0, 1 - (dash_ground_timer - 1) / corner_boost_window));
		vel_x += vel_x * corner_boost * boost_factor;
		vel_y += vel_y * corner_boost * boost_factor;
		
		if(boost_factor >= 0.6 and abs(vel_x) >= dash_ground_speed)
		{
			CornerBoostFx@ fx = CornerBoostFx();
			fx.size = radius;
			fx.colour = boost_factor >= 0.8 ? 0x000000 : 0xFFFFFF;
			scripttrigger@ st = create_scripttrigger(fx);
			st.set_xy(x, y);
			g.add_entity(st.as_entity(), false);
			
			player_camera.add_screen_shake(x, y, dash_angle * RAD2DEG, dash_slam_shake);
		}
	}
	
	void state_attack_enter()
	{
		if(prev_state == box_state_dash)
		{
			const float deg = dash_angle * RAD2DEG - 45;
			if(deg % 90 == 0)
			{
				vel_x *= attack_diagonal_speed_multiplier;
			}
		}
		
		self.light_intent(0);
		ignore_gravity = true;
		vel_y = 0;
		rotation = prev_rotation = 0;
		buffer_dash = false;
		attack_kill = false;
		
		hitbox@ attack_hitbox = create_hitbox(@self.as_controllable(), 0,
			x, y, -attack_radius, attack_radius, -attack_radius, attack_radius);
		attack_hitbox.damage(attack_damage);
		attack_hitbox.aoe(true);
		attack_hitbox.attack_strength(600);
		g.add_entity(@attack_hitbox.as_entity(), false);
		
		AttackFx@ fx = AttackFx();
		fx.size = attack_radius;
		@fx.e = self.as_controllable();
		scripttrigger@ st = create_scripttrigger(fx);
		st.set_xy(x, y);
		g.add_entity(st.as_entity(), false);
	}
	void state_attack()
	{
		if(dash_charge and (self.dash_intent() != 0 or self.fall_intent() != 0))
		{
			buffer_dash = true;
		}
		
		if(self.jump_intent() != 0 and (self.ground() or ground or jump_charge))
		{
			state = box_state_jump;
			jump_charge = false;
			return;
		}
		
		if(state_timer > attack_duration)
		{
			attack_cooldown = attack_kill ? 0 : attack_cooldown_max;
			state = ground ? box_state_stand : box_state_air;
			return;
		}
	}
	
	// ---------------------------------------------------
	
	void step()
	{
		prev_rotation = (self.rotation() + this.rotation) * DEG2RAD;
		
		x = self.x();
		y = self.y();
		pos_dx = x - prev_x;
		pos_dy = y - prev_y;
		pos_vel_x = pos_dx / DT;
		pos_vel_y = pos_dy / DT;
		prev_x = x;
		prev_y = y;
		
		wall_collision = false;
		for(int i = -1; i <= 1; i += 2)
		{
			raycast@ ray = check_collisions(i, 0, i == -1 ? EDGE_RIGHT : EDGE_LEFT);
			
			if(ray.hit())
			{
				const float angle = (ray.angle() + 90) * DEG2RAD;
				const float wall_x = -cos(angle);
				const float wall_y = -sin(angle);
				const float dp = dot(vel_x, vel_y, wall_x, wall_y);
				if(dp <= 1)
				{
//					project(vel_x, vel_y, -wall_y, wall_x, vel_x, vel_y);
//					vel_x = 0;
					
					if(state == box_state_stand)
					{
						vel_y = 0;
					}
					
					wall_collision = true;
					wall_angle = ray.angle();
					check_tile(ray.tile_x(), ray.tile_y(), ray.tile_side());
					
					break;
				}
			}
		}
		
		if(self.roof())
		{
			raycast@ ray = check_collisions(0, -1, EDGE_BOTTOM);
			
			if(ray.hit())
			{
				float angle = ray.angle() * DEG2RAD;
				project(vel_x, vel_y, cos(angle), sin(angle), vel_x, vel_y);
				roof_angle = ray.angle();
				check_tile(ray.tile_x(), ray.tile_y(), ray.tile_side());
			}
		}
		
		
		raycast@ ground_ray = check_collisions(0, self.ground() ? 1.5 : 1, EDGE_TOP);
		ground = ground_ray.hit() or self.ground();
		if(ground)
		{
			ground_angle = ground_ray.angle();
			if(abs(ground_angle) == 27)
			{
				ground_angle = 26.5651 * sgn(ground_angle);
			}
			check_tile(ground_ray.tile_x(), ground_ray.tile_y(), ground_ray.tile_side());
		}
		
		// Clear dust
		if(ground or self.ground() or self.roof() or wall_collision)
		{
			g.project_tile_filth(
				x, y, 1, 1,
				0, 0,
				radius * 3, 360,
				true, true, true, true,
				false, true);
		}
		
		const BoxState start_state = state;
		
		if(state == box_state_stand)
			state_stand();
		else if(state == box_state_jump)
			state_jump();
		else if(state == box_state_air)
			state_air();
		else if(state == box_state_dash)
			state_dash();
		else if(state == box_state_attack)
			state_attack();
		
		if(start_state != state)
		{
			prev_state = start_state;
			ignore_gravity = false;
			state_timer = 0;
			
			if(state == box_state_stand)
				state_stand_enter();
			else if(state == box_state_jump)
				state_jump_enter();
			else if(state == box_state_air)
				state_air_enter();
			else if(state == box_state_dash)
				state_dash_enter();
			else if(state == box_state_attack)
				state_attack_enter();
		}
		else
		{
			state_timer++;
		}
		
		script.debug.print("pos_vel: <"+str(pos_vel_x)+", "+str(pos_vel_y)+">  " + str(magnitude(pos_vel_x, pos_vel_y)), 3);
		script.debug.print("vel: <"+str(vel_x)+", "+str(vel_y)+">  " + str(magnitude(vel_x, vel_y)), 2);
		script.debug.print("pos: <"+str(x)+", "+str(y)+">", 1);
		
		if(!self.ground() and !ignore_gravity)
		{
			vel_y += gravity * DT;
		}
		
		if(vel_y > terminal_velocity)
		{
			vel_y = terminal_velocity;
		}
		
		hit_box.rectangle(y - radius, y + radius, x - radius, x + radius);
		self.set_speed_xy(vel_x, vel_y);
		
		if(attack_cooldown > 0)
		{
			attack_cooldown--;
		}
		
		prev_ground = self.ground();
		prev_wall = wall_collision;
		prev_roof = self.roof();
		
		prev_x_intent = self.x_intent();
		prev_y_intent = self.y_intent();
		
		inside_rotation += inside_rotation_speed;
	}
	
	void draw(float sub_frame)
	{
		const float square_radius = sqrt(radius * radius + radius * radius);
		const float x = lerp(self.prev_x(), self.x(), sub_frame);
		const float y = lerp(self.prev_y(), self.y(), sub_frame);
		const float rotation = lerp(prev_rotation, (self.rotation() + this.rotation) * DEG2RAD, sub_frame);
		
//		draw_inside(x, y);
		draw_outside(x, y, rotation);
	}
	
	void draw_inside(float x, float y)
	{
		const float thickness = 3;
		
		g.draw_rectangle_world(18, 7,
			x - inside_dot_size, y - inside_dot_size,
			x + inside_dot_size, y + inside_dot_size, 0, 0xFFFFFFFF);
		
		for(int i = 0; i < 4; i++)
		{
			float t1 = (i - 1) / 4.0;
			float t2 = (i) / 4.0;
			float x1 = cos(t1 * PI2 + inside_rotation) * inside_size;
			float y1 = sin(t1 * PI2 + inside_rotation) * inside_size;
			float x2 = cos(t2 * PI2 + inside_rotation) * inside_size;
			float y2 = sin(t2 * PI2 + inside_rotation) * inside_size;
			
			g.draw_line(18, 7,
				x + x1, y + y1,
				x + x2, y + y2, thickness, 0xFFFFFFFF);
		}
	}
	
	void draw_outside(float x, float y, float rotation)
	{
		const float thickness = 5;
		const float r = radius - thickness / 2;
		
		array<float> corners = {
			-r, r, // foot l
			-r, -r, // head l
			+r, -r, // head r
			+r, r, // foot r
		};
		
		if(state == box_state_stand)
		{
			if(self.x_intent() != 0)
			{
				const float t = min(1, abs(run_vel) / run_speed);
				corners[2] -= run_skew_amount * self.x_intent() * t;
				corners[4] -= run_skew_amount * self.x_intent() * t;
			}
		}
		else if(state == box_state_jump)
		{
			const float t = min(1, state_timer / jump_startup);
			corners[0] -= radius * jump_feet_stretch_factor * t;
			corners[6] += radius * jump_feet_stretch_factor * t;
			corners[2] += radius * jump_head_pinch_factor * t;
			corners[4] -= radius * jump_head_pinch_factor * t;
			corners[3] += radius * (1 + jump_squash_factor) * t;
			corners[5] += radius * (1 + jump_squash_factor) * t;
		}
		
		uint colour = 0xFF000000;
		float outline = 3;
		for(int j = 0; j < 2; j++)
		{
			float x1 = corners[6];
			float y1 = corners[7];
			rotate(x1, y1, rotation, x1, y1);
			for(int i = 0; i < 8; i += 2)
			{
				float x2 = corners[i];
				float y2 = corners[i + 1];
				rotate(x2, y2, rotation, x2, y2);
				g.draw_line(18, 8,
					x + x1, y + y1,
					x + x2, y + y2, thickness + outline, colour);
				
				x1 = x2;
				y1 = y2;
			}
			
			colour += 0xFFFFFF;
			outline = 0;
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}