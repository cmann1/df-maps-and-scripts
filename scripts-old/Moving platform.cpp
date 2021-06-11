/*

GLOBAL FUNCTIONS
scene@ get_scene()
entity@ controller_entity(uint)
entity@ entity_by_id(uint)
entity@ entity_by_index(uint)

scene FUNCTIONS
tileinfo@ get_tile(int, int)
void set_tile(int, int, int, bool, int16, int16, int16, int16)
tilefilth@ get_tile_filth(int, int)
uint set_tile_filth(int, int, uint8, uint8, uint8, uint8, bool, bool)
uint project_tile_filth(float, float, float, float, uint8, float, float, float, bool, bool, bool, bool, bool, bool)

tileinfo FUNCTIONS
bool solid()
int angle()
int offset()
uint8 sprite_set()
uint8 sprite_tile()
uint8 sprite_palette()
uint16 sprite_data()

entity FUNCTIONS
string type_name()
entity@ as_entity()
controllable@ as_controllable()
dustman@ as_dustman()
void remove_entity()
int id()
float x()
void x(float)
float y()
void y(float)
float rotation()
void rotation(float)
int layer()
void layer(int)
int face()
void face(int)
int palette()
void palette(int)

controllable FUNCTIONS (extends entity)
void reset()
int x_intent()
void x_intent(int)
int y_intent()
void y_intent(int)
int taunt_intent()
void taunt_intent(int)
int heavy_intent()
void heavy_intent(int)
int light_intent()
void light_intent(int)
int dash_intent()
void dash_intent(int)
int jump_intent()
void jump_intent(int)
int fall_intent()
void fall_intent(int)

dustman FUNCTIONS (extends controllable)
(none atm)
*/

scene@ g;
controllable@ player;
float t = 0.0;
int start = 7;
int move_distance = 5;
int length = 5;

void init() {
  puts("INIT");
  @g = get_scene();
}

void checkpoint_save() {
  puts("CHECKPOINT SAVE1");
}

void checkpoint_load() {
  puts("CHECKPOINT LOAD1");
  @player = null;
}

void step(int entities) {
  if (@player == null) {
    entity@e = controller_entity(0);
    if (@e != null) {
      puts("TRY AS CONTROLLABLE");
      @player = e.as_controllable();
      if (@player != null) {
        puts("FOUND PLAYER " + player.type_name() + ", " + player.id());
		g.set_tile(-1000, 0, 19, true, 0, 2, 2, 3);
      }
    }
  }
  
  int y = 0;
  int x = int(start + sin(t) * move_distance);
  for( int n = 0; n < length; n++ ) 
  {
     g.set_tile(x + n, y, 19, false, 0, 1, 1, 1);
  }
  
  tileinfo@ info = g.get_tile(0,0);
  
  bool move_player = false;
  
  if (@player != null) {
	  float top = y * 48;
	  float bottom = top + 48;
	  float left = x * 48;
	  float right = left + length * 48;
	  float px = player.x();
	  float py = player.y();
	  if(px >= left and px <= right and py >= top and py <= bottom){
		move_player = true;
	  }
  }
  
  int prev_x = x;
  int prev_y = y;
  t += 0.05;
  y = 0;
  x = int(start + sin(t) * move_distance);
  for( int n = 0; n < length; n++ ) 
  {
     g.set_tile(x + n, y, 19, true, 0, 2, 2, 3);
  }
  
  if(move_player and prev_x != x){
	player.x(player.x() + (x - prev_x) * 48.0);
	player.y(player.y() + (y - prev_y) * 48.0);
  }
  
  // if (@player != null) {
    // int x = floor(player.x() / 48.0);
    // int y = floor(player.y() / 48.0);
    // g.set_tile(x, y, 19, false, 0, 1, 1, 1);
  // }
/*
  if (@player != null) {
      player.remove_entity();
    player.jump_intent(1);
    player.x(player.x() + 1);
    //puts("HELLO STEP " + player.x() + ", " + player.y());
  }
*/
}

void step_post(int entities) {
}