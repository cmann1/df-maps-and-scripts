#include '../lib/triggers/EnterExitTrigger.cpp';

class CamDetachTrigger : trigger_base, EnterExitTrigger
{
	
	[entity,camera] uint connect_node;
	
	private script@ script;
	private scripttrigger@ self;
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
	}
	
	void activate(controllable@ c)
	{
		activate_enter_exit(c);
	}
	
	void step()
	{
		step_enter_exit();
	}
	
	bool can_trigger_enter_exit(controllable@ c)
	{
		return c.player_index() != -1;
	}
	
	void on_trigger_enter(controllable@ c)
	{
		camera@ cam = get_camera(c.player_index());
		
		if(connect_node != 0)
		{
			entity@ e = entity_by_id(connect_node);
			camera_node@ node = @e != null ? e.as_camera_node() : null;
			if(@node != null && cam.try_connect(node))
				return;
		}
		
		cam.force_disconnect();
	}
	
}
