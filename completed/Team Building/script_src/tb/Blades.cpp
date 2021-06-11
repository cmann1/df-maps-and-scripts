#include "../common/math.cpp"

class Blades : trigger_base
{
	
	scene@ g;
	script@ script;
	scripttrigger@ self;
	
	sprites@ blade_spr;
	
	float t = 0;
	float blade_width;
	
	[position,layer:19,y:blades_y] float blades_x = 0;
	[hidden] float blades_y = 0;
	[angle,radian] float angle = 0;
	[text] int count = 2;
	[text] float scale = 0.75;
	[text] float speed = 50;
	[text] float spacing = -10;
	[angle,radian] float force_angle = 0;
	[text] float force = 100;
	
	Blades()
	{
		@g = get_scene();
		@blade_spr = create_sprites();
		
		blade_spr.add_sprite_set("script");
		rectangle@ r = blade_spr.get_sprite_rect("blade", 0);
		blade_width = r.get_width();
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = @script;
		@this.self = @self;
//		self.editor_handle_size(4);
//		audio@ a = g.play_script_stream("grind_loop", 2, blades_x, blades_y, true, 0.5);
//		a.positional(true);
	}
	
	void activate(controllable@ e)
	{
		dustman@ dm = e.as_dustman();
		if (@dm != null) {
			dm.set_speed_xy(dm.x_speed() + cos(force_angle - HALF_PI) * force, dm.y_speed() + sin(force_angle - HALF_PI) * force);
			g.remove_entity(e.as_entity());
			g.play_sound("sfx_trash_med", dm.x(), dm.y(), 1, false, true);
			g.play_sound("sfx_trash_med", dm.x(), dm.y(), 1, false, true);
			audio@ a = g.play_script_stream("grind_0" + (rand() % 2 + 1), 2, dm.x(), dm.y(), false, 0.75);
			a.positional(true);
			
			for(int i = int(num_cameras()) - 1; i >=0; i--)
			{
				if(dm.is_same(controller_controllable(i)))
				{
					dm.kill(false);
					break;
				}
			}
		}
	}
	
	void step()
	{
		t++;
	}
	
	void editor_step()
	{
		t++;
	}
	
	void draw(float sub_frame)
	{
		float x = blades_x;
		float y = blades_y;
		const float gap = (blade_width * scale + spacing);
		const float dx = cos(angle) * gap;
		const float dy = sin(angle) * gap;
		for(int i = 0; i < count; i++)
		{
			blade_spr.draw_world(15, 15, "blade", 0, 0, x, y, lerp((t - 1) * speed, t * speed, sub_frame), scale, scale, 0xFFFFFFFF);
			x += dx;
			y += dy;
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		
		g.draw_rectangle_world(21, 21, blades_x - 5, blades_y - 5, blades_x + 5, blades_y + 5, 0, 0xFFFFFFFF);
	}
	
}