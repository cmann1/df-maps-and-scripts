#include "common/utils.cpp"
#include "common/math.cpp"
#include "common/drawing_utils.cpp"

const float ASPECT_RATIO = 1920.0 / 1080;
const float GRID = 48;

class script
{
	scene@ g;
	
	[colour] uint ambient = 0xFF000000;
	float ambient_r;
	float ambient_g;
	float ambient_b;
	
	Light@ lights = null;
	
	controllable@ player;
	Light player_light;
	
	int tile_start_x;
	int tile_start_y;
	int tile_count_x;
	int tile_count_y;
	int tile_row_count;
	array<uint> vertex_colours;
	
	script()
	{
		@g = get_scene();
	}
	
	void on_level_start()
	{
		player_light.colour = 0xFFFFFFCC;
		player_light.radius = 700;
		entity@ e = create_scripttrigger(player_light).as_entity();
		g.add_entity(e, false);
		
		ambient_r = (ambient >> 16) & 0xFF;
		ambient_g = (ambient >> 8) & 0xFF;
		ambient_b = ambient & 0xFF;
	}
	
	void add_light(Light@ light)
	{
		if(@lights == null)
		{
			@lights = light;
		}
		else
		{
			@lights.prev = light;
			@light.next = lights;
			@lights = light;
		}
	}
	
	void remove_light(Light@ light)
	{
		if(@light.prev != null)
		{
			@light.prev.next = light.next;
		}
		if(@light.next != null)
		{
			@light.next.prev = light.prev;
		}
		if(@lights == @light)
		{
			@lights = @lights.next;
		}
	}
	
	void step(int entities)
	{
		if(@player == null)
		{
			@player = controller_controllable(0);
		}
		
		if(@player != null)
		{
			if(@player_light.self != null)
			{
				player_light.self.set_xy(player.x(), player.y() - 48);
			}
		}
		
		camera@ cam = get_active_camera();
		const float cam_x = cam.x();
		const float cam_y = cam.y();
		const float screen_width = cam.screen_height() * ASPECT_RATIO / 2;
		const float screen_height = cam.screen_height() / 2;
		const float left = min(cam.x(), cam.prev_x()) - screen_width;
		const float right = max(cam.x(), cam.prev_x()) + screen_width;
		const float top = min(cam.y(), cam.prev_y()) - screen_height;
		const float bottom = max(cam.y(), cam.prev_y()) + screen_height;
		tile_start_x = int(floor(left / GRID));
		tile_start_y = int(floor(top / GRID));
		tile_count_x = int(ceil(right / GRID)) - tile_start_x;
		tile_count_y = int(ceil(bottom / GRID)) - tile_start_y;
		
		tile_row_count = tile_count_y + 1;
		vertex_colours.resize( (tile_count_x + 1) * tile_row_count );
		
		int i = 0;
		for(int tile_x = 0; tile_x <= tile_count_x; tile_x++)
		{
			for(int tile_y = 0; tile_y <= tile_count_y; tile_y++)
			{
				const float x = (tile_start_x + tile_x) * GRID;
				const float y = (tile_start_y + tile_y) * GRID;
				Light@ light = lights;
				float red = ambient_r;
				float green = ambient_g;
				float blue = ambient_b;
				while(@light != null)
				{
					const float light_x = light.self.x();
					const float light_y = light.self.y();
					const float light_radius = light.radius;
					const float dx = light_x - x;
					const float dy = light_y - y;
					if(dx <= light_radius and dy <= light_radius)
					{
						const float dist = sqrt(dx * dx + dy * dy);
						if(dist <= light_radius)
						{
							raycast@ ray = g.ray_cast_tiles(abs(dx) < 2 ? light_x + 2 : light_x, abs(dy) < 2 ? light_y + 2 : light_y, x, y);
							
							if(!ray.hit())
							{
								float att = max(1.0 - dist*dist/light.radius_sqr, 0.0);
								att *= att;
								red += light.red * att;
								green += light.green * att;
								blue += light.blue * att;
							}
						}
					}
					
					@light = light.next;
				}
				
				if(red > 255) red = 255;
				if(green > 255) green = 255;
				if(blue > 255) blue = 255;
				float brightness = 255 - (red + green + blue) / 3;
//				vertex_colours[i++] = (uint(brightness) << 24) + (uint(red) << 16) + (uint(green) << 8) + uint(blue);
				vertex_colours[i++] = (uint(brightness) << 24);
//				vertex_colours[i++] = 0xFF000000 + (uint(red) << 16) + (uint(green) << 8) + uint(blue);
//				vertex_colours[i++] = 0x00000000 + (uint(red) << 16) + (uint(green) << 8) + uint(blue);
			}
		}
	}
	
	void draw(float sub_frame)
	{
		const int row_count = tile_count_y + 1;
		for(int tile_x = 0; tile_x < tile_count_x; tile_x++)
		{
			for(int tile_y = 0; tile_y < tile_count_y; tile_y++)
			{
				const float x = (tile_start_x + tile_x) * GRID;
				const float y = (tile_start_y + tile_y) * GRID;
				
				g.draw_quad_world(20, 20, false,
					x, y,
					x, y + GRID,
					x + GRID, y + GRID,
					x + GRID, y,
					vertex_colours[tile_x * tile_row_count + tile_y],
					vertex_colours[tile_x * tile_row_count + tile_y + 1],
					vertex_colours[(tile_x + 1) * tile_row_count + tile_y + 1],
					vertex_colours[(tile_x + 1) * tile_row_count + tile_y]);
			}
		}
	}
	
}

class Light : trigger_base
{
	
	scene@ g;
	script@ script;
	scripttrigger@ self;
	
	Light@ prev = null;
	Light@ next = null;
	
	[text] float radius = 500;
	[colour] uint colour = 0xFF999999;
	float radius_sqr;
	float red;
	float green;
	float blue;
	
	Light()
	{
		@g = get_scene();
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = @script;
		@this.self = @self;
		self.editor_handle_size(6);
		radius_sqr = radius * radius;
		red = (colour >> 16) & 0xFF;
		green = (colour >> 8) & 0xFF;
		blue = colour & 0xFF;
	}
	
	void on_add()
	{
		script.add_light(this);
	}
	
	void on_remove()
	{
		script.remove_light(this);
	}
	
	void editor_draw(float sub_frame)
	{
		g.draw_rectangle_world(21, 21, self.x() - 15, self.y() - 15, self.x() + 15, self.y() + 15, 0, 0xBB000000 + (colour & 0x00FFFFFF));
		draw_circle(g, self.x(), self.y(), radius, 32, 21, 21, 2, 0xFFFFFFFF);
	}
	
}









