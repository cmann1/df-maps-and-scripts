#include "../common/States.cpp";

class SecretTrigger : trigger_base
{
	
	scene@ g;
	script@ scr;
	scripttrigger@ self;
	
	[position,layer:19,y:teleport_y] float teleport_x = 0;
	[hidden] float teleport_y = 0;
	
	[position,layer:19,y:return_y] float return_x = 0;
	[hidden] float return_y = 0;
	
	array<controllable@> players;
	array<int> players_index;
	array<int> players_time;
	
	SecretTrigger()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		@scr = s;
	}
	
	void activate(controllable@ e)
	{
		for(uint i = 0; i < num_cameras(); i++)
		{
			entity@ player = controller_entity(i);
			if(@player != null and e.is_same(player) and players_index.find(i) < 0)
			{
				players.insertLast(player.as_controllable());
				players_index.insertLast(i);
				players_time.insertLast(300);
				e.attack_state(0);
				e.set_speed_xy(0, 0);
				e.set_xy(teleport_x, teleport_y);
				e.face(1);
				reset_camera(i);
				scr.players_enabled[i] = false;
				return;
			}
		}
	}
	
	void step()
	{
		for(int i = players.length() - 1; i >= 0; i--)
		{
			controllable@ player = players[i];
			player.set_speed_xy(0, 0);
			if(--players_time[i] < 0)
			{
				players.removeAt(i);
				players_index.removeAt(i);
				players_time.removeAt(i);
				scr.players_enabled[i] = true;
				player.set_xy(return_x, return_y);
				reset_camera(i);
			}
		}
	}
	
	void editor_draw(float sub_frame)
	{
		g.draw_rectangle_world(22, 10, teleport_x - 5, teleport_y - 5, teleport_x + 5, teleport_y + 5, 0, 0xAAFF0000);
		g.draw_rectangle_world(22, 10, return_x - 5, return_y - 5, return_x + 5, return_y + 5, 0, 0xAA00FF00);
	}
	
}