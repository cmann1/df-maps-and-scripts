#include '../../lib/enums/EmitterId.cpp';
#include '../../lib/emitters/common.cpp';

#include '../Vehicle.cpp';

namespace Fairy
{
	const float BaseMaxForce = 0.3;
	const float BaseMaxSpeed = 6;
	const float FleeMaxForce = 0.5;
	const float FleeMaxSpeed = 8;
	const float EmergeMaxForce = 0.05;
	const float EmergeMaxSpeed = 0.5;
	
	const float MinFlapSpeed = 4;
	const float MaxFlapSpeed = 6;
	const float MoveFlapSpeed = 9;
	const float FleeFlapSpeed = 12;
	const float DetectRadius = 100;
	
	const float IdleRangeMin = 20;
	const float IdleRangeMax = 78;
	
	const float WaitTimeMin = 0.9;
	const float WaitTimeMax = 1.9;
	const float HideTimeMin = 2;
	const float HideTimeMax = 6;
	
	const float PlayTimeMin = 1.5;
	const float PlayTimeMax = 3.0;
	const float PlayCircleRadiusMin = 30;
	const float PlayCircleRadiusMax = 60;
	const float PlayChaseMaxForce = 0.35;
	const float PlayChaseMaxSpeed = 6.5;
	const float PlayFleeMaxForce = 0.4;
	const float PlayFleeMaxSpeed = 7;
	
}

enum FairyState
{
	/// Dart around a fixed location
	Idle,
	/// Wait before going to the next idle position
	Wait,
	/// Wander to a new location
	Wander,
	/// Chasing another fairy
	PlayChase,
	/// Fleeing another fairy
	PlayFlee,
	/// Fly around in circle
	PlayCircle,
	/// Flee when the player gets too close
	Flee,
	/// Hiding from the player
	Hide,
}

class Fairy : Vehicle
{
	
	script@ script;
	canvas@ c;
	
	int index;
	FairyState prev_state = Idle;
	FairyState state = Idle;
	float state_timer;
	float state_counter;
	Fairy@ playmate;
	
	float target_x;
	float target_y;
	float base_x;
	float base_y;
	float wander_x;
	float wander_y;
	float target_ox;
	float target_oy;
	
	int layer;
	int sub_layer;
	float base_alpha;
	float scale;
	float alpha = 1;
	float prev_alpha = 1;
	float target_alpha = 1;
	float circle_radius;
	float circle_dir;
	
	float flap_speed;
	float flap;
	float flap_prev;
	
	float idle_base_x;
	float idle_base_y;
	float rotation = rand_range(-10, 30);
	float rotation_prev = rotation;
	float facing = 1;
	float facing_prev = 1;
	float target_facing = 1;
	float seed = rand_range(0.0, 999.0);
	float frequency = 1 / rand_range(20.0, 50.0);
	
	entity@ emitter;
	Sprite body_spr('props2', 'backdrops_4');
	Sprite wing_spr('props3', 'backdrops_4', 0.2, 1);
	Sprite wing_glow_spr('props3', 'backdrops_3', -0.05, 0.55);
	
	void init(script@ script,
		const int index,
		const int layer, const int sub_layer,
		const float alpha, const float scale,
		const float base_x, const float base_y, const float wander_x, const float wander_y)
	{
		@this.script = script;
		@c = create_canvas(false, layer, sub_layer);
		this.index = index;
		this.layer = layer;
		this.sub_layer = sub_layer;
		this.base_alpha = alpha;
		this.scale = rand_range(scale * 0.7, scale * 1);
		this.base_x = base_x;
		this.base_y = base_y;
		this.wander_x = wander_x;
		this.wander_y = wander_y;
		
		pick_random_pos(x, y);
		idle_base_x = x;
		idle_base_y = y;
		pick_idle_pos();
		prev_x = x;
		prev_y = y;
		
		const float start_vel = 4;
		vel_x = rand_range(-start_vel, start_vel);
		vel_y = rand_range(-start_vel, start_vel);
		
		arrive_dist = 70;
		in_sight_dist = 48;
		too_close_dist = 10;
		set_vehicle_speed();
		
		choose_flap_speed();
		choose_idle_time();
	}
	
	private void set_vehicle_speed()
	{
		switch(state)
		{
			case Flee:
				max_force = Fairy::FleeMaxForce;
				max_speed = Fairy::FleeMaxSpeed;
				break;
			case PlayChase:
				max_force = Fairy::PlayChaseMaxForce;
				max_speed = Fairy::PlayChaseMaxSpeed;
				break;
			case PlayFlee:
				max_force = Fairy::PlayFleeMaxForce;
				max_speed = Fairy::PlayFleeMaxSpeed;
				break;
			default:
				max_force = Fairy::BaseMaxForce;
				max_speed = Fairy::BaseMaxSpeed;
		}
	}
	
	void step(Fairies@ flock)
	{
		target_ox = sin((seed + script.frame) * 0.03) * 20;
		target_oy = sin((seed + script.frame) * 0.05) * 20;
		
		check_flee(flock);
		
		switch(state)
		{
			case FairyState::Idle: state_idle_wander(); break;
			case FairyState::Wander: state_idle_wander(); break;
			case FairyState::PlayChase: state_play_chase(); break;
			case FairyState::PlayFlee: state_play_flee(); break;
			case FairyState::PlayCircle: state_play_circle(); break;
			case FairyState::Wait: state_wait(flock); break;
			case FairyState::Flee: state_flee(flock); break;
			case FairyState::Hide: state_hide(flock); break;
		}
		
		state_timer -= DT;
		
		if(@emitter != null)
		{
			emitter.set_xy(x, y);
		}
		
		update();
		
		flap += flap_speed;
		
		rotation_prev = rotation;
		rotation = atan2(vel_y, vel_x);
		
		rotation = normalize_angle(rotation + 90 * DEG2RAD) * RAD2DEG;
		target_facing = rotation > 0 ? 1 : -1;
		const float range = 50;
		rotation = clamp(abs(rotation), 90 - range, 90 + range);
		rotation -= 90;
		
		if(facing != target_facing)
		{
			facing_prev = facing;
			facing += (target_facing - facing) * 0.15;
			if(abs(target_facing - facing) <= 0.05)
			{
				facing = target_facing;
			}
		}
		
		if(alpha != target_alpha)
		{
			prev_alpha = alpha;
			alpha += (target_alpha - alpha) * 0.15;
			if(abs(target_alpha - alpha) <= 0.001)
			{
				alpha = target_alpha;
			}
		}
	}
	
	private void check_flee(Fairies@ flock)
	{
		if(state == Flee || state == Hide)
			return;
		if(flock.hide_pos.length == 0)
			return;
		
		controllable@ c = get_flee_player(flock);
		
		if(@c == null)
			return;
		
		float closest_dist_sqr = 9999999;
		Location@ closest_pos = null;
		for(uint i = 0; i < flock.hide_pos.length; i++)
		{
			Location@ pos = flock.hide_pos[i];
			const float d = dist_sqr(pos.x, pos.y, x, y) + rand_range(-(110 * 110), 30 * 30);
			if(d < closest_dist_sqr)
			{
				closest_dist_sqr = d;
				@closest_pos = @pos;
			}
		}
		
		target_x = closest_pos.x;
		target_y = closest_pos.y;
		
		state = Flee;
		set_vehicle_speed();
		flap_speed = DT * Fairy::FleeFlapSpeed;
		
		if(@emitter != null)
		{
			script.g.remove_entity(emitter);
			@emitter = null;
		}
		
		if(@playmate != null)
		{
			playmate.set_vehicle_speed();
			playmate.wander();
			@playmate.playmate = null;
			@playmate = null;
		}
	}
	
	controllable@ get_flee_player(Fairies@ flock, const bool check_all=false)
	{
		for(uint i = 0; i < flock.players.length; i++)
		{
			controllable@ c = flock.players[i];
			
			if(check_all)
				return c;
			
			rectangle@ r = c.base_rectangle();
			const float cx = c.x() + r.left() + r.width * 0.5;
			const float cy = c.y() + r.top() + r.height * 0.5;
			const float d = dist_sqr(x, y, cx, cy);
			
			if(d > Fairy::DetectRadius * Fairy::DetectRadius)
				continue;
			
			return c;
		}
		
		return null;
	}
	
	private void state_idle_wander()
	{
		try_create_emitter();
		
		if(arrived())
		{
			wait();
			set_vehicle_speed();
			choose_flap_speed();
		}
	}
	
	private void state_play_chase()
	{
		seek(
			playmate.x + target_ox,
			playmate.y + target_oy);
		//draw_line(script.g, layer, 24, x, y, playmate.x, playmate.y, 1, 0x55ff0000);
	}
	
	private void state_play_flee()
	{
		target_x = base_x + cos((seed + script.frame) * 0.03) * wander_x;
		target_y = base_y + sin((seed + script.frame) * 0.05) * wander_y;
		//draw_line(script.g, layer, 24, x, y, target_x, target_y, 1, 0x55ffff00);
		seek(
			target_x + target_ox,
			target_y + target_oy);
		
		if(state_timer <= 0)
		{
			state_counter++;
			
			if(state_counter < 2 || frand() < 0.4)
			{
				state_timer = rand_range(Fairy::PlayTimeMin, Fairy::PlayTimeMax);
				state = frand() < 0.5 ? PlayCircle : PlayFlee;
				
				if(state == PlayCircle)
				{
					circle_radius = rand_range(Fairy::PlayCircleRadiusMin, Fairy::PlayCircleRadiusMax);
					circle_dir = frand() < 0.5 ? -1 : 1;
					const float t = (seed + script.frame) * 0.06;
					idle_base_x = x + cos(t) * circle_radius * circle_dir;
					idle_base_y = y + sin(t) * circle_radius * circle_dir;
				}
			}
			else
			{
				playmate.wander();
				wander();
				set_vehicle_speed();
				playmate.set_vehicle_speed();
				@playmate.playmate = null;
				@playmate = null;
			}
		}
	}
	
	private void state_play_circle()
	{
		const float t = (seed + script.frame) * 0.06;
		target_x = idle_base_x - cos(t) * circle_radius * circle_dir;
		target_y = idle_base_y - sin(t) * circle_radius * circle_dir;
		//draw_line(script.g, layer, 24, x, y, target_x, target_y, 1, 0x55ffff00);
		//draw_dot(script.g, layer, 24, idle_base_x, idle_base_y, 3, 0xffffff00, 45);
		seek(target_x, target_y);
		
		if(state_timer <= 0)
		{
			state_counter++;
			state = PlayFlee;
			state_timer = rand_range(Fairy::PlayTimeMin, Fairy::PlayTimeMax);
		}
	}
	
	private void state_wait(Fairies@ flock)
	{
		try_create_emitter();
				
		arrived();
		
		if(state_timer > 0)
			return;
		
		switch(prev_state)
		{
			case FairyState::Idle: {
				// Play chase
				if(flock.playful && state_counter > 2 && frand() < 0.075)
				{
					find_playmate(flock);
					
					if(@playmate != null)
					{
						state = PlayChase;
						@playmate.playmate = this;
						playmate.state = PlayFlee;
						flap_speed = DT * Fairy::MoveFlapSpeed;
						playmate.flap_speed = DT * Fairy::MoveFlapSpeed;
						playmate.state_timer = rand_range(Fairy::PlayTimeMin, Fairy::PlayTimeMax);
						playmate.state_counter = 0;
						
						set_vehicle_speed();
						playmate.set_vehicle_speed();
					}
				}
				
				if(state != PlayChase)
				{
					// Wander
					if(state_counter > 4 && frand() > 0.75)
					{
						wander();
					}
					else
					{
						state = prev_state;
						pick_idle_pos();
						choose_flap_speed();
					}
				}
			} break;
			case FairyState::Wander: {
				state = Idle;
				idle_base_x = target_x;
				idle_base_y = target_y;
				pick_idle_pos();
				choose_flap_speed();
				state_counter = 0;
			} break;
		}
	}
	
	private void state_flee(Fairies@ flock)
	{
		float t = distance(x, y, target_x, target_y);
		if(arrived(clamp01(t / 100) * 1.5))
		{
			state = Hide;
			state_timer = rand_range(Fairy::HideTimeMin, Fairy::HideTimeMax);
			target_alpha = 0;
		}
	}
	
	private void state_hide(Fairies@ flock)
	{
		vel_x *= 0.85;
		vel_y *= 0.85;
		
		if(state_timer > 0)
			return;
		
		if(@get_flee_player(flock, true) == null)
		{
			wander();
			max_force = Fairy::EmergeMaxForce;
			max_speed = Fairy::EmergeMaxSpeed;
			
			target_alpha = 1;
		}
		else
		{
			state_timer = rand_range(Fairy::HideTimeMin, Fairy::HideTimeMax);
		}
	}
	
	void wander()
	{
		state = Wander;
		state_counter = 0;
		pick_random_pos(target_x, target_y);
		flap_speed = DT * Fairy::MoveFlapSpeed;
	}
	
	void draw(const float sub_frame)
	{
		const float rotation = lerp_angle_degrees(rotation_prev, this.rotation, sub_frame);
		const float x = lerp(prev_x, this.x, sub_frame);
		const float y = lerp(prev_y, this.y, sub_frame);
		//draw_line(script.g, layer, sub_layer, x, y, target_x, target_y, 1, 0x55ffff00);
		//draw_dot(script.g, layer, sub_layer, target_x, target_y, 3, 0xFFFFFF00, 0);
		
		const uint clr = rgba(1.0, 1.0, 1.0, base_alpha * lerp(prev_alpha, alpha, sub_frame));
		
		c.push();
		c.translate(x, y);
		
		c.push();
		c.scale(lerp(facing_prev, facing, sub_frame), 1);
		const float flap_1 = flap_value(flap, sub_frame);
		draw_wing(layer, sub_layer, 1.0 * scale, rotation, flap_1, 1, clr, sub_frame);
		draw_wing(layer, sub_layer, 0.7 * scale, rotation - 110, flap_1, -1, clr, sub_frame);
		const float flap_2 = flap_value(flap + 0.2, sub_frame);
		draw_wing(layer, sub_layer, 0.8, rotation + 30, flap_2, 1, clr, sub_frame);
		draw_wing(layer, sub_layer, 0.7 * 0.8, rotation - 30 - 110, flap_2, -1, clr, sub_frame);
		c.pop();
		
		// Body outer
		body_spr.draw(layer, sub_layer, 0, 0, x, y, 0, 0.1959 * scale, 0.1959 * scale, clr);
		// Inner body halo
		const float pulse = map(sin(seed + script.frame * frequency * 4), -1, 1, 1, 1.5);
		body_spr.draw(layer, sub_layer, 0, 0, x, y, 0, 0.1202 * scale * pulse, 0.1202 * scale * pulse, clr);
		
		const float co = cos(rotation * DEG2RAD * facing) * facing;
		const float ss = sin(rotation * DEG2RAD * facing) * facing;
		//draw_line(script.g, layer, sub_layer, x, y, x+co * 20, y+ss*20, 1, 0xffff0000);
		
		c.pop();
	}
	
	private void find_playmate(Fairies@ flock)
	{
		@playmate = null;
		
		array<int> candidates(flock.fairies.length);
		int num_candidates = 0;
		for(uint i = 0; i < flock.fairies.length; i++)
		{
			if(@flock.fairies[i] != @this)
			{
				candidates[i] = i;
				num_candidates++;
			}
		}
		
		@playmate = null;
		while(@playmate == null && num_candidates > 0)
		{
			const int index = rand_range(0, num_candidates);
			@playmate = @flock.fairies[candidates[index]];

			if(playmate.state != Idle && playmate.state != Wander && playmate.state != Wait)
			{
				@playmate = null;
				candidates[index] = candidates[--num_candidates];
			}
			else
			{
				break;
			}
		}
	}
	
	private float flap_value(const float flap, const float sub_frame)
	{
		return abs(lerp(flap - flap_speed, flap, sub_frame) % 1.0 - 0.5) * 2;
	}
	
	private void choose_flap_speed()
	{
		flap_speed = DT * rand_range(Fairy::MinFlapSpeed, Fairy::MaxFlapSpeed);
	}
	
	private void choose_idle_time()
	{
		state_timer = rand_range(2.0, 5.0);
	}
	
	private void draw_wing(
		const int layer, const int sub_layer,
		const float scale, float rotation,
		const float flap, const float dir,
		const uint clr, const float sub_frame)
	{
		const float qf = map(flap, 0, 1, 0.75, 1);
		const float hf = map(flap, 0, 1, 0.25, 1);
		
		rotation -= flap * 25 * dir;
		wing_spr.draw(c, 0, 0, 0, 0,
			rotation - 32,
			0.1959 * scale * flap, 0.1959 * scale * flap, clr);
		wing_spr.draw(c, 0, 0, 0, 0,
			rotation - 22,
			0.1959 * scale * flap, 0.1959 * scale * flap, clr);
		wing_glow_spr.draw(c, 0, 0, 0, 0,
			rotation - 90 - 20,
			0.0532 * scale, 0.0532 * scale * hf, clr);
	}
	
	private void pick_random_pos(float &out x, float &out y)
	{
		x = base_x + rand_range(-wander_x, wander_x);
		y = base_y + rand_range(-wander_y, wander_y);
	}
	
	private void pick_idle_pos()
	{
		const float dist = rand_range(Fairy::IdleRangeMin, Fairy::IdleRangeMax);
		const float angle = rand_range(-PI, PI);
		target_x = idle_base_x + cos(angle) * dist;
		// * 0.5 - flatting it out a bit so the fairy tends to move more horizontally
		target_y = idle_base_y + sin(angle) * dist * 0.5;
	}
	
	private void wait()
	{
		prev_state = state;
		state = Wait;
		state_timer = rand_range(Fairy::WaitTimeMin, Fairy::WaitTimeMin);
		state_counter++;
	}
	
	private void try_create_emitter()
	{
		if(@emitter != null)
			return;
		
		@emitter = create_emitter(
			EmitterId::Butler, x, y,
			10, 10, layer, sub_layer, 0);
		script.g.add_entity(emitter, false);
	}
	
	private bool arrived(const float offset_factor=1)
	{
		return arrive(
			target_x + target_ox * offset_factor,
			target_y + target_oy * offset_factor) < 1;
	}
	
}
