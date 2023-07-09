#include '../lib/triggers/EnterExitTrigger.cpp';

class SecurityGateTrigger : trigger_base, EnterExitTrigger
{
	
	[entity,trigger] uint gate_id;
	[persist] bool once;
	[option,Unlock,Lock,Toggle] int action;
	
	private DLScript@ script;
	private scripttrigger@ self;
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		self.editor_colour_inactive(0xffffcc77);
		self.editor_colour_circle(0xffffcc77);
		self.editor_colour_active(0xffff7777);
	}
	
	void on_trigger_enter(controllable@ c)
	{
		scripttrigger@ se = scripttrigger_by_id(gate_id);
		if(@se == null)
			return;
		
		SecurityGate@ gate = cast<SecurityGate@>(se.get_object());
		if(@gate == null)
			return;
		
		switch(action)
		{
			case 0: gate.open(); break;
			case 1: gate.open(false); break;
			case 2: gate.toggle(); break;
		}
		
		if(once)
		{
			script.g.remove_entity(self.as_entity());
		}
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
	
	void editor_draw(float)
	{
		if(!script.debug_triggers)
			return;
		
		if(gate_id == 0)
			return;
		
		entity@ e = entity_by_id(gate_id);
		if(@e == null)
			return;
		
		script.g.draw_line_world(22, 22, self.x(), self.y(), e.x(), e.y(), 2, 0x55ffcc77);
	}
	
}
