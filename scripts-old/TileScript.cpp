float round(float x){
	return x >= 0 ? floor(x + 0.5) : ceil(x - 0.5);
}

class script{
	scene@ g;
	controllable@ player;
	dustman@ dm;
	
	bool light_attack = false;

	// Game callbacks

	script(){
		@g = get_scene();
	}

	// void checkpoint_save() {
	// }

	void checkpoint_load() {
		@player = null;
		@dm = null;
	}

	float frand(){
		return float(rand())/float(0x3fffffff);
	}

	void step(int entities){
		if(@player == null){
			entity@e = controller_entity(0);
			if(@e != null){
				@player = e.as_controllable();
				@dm = e.as_dustman();
			}
		}
		else{
			int state = player.state();
			
			if(player.attack_timer() >= 1){
				if(!light_attack){
					int start_x = int(round(player.x() / 48.0));
					int start_y = int(round((player.y() - 96) / 48));
					int face = player.x_intent() != 0 ? player.x_intent() : player.face();
					// int face = player.face();
					if(face < 0) start_x -= 3;
					else start_x++;
					
					for(int x = start_x; x < start_x + 2; x++){
						for(int y = start_y; y < start_y + 2; y++){
							tileinfo@ tile = g.get_tile(x, y, 15);
							if(tile.solid()){
								g.set_tile(x, y, 15, false, 0, 1, 1, 1);
								g.set_tile(x, y, 19, tile, true);
							}
						}
					}
					light_attack = true;
				}
			}
			else{
				light_attack = false;
			}
			
			// puts(player.attack_timer()+"");
		}
	}

	// void step_post(int entities){
		// if(@player != null){
			// int state = player.state();
			
		// }
	// }

}