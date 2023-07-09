class SecurityTrigger : trigger_base
{
	
	[persist] int level = 0;
	[persist] bool once = true;
	
	private DLScript@ script;
	private scripttrigger@ self;
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		self.editor_colour_inactive(0xffaaaaaa);
		self.editor_colour_circle(0xffaaaaaa);
		self.editor_colour_active(0xffff7777);
	}
	
	void activate(controllable@ c)
	{
		if(c.player_index() == -1)
			return;
		if(c.destroyed())
			return;
		
		dustman@ dm = c.as_dustman();
		if(@dm != null && dm.dead())
			return;
		
		script.update_security_level(level);
		
		if(once)
		{
			script.g.remove_entity(self.as_entity());
		}
	}
	
}
