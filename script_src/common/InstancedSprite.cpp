class InstancedSprite : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	[text] string set = "script";
	[text] string sprite = "";
	[text] array<SpriteInstance> instances;
	
	int instance_count = 0;
	
	sprites@ spr;
	
	InstancedSprite()
	{
		@g = get_scene();
		@spr = create_sprites();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		instance_count = instances.length();
		spr.add_sprite_set(set);
		self.editor_handle_size(6);
	}
	
	void draw(float sub_frame)
	{
		for(int i = 0; i < instance_count; i++)
		{
			SpriteInstance@ instance = @instances[i];
			spr.draw_world(instance.layer, instance.sub_layer, sprite, 0, 0, instance.x, instance.y, instance.rotation, instance.scale_x, instance.scale_y, 0xFFFFFFFF);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		
		const float sx = self.x();
		const float sy = self.y();
		const bool selected = self.editor_selected();
		
		for(int i = int(instances.length()) - 1; i >= 0; i--)
		{
			SpriteInstance@ instance = @instances[i];
			const float x = instance.x;
			const float y = instance.y;
			if(selected) g.draw_line_world(21, 19, x, y, sx, sy, 2, 0x77FFFFFF);
			g.draw_rectangle_world(21, 19, x - 4, y - 4, x + 4, y + 4, 0, 0xFFFF0000);
		}
		
		g.draw_rectangle_world(21, 19, sx + 10, sy - 20, sx + 40, sy - 16, 0, 0xFF0000FF);
		g.draw_rectangle_world(21, 19, sx + 10, sy - 2,  sx + 40, sy + 2, 0, 0xFF0000FF);
		g.draw_rectangle_world(21, 19, sx + 10, sy + 16, sx + 40, sy + 20, 0, 0xFF0000FF);
		
		g.draw_rectangle_world(21, 19, sx + 10, sy - 16, sx + 14, sy - 2, 0, 0xFF0000FF);
		g.draw_rectangle_world(21, 19, sx + 36, sy + 2,  sx + 40, sy + 16, 0, 0xFF0000FF);
	}
	
}

class SpriteInstance
{
	
	[position,layer:19,y:y] float x = 0;
	[hidden] float y = 0;
	[text] int layer = 19;
	[text] int sub_layer = 19;
	[angle] float rotation = 0;
	[text] float scale_x = 1;
	[text] float scale_y = 1;
	
}