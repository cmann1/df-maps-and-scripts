#include "../common/math.cpp"

class HangingSkeleton : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	sprites@ spr;
	
	float t = 0;
	
	[text] float speed = 2.0;
	[text] float swing = 4;
	[text] float angle = -6;
	[text] float scale = 1;
	[text] int layer = 17;
	[text] int sub_layer = 19;
	[colour] uint rope_colour = 0xFF000000;
	
	HangingSkeleton()
	{
		@g = get_scene();
		@spr = create_sprites();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		spr.add_sprite_set("script");
		t = rand() % 1000;
		self.editor_handle_size(4);
	}
	
	void step()
	{
		t += DT;
	}
	
	void editor_step()
	{
		t += DT;
	}
	
	void draw(float sub_frame)
	{
		const float x = self.x();
		const float y = self.y();
		const int frame = 0;
		const int palette = 0;
		const float v = sin(t * speed) * swing;
		const float px = cos((angle + v + 90) * DEG2RAD) * 20;
		const float py = sin((angle + v + 90) * DEG2RAD) * 20;
		spr.draw_world(layer, sub_layer, "skeleton_stand", frame, palette, x + px, y + py, angle + v, scale, scale, 0xFFFFFFFF);
		g.draw_line_world(layer, sub_layer, x, y, x + px, y + py, 4, rope_colour);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}


















