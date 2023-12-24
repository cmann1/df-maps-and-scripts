class Mover : trigger_base
{
	
	[entity,flags] uint entity_id;
	[position,mode:world,layer:19,y:y] float x;
	[hidden] float y;
	
	script@ script;
	scripttrigger@ self;
	scene@ g;
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		@g = get_scene();
	}
	
	void activate(controllable@ c)
	{
		if(c.player_index() == -1)
			return;
		
		entity@ e = entity_by_id(entity_id);
		if(@e != null)
		{
			e.set_xy(x, y);
		}
		
		script.g.remove_entity(self.as_entity());
	}
	
	void editor_draw(float)
	{
		draw_dot(g, 22, 22, x, y, 5, 0xff0000ff, 45);
	}
	
}
