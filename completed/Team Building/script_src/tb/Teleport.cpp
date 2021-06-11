class Teleport : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	[position,layer:19,y:y] float x = 0;
	[hidden] float y = 0;
	[hidden] float offset = 0;
	
	Teleport()
	{
		@g = get_scene();
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.self = @self;
	}
	
	void activate(controllable@ e)
	{
		e.set_speed_xy(0, 0);
		e.set_xy(x + (offset % 96) - 48, y);
		offset += 48;
	}
	
	void editor_draw(float sub_frame)
	{
		if(self.editor_selected())
		{
			g.draw_rectangle_world(21, 19, x - 4, y - 4, x + 4, y + 4, 0, 0xFFFF0000);
			g.draw_line_world(21, 19, self.x(), self.y(), x, y, 2, 0xAAFF0000);
		}
	}
	
}