#include '../lib/drawing/common.cpp';
#include '../lib/tiles/get_tile_edge_points.cpp';

class TileData
{
	
	float x;
	float y;
	uint8 type;
	bool is_dustblock;
	int dust_count;
	bool dust_top;
	bool dust_bottom;
	bool dust_left;
	bool dust_right;
	
	void update(scene@ g, const int tile_x, const int tile_y)
	{
		x = tile_x * 48;
		y = tile_y * 48;
		dust_count = 0;
		
		tileinfo@ tile = g.get_tile(tile_x, tile_y);
		
		if(!tile.solid())
			return;
		
		type = tile.type();
		is_dustblock = tile.is_dustblock();
		
		if(is_dustblock)
		{
			dust_count = 1;
		}
		else
		{
			tilefilth@ filth = g.get_tile_filth(tile_x, tile_y);
			
			const uint8 top = filth.top();
			const uint8 bottom = filth.bottom();
			const uint8 left = filth.left();
			const uint8 right = filth.right();
			
			dust_top = top > 0 and top <= 5;
			dust_bottom = bottom > 0 and bottom <= 5;
			dust_left = left > 0 and left <= 5;
			dust_right= right > 0 and right <= 5;
			
			if(dust_top) dust_count++;
			if(dust_bottom) dust_count++;
			if(dust_left) dust_count++;
			if(dust_right) dust_count++;
		}
	}
	
	void render(scene@ g, sprites@ spr,
		const float view_x, const float view_y,
		const float screen_x1, const float screen_y1, const float screen_x2, const float screen_y2,
		const float view_x1, const float view_y1, const float view_x2, const float view_y2)
	{
		const float x1 = x;
		const float y1 = y;
		const float x2 = x1 + 48;
		const float y2 = y1 + 48;
		
		// Dust
		if(x1 <= screen_x2 && x2 >= screen_x1 && y1 <= screen_y2 && y2 >= screen_y1)
		{
			if(is_dustblock)
			{
				spr.draw_world(22, 20, 'dust_block', 0, 0, x1, y1, 0, 1, 1, 0xffffffff);
			}
			else
			{
				float edge_x1;
				float edge_y1;
				float edge_x2;
				float edge_y2;
				
				if(get_tile_top_edge_points(type, edge_x1, edge_y1, edge_x2, edge_y2, x1, y1))
					draw_edge(g, edge_x1, edge_y1, edge_x2, edge_y2, dust_top);
				
				if(get_tile_bottom_edge_points(type, edge_x1, edge_y1, edge_x2, edge_y2, x1, y1))
					draw_edge(g, edge_x1, edge_y1, edge_x2, edge_y2, dust_bottom);
				
				if(get_tile_left_edge_points(type, edge_x1, edge_y1, edge_x2, edge_y2, x1, y1))
					draw_edge(g, edge_x1, edge_y1, edge_x2, edge_y2, dust_left);
				
				if(get_tile_right_edge_points(type, edge_x1, edge_y1, edge_x2, edge_y2, x1, y1))
					draw_edge(g, edge_x1, edge_y1, edge_x2, edge_y2, dust_right);
			}
		}
		
		draw_dust_arrow(g, spr, 'arrow_red', x1, y1, x2, y2, view_x, view_y, view_x1, view_y1, view_x2, view_y2);
	}
	
	private void draw_edge(scene@ g, float x1, float y1, float x2, float y2, bool dust)
	{
		uint sub_layer = dust ? 23 : 22;
		uint outer_colour = dust ? TILE_OUTER_COLOUR : TILE_CLEAN_OUTER_COLOUR;
		uint inner_colour = dust ? TILE_INNER_COLOUR : TILE_CLEAN_INNER_COLOUR;
		float outer_width = dust ? TILE_OUTER_WIDTH : TILE_OUTER_WIDTH * 0.8;
		float inner_width = dust ? TILE_INNER_WIDTH : TILE_INNER_WIDTH * 0.8;
		
		g.draw_line_world(22, sub_layer, x1, y1, x2, y2, outer_width, outer_colour);
		g.draw_line_world(22, sub_layer, x1, y1, x2, y2, inner_width, inner_colour);
	}
	
}