#include '../lib/std.cpp';

class script
{
	
	[text] float texture_size = 256;
	[text] float texture_scale = 1;
	[slider,min:0,max:255] int texture_alpha = 255;
	[text] bool texture_hud = false;
	
	scene@ g;
	
	private canvas@ world_canvas;
	private canvas@ hud_canvas;
	private sprites@ texture_spr;
	private camera@ camera;
	
	private bool initialised;
	
	script()
	{
		@g = get_scene();
		
		@world_canvas = create_canvas(false, 21, 0);
		@hud_canvas = create_canvas(true, 2, 2);
		hud_canvas.scale_hud(false);
		
		@texture_spr = create_sprites();
		@camera = get_active_camera();
	}
	
	void on_level_start()
	{
	}
	
	void initialise()
	{
		texture_spr.add_sprite_set('script');
	}
	
	void entity_on_add(entity@ e)
	{
		const string name = e.type_name();
		
		if(name == 'entity_cleansed')
		{
			g.remove_entity(e);
		}
	}
	
	void step(int num_entities)
	{
		@camera = get_active_camera();
		
		if(!initialised)
		{
			initialise();
			initialised = true;
		}
	}
	
	void editor_step()
	{
		@camera = get_active_camera();
		
		if(!initialised)
		{
			initialise();
			initialised = true;
		}
	}
	
	void draw(float sub_frame)
	{
		const float hud_w = g.hud_screen_width(false);
		const float hud_h = g.hud_screen_height(false);
		
		float view_x, view_y;
		float view_w, view_h;
		canvas@ texture_canvas = texture_hud ? hud_canvas : world_canvas;
		
		const float texture_size = this.texture_size * texture_scale;
		
		if(!texture_hud)
		{
			float left, top, width, height;
			camera.get_layer_draw_rect(sub_frame, 19, left, top, width, height);
			view_x = floor(left / texture_size) * texture_size;
			view_y = floor(top / texture_size) * texture_size;
			view_w = width  + max(0.0, left - view_x);
			view_h = height + max(0.0, top  - view_y);
			
			texture_canvas.reset();
			texture_canvas.translate(view_x, view_y);
		}
		else
		{
			view_w = hud_w;
			view_h = hud_h;
			view_x = 0;
			view_y = 0;
		}
		
		hud_canvas.reset();
		hud_canvas.translate(-hud_w * 0.5, -hud_h * 0.5);
		
		float count_x = ceil_int(view_w / texture_size);
		float count_y = ceil_int(view_h / texture_size) + 1;
		
		if(count_x * count_y < 600)
		{
			for(int x = 0; x < count_x; x++)
			{
				const float draw_x = x * texture_size;
				
				for(int y = 0; y < count_y; y++)
				{
					const float draw_y = y * texture_size;
					
					texture_canvas.draw_sprite(texture_spr, 'texture2', 0, 0,
						draw_x, draw_y,
						0, texture_scale, texture_scale, (texture_alpha << 24) | 0xffffff);
				}
			}
		}
		
		//texture_spr.draw_world(17, 17, 'hand', 0, 0, 2000, 2000, 0, 1, 1, 0xffffffff);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}
