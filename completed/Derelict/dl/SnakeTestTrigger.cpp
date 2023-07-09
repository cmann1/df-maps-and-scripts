class SnakeTestTrigger : trigger_base
{
	
	[entity] uint snake_id;
	
	private DLScript@ script;
	private scripttrigger@ self;
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		self.editor_colour_inactive(0xffff2222);
		self.editor_colour_circle(0xffff2222);
		self.editor_colour_active(0xffff6666);
	}
	
	void activate(controllable@ c)
	{
		if(c.player_index() == -1)
			return;
		
		dustman@ dm = c.as_dustman();
		if(@dm == null)
			return;
		if(!dm.ground())
			return;
		if(dm.dead() || dm.destroyed())
			return;
		
		scriptenemy@ se = scriptenemy_by_id(snake_id);
		if(@se == null)
			return;
		
		Snake@ snake = cast<Snake@>(se.get_object());
		if(@snake == null)
			return;
		
		script.players[c.player_index()].control_snake(snake);
		script.g.remove_entity(self.as_entity());
	}
	
}
