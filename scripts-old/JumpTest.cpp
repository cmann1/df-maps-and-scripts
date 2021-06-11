class script {
	scene@ g;
	controllable@ player;
	dustman@ dm;

	// Game callbacks

	script() {
		@g = get_scene();
	}

	void checkpoint_save() {
		puts("CHECKPOINT SAVE");
	}

	void checkpoint_load() {
		puts("CHECKPOINT LOAD");
		@player = null;
		@dm = null;
	}

	float frand(){
		return float(rand())/float(0x3fffffff);
	}

	void entity_on_add(entity@ e) {
		// puts("HELLO " + e.type_name());
	}

	void entity_on_remove(entity@ e) {
		// puts("BYE " + e.type_name());
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
			
			if(player.jump_intent() == 1 and (state == 5 or state == 7 or state == 8)){
				float player_x = player.x();
				float player_y = player.y();
				float head_width = 30;
				float head_height = 20;
				float head_offset = 0;
				for (int i = 0; i < entities; i++) {
					entity@ e = entity_by_index(i);
					string type = e.type_name();
					if(type.substr(0, 5) == "enemy"){
						float e_x = e.x();
						float e_y = e.y();
						
						if(type == "enemy_bear")
							head_offset = -48 * 2;
						else
							head_offset = 0;
						
						if(abs(e_x - player_x) < head_width and abs(e_y + head_offset - player_y) < head_height){
							player.state(6);
							dm.dash_max(-1);
							dm.dash_max(1);
							player.state(8);
							
							controllable@ ec = e.as_controllable();
							
							ec.set_speed_xy(ec.x_speed(), ec.y_speed() + 200);
							ec.y(ec.y() + 20);
							break;
						}
					}
				}
			}
		}
	}

	void step_post(int entities){
		if(@player != null){
			int state = player.state();
			
		}
	}
}