class EasterEgg : enemy_base
{
	
	scene@ g;
	script@ script;
	scriptenemy@ self;
	sprites@ spr;
	
	string sprite_name;
	bool needs_init = true;
	float scale;
	
	EasterEgg(const float scale_offset = 0)
	{
		@g = get_scene();
		this.scale = clamp(rand_range(0.5, 0.75) + scale_offset, 0.5, 0.75);
	}
	
	void init(script@ s, scriptenemy@ self)
	{
		@this.self = self;
		@this.script = s;
		
		const float l = -24 * scale;
		const float r =  23 * scale;
		const float t = -30 * scale;
		const float b =  30 * scale;
		
		self.base_rectangle(t, b, l, r);
		self.hit_rectangle(t, b, l, r);
		
		self.auto_physics(true);
		self.scale(scale);
		
		@spr = create_sprites();
		spr.add_sprite_set('script');
		sprite_name = 'egg_' + (rand_range(1, 5));
	}
	
	void step()
	{
		if(needs_init)
		{
			self.hit_collision().remove();
			needs_init = false;
		}
		
		self.set_speed_xy(self.x_speed(), self.y_speed() + 1000 * DT);
	}
	
	void draw(float sub_frame)
	{
		spr.draw_world(18, 2, sprite_name, 0, 0,
			lerp(self.prev_x(), self.x(), sub_frame),
			lerp(self.prev_y(), self.y(), sub_frame),
			0, scale, scale, 0xffffffff);
	}
	
}
