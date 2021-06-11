#include "WarMachine.cpp"
#include "../common/math.cpp"
#include "../common/Sprite.cpp"
#include "../common/sprite_group.cpp"

class script
{
	
	scene@ g;
	controllable@ player;
	dustman@ dm;
	
	WarMachine@ war_machine = null;
	
	script()
	{
		@g = get_scene();
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
				// dm.auto_respawn(false);
			}
		}
		else
		{
			if(@war_machine == null)
			{
				for(int i = 0; i < entities; i++)
				{
					scripttrigger@ se = entity_by_index(i).as_scripttrigger();
					if(@se != null)
					{
						puts(se.type_name());
						@war_machine = cast<WarMachine>(se.get_object());
						if(@war_machine != null)
						{
							@war_machine.event_callback = EventCallback(this.on_anim_event);
							break;
						}
					}
				}
			}
			if(@war_machine != null)
			{
				// float x = 0;
				// float y = 0;
				
				// const string anchor_name = "end";
				// if(war_machine.get_anchor_pos(anchor_name, x, y))
				// {
					// x += war_machine.self.x();
					// y += war_machine.self.y();
					
					// float rotation;
					// float scale_x;
					// float scale_y;
					
					// war_machine.get_anchor_rot(anchor_name, rotation);
					// war_machine.get_anchor_scale(anchor_name, scale_x, scale_y);
					
					// spr.draw_world(20, 20, 0, 0,
						// x, y, rotation*RAD2DEG, scale_x, scale_y);
				// }
			}
		}
	}
	
	void on_anim_event(string event)
	{
		puts(event);
	}
	
	void checkpoint_load()
	{
		puts("checkpoint_load");
		@player = null;
		@dm = null;
	}
	
	void draw(float sub_frame)
	{
	}
	
//	void step_post(int entities) {}
//	void checkpoint_save() {}
//	void entity_on_add(entity@ e) {}
//	void entity_on_remove(entity@ e) {}
//	void move_cameras() {}
	
}