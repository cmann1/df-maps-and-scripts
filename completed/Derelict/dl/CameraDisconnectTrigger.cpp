#include '../lib/triggers/EnterExitTrigger.cpp';

class CameraDisconnectTrigger : trigger_base, EnterExitTrigger
{
	
	[entity,camera] uint connect_node;
	[persist] float smooth = 1;
	
	private DLScript@ script;
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
		const int player_index = c.player_index();
		camera@ cam = get_camera(player_index);
		
		if(connect_node != 0)
		{
			entity@ e = entity_by_id(connect_node);
			camera_node@ node = @e != null ? e.as_camera_node() : null;
			if(@node != null && cam.try_connect(node))
			{
				on_connect(player_index, node);
				return;
			}
		}
		
		if(cam.camera_type() != 'player')
		{
			cam.force_disconnect();
			on_connect(player_index, null);
		}
	}
	
	protected void on_connect(const int player_index, camera_node@ node)
	{
		if(smooth < 1)
		{
			script.players[player_index].set_cam_speed(smooth, 2);
		}
	}
	
	void editor_draw(float)
	{
		if(!script.debug_triggers)
			return;
		if(connect_node == 0)
			return;
		
		entity@ e = entity_by_id(connect_node);
		if(@e == null)
			return;
		
		camera_node@ node = e.as_camera_node();
		if(@node == null)
			return;
		
		script.g.draw_line_world(22, 22, self.x(), self.y(), node.x(), node.y(), 3, 0xaaaa6600);
	}
	
}
