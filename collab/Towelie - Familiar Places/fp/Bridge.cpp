#include '../common/ColType.cpp'
#include '../common/TileShape.cpp'
#include '../common/Sprite.cpp'
#include '../common/drawing_utils.cpp'

class Bridge : trigger_base, callback_base
{
	
	script@ script;
	scene@ g;
	scripttrigger @self;
	
	Sprite gear_spr('props3', 'sidewalk_10', 0.5, 0.5);
	float t = 0;
	
	[hidden] int key_count = 8;
	[hidden] bool opening = false;
	[hidden] float open_timer = 0;
	[hidden] int open_x = 0;
	[hidden] int open_y = 0;
	[hidden] int open_end_x = 0;
	[hidden] int open_end_y = 0;
	
	[position,mode:world,layer:19,y:gears_y] float gears_x;
	[hidden] float gears_y;
	[text] int gears_layer = 17;
	[text] int gears_sublayer = 19;
	[text] float gears_spacing = 85;
	[text] float gears_speed = 25;
	
	[position,mode:world,layer:19,y:from_y] float from_x;
	[hidden] float from_y;
	[position,mode:world,layer:19,y:to_y] float to_x;
	[hidden] float to_y;
	[text] float open_block_interval = 6;
	[text] int tile_set = 1;
	[text] int tile_tile = 2;
	[text] int tile_palette = 0;
	
	Bridge()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@script = s;
		@this.self = self;
		self.radius(38);
		
//		add_broadcast_receiver('unlock_bridge',this, 'on_unlock_bridge');
	}
	
//	void on_unlock_bridge(string id, message@ msg)
//	{
//		key_count--;
//		
//		if(key_count <= 0)
//		{
//			const float x = self.x();
//			const float y = self.y();
//			int count = g.get_entity_collision(y - 5, y + 5, x - 5, x + 5, COL_TYPE_KILL_ZONE);
//			for(int i = 0; i < count; i++)
//			{
//				entity@ e = g.get_entity_collision_index(i);
//				g.remove_entity(e);
//			}
//			
//			opening = true;
//			open_x = int(floor(from_x / 48));
//			open_y = int(floor(from_y / 48));
//			open_end_x = int(floor(to_x / 48));
//			open_end_y = int(floor(to_y / 48));
//			open_timer = open_block_interval;
//		}
//	}
	
//	void step()
//	{
//		if(opening)
//		{
//			t += DT;
//			open_timer--;
//			if(open_timer <= 0)
//			{
//				g.set_tile(open_x, open_y, 19, true, TILE_SHAPE_FULL, tile_set, tile_tile, tile_palette);
//				g.set_tile(open_x, open_y + 1, 19, true, TILE_SHAPE_FULL, tile_set, tile_tile, tile_palette);
//				
//				open_timer = open_block_interval;
//				
//				if(to_x > from_x)
//				{
//					open_x++;
//					if(open_x > open_end_x)
//						opening = false;
//				}
//				else
//				{
//					open_x--;
//					if(open_x < open_end_x)
//						opening = false;
//				}
//			}
//		}
//	}
	
//	void editor_step()
//	{
//		t += DT;
//	}
	
	void draw(float sub_frame)
	{
		gear_spr.draw_world(gears_layer, gears_sublayer, 0, 0, gears_x - gears_spacing, gears_y, -t * -gears_speed, 1, 1, 0xFFFFFFFF);
		gear_spr.draw_world(gears_layer, gears_sublayer, 0, 0, gears_x + gears_spacing, gears_y, -t * gears_speed, 1, 1, 0xFFFFFFFF);
	}
 
//	void editor_draw(float sub_frame)
//	{
//		g.draw_line_world(22, 22, from_x, from_y, to_x, to_y, 4, 0xFFFFFFFF);
//		draw_dot(g, 22, 22, gears_x, gears_y, 10, 0xFFFF0000, 0);
//		draw_dot(g, 22, 22, from_x, from_y, 10, 0xFF00FF00, 0);
//		draw_dot(g, 22, 22, to_x, to_y, 10, 0xFF0000FF, 0);
//		
//		draw(sub_frame);
//	}
	
}