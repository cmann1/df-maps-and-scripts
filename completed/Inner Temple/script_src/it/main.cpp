#include '../common/utils.cpp'
#include 'embeds.cpp'
#include 'Door.cpp'
#include 'ShrineTrigger.cpp'
#include 'LockTrigger.cpp'
#include 'UnlockTrigger.cpp'

#include '_PropSelect.cpp'

// 154 = 10011010
// 36  = 00100100
// 216 = 11011000

class script
{
	
	bool in_game = false;
	
	script()
	{
		
	}
	
	void build_sprites(message@ msg)
	{
		embed_build_sprites(msg);
	}
	
	void on_level_start()
	{
		in_game = true;
	}
	
}

void draw_line_glowing(scene@ g, int layer, int sub_layer, float x1, float y1, float x2, float y2, float width, float glow_width, uint glow_colour, uint colour)
{
	g.draw_line_world(layer, sub_layer, x1, y1, x2, y2, width, colour);
	
	const uint fade_colour = (colour & 0xFFFFFF);
	float dx = x2 - x1;
	float dy = y2 - y1;
	normalize(-dy, dx, dx, dy);
	dx *= glow_width;
	dy *= glow_width;
	g.draw_quad_world(layer, sub_layer, false,
		x1, y1, x2, y2,
		x2 + dx, y2 + dy, x1 + dx, y1 + dy,
		glow_colour, glow_colour, fade_colour, fade_colour);
	g.draw_quad_world(layer, sub_layer, false,
		x1, y1, x2, y2,
		x2 - dx, y2 - dy, x1 - dx, y1 - dy,
		glow_colour, glow_colour, fade_colour, fade_colour);
}