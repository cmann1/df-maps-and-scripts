class _PropSelect : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	[text] uint layer = 15;
	
	bool run_prev = false;
	[text] bool run = false;
	
	_PropSelect()
	{
		@g = get_scene();
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.self = @self;
		self.editor_handle_size(5);
		
		if(script.in_game)
		{
			
		}
	}
	
	void editor_step()
	{
		if(run == run_prev) return;
		
		const float x = self.x();
		const float y = self.y();
		const float r = 1;
		int prop_count = g.get_prop_collision(y-r, y+r, x-r, x+r);
		
		for(int i = 0; i < prop_count; i++)
		{
			prop@ p = g.get_prop_collision_index(i);
			if(p.layer() != layer) continue;
			
			puts('PropStruct(0, ' + p.x() + ', ' + p.y() + ', ' +
				p.rotation() + ', ' + p.scale_x() + ', ' + p.scale_y() + ', ' +
				p.prop_set() + ', ' + p.prop_group() + ', ' + p.prop_index() + ', ' +
				p.palette() + ', ' + p.layer() + ', ' + p.sub_layer() + '),');
		}
		
		run_prev = run;
	}
	
}