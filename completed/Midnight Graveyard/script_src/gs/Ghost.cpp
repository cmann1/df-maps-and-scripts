#include "../common/math.cpp"

class Ghost : trigger_base
{
	
	script@ script;
	scene@ g;
	scripttrigger@ self;
	
	sprites@ spr;
	
	[hidden] float t = 0;
	[text] float wait_max = 500;
	[hidden] float wait = 0;
	
	[text] float speed = 0.05;
	[text] float scale = 0.75;
	[text] int layer = 17;
	[text] int sub_layer = 23;
	[text] bool flip = false;
	[text] bool play_sound = false;
	
	Ghost()
	{
		@g = get_scene();
		@spr = create_sprites();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		spr.add_sprite_set("script");
		t = rand() % 1000;
		
		self.editor_colour_inactive(0x5557cca5);
		self.editor_colour_active(0x556bf3c6);
		self.editor_colour_circle(0x5557cca5);
	}
	
	void activate(controllable@ e)
	{
		if(wait > 0 or !play_sound) return;
		
		for(uint i = 0; i < num_cameras(); i++)
		{
			entity@ player = controller_entity(i);
			if(@player != null and e.is_same(player))
			{
				wait = wait_max * 0.75 + frand() * wait_max * 0.25;
				audio@ a = g.play_script_stream("crying", 2, self.x(), self.y(), false, 1.0);
				a.positional(true);
				return;
			}
		}
	}
	
	void step()
	{
		t++;
		if(wait > 0) wait--;
	}
	
	void editor_step()
	{
		t++;
	}
	
	void draw(float sub_frame)
	{
		const float x = self.x();
		const float y = self.y();
		const int frame = 0;
		const int palette = 0;
		const float s = sin(t * speed);
		const float v = sgn(s);
		spr.draw_world(layer, sub_layer, "ghost_0" + (round(s) + 2), frame, palette, x, y + sin(t * speed * 0.2) * 8, 0, scale * (flip ? -1 : 1), scale, 0xFFFFFFFF);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}


















