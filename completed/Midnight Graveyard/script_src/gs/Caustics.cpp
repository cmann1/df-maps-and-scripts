class Caustics : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	sprites@ spr;
	
	float t = 0;
	
	[text] float anim_speed = 1.5;
	[text] int repeat = 1;
	[text] int layer = 16;
	[text] int sub_layer = 19;
	[text] float scale = 2;
	[text] bool large = false;
	
	string prefix = "";
	
	Caustics()
	{
		@g = get_scene();
		@spr = create_sprites();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		spr.add_sprite_set("script");
		t = rand() % 1000;
		
		prefix = "caustics" + (large ? "" : "_h") + "_0";
	}
	
	void step()
	{
		t += anim_speed * DT;
	}
	
	void editor_step()
	{
		step();
	}
	
	void draw(float sub_frame)
	{
		const float x = self.x();
		const float y = self.y();
		const int frame = 0;
		const int palette = 0;
		const string sprite_name = prefix + (int(t) % 3 + 1);
		
		for(int i = 0; i < repeat; i++)
		{
			spr.draw_world(layer, sub_layer, 
				sprite_name,
				frame, palette, x + i * 205 * scale, y, 0, scale, scale, 0xFFFFFFFF);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}


















