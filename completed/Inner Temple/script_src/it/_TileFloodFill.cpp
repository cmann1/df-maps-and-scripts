class TileFloodFill : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	[text] array<TilePos> tile;
	
	array<array<int>> tile_queue;
	array<int> tile_queue_size;
	int tile_queue_count;
	
	[text] int layer = 14;
	[text] int sprite_set = 1;
	[text] int sprite_tile = 20;
	
	dictionary processed_tiles;
	
	array<array<int>> tiles_out_tmp;
	int total_queue = 0;
	
	TileFloodFill()
	{
		@g = get_scene();
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.self = @self;
		
		if(script.in_game)
		{
			tile_queue_count = int(tile.length());
			tile_queue.resize(tile_queue_count);
			tile_queue_size.resize(tile_queue_count);
			tiles_out_tmp.resize(tile_queue_count);
			
			for(int i = 0; i < tile_queue_count; i++)
			{
				const int tx = int(floor(tile[i].x / 48));
				const int ty = int(floor(tile[i].y / 48));
				tileinfo@ tile = g.get_tile(tx, ty, layer);
				tile_queue[i].resize(tile_queue_size[i] + 3);
				tile_queue[i][tile_queue_size[i]++] = tx;
				tile_queue[i][tile_queue_size[i]++] = ty;
				tile_queue[i][tile_queue_size[i]++] = tile.type();
				total_queue++;
			}
		}
	}
	
	void step()
	{
		for(int i = 0; i < tile_queue_count; i++)
		{
			if(tile_queue_size[i] == 0) continue;
			
			int ttype = tile_queue[i][--tile_queue_size[i]];
			int ty = tile_queue[i][--tile_queue_size[i]];
			int tx = tile_queue[i][--tile_queue_size[i]];
			total_queue--;
			tileinfo@ tile = g.get_tile(tx, ty, layer);
			
			processed_tiles[tx + '.' + ty] = true;
			tiles_out_tmp[i].insertLast(tx);
			tiles_out_tmp[i].insertLast(ty);
			tiles_out_tmp[i].insertLast(ttype);
			
			const array<int> side = {-1,0, 1,0, 0,-1, 0,1};
			for(int j = 0; j < 8; j += 2)
			{
				int sx = tx + side[j];
				int sy = ty + side[j + 1];
				
				if(processed_tiles.exists(sx + '.' + sy)) continue;
				
				@tile = g.get_tile(sx, sy, layer);
				if(tile.solid() and sprite_set == int(tile.sprite_set()) and sprite_tile == int(tile.sprite_tile()))
				{
					tile_queue[i].resize(tile_queue_size[i] + 3);
					tile_queue[i][tile_queue_size[i]++] = sx;
					tile_queue[i][tile_queue_size[i]++] = sy;
					tile_queue[i][tile_queue_size[i]++] = tile.type();
					total_queue++;
				}
			}
		}
		
		if(total_queue == 0)
		{
			string out_str = 'array<array<int>> tiles = {';
			for(int i = 0; i < tile_queue_count; i++)
			{
				out_str += '{';
				array<int>@ queue = tiles_out_tmp[i];
				for(int j = 0; j < int(queue.length()); j += 3)
				{
					if(j > 0) out_str += ', ';
					out_str += queue[j] + ',' + queue[j + 1] + ',' + queue[j + 2];
				}
				out_str += '},\n';
			}
			out_str += '};';
			puts(out_str);
			total_queue = -1;
		}
	}
	
}

class TilePos
{
	[position,layer:19,y:y] float x;
	[hidden] float y;
}