class script
{
	
	int num_players;
	array<controllable@> players;
	
	int max_tiles_to_check_below = 5;
	scene@ g;
	
	script()
	{
		num_players = num_cameras();
		players.resize(num_players);
		
		@g = get_scene();
	}
	
	void step(int entities)
	{
		for(int i = 0; i < num_players; i++)
		{
			controllable@ player = players[i];
			
			if(player is null)
			{
				@player = controller_controllable(i);
				if(player is null) continue;
			}
			
			if(player.x_intent() != 0) continue;
			
			float player_x = player.x();
			float player_y = player.y();
			int player_tile_x = int(floor(player_x / 48));
			int player_tile_y = int(floor(player_y / 48));
			
			for(int oy = 0; oy < max_tiles_to_check_below; oy++)
			{
				tileinfo@ tile = g.get_tile(player_tile_x, player_tile_y + oy);
				if(tile.solid() && !tile.is_dustblock()) break;
				
				int found_spiked_tile_side = 0;
				// Left tile
				uint filth = g.get_tile_filth(player_tile_x - 1, player_tile_y + oy).right();
				if(filth >= 9 && filth <= 13)
				{
					@tile = g.get_tile(player_tile_x - 1, player_tile_y + oy);
					int type = tile.type();
					if(type == 0 || type == 4 || type == 7 || type == 8 || type == 9 || type == 11 || type == 12 || type == 19 || type == 20)
					{
						found_spiked_tile_side = -1;
					}
				}
				
				// Right tile
				if(found_spiked_tile_side == 0)
				{
					filth = g.get_tile_filth(player_tile_x + 1, player_tile_y + oy).right();
					if(filth >= 9 && filth <= 13)
					{
						@tile = g.get_tile(player_tile_x - 1, player_tile_y + oy);
						int type = tile.type();
						if(type == 0 || type == 1 || type == 3 || type == 4 || type == 13 || type == 15 || type == 16 || type == 17 || type == 18)
						{
							found_spiked_tile_side = 1;
						}
					}
				}
				
				if(found_spiked_tile_side != 0)
				{
					player.set_speed_xy(player.x_speed() * 0.5, player.y_speed());
					const float dx = (player_tile_x * 48 + 24) - player_x;
					player.x(player_x + dx * 0.25);
				}
			}
		}// end player loop
	}
	
}