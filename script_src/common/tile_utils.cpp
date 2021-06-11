// 0 Top, 1 Bottom, 2 Left, 3 Right
const array<array<float>> TILE_SIDES = {
	// FULL
	{0,0,  1,1,  0,1,  1,0},
	// BIG_1
	{0,0,  1,1,  0,1,  1,0.5},
	// SMALL_1
	{0,0.5,  1,1, 0,1, 1,1},
	// BIG_2
	{0,0,  0.5,1,  0,1, 1,0},
	// SMALL_2
	{0,0,  0,1,  0,1,  0.5,0},
	// BIG_3
	{0,0,  1,1,  0,0.5,  1,0},
	// SMALL_3
	{0,0,  1,0.5,  0,0,  1,0},
	// BIG_4
	{0.5,0,  1,1,  0,1,  1,0},
	// SMALL_4
	{1,0,  1,1,  0.5,1,  1,0},
	// BIG_5
	{0,0.5,  1,1,  0,1,  1,0},
	// SMALL_5
	{0,1,  1,1,  0,1,  1,0.5},
	// BIG_6
	{0,0,  1,1,  0.5,1,  1,0},
	// SMALL_6
	{0.5,0,  1,1,  1,1,  1,0},
	// BIG_7
	{0,0,  1,0.5, 0,1,  1,0},
	// SMALL_7
	{0,0,  1,0,  0,0.5,  1,0},
	// BIG_8
	{0,0,  1,1,  0,1,  0.5,0},
	// SMALL_8
	{0,0,  0.5,1,  0,1,  0,0},
	// HALF_A
	{0,0,  1,1,  0,1,  1,1},
	// HALF_B
	{0,0,  1,0,  0,1,  1,0},
	// HALF_C
	{0,0,  1,1,  0,0,  1,0},
	// HALF_D
	{0,1,  1,1,  0,1,  1,0},
};

void get_left_tile(int tile_x, int tile_y, const uint8 type, const int side, int &out out_x, int &out out_y)
{
	// Tile to the left (clockwise)
	int tile1_x = tile_x;
	int tile1_y = tile_y;
	
	if(side == 0) // Top
	{
		tile1_x--;
		
		if(
			type == TILE_SHAPE_BIG_1 or
			type == TILE_SHAPE_HALF_A)
		{
			tile1_y--;
		}
		else if(
			type == TILE_SHAPE_SMALL_5 or
			type == TILE_SHAPE_HALF_D)
		{
			tile1_y++;
		}
	}
	else if(side == 1) // Bottom
	{
		tile1_x++;
		
		if(
			type == TILE_SHAPE_SMALL_7 or
			type == TILE_SHAPE_HALF_B)
		{
			tile1_y--;
		}
		else if(
			type == TILE_SHAPE_BIG_3 or
			type == TILE_SHAPE_HALF_C)
		{
			tile1_y++;
		}
	}
	else if(side == 2) // Left
	{
		tile1_y++;
		
		if(type == TILE_SHAPE_BIG_4)
		{
			tile1_x--;
		}
		else if(type == TILE_SHAPE_SMALL_6)
		{
			tile1_x++;
		}
	}
	else if(side == 3) // Right
	{
		tile1_y--;
		
		if(type == TILE_SHAPE_SMALL_8)
		{
			tile1_x--;
		}
		else if(type == TILE_SHAPE_BIG_2)
		{
			tile1_x++;
		}
	}
	
	out_x = tile1_x;
	out_y = tile1_y;
}

void get_right_tile(int tile_x, int tile_y, const uint8 type, const int side, int &out out_x, int &out out_y)
{
	// Tile to the right (clockwise)
	int tile2_x = tile_x;
	int tile2_y = tile_y;
	
	if(side == 0) // Top
	{
		tile2_x++;
		
		if(
			type == TILE_SHAPE_BIG_5 or
			type == TILE_SHAPE_HALF_D)
		{
			tile2_y--;
		}
		else if(
			type == TILE_SHAPE_SMALL_1 or
			type == TILE_SHAPE_HALF_A)
		{
			tile2_y++;
		}
	}
	else if(side == 1) // Bottom
	{
		tile2_x--;
		
		if(
			type == TILE_SHAPE_SMALL_3 or
			type == TILE_SHAPE_HALF_C)
		{
			tile2_y--;
		}
		else if(
			type == TILE_SHAPE_BIG_7 or
			type == TILE_SHAPE_HALF_B)
		{
			tile2_y++;
		}
	}
	else if(side == 2) // Left
	{
		tile2_y--;
		
		if(type == TILE_SHAPE_BIG_6)
		{
			tile2_x--;
		}
		else if(type == TILE_SHAPE_SMALL_4)
		{
			tile2_x++;
		}
	}
	else if(side == 3) // Right
	{
		tile2_y++;
		
		if(type == TILE_SHAPE_SMALL_2)
		{
			tile2_x--;
		}
		else if(type == TILE_SHAPE_BIG_8)
		{
			tile2_x++;
		}
	}
	
	out_x = tile2_x;
	out_y = tile2_y;
}

void get_adjacent_tiles(int tile_x, int tile_y, const uint8 type, const int side, int &out out1_x, int &out out1_y, int &out out2_x, int &out out2_y)
{
	// Tile to the left (clockwise)
	int tile1_x = tile_x;
	int tile1_y = tile_y;
	// Tile to the right (clockwise)
	int tile2_x = tile_x;
	int tile2_y = tile_y;
	
	if(side == 0) // Top
	{
		tile1_x--;
		tile2_x++;
		
		if(
			type == TILE_SHAPE_BIG_1 or
			type == TILE_SHAPE_HALF_A)
		{
			tile1_y--;
		}
		else if(
			type == TILE_SHAPE_SMALL_5 or
			type == TILE_SHAPE_HALF_D)
		{
			tile1_y++;
		}
		
		if(
			type == TILE_SHAPE_BIG_5 or
			type == TILE_SHAPE_HALF_D)
		{
			tile2_y--;
		}
		else if(
			type == TILE_SHAPE_SMALL_1 or
			type == TILE_SHAPE_HALF_A)
		{
			tile2_y++;
		}
	}
	else if(side == 1) // Bottom
	{
		tile1_x++;
		tile2_x--;
		
		if(
			type == TILE_SHAPE_SMALL_7 or
			type == TILE_SHAPE_HALF_B)
		{
			tile1_y--;
		}
		else if(
			type == TILE_SHAPE_BIG_3 or
			type == TILE_SHAPE_HALF_C)
		{
			tile1_y++;
		}
		
		if(
			type == TILE_SHAPE_SMALL_3 or
			type == TILE_SHAPE_HALF_C)
		{
			tile2_y--;
		}
		else if(
			type == TILE_SHAPE_BIG_7 or
			type == TILE_SHAPE_HALF_B)
		{
			tile2_y++;
		}
	}
	else if(side == 2) // Left
	{
		tile1_y++;
		tile2_y--;
		
		if(type == TILE_SHAPE_BIG_4)
		{
			tile1_x--;
		}
		else if(type == TILE_SHAPE_SMALL_6)
		{
			tile1_x++;
		}
		
		if(type == TILE_SHAPE_BIG_6)
		{
			tile2_x--;
		}
		else if(type == TILE_SHAPE_SMALL_4)
		{
			tile2_x++;
		}
	}
	else if(side == 3) // Right
	{
		tile1_y--;
		tile2_y++;
		
		if(type == TILE_SHAPE_SMALL_8)
		{
			tile1_x--;
		}
		else if(type == TILE_SHAPE_BIG_2)
		{
			tile1_x++;
		}
		
		if(type == TILE_SHAPE_SMALL_2)
		{
			tile2_x--;
		}
		else if(type == TILE_SHAPE_BIG_8)
		{
			tile2_x++;
		}
	}
	
	out1_x = tile1_x;
	out1_y = tile1_y;
	out2_x = tile2_x;
	out2_y = tile2_y;
}

string get_side_name(int side)
{
	if(side == 0) // Top
	{
		return "Top";
	}
	if(side == 1) // Bottom
	{
		return "Bottom";
	}
	if(side == 2) // Left
	{
		return "Left";
	}
	if(side == 3) // Right
	{
		return "Right";
	}
	
	return "???";
}

uint8 get_matching_tile(uint8 type)
{
	if(type > TILE_SHAPE_FULL and type <= TILE_SHAPE_SMALL_8)
	{
		if((type - 1) % 2 == 0)
		{
			return type + 1;
		}
		else
		{
			return type - 1;
		}
	}
	
	return type;
}

int get_tile_angle(int angle, int side)
{
	if(angle == 0)
	{
		// Top
		// side == 0, angle = 0
		// Bottom
		if(side == 1)
		{
			angle = 180;
		}
		// Left
		else if(side == 2)
		{
			angle = -90;
		}
		// Right
		else if(side == 3)
		{
			angle = 90;
		}
	}
	
	return angle;
}

uint8 get_tile_edge(tileinfo@ tile, int side)
{
	if(side == 0) // Top
	{
		return tile.edge_top();
	}
	if(side == 1) // Bottom
	{
		return tile.edge_bottom();
	}
	if(side == 2) // Left
	{
		return tile.edge_left();
	}
	if(side == 3) // Right
	{
		return tile.edge_right();
	}
	
	return 0;
}

void set_tile_edge(tileinfo@ tile, int side, uint8 edge_bits)
{
	if(side == 0) // Top
	{
		tile.edge_top(edge_bits);
	}
	else if(side == 1) // Bottom
	{
		tile.edge_bottom(edge_bits);
	}
	else if(side == 2) // Left
	{
		tile.edge_left(edge_bits);
	}
	else if(side == 3) // Right
	{
		tile.edge_right(edge_bits);
	}
}

bool is_full_edge(int shape, int side)
{
	if(shape == TILE_SHAPE_FULL)
	{
		return true;
	}
	
	if(side == 0) // Top
	{
		return	shape == TILE_SHAPE_BIG_2 or
				shape == TILE_SHAPE_BIG_3 or
				shape == TILE_SHAPE_BIG_6 or
				shape == TILE_SHAPE_BIG_7 or
				shape == TILE_SHAPE_SMALL_3 or
				shape == TILE_SHAPE_SMALL_7 or
				shape == TILE_SHAPE_HALF_B or
				shape == TILE_SHAPE_HALF_C;
	}
	
	if(side == 1) // Bottom
	{
		return	shape == TILE_SHAPE_BIG_1 or
				shape == TILE_SHAPE_BIG_4 or
				shape == TILE_SHAPE_BIG_5 or
				shape == TILE_SHAPE_BIG_8 or
				shape == TILE_SHAPE_SMALL_1 or
				shape == TILE_SHAPE_SMALL_5 or
				shape == TILE_SHAPE_HALF_A or
				shape == TILE_SHAPE_HALF_D;
	}
	
	if(side == 2) // Left
	{
		return	shape == TILE_SHAPE_BIG_1 or
				shape == TILE_SHAPE_BIG_2 or
				shape == TILE_SHAPE_BIG_7 or
				shape == TILE_SHAPE_BIG_8 or
				shape == TILE_SHAPE_SMALL_2 or
				shape == TILE_SHAPE_SMALL_8 or
				shape == TILE_SHAPE_HALF_A or
				shape == TILE_SHAPE_HALF_B;
	}
	
	if(side == 3) // Right
	{
		return	shape == TILE_SHAPE_BIG_3 or
				shape == TILE_SHAPE_BIG_4 or
				shape == TILE_SHAPE_BIG_5 or
				shape == TILE_SHAPE_BIG_6 or
				shape == TILE_SHAPE_SMALL_4 or
				shape == TILE_SHAPE_SMALL_6 or
				shape == TILE_SHAPE_HALF_C or
				shape == TILE_SHAPE_HALF_D;
	}
	
	return false;
}

int next_side_clockwise(int side)
{
	// 0 top, 1 bottom, 2 left, 3 right
	
	if(side == 0) // top > right
	{
		return 3;
	}
	
	if(side == 1) // bottom > left
	{
		return 2;
	}
	
	if(side == 2) // left > top
	{
		return 0;
	}
	
	if(side == 3) // right > bottom
	{
		return 1;
	}
	
	return 0;
}

bool point_in_tile(scene@ g, float x, float y, float &out normal_x, float &out normal_y, int layer=19)
{
	int tile_ix = int(floor(x / 48));
	int tile_iy = int(floor(y / 48));
	
	tileinfo@ tile = g.get_tile(tile_ix, tile_iy, layer);
	
	if(!tile.solid())
	{
		return false;
	}
	
	float tile_x = (x - float(tile_ix * 48)) / 48;
	float tile_y = (y - float(tile_iy * 48)) / 48;
	
	// Right and down facing (1/1 gradient)
	const float n1 = 0.7071067811865475;
	// Right and down facing (2/1 gradient)
	const float n2x = 0.8944271909999159;
	const float n2y = 0.4472135954999579;
	
	switch(tile.type())
	{
		case TILE_SHAPE_FULL:
//			puts("FULL "+tile_x+", "+tile_y);
			if(abs(tile_x - 0.5) > abs(tile_y - 0.5))
			{
				normal_x = tile_x < 0.5 ? -1 : 1;
				normal_y = 0;
			}
			else
			{
				normal_x = 0;
				normal_y = tile_y < 0.5 ? -1 : 1;
			}
			
			return true;
		
		case TILE_SHAPE_BIG_1:
			normal_x = n2y;
			normal_y = -n2x;
			return tile_y >= tile_x * 0.5;
		case TILE_SHAPE_SMALL_1:
			normal_x = n2y;
			normal_y = -n2x;
			return tile_y >= 0.5 + tile_x * 0.5;
		
		case TILE_SHAPE_BIG_2:
			normal_x = n2x;
			normal_y = n2y;
			return tile_x <= 1 - tile_y * 0.5;
		case TILE_SHAPE_SMALL_2:
			normal_x = n2x;
			normal_y = n2y;
			return tile_x <= 0.5 - tile_y * 0.5;
		
		case TILE_SHAPE_BIG_3:
			normal_x = -n2y;
			normal_y = n2x;
			return tile_y <= 0.5 + tile_x * 0.5;
		case TILE_SHAPE_SMALL_3:
			normal_x = -n2y;
			normal_y = n2x;
			return tile_y <= tile_x * 0.5;
		
		case TILE_SHAPE_BIG_4:
			normal_x = -n2x;
			normal_y = -n2y;
			return tile_x >= 0.5 - tile_y * 0.5;
		case TILE_SHAPE_SMALL_4:
			normal_x = -n2x;
			normal_y = -n2y;
			return tile_x >= 1 - tile_y * 0.5;
			
		case TILE_SHAPE_BIG_5:
			normal_x = -n2y;
			normal_y = -n2x;
			return tile_y >= 0.5 - tile_x * 0.5;
		case TILE_SHAPE_SMALL_5:
			normal_x = -n2y;
			normal_y = -n2x;
			return tile_y >= 1 - tile_x * 0.5;
		
		case TILE_SHAPE_BIG_6:
			normal_x = -n2x;
			normal_y = n2y;
			return tile_x >= tile_y * 0.5;
		case TILE_SHAPE_SMALL_6:
			normal_x = -n2x;
			normal_y = n2y;
			return tile_x >= 0.5 + tile_y * 0.5;
		
		case TILE_SHAPE_BIG_7:
			normal_x = n2y;
			normal_y = n2x;
			return tile_y <= 1 - tile_x * 0.5;
		case TILE_SHAPE_SMALL_7:
			normal_x = n2y;
			normal_y = n2x;
			return tile_y <= 0.5 - tile_x * 0.5;
		
		case TILE_SHAPE_BIG_8:
			normal_x = n2x;
			normal_y = -n2y;
			return tile_x <= 0.5 + tile_y * 0.5;
		case TILE_SHAPE_SMALL_8:
			normal_x = n2x;
			normal_y = -n2y;
			return tile_x <= tile_y * 0.5;
		
		case TILE_SHAPE_HALF_A:
			normal_x = n1;
			normal_y = -n1;
			return tile_x <= tile_y;
		case TILE_SHAPE_HALF_B:
			normal_x = n1;
			normal_y = n1;
			return tile_x <= 1 - tile_y;
		case TILE_SHAPE_HALF_C:
			normal_x = -n1;
			normal_y = n1;
			return tile_x >= tile_y;
		case TILE_SHAPE_HALF_D:
			normal_x = -n1;
			normal_y = -n1;
			return tile_x >= 1 - tile_y;
	}
	
	return false;
}