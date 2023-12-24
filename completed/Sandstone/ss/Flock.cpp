#include 'settings/FlockSettings.cpp';
#include 'Bird.cpp';

class Flock
{
	
	script@ script;
	
	float x;
	float y;
	float size_x = 90;
	float size_y1 = 100;
	float size_y2 = 90;
	
	array<Bird> birds;
	array<Vehicle@> vehicles;
	int bird_count;
	
	int targeted_count;
	int completed_count;
	Player@ player;
	float target_timer = 0;
	
	float bounds_x1, bounds_y1, bounds_x2, bounds_y2;
	
	Flock()
	{
	}
	
	void add_birds(const int count)
	{
		birds.resize(bird_count + count);
		vehicles.resize(bird_count + count);
		
		for(int i = 0; i < count; i++)
		{
			Bird@ bird = @birds[bird_count];
			float bx, by;
			get_target_xy(bird_count, bx, by);
			bird.init(script, bx, by);
			bird.index = bird_count;
			@vehicles[bird_count++] = @bird.vehicle;
		}
	}
	
	void give_scarf(Player@ player)
	{
		if(@this.player != null)
			return;
		
		targeted_count = 0;
		completed_count = 0;
		target_timer = Flock::TargetPeriod;
		@this.player = player;
	}
	
	void step()
	{
		if(@player != null && targeted_count < bird_count)
		{
			if(target_timer >= Flock::TargetPeriod)
			{
				Bird@ bird = @birds[targeted_count++];
				
				if(@bird.target == null)
				{
					@bird.target = player;
					bird.state = BirdState::GiveScarf;
					bird.anticipate = true;
					const float dx = bird.vehicle.x - player.x;
					// A point between the player and the bird
					bird.anticipate_x = (dx < 0 ? min(dx, -48.0) : max(dx, 48.0)) * rand_range(0.5, 1.0);
					// A point above both the bird and the player
					bird.anticipate_y = min(bird.vehicle.y - player.y, -player.rect_h) - rand_range(50.0, 70.0);
					bird.vehicle.max_speed = Flock::TargetMaxSpeed;
					bird.vehicle.max_force = Flock::TargetMaxForce;
					bird.is_default = false;
				}
				
				target_timer -= Flock::TargetPeriod;
			}
			
			target_timer += DT;
		}
		
		bounds_x1 = bounds_y2 = MAX_FLOAT;
		bounds_x2 = bounds_y2 = MIN_FLOAT;
		
		for(int i = 0; i < bird_count; i++)
		{
			Bird@ bird = @birds[i];
			
			switch(bird.state)
			{
				case BirdState::Idle: do_bird_idle_state(bird, i); break;
				case BirdState::GiveScarf: do_bird_give_scarf_state(bird, i); break;
				case BirdState::JumpPause: do_bird_jump_pause_state(bird, i); break;
				case BirdState::JumpAnticipate: do_bird_jump_anticipate_state(bird, i); break;
				case BirdState::Jump: do_bird_jump_state(bird, i); break;
			}
			
			bird.vehicle.arrive(bird.target_x, bird.target_y);
			bird.step();
			
			if(@bird.target == null && !bird.is_default)
			{
				bird.reset_vehicle();
			}
			
			if(bird.vehicle.x < bounds_x1)
				bounds_x1 = bird.vehicle.x;
			if(bird.vehicle.x > bounds_x2)
				bounds_x2 = bird.vehicle.x;
			if(bird.vehicle.y < bounds_y1)
				bounds_y1 = bird.vehicle.y;
			if(bird.vehicle.y > bounds_y2)
				bounds_y2 = bird.vehicle.y;
		}
		
		if(@player != null && completed_count == targeted_count)
		{
			@player = null;
		}
	}
	
	void draw(const float sub_frame)
	{
		for(int i = 0; i < bird_count; i++)
		{
			birds[i].draw(script.g, sub_frame);
		}
	}
	
	private void do_bird_idle_state(Bird@ bird, const int i)
	{
		get_target_xy(i, bird.target_x, bird.target_y);
	}
	
	private void do_bird_give_scarf_state(Bird@ bird, const int i)
	{
		if(@bird.target != null)
		{
			if(bird.anticipate)
			{
				bird.target_x = bird.target.x + bird.anticipate_x;
				bird.target_y = bird.target.y + bird.anticipate_y;
			}
			else
			{
				bird.target_x = bird.target.head_x;
				bird.target_y = bird.target.head_y;
			}
		}
		else
		{
			get_target_xy(i, bird.target_x, bird.target_y);
		}
		
		do_bird_give_scarf_post_state(bird, i);
	}
	
	private void do_bird_give_scarf_post_state(Bird@ bird, const int i)
	{
		if(@bird.target == null)
			return;
		if(!bird.arrived())
			return;
		
		if(bird.anticipate)
		{
			bird.anticipate = false;
			return;
		}
		
		audio@ a = script.g.play_sound(
			'sfx_hill_lost',
			bird.vehicle.x, bird.vehicle.y,
			rand_range(0.25, 0.4), false, true);
		a.time_scale(rand_range(0.7, 1.0));
		
		completed_count++;
		@bird.target = null;
		bird.state = BirdState::Idle;
		
		script.add_emitter_burst(
			Player::ScarfGetEmitterId,
			Flock::Layer, Flock::GlowSubLayer,
			bird.vehicle.x, bird.vehicle.y,
			20, 20, 0, 0.25, 2);
	}
	
	private void do_bird_jump_pause_state(Bird@ bird, const int i)
	{
		if(bird.anticipate)
		{
			get_target_xy(i, bird.target_x, bird.target_y);
			
			if(bird.glow >= 0)
			{
				bird.anticipate = false;
			}
			
			return;
		}
		
		bird.vehicle.time_scale = clamp01(bird.vehicle.time_scale - DT / Flock::JumpPauseTimeSeconds);
		
		if(bird.vehicle.time_scale <= 0)
		{
			bird.state = BirdState::JumpAnticipate;
			bird.vehicle.time_scale = 1;
			bird.vehicle.max_speed = Flock::TargetMaxSpeed;
			bird.vehicle.max_force = Flock::TargetMaxForce;
			bird.is_default = false;
		}
	}
	
	private void do_bird_jump_anticipate_state(Bird@ bird, const int i)
	{
		bird.target_x = bird.target.cx + cos(i * 10) * 5;
		bird.target_y = bird.target.cy + sin(i * 3.53) * 5;
		
		if(bird.arrived())
		{
			bird.state = BirdState::Jump;
			bird.vehicle.max_speed = Flock::JumpMaxSpeed;
			bird.vehicle.max_force = Flock::JumpMaxForce;
		}
	}
	
	private void do_bird_jump_state(Bird@ bird, const int i)
	{
		bird.target_x = bird.target.cx + cos(script.frame * 0.1 + i * 2) * 96;
		bird.target_y = bird.target.cy + sin(script.frame * 0.07 + i * 3.53) * 48;
	}
	
	private void get_target_xy(const int i, float &out x, float &out y)
	{
		x = this.x + sin((script.frame) * 0.02 + i * 2) * size_x;
		y = this.y + (cos((script.frame) * 0.03 + i * 5) * 0.5 + 0.5) * size_y2 - (sin(i + script.frame * 0.005) * 0.5 + 0.5) * size_y1;
	}
	
}
