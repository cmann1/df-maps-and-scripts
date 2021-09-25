class LeverLock : trigger_base
{
	
	[entity] int lever_id;
	[position,mode:world,layer:19,y:y1] float x1;
	[hidden] float y1;
	[position,mode:world,layer:19,y:y2] float x2;
	[hidden] float y2;
	
	script@ script;
	scripttrigger@ self;
	
	Lever@ lever;
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = script;
		@this.self = self;
		
		self.radius(0);
		self.editor_handle_size(5);
		
		if(!script.is_playing)
			return;
		
		update_lever();
	}
	
	void step()
	{
		if(lever_id != 0 && @lever == null)
		{
			update_lever();
		}
		
		if(@lever == null)
			return;
		
		int i = script.g.get_entity_collision(
			y1, y2, x1, x2,
			ColType::Player);
		
		while(--i >= 0)
		{
			controllable@ c = script.g.get_controllable_collision_index(i);
			if(@c == null)
				continue;
			if(c.player_index() == -1)
				continue;
			
			lever.unlock();
			break;
		}
	}
	
	void editor_step()
	{
		if(lever_id != 0 && @lever == null)
		{
			update_lever();
		}
	}
	
	void editor_draw(float sub_frame)
	{
		outline_rect(script.g, 22, 22, x1, y1, x2, y2, 1 * script.ed_zoom, 0xaaffffff);
		
		if(@lever != null)
		{
			draw_line(
				script.g, 21, 21,
				self.x(), self.y(), lever.self.x(), lever.self.y(),
				1 * script.ed_zoom, 0xaaffffff);
		}
	}
	
	void editor_var_changed(var_info@ var)
	{
		const string name = var.name;
		if(name == 'lever_id')
		{
			update_lever();
		}
	}
	
	private void update_lever()
	{
		scripttrigger@ st = scripttrigger_by_id(lever_id);
		@lever = @st != null ? cast<Lever@>(st.object) : null;
	}
	
}
