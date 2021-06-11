scene@ g;
controllable@ player;

int cursor_x = 0;
int cursor_y = 0;
bool mine_block_input = false;
bool change_block_input = false;
uint invalid_cursor = 0;

array<Block@> blocks;
Block@ current_block = null;
uint current_block_index = 0;

uint display_current_block = 0;
int display_x = 0;
int display_y = 0;
int display_index = -1;

class Block{
	int set = -1;
	int tile = -1;
	int palette = -1;
	uint count = 0;

	Block(int set, int tile, int palette){
		this.set = set;
		this.tile = tile;
		this.palette = palette;
	}
	
	bool equals(int set, int tile, int palette){
		return this.set == set and
			this.tile == tile and
			this.palette == palette;
	}
}

float round(float x){
	return x >= 0 ? floor(x + 0.5) : ceil(x - 0.5);
}

void update_current_block(uint index){
	if(display_index != -1){
		display_index = -1;
		display_current_block = 0;
		g.set_tile(display_x, display_y, 15, false, 0, 1, 1, 1);
	}

	if(index + 1 > blocks.length()){
		@current_block = null;
		current_block_index = 0;
		return;
	}
	
	@current_block = blocks[index];
	current_block_index = index;
	display_current_block = 40;
	
	display_x = int(round(player.x() / 48.0));
	display_y = int(round((player.y() - 48 * 3) / 48));
	g.set_tile(display_x, display_y, 15, true, 0, current_block.set, current_block.tile, current_block.palette);
	display_index = index;
}

void update_cursor(){
	g.set_tile(cursor_x, cursor_y, 20, false, 0, 1, 1, 1);
	
	cursor_x = int(round(player.x() / 48.0));
	cursor_y = int(round((player.y() - 48) / 48));
	
	int player_facing = player.face();
	
	if(player.y_intent() < 0){
		cursor_y--;
		if(player.jump_intent() != 0)
			cursor_y--;
	}
	else if(player.y_intent() > 0){
		cursor_y++;
	}
	
	if(player_facing > 0){
		// Try not to mine blocks through other blocks
		tileinfo@ tile = g.get_tile(cursor_x, cursor_y);
		if(!tile.solid())
			cursor_x++;
	}
	else{
		cursor_x--;
		// Try not to mine blocks through other blocks
		tileinfo@ tile = g.get_tile(cursor_x, cursor_y);
		if(!tile.solid())
			cursor_x--;
	}
	
	g.set_tile(cursor_x, cursor_y, 20, true, 0, 5, 1, invalid_cursor > 0 ? 4 : 5);
	if(invalid_cursor > 0)
		invalid_cursor--;
	
	if(display_current_block > 0){
		if(--display_current_block == 0){
			g.set_tile(display_x, display_y, 15, false, 0, 1, 1, 1);
		}
	}
}

void mine_block(){
	if(player.light_intent() != 0){
		if(!mine_block_input){
			tileinfo@ tile = g.get_tile(cursor_x, cursor_y);
			if(tile.solid()){
				g.set_tile(cursor_x, cursor_y, 19, false, 0, 1, 1, 1);
				int block_set = tile.sprite_set();
				int block_tile = tile.sprite_tile();
				int block_palette = tile.sprite_palette();
				
				Block@ mined_block = null;
				
				for(uint i=0; i<blocks.length(); i++){
					if(blocks[i].equals(block_set, block_tile, block_palette)){
						@mined_block = blocks[i];
						break;
					}
				}
				
				if(mined_block is null){
					@mined_block = Block(block_set, block_tile, block_palette);
					blocks.insertLast(mined_block);
					
					if(current_block is null)
						update_current_block(blocks.length() - 1);
				}
				
				mined_block.count++;
			}
			mine_block_input = true;
		}
	}
	else{
		mine_block_input = false;
	}
}

void place_block(){
	if(current_block is null) return;
	
	

	if(!mine_block_input and player.taunt_intent() != 0){
		if(current_block.count == 0){
			invalid_cursor = 20;
			return;
		}
		
		tileinfo@ tile = g.get_tile(cursor_x, cursor_y);
		if(!tile.solid()){
			g.set_tile(cursor_x, cursor_y, 19, true, 0, current_block.set, current_block.tile, current_block.palette);
			current_block.count--;
		}
	}
}

void change_block(){
	if(player.heavy_intent() != 0){
		if(!change_block_input){
			current_block_index++;
			if(current_block_index >= blocks.length())
				current_block_index = 0;
			update_current_block(current_block_index);
			
			change_block_input = true;
		}
	}
	else{
		change_block_input = false;
	}
}

// Game callbacks

void init(){
	puts("INIT");
	@g = get_scene();
}

void checkpoint_save() {
	puts("CHECKPOINT SAVE");
}

void checkpoint_load() {
	puts("CHECKPOINT LOAD");
	@player = null;
	mine_block_input = false;
	change_block_input = false;

	blocks.resize(0);
	@current_block = null;
	current_block_index = 0;

	display_current_block = 1;
	display_x = 0;
	display_y = 0;
	display_index = -1;
}

void step(int entities){
	if(@player == null){
		entity@e = controller_entity(0);
		if(@e != null){
			@player = e.as_controllable();
		}
	}
	else{
		update_cursor();
		
		mine_block();
		
		place_block();
		
		change_block();
	}
}

void step_post(int entities){
	if (@player != null){
	
	}
}