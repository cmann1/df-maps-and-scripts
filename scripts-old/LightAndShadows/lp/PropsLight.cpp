class PropsLight
{
	scene@ g;
	float cell_size = 32 * 48;
	
	dictionary cells;
	
	dictionary props = {
		{'1/22/4', PropSettings(-100 + 79.5, -170 + 79.5, 350, 0xffffe1)}, // Moon
		{'2/22/4', PropSettings(-109 + 93.5, -181 + 93, 450, 0xFFFFFF)}, // Sun
		
		{'1/8/1',  PropSettings(0, 0, 400, 0xffe0b3)}, // Candle
		{'1/8/2',  PropSettings(0, 0, 400, 0xffe0b3)}, // Light1 front
		{'1/8/3',  PropSettings(-30, 0, 400, 0xffe0b3)}, // Light1 side
		{'1/8/4',  PropSettings(0, 0, 400, 0xffe0b3)}, // Light2 front
		{'1/8/5',  PropSettings(-25, 0, 400, 0xffe0b3)}, // Light2 side
		{'1/8/6',  PropSettings(0, 0, 400, 0xffe0b3)}, // Lamp
		{'1/8/7',  PropSettings(0, -15, 400, 0xffe0b3)}, // Torch front
		{'1/8/8',  PropSettings(-15, -15, 400, 0xffe0b3)}, // Torch side
		{'1/8/9',  PropSettings(0, 25, 400, 0xffe0b3)}, // Fireplace
		{'1/8/10', PropSettings(-95, -100, 250, 0xbcd3e7)}, // Console
		{'1/8/11', PropSettings(-60, -30, 300, 0xbee0fa)}, // Computer
		{'2/8/1',  PropSettings(0, -20, 500, 0xfdefdf)}, // Forest lamp pole
		{'2/8/2',  PropSettings(5, 5, 500, 0xfdefdf)}, // Forest lamp
		{'3/8/1',  PropSettings(175, 80, 600, 0xFFFFFF)}, // City lamp pole
		{'3/8/2',  PropSettings(175, 80, 600, 0xFFFFFF)} // City lamp
	};
	
	PropsLight()
	{
		@g = get_scene();
	}
	
	void checkpoint_load()
	{
		cells.deleteAll();
	}
	
	void player(controllable@ player)
	{
		const int x = int(floor(player.x() / cell_size));
		const int y = int(floor(player.y() / cell_size));
		for(int ix = x - 1; ix <= x + 1; ix++)
		{
			for(int iy = y - 1; iy <= y + 1; iy++)
			{
				const string cell_key = ix + "," + iy;
				if(!cells.exists(cell_key))
				{
					const float cx = ix * cell_size;
					const float cy = iy * cell_size;
					const int prop_count = g.get_prop_collision(
						cy, cy + cell_size,
						cx, cx + cell_size 
					);
					
					for(int i = 0; i < prop_count; i++)
					{
						prop@ p = g.get_prop_collision_index(i);
						if(p.layer() < 12) continue;
						
						const string key = p.prop_set() + "/" + p.prop_group() + "/" + p.prop_index();
						PropSettings@ settings = cast<PropSettings>(props[key]);
						if(settings is null) continue;
						
						Light@ light = Light();
						light.colour = settings.colour;
						light.radius = settings.radius * (abs(p.scale_x()) + abs(p.scale_y())) / 2;
						entity@ le = create_scripttrigger(light).as_entity();
						float offset_x = settings.offset_x * p.scale_x();
						float offset_y = settings.offset_y * p.scale_y();
						rotate(offset_x, offset_y, p.rotation() * DEG2RAD, offset_x, offset_y);
						le.set_xy(p.x() + offset_x, p.y() + offset_y);
						g.add_entity(le, false);
					}
					
					cells[cell_key] = true;
				}
			}
		}
	}
	
}

class PropSettings
{
	
	float offset_x;
	float offset_y;
	float radius;
	uint colour;
	
	PropSettings(float offset_x, float offset_y, float radius, uint colour)
	{
		this.offset_x = offset_x;
		this.offset_y = offset_y;
		this.radius = radius;
		this.colour = colour;
	}
	
}