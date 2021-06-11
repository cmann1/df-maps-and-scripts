#include "../common/utils.cpp"
#include "../common/math.cpp"
#include "../common/drawing_utils.cpp"
#include "Light.cpp"
#include "PropsLight.cpp"

const float ASPECT_RATIO = 1920.0 / 1080;
const float GRID = 48;
const float HGRID = GRID * 0.5;

class script
{
	scene@ g;
	
	[colour] uint ambient = 0xFF000000;
	float ambient_r;
	float ambient_g;
	float ambient_b;
	
	Light@ lights = null;
	int light_count = 0;
	
	int num_players;
	array<controllable@> players;
	array<Light@> player_lights;
	[colour] uint player_colour = 0xFFFFFFCC;
	[text] float player_radius = 600;
	
	int tile_start_x;
	int tile_start_y;
	int tile_count_x;
	int tile_count_y;
	int tile_row_count;
	array<uint> vertex_colours;
	array<float>@ tile_colours = {};
	array<float>@ prev_tile_colours = {};
	
	PropsLight props_light;
	
	script()
	{
		@g = get_scene();
		
		num_players = num_cameras();
		players.resize(num_players);
		player_lights.resize(num_players);
	}
	
	void on_level_start()
	{
		ambient_r = (ambient >> 16) & 0xFF;
		ambient_g = (ambient >> 8) & 0xFF;
		ambient_b = ambient & 0xFF;
	}
	
	void checkpoint_load()
	{
		for(int i = 0; i < num_players; i++)
		{
			@players[i] = null;
		}
		props_light.checkpoint_load();
//		@lights = null;
	}
	
	void add_light(Light@ light)
	{
		light_count++;
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
		light_count--;
		if(@light.prev != null)
		{
			@light.prev.next = light.next;
			@light.prev = null;
		}
		if(@light.next != null)
		{
			@light.next.prev = light.prev;
			@light.next = null;
		}
		if(@lights == @light)
		{
			@lights = @lights.next;
		}
	}
	
	void step(int entities)
	{
		controllable@ player;
		for(int i = 0; i < num_players; i++)
		{
			if((@player = players[i]) != null)
			{
				Light@ player_light = @player_lights[i];
				if(@player_light.self != null)
				{
					player_light.self.set_xy(player.x(), player.y() - 48);
				}
				
				props_light.player(player);
			}
			else
			{
				entity@ e = controller_entity(i);
				@players[i] = @player = (@e != null ? e.as_controllable() : null);
				
				if(@player != null)
				{
					Light@ player_light = @player_lights[i] = Light();
					player_light.colour = player_colour;
					player_light.radius = player_radius;
					entity@ le = create_scripttrigger(player_light).as_entity();
					le.set_xy(player.x(), player.y() - 48);
					g.add_entity(le, false);
				}
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
		
		vertex_colours.resize((tile_count_x + 1) * (tile_count_y + 1));
		
		array<Light@> light_list;
		Light@ light = @lights;
		int light_count = 0;
		while(@light != null)
		{
			const float x = light.self.x();
			const float y = light.self.y();
			const float radius = light.radius;
			if(x - radius <= right and x + radius >= left and y - radius <= bottom and y + radius >= top)
			{
				light_list.insertLast(light);
				light_count++;
			}
			@light = @light.next;
		}
		
		tile_colours.resize((tile_count_x + 2) * 3);
		prev_tile_colours.resize(tile_colours.length());
		float prev_r = 0;
		float prev_g = 0;
		float prev_b = 0;
		
		int i = 0;
		for(int tile_y = -1; tile_y <= tile_count_y; tile_y++)
		{
			for(int tile_x = -1; tile_x <= tile_count_x; tile_x++)
			{
				float red = ambient_r;
				float green = ambient_g;
				float blue = ambient_b;
				
				if(!g.get_tile(tile_start_x + tile_x, tile_start_y + tile_y).solid())
				{
					const float x = (tile_start_x + tile_x) * GRID + HGRID;
					const float y = (tile_start_y + tile_y) * GRID + HGRID;
					for(int li = 0; li < light_count; li++)
					{
						@light = @light_list[li];
						
						const float light_x = light.self.x();
						const float light_y = light.self.y();
						const float light_radius = light.radius;
						const float dx = light_x - x;
						const float dy = light_y - y;
						if(abs(dx) <= light_radius and abs(dy) <= light_radius)
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
					}
				}
				
				if(red > 255) red = 255;
				if(green > 255) green = 255;
				if(blue > 255) blue = 255;
				
				const int ix = (tile_x + 1) * 3;
				
				if(tile_x >= 0 and tile_x < tile_count_x and tile_y >= 0 and tile_y < tile_count_y)
				{
					float r = (red + prev_r + prev_tile_colours[ix] + prev_tile_colours[ix - 3]) * 0.25;
					float g = (green + prev_g + prev_tile_colours[ix + 1] + prev_tile_colours[ix - 2]) * 0.25;
					float b = (blue + prev_b + prev_tile_colours[ix + 2] + prev_tile_colours[ix - 1]) * 0.25;
					float brightness = 255 - (r + g + b) / 3;
//					vertex_colours[i++] = (uint(brightness) << 24) + (uint(r) << 16) + (uint(g) << 8) + uint(b);
//					vertex_colours[i++] = 0xFF000000 + (uint(r) << 16) + (uint(g) << 8) + uint(b);
					vertex_colours[i++] = (uint(brightness) << 24);
				}
				
				tile_colours[ix] = red;
				tile_colours[ix + 1] = green;
				tile_colours[ix + 2] = blue;
				prev_r = red;
				prev_g = green;
				prev_b = blue;
			}
			
			array<float>@ tmp = @prev_tile_colours;
			@prev_tile_colours = @tile_colours;
			@tile_colours = @tmp;
		}
	}
	
	void draw(float sub_frame)
	{
		for(int tile_y = 0; tile_y < tile_count_y; tile_y++)
		{
			for(int tile_x = 0; tile_x < tile_count_x; tile_x++)
			{
				const float x = (tile_start_x + tile_x) * GRID;
				const float y = (tile_start_y + tile_y) * GRID;
				g.draw_quad_world(20, 20, false,
					x, y,
					x, y + GRID,
					x + GRID, y + GRID,
					x + GRID, y,
					vertex_colours[tile_y * tile_count_x + tile_x],
					vertex_colours[(tile_y + 1) * tile_count_x + tile_x],
					vertex_colours[(tile_y + 1) * tile_count_x + tile_x + 1],
					vertex_colours[tile_y * tile_count_x + tile_x + 1]);
			}
		}
	}
	
}









