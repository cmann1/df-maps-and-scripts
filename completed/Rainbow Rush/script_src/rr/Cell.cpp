class Cell
{
	
	array<float> edges;
	array<float> tiles;
	
	Cell(scene@ g, int cell_x, int cell_y, int cell_size, int rainbow_tile_layer)
	{
		const int tile_x = cell_x * cell_size;
		const int tile_y = cell_y * cell_size;
		const int end_x = tile_x + cell_size;
		const int end_y = tile_y + cell_size;
		
		for(int x = tile_x; x < end_x; x++)
		{
			for(int y = tile_y; y < end_y; y++)
			{
				tileinfo@ tile = g.get_tile(x, y, rainbow_tile_layer);
				if(!tile.solid() or tile.is_dustblock()) continue;
				
				const float tx = x * TILE2PIXEL;
				const float ty = y * TILE2PIXEL;
				const uint8 type = tile.type();
				float x1, y1, x2, y2;
				
				if(tile.edge_bottom() != 0)
				{
					if(get_tile_bottom_edge(type, x1, y1, x2, y2))
					{
						edges.insertLast(tx + x1); edges.insertLast(ty + y1);
						edges.insertLast(tx + x2); edges.insertLast(ty + y2);
					}
				}
				if(tile.edge_top() != 0)
				{
					if(get_tile_top_edge(type, x1, y1, x2, y2))
					{
						edges.insertLast(tx + x1); edges.insertLast(ty + y1);
						edges.insertLast(tx + x2); edges.insertLast(ty + y2);
					}
				}
				if(tile.edge_left() != 0)
				{
					if(get_tile_left_edge(type, x1, y1, x2, y2))
					{
						edges.insertLast(tx + x1); edges.insertLast(ty + y1);
						edges.insertLast(tx + x2); edges.insertLast(ty + y2);
					}
				}
				if(tile.edge_right() != 0)
				{
					if(get_tile_right_edge(type, x1, y1, x2, y2))
					{
						edges.insertLast(tx + x1); edges.insertLast(ty + y1);
						edges.insertLast(tx + x2); edges.insertLast(ty + y2);
					}
				}
				
				float x3, y3, x4, y4;
				get_tile_quad(type, x1, y1, x2, y2, x3, y3, x4, y4);
				tiles.insertLast(tx + x1); tiles.insertLast(ty + y1);
				tiles.insertLast(tx + x2); tiles.insertLast(ty + y2);
				tiles.insertLast(tx + x3); tiles.insertLast(ty + y3);
				tiles.insertLast(tx + x4); tiles.insertLast(ty + y4);
			}
		}
	}
	
	void step(float t)
	{
		
	}
	
	void draw(scene@ g, float t, int rainbow_draw_layer, int rainbow_draw_sublayer, float edge_width)
	{
		for(int i = 0, count = int(tiles.size()) - 1; i < count; i += 8)
		{
			const float x1 = tiles[i];
			const float y1 = tiles[i + 1];
			const float x2 = tiles[i + 2];
			const float y2 = tiles[i + 3];
			const float x3 = tiles[i + 4];
			const float y3 = tiles[i + 5];
			const float x4 = tiles[i + 6];
			const float y4 = tiles[i + 7];
			
			const uint c = 0xFF000000 | hsl_to_rgb(abs(t + x1 * 0.001 + y1 * 0.001) * 0.1 % 1, 1, 0.75);
//			g.draw_quad_world(rainbow_draw_layer, rainbow_draw_sublayer, false, x1, y1, x2, y2, x3, y3, x4, y4, c, c, c, c);
		}
		
		for(int i = 0, count = int(edges.size()) - 1; i < count; i += 4)
		{
			const float x1 = edges[i];
			const float y1 = edges[i + 1];
			const float x2 = edges[i + 2];
			const float y2 = edges[i + 3];
			
//			g.draw_line_world(rainbow_draw_layer, rainbow_draw_sublayer, x1, y1, x2, y2, edge_width, 0xFF000000 | hsl_to_rgb(abs(t - x1 * 0.001 + y1 * 0.001) * 0.1 % 1, 1, 0.65));
		}
	}
	
}