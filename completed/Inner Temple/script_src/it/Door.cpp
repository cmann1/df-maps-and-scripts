#include '../common/Fx.cpp'

class Door : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	[position,layer:19,y:y1] float x1 = 0;
	[hidden] float y1 = 0;
	[position,layer:19,y:y2] float x2 = 0;
	[hidden] float y2 = 0;
	
	Door()
	{
		@g = get_scene();
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.self = @self;
	}
	
	void activate(controllable@ e)
	{
		if(e.type_name() == 'hittable_apple')
		{
			const int x3 = int(floor(x1 / 48));
			const int y3 = int(floor(y1 / 48));
			const int x4 = int(floor(x2 / 48));
			const int y4 = int(floor(y2 / 48));
			
			for(int x = x3; x <= x4; x++)
			{
				for(int y = y3; y <= y4; y++)
				{
					g.set_tile(x, y, 19, false, 0, 0, 0, 0);
				}
			}
			
			spawn_fx(self.x(), self.y(), 'editor', 'respawnteam2', 0, 15, 0, 1, 1, 0xFFFFFFFF);
			
			g.remove_entity(e.as_entity());
			g.remove_entity(self.as_entity());
		}
	}
	
	void editor_draw(float sub_frame)
	{
		const float x3 = floor(x1 / 48) * 48;
		const float y3 = floor(y1 / 48) * 48;
		const float x4 = floor(x2 / 48) * 48 + 48;
		const float y4 = floor(y2 / 48) * 48 + 48;
		g.draw_rectangle_world(21, 21, x3, y3, x4, y4, 0, 0x66FFFFFF);
	}
	
}