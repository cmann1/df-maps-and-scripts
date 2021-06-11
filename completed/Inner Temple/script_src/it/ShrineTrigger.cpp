#include '../common/math.cpp'
#include '../common/Rnd.cpp'
#include '../common/Sprite.cpp'
#include '../common/SpriteBatch.cpp'

class ShrineTrigger : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	Rnd rnd;
	
	array<int> polygons;
	int num_polygons;
	float radius = 48;
	float padding = 24;
	float offset_x = 0;
	float offset_y = -48 * 4;
	
	int player_count = 0;
	
	float alpha = 0;
	float fade_speed = 1.0 / 1;
	float t = 0;
	
	string icon_spr_name;
	
	Sprite dot_spr('props2', 'backdrops_4');
	
	Sprite block_spr('props1', 'buildingblocks_8');
	sprites@ icon_spr;
	
	[position,layer:19,y:spr_y] float spr_x = 0;
	[hidden] float spr_y = 0;
	[text] int block_layer = 17;
	[text] int layer = 17;
	[text] int sub_layer = 24;
	[colour] uint colour = 0x9FF2FF;
	
	ShrineTrigger()
	{
		@g = get_scene();
		
		@icon_spr = create_sprites();
		icon_spr.add_sprite_set('script');
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.self = @self;
		
		num_polygons = int(polygons.length());
		alpha = 0;
		colour = colour & 0xFFFFFF;
	}
	
	void activate(controllable@ e)
	{
		for(int i = int(num_cameras()) - 1; i >= 0; i--)
		{
			if(controller_controllable(i).is_same(e))
			{
				player_count++;
				return;
			}
		}
	}
	
	void step()
	{
		if(player_count > 0)
		{
			player_count = 0;
			
			alpha += fade_speed * DT;
			if(alpha > 1) alpha = 1;
		}
		else
		{
			alpha -= fade_speed * DT;
			if(alpha < 0) alpha = 0;
		}
		
		if(alpha > 0)
		{
			t++;
		}
	}
	
	void draw(float sub_frame)
	{
		if(alpha > 0)
		{
			float width = num_polygons * radius * 2.0 + (num_polygons - 1) * padding;
			
			float x = self.x() + offset_x - width * 0.5 + radius;
			float y = self.y() + offset_y;
			
			const uint spr_colour = (uint(alpha * 0xFF) << 24) + colour;
			const uint glow_colour = (uint(alpha * 0x55) << 24) + colour;
			const float glow_width = 15;
			
			rnd.seed = uint(1000523 + t / 4);
			rnd.random();
			
			for(int i = 0; i < num_polygons; i++)
			{
				const int num_sides = polygons[i];
				float angle = -PI * 0.5;
				float angle_step = (PI * 2) / num_sides;
				float ox, oy;
				float prev_x = 0, prev_y = 0;
				float r = num_sides < 2 ? 0 : radius;
				
				const float spr_scale = num_sides == 1 ? 0.125 : 0.075;
				
				const float r_rnd = 1;// + frand() * 0.2 - 0.1;
				
				for(float j = 0; j < num_sides; j++)
				{
					float px = x + cos(angle) * r * r_rnd + rnd.range(-1, 1);
					float py = y + sin(angle) * r * r_rnd + rnd.range(-1, 1);
					
					if(j > 0)
					{
						draw_line_glowing(g, layer, sub_layer, prev_x, prev_y, px, py, 2, glow_width, glow_colour, spr_colour);
					}
					else
					{
						ox = px;
						oy = py;
					}
					
					dot_spr.draw_world(layer, sub_layer, 0, 0, px, py, 0, spr_scale, spr_scale, spr_colour);
					angle += angle_step;
					
					prev_x = px;
					prev_y = py;
				}
				
				if(num_sides > 2)
				{
					draw_line_glowing(g, layer, sub_layer, prev_x, prev_y, ox, oy, 2, glow_width, glow_colour, spr_colour);
				}
				
				x += radius * 2 + padding;
			}
		}
		
		block_spr.draw_world(block_layer, 19, 0, 0, spr_x, spr_y, 0, 1.25, 1.25, 0xFFFFFFFF);
		icon_spr.draw_world(layer, sub_layer - 1, icon_spr_name, 0, 0, spr_x - 1, spr_y - 1, 0, 1, 1, 0xFFFFFFFF);
		
		if(alpha > 0)
		{
			icon_spr.draw_world(layer, sub_layer, icon_spr_name, 0, 0, spr_x, spr_y, 0, 1, 1, (uint(alpha * 0xFF) << 24) + 0xFFFFFF);
		}
	}
	
//	void editor_step() { t++; }
	void editor_draw(float sub_frame)
	{
		block_spr.draw_world(block_layer, 19, 0, 0, spr_x, spr_y, 0, 1.25, 1.25, 0xFFFFFFFF);
//		alpha = 1; draw(sub_frame);
	}
	
}

class WestShrineTrigger : ShrineTrigger
{
	WestShrineTrigger()
	{
		super();
		polygons = (array<int> = {1, 5, 4});
		icon_spr_name = 'sun';
	}
}

class EastShrineTrigger : ShrineTrigger
{
	EastShrineTrigger()
	{
		super();
		polygons = (array<int> = {2, 1, 6});
		icon_spr_name = 'river';
	}
}
class NorthShrineTrigger : ShrineTrigger
{
	NorthShrineTrigger()
	{
		super();
		polygons = (array<int> = {3, 6});
		icon_spr_name = 'moon';
	}
}