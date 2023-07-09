#include '../lib/drawing/Sprite.cpp';

class SpeechBubbleBox
{
	
	private sprites@ spr;
	private sprites@ spr_fill;
	
	private string spr_name = 'buildingblocks_8';
	private float spr_size;
	private float spr_spacing = 80;
	private float spr_base_ox, spr_base_oy;
	
	private string spr_fill_name = 'backdrops_6';
	private float spr_fill_base_ox, spr_fill_base_oy;
	private float spr_fill_size_x, spr_fill_size_y;
	
	private float spr_scale_x, spr_scale_y;
	private float spr_ox, spr_oy;
	
	private float spr_size_x, spr_size_y;
	
	SpeechBubbleBox()
	{
		@spr = create_sprites();
		spr.add_sprite_set('props1');
		rectangle@ r = spr.get_sprite_rect(spr_name, 0);
		spr_base_ox = -r.left() - 2.5;
		spr_base_oy = -r.top() - 2.5;
		spr_size = r.width - 4;
		
		@r = spr.get_sprite_rect(spr_fill_name, 0);
		spr_fill_base_ox = -r.left() - 2.5;
		spr_fill_base_oy = -r.top() - 3;
		spr_fill_size_x = r.width - 5;
		spr_fill_size_y = r.height - 4.5;
	}
	
	void draw(const float x, const float y, const float width, const float height, const int layer, const int sub_layer, const uint clr)
	{
		const float spr_scale_x = min(width / spr_size, 1.0);
		const float spr_scale_y = min(height / spr_size, 1.0);
		const float spr_size_x = spr_size * spr_scale_x;
		const float spr_size_y = spr_size * spr_scale_y;
		
		const float spr_ox = spr_base_ox * spr_scale_x;
		const float spr_oy = spr_base_oy * spr_scale_y;
		
		// Top left corner
		spr.draw_world(
			layer, sub_layer, spr_name, 0, 0,
			x + spr_ox, y + spr_oy,
			0, spr_scale_x, spr_scale_y, clr);
		// Top right corner
		if(width > spr_size)
		{
			spr.draw_world(
				layer, sub_layer, spr_name, 0, 0,
				x + width - spr_size_x + spr_ox, y + spr_oy,
				0, spr_scale_x, spr_scale_y, clr);
		}
		
		// Bottom left corner
		if(height > spr_size)
		{
			spr.draw_world(
				layer, sub_layer, spr_name, 0, 0,
				x + spr_ox, y + height - spr_size_y + spr_oy,
				0, spr_scale_x, spr_scale_y, clr);
			
			// Bottom Right corner
			if(width > spr_size)
			{
				spr.draw_world(
					layer, sub_layer, spr_name, 0, 0,
					x + width - spr_size_x + spr_ox, y + height - spr_size_y + spr_oy,
					0, spr_scale_x, spr_scale_y, clr);
			}
		}
		
		// Top/Bottom
		if(width > spr_size * 2 - (spr_size - spr_spacing) * 0.75)
		{
			const float fill_sx = (width - spr_size) / spr_fill_size_x;
			const float fill_sy = min(height / spr_fill_size_y, 1.0);
			
			// Top
			spr.draw_world(
				layer, sub_layer, spr_fill_name, 0, 0,
				x + spr_size * 0.5 + spr_fill_base_ox * fill_sx,
				y + spr_fill_base_oy * fill_sy,
				0, fill_sx, fill_sy, clr);
			
			// Bottom
			if(height > spr_fill_size_y)
			{
				spr.draw_world(
					layer, sub_layer, spr_fill_name, 0, 0,
					x + spr_size * 0.5 + spr_fill_base_ox * fill_sx,
					y + height - spr_fill_size_y + spr_fill_base_oy * fill_sy,
					0, fill_sx, fill_sy, clr);
			}
		}
		
		// Left/Right
		if(height > spr_size * 2 - (spr_size - spr_spacing) * 0.75)
		{
			// Left
			const float fill_sx = min(width / spr_fill_size_x, 1.0);
			const float fill_sy = (height - spr_size) / spr_fill_size_y;
			
			spr.draw_world(
				layer, sub_layer, spr_fill_name, 0, 0,
				x + spr_fill_base_ox * fill_sx,
				y + spr_size * 0.5 + spr_fill_base_oy * fill_sy,
				0, fill_sx, fill_sy, clr);
			
			// Bottom
			if(width > spr_fill_size_x)
			{
				spr.draw_world(
					layer, sub_layer, spr_fill_name, 0, 0,
					x + width - spr_fill_size_x + spr_fill_base_ox * fill_sx,
					y + spr_size * 0.5 + spr_fill_base_oy * fill_sy,
					0, fill_sx, fill_sy, clr);
			}
		}
		
		// Centre
		if(width > spr_fill_size_x * 2 - 1 && height > spr_fill_size_y * 2 - 1)
		{
			const float fill_sx = (width - spr_fill_size_x * 2 + 8) / spr_fill_size_x;
			const float fill_sy = (height - spr_fill_size_y * 2 + 8) / spr_fill_size_y;
			spr.draw_world(
					layer, sub_layer, spr_fill_name, 0, 0,
					x + spr_fill_size_x - 4 + spr_fill_base_ox * fill_sx,
					y + spr_fill_size_y - 4 + spr_fill_base_oy * fill_sy,
					0, fill_sx, fill_sy, clr);
		}
	}
	
	void draw_scary(const float x, const float y, const float width, const float height, const int layer, const int sub_layer, const uint clr)
	{
		// Fill
		
		const float spr_fill_ox = spr_fill_base_ox - 0.5;
		const float spr_fill_oy = spr_fill_base_oy;
		const float spr_fill_size_x = this.spr_fill_size_x + 0.5;
		const float spr_fill_size_y = this.spr_fill_size_y;
		
		//scene.draw_rectangle_world(22, 22, x, y, x + w, y + h, 0, 0x33ffffff);
		
		const float fill_sx = width / spr_fill_size_x;
		const float fill_sy = height / spr_fill_size_y;
		spr.draw_world(
			layer, sub_layer, spr_fill_name, 0, 0,
			x + spr_fill_ox * fill_sx,
			y + spr_fill_oy * fill_sy,
			0, fill_sx, fill_sy, clr);
	}
	
}
