#include "math.cpp"

void draw_circle(scene@ g, float x, float y, float radius, uint segments, uint layer, uint sub_layer, float thickness=2, uint colour=0xFFFFFFFF, bool world=true)
{
	if(segments < 3) segments = 3;
	
	float angle = 0;
	float angle_step = PI * 2 / segments;
	
	for(uint i = 0; i < segments; i++)
	{
		if(world)
		{
			g.draw_line(
				layer, sub_layer, 
				x + cos(angle) * radius, y + sin(angle) * radius,
				x + cos(angle + angle_step) * radius, y + sin(angle + angle_step) * radius,
				thickness, colour);
		}
		else
		{
//			g.draw_line_hud(
//				layer, sub_layer, 
//				x + cos(angle) * radius, y + sin(angle) * radius,
//				x + cos(angle + angle_step) * radius, y + sin(angle + angle_step) * radius,
//				thickness, colour);
			draw_line(g,
				x + cos(angle) * radius, y + sin(angle) * radius,
				x + cos(angle + angle_step) * radius, y + sin(angle + angle_step) * radius,
				layer, sub_layer,
				thickness, colour, false);
		}
		
		angle += angle_step;
	}
}

void fill_circle_world(scene@ g, uint layer, uint sub_layer, float x, float y, float radius, uint segments, uint colour=0xFFFFFFFF, bool world=true)
{
	if(segments < 3) segments = 3;
	
	float angle = 0;
	float angle_step = PI * 2 / segments;
	int quad_group_index = 0;
	
	const float anchor_x = x + cos(angle) * radius;
	const float anchor_y = y + sin(angle) * radius;
	angle += angle_step;
	array<float> quad_xy(6);
	
	for(uint i = 1; i < segments; i++)
	{
		
		quad_xy[quad_group_index] = x + cos(angle) * radius;
		quad_xy[quad_group_index + 1] = y + sin(angle) * radius;
		
		quad_group_index += 2;
		
		if(quad_group_index == 6)
		{
//			g.draw_line_world(layer, sub_layer, anchor_x, anchor_y, quad_xy[0], quad_xy[1], 2, colour);
//			g.draw_line_world(layer, sub_layer, quad_xy[0], quad_xy[1], quad_xy[2], quad_xy[3], 2, colour);
//			g.draw_line_world(layer, sub_layer, quad_xy[2], quad_xy[3], quad_xy[4], quad_xy[5], 2, colour);
//			g.draw_line_world(layer, sub_layer, quad_xy[4], quad_xy[5], anchor_x, anchor_y, 2, colour);
			g.draw_quad_world(layer, sub_layer, false,
				anchor_x, anchor_y,
				quad_xy[0], quad_xy[1],
				quad_xy[2], quad_xy[3],
				quad_xy[4], quad_xy[5],
				colour, colour, colour, colour);

			quad_group_index = 2;
			quad_xy[0] = quad_xy[4];
			quad_xy[1] = quad_xy[5];
		}
		
		angle += angle_step;
	}
	
	if(quad_group_index == 4)
	{
		g.draw_line_world(layer, sub_layer, anchor_x, anchor_y, quad_xy[0], quad_xy[1], 2, colour);
			g.draw_line_world(layer, sub_layer, quad_xy[0], quad_xy[1], quad_xy[2], quad_xy[3], 2, colour);
			g.draw_line_world(layer, sub_layer, quad_xy[2], quad_xy[3], anchor_x, anchor_y, 2, colour);
//		g.draw_quad_world(layer, sub_layer, false,
//			anchor_x, anchor_y,
//			anchor_x, anchor_y,
//			quad_xy[0], quad_xy[1],
//			quad_xy[2], quad_xy[3],
//			colour, colour, colour, colour);
	}
}

void draw_line(scene@ g, float x1, float y1, float x2, float y2, uint layer, uint sub_layer, float thickness=2, uint colour=0xFFFFFFFF, bool world=true)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	float length = sqrt(dx * dx + dy * dy);
	
	float mx = (x1 + x2) * 0.5;
	float my = (y1 + y2) * 0.5;
	
	if(world)
		g.draw_rectangle_world(layer, sub_layer,
			mx - thickness, my - length * 0.5,
			mx + thickness, my + length * 0.5, atan2(-dx, dy) * RAD2DEG, colour);
	else
		g.draw_rectangle_hud(layer, sub_layer,
			mx - thickness, my - length * 0.5,
			mx + thickness, my + length * 0.5, atan2(-dx, dy) * RAD2DEG, colour);
}

void draw_line_glowing(scene@ g, int layer, int sub_layer, float x1, float y1, float x2, float y2, float width, float glow_width, uint glow_colour, uint colour)
{
	g.draw_line_world(layer, sub_layer, x1, y1, x2, y2, width, colour);
	
	const uint fade_colour = (colour & 0xFFFFFF);
	float dx = x2 - x1;
	float dy = y2 - y1;
	normalize(-dy, dx, dx, dy);
	const float ldx = dx * width * 0.5;
	const float ldy = dy * width * 0.5;
	dx *= glow_width + width;
	dy *= glow_width + width;
	g.draw_quad_world(layer, sub_layer, false,
		x1 + ldx, y1 + ldy, x2 + ldx, y2 + ldy,
		x2 + dx, y2 + dy, x1 + dx, y1 + dy,
		glow_colour, glow_colour, fade_colour, fade_colour);
	g.draw_quad_world(layer, sub_layer, false,
		x1 - ldx, y1 - ldy, x2 - ldx, y2 - ldy,
		x2 - dx, y2 - dy, x1 - dx, y1 - dy,
		glow_colour, glow_colour, fade_colour, fade_colour);
}

void draw_arrow(scene@ g, uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float width=2, float head_size=20, float head_position=1, uint colour=0xFFFFFFFF, bool world=true)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	float length = sqrt(dx * dx + dy * dy);
	
	if(length <= 0)
		return;
	
	const float x3  = x1 + dx * head_position;
	const float y3  = y1 + dy * head_position;
	
	dx = dx / length * head_size;
	dy = dy / length * head_size;
	const float x4  = x3 - dx;
	const float y4  = y3 - dy;
	
	if(world)
	{
		g.draw_line_world(layer, sub_layer, x1, y1, x2, y2, width, colour);
		g.draw_line_world(layer, sub_layer, x3, y3, x4 + dy, y4 - dx, width, colour);
		g.draw_line_world(layer, sub_layer, x3, y3, x4 - dy, y4 + dx, width, colour);
	}
	else
	{
		g.draw_line_hud(layer, sub_layer, x1, y1, x2, y2, width, colour);
	}
}

void draw_window(scene@ g, float x, float y, float size_x, float size_y, float padding=20, uint glass_colour=0x99000000, uint glass_blur=0x00FFFFFF)
{
	size_x += padding * 2;
	size_y += padding * 2;
	
	g.draw_rectangle_hud(10, 11,
		x, y,
		x + size_x, y + size_y,
		0, glass_colour);
	
	g.draw_glass_hud(10, 10,
		x, y,
		x + size_x, y + size_y, 0,
		glass_blur);
}

void shadowed_text_world(textfield@ tf, int layer, int sub_layer, float x, float y, float scale_x=1, float scale_y=1, float rotation=0, uint shadow_colour=0x77000000, float ox=5, float oy=5)
{
	const uint colour = tf.colour();
	tf.colour(shadow_colour);
	tf.draw_world(layer, sub_layer, x + ox, y + oy, scale_x, scale_y, rotation);
	tf.colour(colour);
	tf.draw_world(layer, sub_layer, x, y, scale_x, scale_y, rotation);
}

void shadowed_text_hud(textfield@ tf, int layer, int sub_layer, float x, float y, float scale_x=1, float scale_y=1, float rotation=0, uint shadow_colour=0x77000000, float ox=5, float oy=5)
{
	const uint colour = tf.colour();
	tf.colour(shadow_colour);
	tf.draw_hud(layer, sub_layer, x + ox, y + oy, scale_x, scale_y, rotation);
	tf.colour(colour);
	tf.draw_hud(layer, sub_layer, x, y, scale_x, scale_y, rotation);
}

void draw_rect(scene@ g, float x, float y, float half_w, float half_h, float rotation, uint layer, uint sub_layer, uint colour=0xFFFFFFFF, bool world=true)
{
	if(world)
		g.draw_rectangle_world(layer, sub_layer, x - half_w, y - half_h, x + half_w, y + half_h, rotation, colour);
	else
		g.draw_rectangle_hud(layer, sub_layer, x - half_w, y - half_h, x + half_w, y + half_h, rotation, colour);
}

void draw_dot(scene@ g, int layer, int sub_layer, float x, float y, float size=1, uint colour=0xFFFFFFFF, float rotation=0)
{
	g.draw_rectangle_world(layer, sub_layer, x-size, y-size, x+size, y+size, rotation, colour);
}

void rect(scene@ g, float x, float y, float half_w, float half_h, float rotation, uint layer, uint sub_layer, float thickness=2, uint colour=0xFFFFFFFF, bool world=true)
{
	float x1;
	float y1;
	float x2 = 0;
	float y2 = 0;
	float angle = rotation + PI * 0.25;

	/*
	// NO LOOPS, ONLY TWO COS AND SIN
	rotatedRect(float x, float y, float halfWidth, float halfHeight, float angle)
	{
        float c = cos(angle);
        float s = sin(angle);
        float r1x = -halfWidth * c - halfHeight * s;
        float r1y = -halfWidth * s + halfHeight * c;
        float r2x =  halfWidth * c - halfHeight * s;
        float r2y =  halfWidth * s + halfHeight * c;

        // Returns four points in clockwise order starting from the top left.
        return
            (x + r1x, y + r1y),
            (x + r2x, y + r2y),
            (x - r1x, y - r1y),
            (x - r2x, y - r2y);
    }
	*/

	for(int i = 0; i < 4; i++)
	{
		if(i > 0)
		{
			x1 = x2;
			y1 = y2;
		}
		else
		{
			x1 = cos(angle - PI * 0.5) * half_w;
			y1 = sin(angle - PI * 0.5) * half_h;
		}
		
		x2 = cos(angle) * half_w;
		y2 = sin(angle) * half_h;
		
		if(world)
		{
			g.draw_line_world(layer, sub_layer, x + x1, y + y1, x + x2, y + y2, thickness, colour);
		}
		else
		{
			g.draw_line_hud(layer, sub_layer, x + x1, y + y1, x + x2, y + y2, thickness, colour);
		}
		
		angle += PI * 0.5;
	}
}

void outline_rect(scene@ g, float x1, float y1, float x2, float y2, uint layer, uint sub_layer, float thickness=2, uint colour=0xFFFFFFFF, bool world=true)
{
	if(world)
	{
		// Top
		g.draw_rectangle_world(layer, sub_layer,
			x1 - thickness, y1 - thickness,
			x2 + thickness, y1 + thickness,
			0, colour);
		// Bottom
		g.draw_rectangle_world(layer, sub_layer,
			x1 - thickness, y2 - thickness,
			x2 + thickness, y2 + thickness,
			0, colour);
		// Left
		g.draw_rectangle_world(layer, sub_layer,
			x1 - thickness, y1 - thickness,
			x1 + thickness, y2 + thickness,
			0, colour);
		// Right
		g.draw_rectangle_world(layer, sub_layer,
			x2 - thickness, y1 - thickness,
			x2 + thickness, y2 + thickness,
			0, colour);
	}
	else
	{
		// Top
		g.draw_rectangle_hud(layer, sub_layer,
			x1 - thickness, y1 - thickness,
			x2 + thickness, y1 + thickness,
			0, colour);
		// Bottom
		g.draw_rectangle_hud(layer, sub_layer,
			x1 - thickness, y2 - thickness,
			x2 + thickness, y2 + thickness,
			0, colour);
		// Left
		g.draw_rectangle_hud(layer, sub_layer,
			x1 - thickness, y1 - thickness,
			x1 + thickness, y2 + thickness,
			0, colour);
		// Right
		g.draw_rectangle_hud(layer, sub_layer,
			x2 - thickness, y1 - thickness,
			x2 + thickness, y2 + thickness,
			0, colour);
	}
}

void outline_tile(scene@ g, int x, int y, uint layer, uint sub_layer, float thickness=1, uint colour=0xFFFFFFFF, bool world=true)
{
	x *= 48;
	y *= 48;
	outline_rect(g, x, y, x + 48, y + 48, layer, sub_layer, thickness, colour, world);
}

class HelpBox
{
	
	private textfield@ text_field;
	private float width;
	private float height;
	private float offset_x;
	private float offset_y;
	private float scale;
	
	int halign;
	int valign;
	float x = 0;
	float y = 0;
	float padding = 20;
	
	HelpBox(string text, int halign=-1, int valign=-1, float text_scale=0.6)
	{
		@text_field = create_textfield();
		text_field.text(text);
		text_field.align_horizontal(halign);
		text_field.align_vertical(valign);
		
		const float padding = 10;
		
		this.halign = halign;
		this.valign = valign;
		this.scale = text_scale;
		
		width = text_field.text_width() * text_scale;
		height = text_field.text_height() * text_scale;
		
		if(halign == -1) offset_x = 0;
		else if(halign == 0) offset_x = width * 0.5;
		else if(halign == 1) offset_x = width;
		
		if(valign == -1) offset_y = 0;
		else if(valign == 0) offset_y = height * 0.5;
		else if(valign == 1) offset_y = height;
	}
	
	void draw(scene@ g)
	{
		float wx = x + offset_x;
		float wy = y + offset_y;
		
		float x1 = wx;
		float y1 = wy;
		float x2 = wx + width  + padding * 2;
		float y2 = wy + height + padding * 2;
		
		g.draw_glass_hud(19, 18, x1, y1, x2, y2, 0, 0x00000000);
		g.draw_rectangle_hud(19, 19, x1, y1, x2, y2, 0, 0xBB000000);
		text_field.draw_hud(20, 20, wx + padding, wy + padding, scale, scale, 0);
	}
	
}