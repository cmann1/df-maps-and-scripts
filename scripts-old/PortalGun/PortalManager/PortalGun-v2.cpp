#include "../common/Debug.cpp"
#include "../common/utils.cpp"
#include "../common/drawing_utils.cpp"
#include "PortalManager.cpp"

class script
{
	
	scene@ g;
	controllable@ player;
	dustman@ dm;
	
	PortalManager@ portal_manager;
	Portal@ current_portal = null;
	uint colour1 = 0xDDffb85e;
	uint colour2 = 0xDD5ea5ff;
	
	float aim_x = 0;
	float aim_y = 0;
	float aim_sx = 0;
	float aim_sy = 0;
	
	Portal@ portal1 = null;
	Portal@ portal2 = null;
	int portal_index = 1;
	bool requires_init = true;
	
	Debug@ debug;
	HelpBox@ help;
	
	script()
	{
		puts("INIT");
		@g = get_scene();
		@debug = Debug(g);
		
		@help = HelpBox(
			"Aim: Directions\n" +
			"Place Portal: Taunt"
		);
		help.x = -790;
		help.y = -440;
		
		@portal_manager = PortalManager(debug);
		// @portal_manager = PortalManager();
		
		
	}
	
	void script_init()
	{
		if(!requires_init) return;
		
		array<float> portals = {
			-10, -9, 2, 4,
			-10, -2, 3, 4,
			
			19,  0,  0, 5,
			12, -3, 0, 5,
			
			19, -17, 1, 5,
			26,  0,  0, 5 
		};
		
		Portal@ p = null;
		for(uint i = 0; i < portals.length(); i += 4)
		{
			int px = int(portals[i]);
			int py = int(portals[i+1]);
			int side = int(portals[i+2]);
			int size = int(portals[i+3]);
			Portal@ new_p = portal_manager.try_create(
				px, py,
				side, size,
				@p == null ? colour1 : colour2);
			
			if(@new_p == null)
			{
				puts("Could not place portal at "+px+","+py);
				i += 4;
				continue;
			}
			
			if(@p != null)
			{
				new_p.link(p);
				@p = null;
			}
			else{
				@p = new_p;
			}
		}
		
		requires_init = false;
	}
	
	void checkpoint_save()
	{
		
	}

	void checkpoint_load()
	{
		@player = null;
		@dm = null;
	}
	
	void step(int entities)
	{
		if(@player == null)
		{
			entity@e = controller_entity(0);
			if(@e != null)
			{
				@player = e.as_controllable();
				@dm = e.as_dustman();
			}
			
			script_init();
		}
		else
		{
			aim_sx = player.x();
			aim_sy = player.y() - 60;
			
			int throw_dir = 0;
			
			int intent_x = player.x_intent();
			int face = intent_x == 0 ? player.face() : intent_x;
			int intent_y = player.y_intent();
			
			if(intent_y == -1)
			{
				throw_dir = intent_x == 0 ? 0 : 45;
			}
			else if(intent_y == 1)
			{
				throw_dir = intent_x == 0 ? 180 : 135;
			}
			else
			{
				throw_dir = 90;
			}
			
			throw_dir *= face;
			
			float throw_angle = deg2rad(throw_dir);
			aim_x = sin(throw_angle);
			aim_y = -cos(throw_angle);
				
			if(player.taunt_intent() != 0)
			{
				float x2 = aim_sx + aim_x * 2000;
				float y2 = aim_sy + aim_y * 2000;
				
				if(portal_index == 1)
				{
					portal_manager.remove(@portal1);
				}
				else
				{
					portal_manager.remove(@portal2);
				}
				
				Portal@ new_portal = portal_manager.try_create(aim_sx, aim_sy, x2, y2, 3, portal_index == 1 ? colour1 : colour2);
				
				
				if(@new_portal != null)
				{
					if(portal_index == 1)
					{
						@portal1 = new_portal;
						portal1.link(portal2);
					}
					else
					{
						@portal2 = new_portal;
						portal2.link(portal1);
					}
					portal_index = portal_index == 1 ? 2 : 1;
				}
				
//				if(@new_portal != null)
//				{
//					if(@current_portal != null)
//					{
//						current_portal.link(new_portal);
//						@current_portal = null;
//					}
//					else
//					{
//						@current_portal = new_portal;
//					}
//				}
				
			}
			
			// int tile_x = int(floor(player.x() / 48));
			// int tile_y = int(floor(player.y() / 48));
			
			// debug.outline_tile(tile_x, tile_y, 19, 20);
			// debug.print(tile_x+", "+tile_y, 88888, 0xFFFFFFFF, 60);
		}
		
		portal_manager.step();
		
		debug.step();
	}
	
	void draw(float sub_frame)
	{
		uint c = (portal_index == 1 ? colour1 : colour2) | 0xFF000000;
//		g.draw_rectangle_world(18, 11,
//			aim_sx - 7, aim_sy - 7,
//			aim_sx + 7, aim_sy + 7,
//			0, 0xFF000000);
//		draw_line(g, aim_sx, aim_sy, aim_sx + aim_x * 56, aim_sy + aim_y * 56, 18, 11, 5, 0xFF000000);
		g.draw_rectangle_world(18, 11,
			aim_sx - 5, aim_sy - 5,
			aim_sx + 5, aim_sy + 5,
			0, c);
		draw_line(g, aim_sx, aim_sy, aim_sx + aim_x * 56, aim_sy + aim_y * 56, 18, 11, 2, c);
		
		portal_manager.draw();
		debug.draw();
		
		help.draw(g);
	}
	
	void step_post(int entities) {}
	void entity_on_add(entity@ e) {}
	void entity_on_remove(entity@ e) {}
	
}