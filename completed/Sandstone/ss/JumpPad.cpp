#include '../lib/enums/EntityState.cpp';

class JumpPad : trigger_base, LifecycleEntity
{
	
	[persist] float size_x = 90;
	[persist] float size_y1 = 40;
	[persist] float size_y2 = 120;
	[persist] float accel = 100;
	[persist] float time = 1;
	[entity,kill_zone] uint death_zone;
	
	script@ script;
	scripttrigger@ self;
	
	Flock flock;
	Player@ player;
	bool jump_active = false;
	float power;
	float timer;
	
	string get_identifier() const override { return 'JumpPad' + self.id(); }
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.self = self;
		@this.script = script;
		trigger_view_events = true;
		
		self.radius(0);
		
		if(!script.is_playing)
			return;
		
		@flock.script = script;
		flock.x = self.x();
		flock.y = self.y();	
		flock.size_x = size_x;
		flock.size_y1 = size_y1;
		flock.size_y2 = size_y2;
		
		flock.add_birds(round_int(map(sin(self.x() * 12345 + self.y() * 8323), -1, 1, 10, 16)));
	}
	
	void step()
	{
		const float x = self.x();
		const float y = self.y();
		
		init_lifecycle();
		
		if(_in_view)
		{
			flock.step();
			bounds_x1 = flock.bounds_x1 - x;
			bounds_y1 = flock.bounds_y1 - y;
			bounds_x2 = flock.bounds_x2 - x;
			bounds_y2 = flock.bounds_y2 - y;
		}
		
		do_jump();
	}
	
	private void do_jump()
	{
		if(@player == null)
			return;
		
		if(!jump_active)
		{
			if(player.player.ground())
			{
				player.player.jump_intent(1);
				if(player.player.state() == EntityState::CrouchJump)
				{
					if(player.player.state_timer() >= 2.5)
					{
						player.player.state_timer(2.5);
					}
				}
			}
			
			int active_count = 0;
			for(int i = 0; i < flock.bird_count; i++)
			{
				Bird@ bird = @flock.birds[i];
				if(bird.state == BirdState::Jump)
				{
					active_count++;
				}
			}
			
			if(active_count >= flock.bird_count / 2 && player.player.ground())
			{
				jump_active = true;
				player.lock(false);
				
				for(int i = 0; i < flock.bird_count; i++)
				{
					Bird@ bird = @flock.birds[i];
					bird.state = BirdState::Jump;
					bird.vehicle.max_speed = Flock::JumpMaxSpeed;
					bird.vehicle.max_force = Flock::JumpMaxForce;
				}
			}
		}
		else
		{
			float final_accel = accel;
			
			if(@player.dm != null)
			{
				final_accel -= (3456 - player.dm.fall_accel()) * 2.5 * DT;
			}
			
			player.player.set_speed_xy(
				player.player.x_speed(),
				player.player.y_speed() - final_accel * power);
			
			if(!player.player.ground())
			{
				timer -= DT;
				if(timer <= 0)
				{
					jump_active = false;
					@player = null;
					
					for(int i = 0; i < flock.bird_count; i++)
					{
						Bird@ bird = @flock.birds[i];
						bird.do_glow = false;
						bird.glow = 0;
						bird.state = BirdState::Idle;
						@bird.target = null;
					}
				}
			}
		}
	}
	
	void draw(float sub_frame)
	{
		if(!_in_view && script.is_playing)
			return;
		
		flock.draw(sub_frame);
	}
	
	bool activate_jump(Player@ player, const float radius, const float power)
	{
		if(@this.player != null)
			return false;
		
		int activated_count = 0;
		for(int i = 0; i < flock.bird_count; i++)
		{
			Bird@ bird = @flock.birds[i];
			const float bird_distance = distance(player.cx, player.cy, bird.vehicle.x, bird.vehicle.y);
			if(bird_distance > radius)
				continue;
			
			bird.do_glow = true;
			bird.glow = -bird_distance / (48 * 20);
			bird.anticipate_timer = -bird.glow;
			activated_count++;
		}
		
		if(activated_count < flock.bird_count)
			return false;
		
		for(int i = 0; i < flock.bird_count; i++)
		{
			Bird@ bird = @flock.birds[i];
			bird.state = BirdState::JumpPause;
			bird.anticipate = true;
			@bird.target = player;
		}
		
		
		if(power >= 1)
		{
			entity@ e = entity_by_id(death_zone);
			if(@e != null)
			{
				script.g.remove_entity(e);
			}
		}
		
		jump_active = false;
		@this.player = player;
		player.give_super();
		this.power = power;
		timer = time;
		return true;
	}
	
}
