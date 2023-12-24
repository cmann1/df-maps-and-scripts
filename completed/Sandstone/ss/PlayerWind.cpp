class PlayerWind : trigger_base
{
	
	[persist] bool outside;
	
	script@ script;
	scripttrigger@ self;
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = script;
		@this.self = self;
	}
	
	void activate(controllable@ c)
	{
		if(c.player_index() == -1)
			return;
		
		script.players[c.player_index()].update_outside(outside);
	}
	
}
