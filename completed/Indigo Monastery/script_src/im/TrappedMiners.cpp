class TrappedMiners : trigger_base, callback_base
{
	
	script@ script;
	scene@ g;
	scripttrigger@ self;
	
	TrappedMiners()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
	}
	
	void activate(controllable@ e)
	{
		if(e.player_index() == -1)
			return;
		
		script.vars.set_int('mine_quest_state', TaskCompleted);
		g.remove_entity(self.as_entity());
	}
	
}