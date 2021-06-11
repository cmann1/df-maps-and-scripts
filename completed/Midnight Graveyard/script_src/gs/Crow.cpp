class Crow : trigger_base
{
	
	script@ script;
	scene@ g;
	scripttrigger@ self;
	
	sprites@ spr;
	
	float t = 0;
	
	audio@ caw_sound = null;
	audio@ flapping_sound = null;
	
	[text] float anim_speed = 0.5;
	[text] float speed_xmin = 20;
	[text] float speed_xmax = 80;
	[text] float speed_ymin = 150;
	[text] float speed_ymax = 300;
	[text] int layer = 17;
	[text] int sub_layer = 19;
	[text] bool face_right = true;
	[text] int fly_direction = 0;
	
	[text] float sound_dist_min = -1;
	[text] float sound_dist_max = -1;
	
	[hidden] bool flying = false;
	[hidden] float speed_x = 0;
	[hidden] float speed_y = 0;
	[hidden] float vel_x = 0;
	[hidden] float vel_y = 0;
	[hidden] float prev_x = 0;
	[hidden] float prev_y = 0;
	
	[text] int caw_interval_max = 800;
	int caw_interval;
	int caw_count = -1;
	
	Crow()
	{
		@g = get_scene();
		@spr = create_sprites();
	}
	
	void update_caw_interval()
	{
		caw_interval = int( caw_interval_max + caw_interval_max * (frand() - 0.5) );
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		spr.add_sprite_set("script");
		prev_x = self.x();
		prev_x = self.y();
		self.face(face_right ? 1 : -1);
		t = rand() % 1000;
		
		caw_interval = rand() % (caw_interval_max * 2);
		
		self.editor_handle_size(4);
		self.editor_colour_inactive(0x88818181);
		self.editor_colour_active(0x88afafaf);
		self.editor_colour_circle(0x55818181);
	}
	
	void activate(controllable@ e)
	{
		if(flying) return;
		
		for(uint i = 0; i < num_cameras(); i++)
		{
			entity@ player = controller_entity(i);
			if(@player != null and e.is_same(player))
			{
				speed_x = (speed_xmin + (speed_xmax - speed_xmin) * frand()) * (frand() > 0.5 ? 1 : -1);
				if(fly_direction != 0) speed_x = abs(speed_x) * sgn(fly_direction);
				speed_y = -(speed_ymin + (speed_ymax - speed_ymin) * frand());
				flying = true;
				if(frand() >= 0.9)
				{
					@caw_sound = g.play_script_stream("crow_fade", 2, self.x(), self.y(), false, 1.0);
					caw_sound.positional(true);
				}
				else
				{
					caw_interval_max = 20 + (rand() % 30);
					update_caw_interval();
					caw_count = 1 + rand() % 2;
				}
				@flapping_sound = g.play_script_stream("bird_flapping_0" + (rand() % 2 + 1), 2, self.x(), self.y(), false, 0.65);
				flapping_sound.positional(true);
				return;
			}
		}
	}
	
	void step()
	{
		prev_x = self.x();
		prev_y = self.y();
		
		t++;
		
		if(flying)
		{
			vel_x += (speed_x - vel_x) * 0.75 * DT;
			vel_y += (speed_y - vel_y) * 0.75 * DT;
			
			self.x(self.x() + vel_x * DT);
			self.y(self.y() + vel_y * DT);
			self.face(int(sgn(vel_x)));
		}
		
		if((caw_count == -1 or caw_count > 0))
		{
			if((@caw_sound == null or !caw_sound.is_playing()) and caw_interval-- <= 0)
			{
				const string s = "crow_0" + (rand() % 5 + 1);
				@caw_sound = g.play_script_stream(s, 2, self.x(), self.y(), false, 0.85);
				caw_sound.positional(true);
				update_caw_interval();
				
				if(caw_count > 0) caw_count--;
			}
		}
		
		if(@caw_sound != null)
		{
			if(!caw_sound.is_playing()) @caw_sound = null;
			else caw_sound.set_position(self.x(), self.y());
		}
		
		if(@flapping_sound != null)
		{
			if(!flapping_sound.is_playing()) @flapping_sound = null;
			else flapping_sound.set_position(self.x(), self.y());
		}
	}
	
	void draw(float sub_frame)
	{
		const float x = flying ? lerp(prev_x, self.x(), sub_frame) : self.x();
		const float y = flying ? lerp(prev_y, self.y(), sub_frame) : self.y();
		const int frame = 0;
		const int palette = 0;
		const float s = sin(t * anim_speed);
		const float v = sgn(s);
		spr.draw_world(layer, sub_layer, 
			!flying ? "crow_01" : (v < 0 ? "crow_02" : "crow_03"),
			frame, palette, x, flying ? y + sin(t * anim_speed * 0.2) * 4 : y, 0, -self.face(), 1, 0xFFFFFFFF);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}