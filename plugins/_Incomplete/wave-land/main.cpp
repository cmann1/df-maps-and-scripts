#include "BoxMan.cpp"
#include "Tiles.cpp"
#include "../common/utils.cpp"
#include "../common/Debug.cpp"

const int MAX_PLAYERS = 4;

const string EMBED_spr_bg = "wave-land/sprites/bg.png";
const string EMBED_spr_grass = "wave-land/sprites/grass.png";
const string EMBED_t_ground_top = "wave-land/sprites/ground_tiles/top.png";
const string EMBED_t_ground_top_left = "wave-land/sprites/ground_tiles/top_left.png";
const string EMBED_t_ground_top_right = "wave-land/sprites/ground_tiles/top_right.png";
const string EMBED_t_ground_bottom = "wave-land/sprites/ground_tiles/bottom.png";
const string EMBED_t_ground_bottom_left = "wave-land/sprites/ground_tiles/bottom_left.png";
const string EMBED_t_ground_bottom_right = "wave-land/sprites/ground_tiles/bottom_right.png";
const string EMBED_t_ground_left = "wave-land/sprites/ground_tiles/left.png";
const string EMBED_t_ground_right = "wave-land/sprites/ground_tiles/right.png";
const string EMBED_t_ground_middle_01 = "wave-land/sprites/ground_tiles/middle_01.png";
const string EMBED_t_ground_middle_02 = "wave-land/sprites/ground_tiles/middle_02.png";
const string EMBED_t_ground_middle_03 = "wave-land/sprites/ground_tiles/middle_03.png";

/*
 * TODO: [ ] Better slope interactions
 * TODO: [ ] Clearing dustblocks
 * TODO: [ ] Ceiling runs?
 * TODO: [ ] Player health, getting hurt, breaking combo
 * TODO: [ ] Invincible during dash
 * TODO: [ ] Timer overlay
 * TODO: [ ] 
 */
 
class script
{
	scene@ g;
	Debug debug(get_scene());
	
	array<BoxMan@> players;
	
	sprites@ spr;
	
	script()
	{
		@g = get_scene();
		players.resize(num_cameras());
		
		@spr = create_sprites();
	}
	
	void on_level_start()
	{
		spr.add_sprite_set("script");
	}
	
	void add_tile(string name, message@ msg)
	{
		msg.set_string(name, "t_" + name);
		msg.set_int(name + "|offsetx", 2);
		msg.set_int(name + "|offsety", 2);
	}
	void build_sprites(message@ msg)
	{
		msg.set_string("sky", "spr_bg");
		msg.set_int("sky|offsetx", 513);
		msg.set_int("sky|offsety", 289);
		
		msg.set_string("grass", "spr_grass");
		msg.set_int("grass|offsetx", 16);
		msg.set_int("grass|offsety", 26);
		
		add_tile("ground_top", msg);
		add_tile("ground_top_left", msg);
		add_tile("ground_top_right", msg);
		add_tile("ground_bottom", msg);
		add_tile("ground_bottom_left", msg);
		add_tile("ground_bottom_right", msg);
		add_tile("ground_left", msg);
		add_tile("ground_right", msg);
		add_tile("ground_middle_01", msg);
		add_tile("ground_middle_02", msg);
		add_tile("ground_middle_03", msg);
	}
	
	void entity_on_remove(entity@ e)
	{
		for(uint i = 0; i < num_cameras(); i++)
		{
			controllable@ c = controller_controllable(i); 
			if(@c != null and controller_controllable(i).is_same(@e))
			{
				if(num_cameras() > 1)
				{
					scriptenemy@ se = create_scriptenemy(box_respawner(i, 1.0));
					se.x(g.get_checkpoint_x(i));
					se.y(g.get_checkpoint_y(i));
					controller_entity(i, @se.as_controllable());
					g.add_entity(@se.as_entity(), false);
				}
				else
				{
					g.combo_break_count(g.combo_break_count() + 1);
					g.load_checkpoint();
				}
				
				DeathFx@ fx = DeathFx();
				scripttrigger@ st = create_scripttrigger(fx);
				st.set_xy(c.x(), c.y());
				g.add_entity(st.as_entity(), false);
			}
		}
	}
	
	void checkpoint_load()
	{
		for(uint i = 0; i < num_cameras(); i++)
		{
			BoxMan@ player = players[i];
			if(@player != null)
			{
				player.checkpoint_load();
			}
		}
	}
	
	void spawn_player(message@ msg)
	{
		float x = msg.get_float("x");
		float y = msg.get_float("y");
		
		scriptenemy@ player = create_player(g, x, y);

		msg.set_entity("player", player.as_entity());
	}
	
	void on_player_add(int index, BoxMan@ player)
	{
		@players[index] = player;
	}
	
	void step(int entities)
	{
		debug.step();
	}
	
	void draw(float sub_frame)
	{
		spr.draw_world(0, 0, "sky", 0, 0, 0, 0,
			0, 1, 1, 0xFFFFFFFF);
		
		debug.draw();
	}
	
}

scriptenemy@ create_player(scene@ g, float x, float y)
{
	BoxMan@ box_man = BoxMan();
	scriptenemy@ player = create_scriptenemy(box_man);
	
	tilecollision@ col = g.collision_ground(
		x - box_man.radius, y - 50,
		x + box_man.radius, y + 50);
	if(col.hit())
	{
		y = col.hit_y();
	}
	
	box_man.set_position(x, y);
	
	SpawnFx@ fx = SpawnFx();
	scripttrigger@ st = create_scripttrigger(fx);
	st.set_xy(x, y - box_man.radius);
	g.add_entity(st.as_entity(), false);
	
	return player;
}

class box_respawner : enemy_base
{
	scene@ g;
	scriptenemy@ self;

	int player_index;
	float timer;

	box_respawner(int player_index, float timer)
	{
		@g = get_scene();

		this.player_index = player_index;
		this.timer = timer;
	}

	void init(script@, scriptenemy@ self)
	{
		@this.self = @self;
	}

	void step()
	{
		if (timer < 0) return;
		
		timer -= inc(1);
		if (timer < 0)
		{
			scriptenemy@ player = create_player(g, self.x(), self.y());
			g.add_entity(@player.as_entity(), false);
			controller_entity(player_index, @player.as_controllable());

			g.remove_entity(@self.as_entity());
		}
	}

	float inc(float x)
	{
		return x / 60.0 * self.time_warp();
	}
}
