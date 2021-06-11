scene@ g;
controllable@ player;

int start_x = 2;
int start_y = -10;
int num_columns = 20;
int num_rows = 10;

int t = 0;
int step_frames = 10;

float player_y = 0;
float player_min = -(num_rows - 1) * 48;
float player_max = 0;
float player_speed = 8;
bool player_fire_down = false;

uint next_colour = 0;
uint max_colour = 4;

array<array<Block@>> blocks(num_columns, array<Block@>(num_rows));

class Block{
	int x;
	int y;
	uint colour;
	bool in_world = false;

	Block(int x, int y, uint colour){
		this.x = x;
		this.y = y;
		this.colour = colour;
	}
	
	void remove(){
		g.set_tile(start_x + x, start_y + y, 16, false, 0, 1, 1, 1);
	}
	
	void add(){
		g.set_tile(start_x + x, start_y + y, 16, true, 0, 5, 1, colour + 1);
	}
}

void init(){
	puts("INIT");
	@g = get_scene();
}

void checkpoint_save() {
	puts("CHECKPOINT SAVE1");
}

void checkpoint_load() {
	puts("CHECKPOINT LOAD1");
	@player = null;
	player_y = -4 * 48;
	t = 0;
	
	for(int x=0; x<num_columns; x++){
		array<Block@> column = blocks[x];
		for(int y=0; y<num_rows; y++){
			Block@ block = column[y];
			
			if(@block != null){
				block.remove();
				@blocks[x][y] = null;
				
			}
		}
	}
}

void step(int entities){
	if(@player == null){
		entity@e = controller_entity(0);
		if(@e != null){
			@player = e.as_controllable();
		}
	}
	else{
		if(player.y_intent() != 0)
			player_y += player.y_intent() * player_speed;
		
		if(player_y < player_min)
			player_y = player_min;
		else if(player_y > player_max)
			player_y = player_max;
		
		if(player.light_intent() != 0){
			if(!player_fire_down){
				fire_block();
				player_fire_down = true;
			}
		}
		else{
			player_fire_down = false;
		}
	}
}

void step_post(int entities){
	if (@player != null){
		player.x(0);
		player.y(player_y);
	}
	
	if(t++ == step_frames){
		for(int x=num_columns-1; x>=0; x--){
			array<Block@> column = blocks[x];
			for(int y=0; y<num_rows; y++){
				// g.set_tile(start_x + x, start_y + y, 16, false, 0, 1, 1, 1);
				Block@ block = column[y];
				
				if(@block != null){
					if(x < num_columns -1 and @blocks[x + 1][y] == null){
						@blocks[x][y] = null;
						@blocks[x + 1][y] = block;
						block.remove();
						block.x++;
						block.add();
					}
				}
			}
		}
		
		for(int x=num_columns-1; x>=0; x--){
			array<Block@> column = blocks[x];
			bool full_column = true;
			for(int y=0; y<num_rows; y++){
				if(@column[y] == null){
					full_column = false;
					break;
				}
			}
			
			if(full_column){
				for(int y=0; y<num_rows; y++){
					Block@ block = column[y];
					block.remove();
					@blocks[x][y] = null;
				}
			}
			else{
				break;
			}
		}
		
		t = 0;
	}
	
	// for(uint n = 0; n < next_active_block; n++ ){
		// Block@ block = active_blocks[n];
		
		// if(block.in_world)
			// g.set_tile(block.x, block.y, 16, false, 0, 1, 1, 1);
		
		// block.x += 1;
		// g.set_tile(block.x, block.y, 16, true, 0, 1, 1, 1);
		// block.in_world = true;
	// }
}

void fire_block(){
	uint block_x = 0;
	uint block_y = int((player_y - 48) / 48) - start_y;
	
	if(@blocks[block_x][block_y] == null){
		Block@ block = @blocks[block_x][block_y] = Block(block_x, block_y, next_colour++);
		block.add();
		if(next_colour > max_colour)
			next_colour = 0;
	}
	// Block@ new_block = Block(2, int((player_y - 48) / 48));
	// @active_blocks[next_active_block++] = new_block;
}