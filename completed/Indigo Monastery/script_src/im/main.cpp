#include '../common/utils.cpp'
#include '../common/Debug.cpp'
#include '../common/EntityRemover.cpp'
#include '../common/PersistVars.cpp'
//#include 'Bell.cpp'
#include 'BellTrigger.cpp'
#include 'BellSpawner.cpp'
#include 'BreakableWall.cpp'
#include 'TriggerText.cpp'
#include 'AppleMan.cpp'
#include 'Clock.cpp'
#include 'CrystalPillar.cpp'
#include 'MineSuperintendent.cpp'
#include 'TrappedMiners.cpp'
#include 'MusicMachine.cpp'
#include 'MusicLock.cpp'
#include 'MainDoor.cpp'
#include 'MusicBox.cpp'
#include 'Tentacles.cpp'

class script : callback_base
{
	
	bool in_game = false;
	Debug@ debug;
	PersistVars vars;
	
	int num_players;
	bool players_disabled = false;
	
	sprite_group music_lock_key_spr(
		array<string>={'props1','buildingblocks_8','props1','buildingblocks_8','props1','buildingblocks_8','props1','buildingblocks_8','props1','buildingblocks_8','props1','buildingblocks_7',},
		array<int>={16,22,16,22,16,22,16,22,16,22,16,24,16,},
		array<float>={0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,},
		array<float>={-47.9759,0.0240993,1.0241,0.0240993,1.0241,-49.9759,49.0241,-0.975901,1.0241,49.0241,2.1113,0,},
		array<float>={0,0,0,0,0,315,},
		array<float>={0.521001,0.521001,0.521001,0.521001,0.521001,0.521001,0.521001,0.521001,0.521001,0.521001,0.37606,0.37606,1,1,1,1,0.84959,0.84959,0.84959,0.84959,1,1,},
		array<uint>={0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,},
		array<uint>={0,0,0,0,0,0,},
		array<uint>={0,0,0,0,0,0,}
		);
	
	script()
	{
		@debug = Debug(get_scene());
		
		get_scene().override_stream_sizes(8, 8);
	}
	
	void checkpoint_save()
	{
		vars.checkpoint_save();
	}
	
	void checkpoint_load()
	{
		vars.checkpoint_load();
	}
	
	void set_players_disabled()
	{
		
	}
	
	void on_level_start()
	{
		in_game = true;
		num_players = int(num_cameras());
	}
	
	
//	void entity_on_add(entity @e)
//	{
//		print_vars(e);
//	}
	
	void step(int entities)
	{
		if(players_disabled)
		{
			for(int i = num_players - 1; i >= 0; i--)
			{
				controllable@ player = controller_controllable(i);
				player.light_intent(0);
				player.heavy_intent(0);
				player.x_intent(0);
				player.y_intent(0);
				player.jump_intent(0);
				player.dash_intent(0);
				player.fall_intent(0);
				player.taunt_intent(0);
			}
		}
		
//		debug.step();
	}
	
	void draw(float sub_frame)
	{
//		debug.draw();
	}
	
}