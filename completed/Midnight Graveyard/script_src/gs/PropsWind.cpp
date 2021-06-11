class PropsWind : callback_base
{
	
	float cell_size = 32 * 48;
	array<int> step_cells;
	array<string> step_cells_hash;
	
	dictionary cells;
	
	float t = 0;
	float wind_speed = 0.05;
	float wind_direction = 1;
	float wind_strength = 4;
	float speed_factor = 1;
	float strength_factor = 1;
	float target_speed_factor = 1;
	float target_strength_factor = 1;
	
	dictionary props = {
		// Grass
		{'1/5/12', PropSettings(1.25, 1)},
		{'1/5/13', PropSettings(1.25, 1)},
		{'1/5/14', PropSettings(1.25, 1)},
		{'2/5/4',  PropSettings(1.25, 1)},
		// "Leaves" (small foliage)
		{'2/5/1',  PropSettings(1.5, 6)},
		// Chains >>
		{'1/2/2',  PropSettings(0.3, 0.65)},
		{'1/2/1',  PropSettings(0.3, 0.65)},
		{'1/2/3',  PropSettings(0.3, 0.65)}
	};
	
	PropsWind()
	{
		add_broadcast_receiver("wind_strength", this, "on_wind_strength");
	}
	
	void on_wind_strength(string id, message@ msg)
	{
		target_speed_factor = msg.get_float("strength");
		target_strength_factor = msg.get_float("speed");
	}
	
	void player(controllable@ player)
	{
		const int x = int(floor(player.x() / cell_size));
		const int y = int(floor(player.y() / cell_size));
		for(int ix = x - 1; ix <= x + 1; ix++)
		{
			for(int iy = y - 1; iy <= y + 1; iy++)
			{
				const string key = ix + "," + iy;
				if(step_cells_hash.find(key) < 0)
				{
					step_cells_hash.insertLast(key);
					step_cells.insertLast(ix);
					step_cells.insertLast(iy);
				}
			}
		}
	}
	
	void checkpoint_load()
	{
		step_cells.resize(0);
		step_cells_hash.resize(0);
		cells.deleteAll();
	}
	
	void step()
	{
		for(uint i = 0; i < step_cells.length(); i += 2)
		{
			const int x = step_cells[i];
			const int y = step_cells[i + 1];
			
			const string key = x + "," + y;
			PropsWindCell@ cell = cast<PropsWindCell>(cells[key]);
			if(cell is null)
			{
				@cells[key] = @cell = PropsWindCell(x, y, cell_size, props);
			}
			
			cell.step(t, wind_direction, wind_strength * speed_factor);
		}
		
		t += wind_speed * speed_factor;
		step_cells.resize(0);
		step_cells_hash.resize(0);
		
		speed_factor += (target_speed_factor - speed_factor) * 0.025;
		strength_factor += (target_strength_factor - strength_factor) * 0.025;
	}
	
}

class PropsWindCell
{
	array<PropData@> prop_list;
	
	PropsWindCell(int x, int y, float cell_size, dictionary props)
	{
		scene@ g = get_scene();
		const float cx = x * cell_size;
		const float cy = y * cell_size;
		const int prop_count = g.get_prop_collision(
			cy, cy + cell_size,
			cx, cx + cell_size 
		);
		for(int i = 0; i < prop_count; i++)
		{
			prop@ p = g.get_prop_collision_index(i);
			const string key = p.prop_set() + "/" + p.prop_group() + "/" + p.prop_index();
			PropSettings@ settings = cast<PropSettings>(props[key]);
			if(settings is null) continue;
			
			prop_list.insertLast(PropData(p, settings));
		}
	}
	
	void step(float t, float wind_direction, float wind_strength)
	{
		const uint count = prop_list.length();
		for(uint i = 0; i < count; i++)
		{
			
			PropData@ data = prop_list[i];
			prop@ p = data.prop;
			const float pr = (sin(p.x() + p.y()) * 0.25 + 1) * data.settings.wind_speed;
			const float wind = sin(t * pr + (p.x() + p.y()) * 0.1f) * wind_direction * wind_strength * data.settings.wind_strength;
			p.rotation(data.rotation + wind);
		}
	}
	
}

class PropSettings
{
	
	float wind_strength;
	float wind_speed;
	
	PropSettings(float wind_strength=1, float wind_speed=1)
	{
		this.wind_strength = wind_strength;
		this.wind_speed = wind_speed;
	}
	
}

class PropData
{
	
	uint id;
	float rotation;
	PropSettings@ settings;
	prop@ prop = null;
	
	PropData(prop@ p, PropSettings@ settings)
	{
		@this.prop = p;
		id = p.id();
		rotation = p.rotation();
		@this.settings = settings;
	}
	
}

class Windtrigger : trigger_base
{
	
	[text] float strength = 1;
	[text] float speed = 1;
	
	Windtrigger()
	{}
	
	void init(script@ s, scripttrigger@ self)
	{
		
	}
	
	void activate(controllable@ e)
	{
		for(uint i = 0; i < num_cameras(); i++)
		{
			entity@ player = controller_entity(i);
			if(@player != null and e.is_same(player))
			{
				message@ msg = create_message();
				msg.set_float("strength", strength);
				msg.set_float("speed", speed);
				broadcast_message("wind_strength", msg);
				return;
			}
		}
	}
	
}