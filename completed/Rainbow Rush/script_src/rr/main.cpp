#include '../lib/std.cpp';
#include '../lib/tiles/TileShape.cpp';
#include '../lib/tiles/get_tile_edge.cpp';
#include '../lib/tiles/get_tile_quad.cpp';
#include '../lib/colour.cpp';

#include 'Cell.cpp';
#include 'RainbowStream.cpp';

const int RAINBOW_TILE_LAYER = 19;

class script
{
	
	scene@ g;
	int num_players;
//	array<controllable@> players;
	
//	int cell_size = 16;
//	array<int> step_cells;
//	dictionary step_cells_hash;
//	dictionary cells;
	
	[text] float speed = 2;
	[text] int rainbow_tile_layer = 19;
	[text] int rainbow_draw_layer = 17;
	[text] int rainbow_draw_sublayer = 19;
	[text] int edge_width = 20;
	
	float t = 0;
	
	script()
	{
		@g = get_scene();
		num_players = num_cameras();
//		players.resize(num_players);
	}
	
	void build_sprites(message@ msg)
	{
		
	}
	
	void checkpoint_load()
	{
//		step_cells.resize(0);
//		step_cells_hash.deleteAll();
//		cells.deleteAll();
	}
	
//	void step(int entities)
//	{
//		step_cells.resize(0);
//		step_cells_hash.deleteAll();
		
//		controllable@ player;
//		for(int i = 0; i < num_players; i++)
//		{
//			if((@player = players[i]) != null)
//			{
//				step_player(player);
//			}
//			else
//			{
//				entity@ e = controller_entity(i);
//				@players[i] = @player = (@e != null ? e.as_controllable() : null);
//			}
//		}
//		
//		update();
//	}
	
//	void draw(float sub_frame)
//	{
//		for(int i = int(step_cells.length()) - 2; i >= 0; i -= 2)
//		{
//			const int x = step_cells[i];
//			const int y = step_cells[i + 1];
//			
//			const string key = x + ',' + y;
//			
//			Cell@ cell = cast<Cell>(cells[key]);
//			if(cell is null) continue;
//			
//			cell.draw(g, t, rainbow_draw_layer, rainbow_draw_sublayer, edge_width);
//		}
//	}
	
//	void update()
//	{
//		for(int i = int(step_cells.length()) - 2; i >= 0; i -= 2)
//		{
//			const int x = step_cells[i];
//			const int y = step_cells[i + 1];
//			
//			const string key = x + ',' + y;
//			Cell@ cell = cast<Cell>(cells[key]);
//			if(cell is null)
//			{
//				@cells[key] = @cell = Cell(g, x, y, cell_size, rainbow_tile_layer);
//			}
//			
//			cell.step(t);
//		}
//		
//		t += speed * DT;
//	}
	
//	void step_player(controllable@ player)
//	{
//		const int x = int(floor(player.x() / (cell_size * 48)));
//		const int y = int(floor(player.y() / (cell_size * 48)));
//		for(int ix = x - 2; ix <= x + 2; ix++)
//		{
//			for(int iy = y - 2; iy <= y + 2; iy++)
//			{
//				const string key = ix + ',' + iy;
//				if(!step_cells_hash.exists(key))
//				{
//					step_cells_hash[key] = true;
//					step_cells.insertLast(ix);
//					step_cells.insertLast(iy);
//				}
//			}
//		}
//	}
	
}