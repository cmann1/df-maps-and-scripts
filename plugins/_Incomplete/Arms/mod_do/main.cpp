#include '../common/utils.cpp'
#include '../common/math.cpp'
#include 'Tentacles.cpp'

class script
{
	
	scene@ g;
	
	int num_players;
	array<controllable@> players;
	array<Tentacles@> tentacles;
	
	script()
	{
		@g = get_scene();
		
		num_players = num_cameras();
		players.resize(num_players);
		tentacles.resize(num_players);
	}
	
	void checkpoint_load()
	{
		for(int i = 0; i < num_players; i++)
		{
			@players[i] = null;
			
			if(tentacles[i] !is null)
			{
				tentacles[i].reset();
			}
		}
	}
	
	void step(int entities)
	{
		controllable@ player;
		for(int i = 0; i < num_players; i++)
		{
			if((@player = players[i]) != null)
			{
				tentacles[i].step();
			}
			else
			{
				entity@ e = controller_entity(i);
				@players[i] = @player = (@e != null ? e.as_controllable() : null);
				if(@player !is null and @tentacles[i] is null) @tentacles[i] = Tentacles();
				tentacles[i].update_player(i, player);
			}
		}
	}
	
	void draw(float sub_frame)
	{
		controllable@ player;
		for(int i = 0; i < num_players; i++)
		{
			if((@player = players[i]) is null) continue;
			tentacles[i].draw(sub_frame);
		}
	}
	
}