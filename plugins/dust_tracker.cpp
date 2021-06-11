class script
{
	
	scene@ g;
	int cell_tile_count = 14;
	float cell_size = cell_tile_count * 48;
	int check_y = -1;
	controllable@ player = null;
	camera@ cam;
	
	dictionary dust;
	dictionary dust_checkpoint;
	dictionary enemies;
	dictionary enemies_checkpoint;
	int dust_view_index = -2;
	int dust_current_view_index = -2;
	array<string>@ dust_keys;
	int num_dust_keys = 0;
	array<string>@ enemies_keys;
	int num_enemies_keys = 0;
	
	script()
	{
		@g = get_scene();
	}
	
	void checkpoint_save()
	{
		dust_checkpoint = dust;
		enemies_checkpoint = enemies;
	}
	
	void checkpoint_load()
	{
		dust = dust_checkpoint;
		enemies = enemies_checkpoint;
		@player = null;
		@cam = null;
		dust_view_index = -2;
		dust_current_view_index = -2;
		@dust_keys = null;
		num_dust_keys = 0;
		@dust_keys = enemies_keys;
		num_enemies_keys = 0;
	}
	
	void entity_on_remove(entity @e)
	{
		if(enemies.exists(e.id() + ""))
		{
//			puts("DELETING " + e.id());
			enemies.delete(e.id() + "");
		}
	}
	
	void move_cameras()
	{
		if(dust_view_index > -1 and dust_current_view_index != dust_view_index)
		{
			float x;
			float y;
			
			if(dust_view_index < num_dust_keys)
			{
				const string key = dust_keys[dust_view_index];
				const int index = key.findFirst(",");
				x = parseFloat(key.substr(0, index)) * 48;
				y = parseFloat(key.substr(index + 1)) * 48;
			}
			else
			{
				const EnemyLoc@ e = cast<EnemyLoc>(enemies[enemies_keys[dust_view_index - num_dust_keys]]);
				x = e.x;
				y = e.y;
			}
			
			cam.x(x);
			cam.y(y);
			dust_current_view_index = dust_view_index;
		}
	}
	
	void step(int entities)
	{
		if(player is null)
		{
			@player = controller_controllable(0);
			@cam = get_camera(0);
		}
		else
		{
			if(player.taunt_intent() == 1)
			{
				if(dust_view_index == -2)
				{
					puts("==== MISSED DUST ==============================================");
					@dust_keys = dust.getKeys();
					num_dust_keys = int(dust_keys.length());
					int missed_dust = 0;
					for(int i = 0; i < num_dust_keys; i++)
					{
						const string key = dust_keys[i];
						const int index = key.findFirst(",");
						const float x = parseFloat(key.substr(0, index)) * 48;
						const float y = parseFloat(key.substr(index + 1)) * 48;
						const int dust_count = int(dust[key]);
						missed_dust += dust_count;
						puts(key + " (" + x + "," + y + ")");
					}
					puts("   === TOTAL: " + missed_dust);
					
					puts("==== MISSED ENEMIES ==============================================");
					@enemies_keys = enemies.getKeys();
					num_enemies_keys = int(enemies_keys.length());
					for(int i = 0; i < num_enemies_keys; i++)
					{
						const string key = enemies_keys[i];
						const EnemyLoc@ e = cast<EnemyLoc>(enemies[key]);
						puts("  " + key + " (" + e.x + "," + e.y + ")");
					}
				}
				else if(dust_view_index == -1)
				{
					@dust_keys = dust.getKeys();
					num_dust_keys = int(dust_keys.length());
					@enemies_keys = enemies.getKeys();
					num_enemies_keys = int(enemies_keys.length());
					cam.script_camera(true);
				}
				
				if(++dust_view_index >= num_dust_keys + num_enemies_keys)
				{
					dust_view_index = -2;
					dust_current_view_index = -2;
					@dust_keys = null;
					num_dust_keys = 0;
					@dust_keys = enemies_keys;
					num_enemies_keys = 0;
					cam.script_camera(false);
				}
			}
			
			const int player_cell_x = int(floor(player.x() / cell_size));
			const int player_cell_y = int(floor(player.y() / cell_size));
			const int cell_y = (player_cell_y + check_y) * cell_tile_count;
			for(int check_x = -1; check_x <= 1; check_x++)
			{
				const int cell_x = (player_cell_x + check_x) * cell_tile_count;
				for(int tx = 0; tx < cell_tile_count; tx++)
				{
					const int tile_x = cell_x + tx;
					for(int ty = 0; ty < cell_tile_count; ty++)
					{
						const int tile_y = cell_y + ty;
						const string key = tile_x + "," + tile_y;
						int dust_count = dust.exists(key) ? int(dust[key]) : 0;
						int new_dust_count = 0;
						
						tileinfo@ tile = g.get_tile(tile_x, tile_y);
						if(tile.solid())
						{
							if(tile.is_dustblock())
							{
								new_dust_count = 1;
							}
							else
							{
								tilefilth@ filth = g.get_tile_filth(tile_x, tile_y);
								const uint8 top = filth.top();
								const uint8 bottom = filth.bottom();
								const uint8 left = filth.left();
								const uint8 right = filth.right();
								if(top > 0 and top <= 5) new_dust_count++;
								if(bottom > 0 and bottom <= 5) new_dust_count++;
								if(left > 0 and left <= 5) new_dust_count++;
								if(right > 0 and right <= 5) new_dust_count++;
							}
						}
						else
						{
							
						}
						
						if(new_dust_count != dust_count)
						{
							if(new_dust_count == 0)
							{
//								puts("Removing dust at " + key);
								dust.delete(key);
							}
							else 
							{
//								puts("Adding dust at " + key);
								dust[key] = new_dust_count;
							}
						}
					}
				}
			}
			
			if(++check_y > 1)
			{
				check_y = -1;
			}
		}
		
		for(int i = 0; i < entities; i++)
		{
			entity@ e = entity_by_index(i);
			if(e.type_name().substr(0, 5) == "enemy" and !enemies.exists(e.id() + ""))
			{
//				puts("ADDING " + e.id());
				@enemies[e.id() + ""] = EnemyLoc(e.x(), e.y());
			}
		}
	}
	
}

class EnemyLoc
{
	float x;
	float y;
	EnemyLoc(float x, float y)
	{
		this.x = x;
		this.y = y;
	}
}