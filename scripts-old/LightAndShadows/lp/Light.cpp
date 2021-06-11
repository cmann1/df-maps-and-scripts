class Light : trigger_base
{
	
	scene@ g;
	script@ script;
	scripttrigger@ self;
	
	Light@ prev = null;
	Light@ next = null;
	
	[text] float radius = 500;
	[colour] uint colour = 0xFF999999;
	float radius_sqr;
	float red;
	float green;
	float blue;
	
	Light()
	{
		@g = get_scene();
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = @script;
		@this.self = @self;
		self.editor_handle_size(6);
		radius_sqr = radius * radius;
		red = (colour >> 16) & 0xFF;
		green = (colour >> 8) & 0xFF;
		blue = colour & 0xFF;
	}
	
	void on_add()
	{
		script.add_light(this);
	}
	
	void on_remove()
	{
		script.remove_light(this);
	}
	
	void editor_draw(float sub_frame)
	{
		g.draw_rectangle_world(21, 21, self.x() - 15, self.y() - 15, self.x() + 15, self.y() + 15, 0, 0xBB000000 + (colour & 0x00FFFFFF));
		draw_circle(g, self.x(), self.y(), radius, 32, 21, 21, 2, 0xFFFFFFFF);
	}
	
}









