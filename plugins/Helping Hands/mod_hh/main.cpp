#include '../common/utils.cpp'
#include '../common/math.cpp'
#include 'HelpingHands.cpp'

class script
{
	
	scene@ g;
	
	int num_players;
	array<controllable@> players;
	array<HelpingHands@> helpers;
	
	script()
	{
		@g = get_scene();
		
		num_players = num_cameras();
		players.resize(num_players);
		helpers.resize(num_players);
	}
	
	void checkpoint_load()
	{
		for(int i = 0; i < num_players; i++)
		{
			@players[i] = null;
			
			if(helpers[i] !is null)
			{
				helpers[i].reset();
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
				helpers[i].step();
			}
			else
			{
				entity@ e = controller_entity(i);
				@players[i] = @player = (@e != null ? e.as_controllable() : null);
				if(@player !is null and @helpers[i] is null) @helpers[i] = HelpingHands();
				helpers[i].update_player(player);
			}
		}
	}
	
	void draw(float sub_frame)
	{
		controllable@ player;
		for(int i = 0; i < num_players; i++)
		{
			if((@player = players[i]) is null) continue;
			helpers[i].draw(sub_frame);
		}
	}
	
}