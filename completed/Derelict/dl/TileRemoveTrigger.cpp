class TileRemoveTrigger : trigger_base
{
	
	[persist] int layer = 20;
	[persist] array<TileRemoveArea> blocks;
	
	private DLScript@ script;
	private scripttrigger@ self;
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		self.editor_colour_inactive(0xff423426);
		self.editor_colour_circle(0xff423426);
		self.editor_colour_active(0xff6A5641);
	}
	
	void activate(controllable@ c)
	{
		if(c.player_index() == -1)
			return;
		
		for(uint i = 0; i < blocks.length; i++)
		{
			TileRemoveArea@ block = @blocks[i];
			const int x1 = floor_int(block.x1 / 48);
			const int y1 = floor_int(block.y1 / 48);
			const int x2 = ceil_int(block.x2 / 48);
			const int y2 = ceil_int(block.y2 / 48);
			
			for(int x = x1; x < x2; x++)
			{
				for(int y = y1; y < y2; y++)
				{
					script.g.set_tile(x, y, layer, false, 0, 0, 0, 0);
				}
			}
		}
		
		script.g.remove_entity(self.as_entity());
	}
	
	void editor_draw(float sub_frame)
	{
		if(!script.debug_triggers)
			return;
		
		for(uint i = 0; i < blocks.length; i++)
		{
			TileRemoveArea@ block = @blocks[i];
			const float x1 = floor(block.x1 / 48) * 48;
			const float y1 = floor(block.y1 / 48) * 48;
			const float x2 = ceil(block.x2 / 48) * 48;
			const float y2 = ceil(block.y2 / 48) * 48;
			
			const uint clr = self.editor_selected() ? 0xff6A5641 : 0xaa6A5641;
			outline_rect(script.g, 22, 22, x1, y1, x2, y2, 2, clr);
			script.g.draw_line_world(22, 22, self.x(), self.y(), x1, y1, 4, clr);
		}
	}
	
}

class TileRemoveArea
{
	
	[position,mode:world,layer:19,y:y1] float x1;
	[hidden] float y1;
	[position,mode:world,layer:19,y:y2] float x2;
	[hidden] float y2;
	
}
