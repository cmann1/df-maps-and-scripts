class SacrificeRumble : trigger_base, callback_base
{
	
	scene@ g;
	script@ script;
	scripttrigger@ self;
	
	[text] string portal = "default";
	[text] int apple_count= 5;
	[angle] float dir = 0;
	[text] float force = 10;
	[text] float final_force = 20;
	[text] int duration = 60;
	[text] int final_duration = 120;
	[text] float shake_interval = 10;
	
	[entity] int apple_id;
	[hidden] bool can_spawn_apple = true;
	
	bool complete = false;
	bool active = false;
	float shake_timer = 0;
	int timer = 0;
	
	float x = 0;
	float y = 0;
	
	SacrificeRumble()
	{
		@g = get_scene();
		add_broadcast_receiver("portal_apple_count", this, "portal_apple_count");
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = @script;
		@this.self = @self;
	}
	
	void portal_apple_count(string id, message@ msg)
	{
		if(msg.get_int("inc") < 0 or msg.get_string("portal") != portal) return;
		
		x = msg.get_float("x");
		y = msg.get_float("y");
		
		active = true;
		timer = duration;
		play_script_stream(g, "rumble", 2, x, y, false, 1.5, true);
		
		if(msg.get_int("count") == apple_count and can_spawn_apple)
		{
			entity@ apple = entity_by_id(apple_id);
			if(@apple != null)
			{
				apple.set_xy(self.x(), self.y());
			}
			complete = true;
			can_spawn_apple = false;
			timer = final_duration;
			
			play_script_stream(g, "rumble", 2, x, y, false, 1.5, true);
		}
		else 
		{
			complete = false;
		}
	}
	
	void step()
	{
		if(active)
		{
			if(shake_timer == 0)
			{
				// quintic
//				const float b = 0.5;
//				const float c = 0.5;
//				const float d = -0.5;
//				const float x = float(timer) / duration;
//				const float t = abs(x - 0.5) / d;
//				const float f = (c*((t-1)*t*t*t*t+1)+b) * force;
				// cubic
				const float b = 0.9;
				const float c = 0.1;
				const float d = -0.27;
				const float x = float(timer) / (complete ? final_duration : duration);
				const float t = abs(x - 0.5) / d;
				const float f = (c*((t-1)*t*t+1)+b) * (complete ? final_force : force);
				
				get_active_camera().add_screen_shake(this.x, this.y, rand() % 360, f);
			}
			
			if(++shake_timer >= shake_interval)
			{
				shake_timer = 0;
			}
			
			if(timer-- < 0)
			{
				active = false;
//				get_scene().remove_entity(self.as_entity());
			}
		}
	}
	
}