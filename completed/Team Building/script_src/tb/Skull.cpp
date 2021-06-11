class Skull : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	sprites@ spr;
	
	[text] int layer = 12;
	[text] int sub_layer = 19;
	[angle] int rotation = 0;
	[text] float scale_x = 1;
	[text] float scale_y = 1;
	
	Skull()
	{
		@g = get_scene();
		@spr = create_sprites();
		spr.add_sprite_set("script");
		spr.add_sprite_set("editor");
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.self = @self;
		self.editor_handle_size(4);
	}
	
	void draw(float sub_frame)
	{
		spr.draw_world(layer, sub_layer, "skull", 0, 0, self.x(), self.y(), rotation, scale_x, scale_y, 0xFFFFFFFF);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}