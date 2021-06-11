class Pumpkin : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	[text] int layer = 17;
	[text] int sub_layer = 19;
	[angle] float rotation = 0;
	[text] float scale_x = 1;
	[text] float scale_y = 1;
	
	sprites@ spr;
	
	Pumpkin()
	{
		@g = get_scene();
		@spr = create_sprites();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		spr.add_sprite_set("script");
	}
	
	void draw(float sub_frame)
	{
		const int frame = 0;
		const int palette = 0;
		spr.draw_world(layer, sub_layer, "pumpkin2", frame, palette, self.x(), self.y(), rotation, scale_x, scale_y, 0xFFFFFFFF);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}


















