const int POKEBALL_LIFE = 60 * 4;
const int16 TILE_SHAPE_FULL = 0;
const float gravity = 0.4;

float round(float x){
	return x >= 0 ? floor(x + 0.5) : ceil(x - 0.5);
}

class Pokeball{
	float x;
	float y;
	float vel_x;
	float vel_y;
	int life = POKEBALL_LIFE;

	Pokeball(float x, float y, float vel_x, float vel_y){
		this.x = x;
		this.y = y;
		this.vel_x = vel_x;
		this.vel_y = vel_y;
	}
}

class script {
	scene@ g;
	controllable@ player;
	dustman@ dm;
	
	array<Pokeball@> pokeballs;

	script() {
		@g = get_scene();
		puts("INIT");
		checkpoint_load();
	}

	void step(int entityCount) {
		if(@player != null){
			if(player.taunt_intent() != 0){
				float face = player.x_intent() != 0 ? player.x_intent() : player.face();
				Pokeball@ ball = Pokeball(player.x(), player.y() - 48, face * 12, -6);
				pokeballs.insertLast(ball);
			}
		}
		
		const float POKEBALL_SIZE = 40;
		
		float x1;
		float y1;
		float x2;
		float y2;
		
		for(int i=pokeballs.length()-1; i>=0; i--){
			Pokeball@ ball = pokeballs[uint(i)];
			g.set_tile(int(floor(ball.x / 48)), int(floor(ball.y / 48)), 16, false, 0, 0, 0, 0);
			
			bool remove = false;
			
			if(--ball.life <= 0 or g.get_tile(int(floor(ball.x / 48)), int(floor(ball.y / 48))).solid()){
				pokeballs.removeAt(i);
				continue;
			}
			
			float bx = ball.x;
			float by = ball.y;
			
			for(int j = 0; j < entityCount; j++){
				entity@ e = entity_by_index(j);
				string type = e.type_name();
				// puts(type);
				if(type != "entity_cleansed" and type.substr(0, 5) != "enemy") continue;
				
				float ex = e.x();
				float ey = e.y();
				
				if(type == "enemy_gargoyle_big" or type == "enemy_trash_can" or type == "enemy_knight" or type == "enemy_stoneboss"){
					x1 = -48;
					y1 = -96;
					x2 = 48;
					y2 = 0;
				}
				else if(type == "enemy_bear" or type == "enemy_trash_beast" or type == "enemy_slime_beast"){
					x1 = -48;
					y1 = -135;
					x2 = 48;
					y2 = 0;
				}
				else if(type == "enemy_porcupine" or type == "enemy_stonebro"){
					x1 = -38;
					y1 = -48;
					x2 = 38;
					y2 = 0;
				}
				else if(type == "enemy_wolf"){
					x1 = -52;
					y1 = -56;
					x2 = 52;
					y2 = 0;
				}
				else if(type == "enemy_maid"){
					x1 = -40;
					y1 = -62;
					x2 = 40;
					y2 = 0;
				}
				else if(type == "entity_cleansed"){
					x1 = -40;
					y1 = -96;
					x2 = 40;
					y2 = 0;
				}
				else{
					// puts(type);
					x1 = -48;
					y1 = -48;
					x2 = 48;
					y2 = 48;
				}
				
				if(bx >= ex + x1 and bx <= ex + x2 and by > ey + y1 and by <= ey + y2){
					remove = true;
					e.remove_entity();
					break;
				}
			}
			
			if(remove){
				pokeballs.removeAt(i);
				continue;
			}
			
			ball.x += ball.vel_x;
			ball.y += ball.vel_y;
			ball.vel_y += gravity;
			
			g.set_tile(int(floor(ball.x / 48)), int(floor(ball.y / 48)), 16, true, TILE_SHAPE_FULL, 1, 15, 3);
		}
	}

	void step_post(int entityCount) {
	}

	void checkpoint_save(){
	}

	void checkpoint_load(){
		entity@ e = controller_entity(0);
		if(@e != null){
			@player = e.as_controllable();
			@dm = e.as_dustman();
		}
		else{
			@player = null;
			@dm = null;
		}
	}

	void entity_on_add(entity@ e) {
	}

	void entity_on_remove(entity@ e) {
	}
}