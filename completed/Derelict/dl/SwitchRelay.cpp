class SwitchRelay : trigger_base, IOpenable
{
	
	[entity,trigger] array<uint> gates;
	
	private DLScript@ script;
	private scripttrigger@ self;
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		self.editor_colour_inactive(0xffffff44);
		self.editor_colour_circle(0xffffff44);
		self.editor_colour_active(0xffff4444);
	}
	
	bool is_closed
	{
		get const override
		{
			for(uint i = 0; i < gates.length; i++)
			{
				IOpenable@ gate = get_gate(gates[i]);
				if(@gate == null)
					continue;
				
				if(gate.is_closed)
					return true;
			}
			
			return false;
		}
	}
	
	bool is_alive
	{
		get const override { return !self.destroyed(); }
	}
	
	void open(const bool open=true) override
	{
		int gate_count = 0;
		
		for(uint i = 0; i < gates.length; i++)
		{
			IOpenable@ gate = get_gate(gates[i]);
			if(@gate == null)
				continue;
			
			gate.open(open);
			
			if(gate.is_alive)
			{
				gate_count++;
			}
		}
		
		if(gate_count <= 0)
		{
			script.g.remove_entity(self.as_entity());
		}
	}
	
	void toggle() override
	{
		int gate_count = 0;
		
		for(uint i = 0; i < gates.length; i++)
		{
			IOpenable@ gate = get_gate(gates[i]);
			if(@gate == null)
				continue;
			
			gate.toggle();
			
			if(gate.is_alive)
			{
				gate_count++;
			}
		}
		
		if(gate_count <= 0)
		{
			script.g.remove_entity(self.as_entity());
		}
	}
	
	void editor_draw(float sub_frame)
	{
		if(!script.debug_triggers)
			return;
		
		for(uint i = 0; i < gates.length; i++)
		{
			entity@ e = entity_by_id(gates[i]);
			if(@e == null)
				return;
			
			draw_arrow(script.g, 22, 22, self.x(), self.y(), e.x(), e.y(), 2, 14, 0.5, 0x55ffcc77);
		}
	}
	
	private IOpenable@ get_gate(const uint id)
	{
		scripttrigger@ se = scripttrigger_by_id(id);
		if(@se == null)
			return null;
		
		return cast<IOpenable@>(se.get_object());
	}
	
}
