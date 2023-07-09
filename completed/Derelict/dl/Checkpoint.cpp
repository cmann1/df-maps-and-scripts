#include '../lib/drawing/common.cpp';

class Checkpoint : trigger_base
{
	
	[position,mode:world,layer:19,y:y] float x;
	[hidden] float y;
	[persist] int facing = 1;
	[persist] array<CPCell> cells;
	
	[hidden] bool has_apple;
	[hidden] bool has_player;
	
	private script@ script;
	private scripttrigger@ self;
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		self.editor_colour_inactive(0xff2222ff);
		self.editor_colour_circle(0xff2222ff);
		self.editor_colour_active(0xff6666ff);
	}
	
	void editor_var_changed(var_info@ var)
	{
		const string name = var.name;
		if(name != 'cells' && var.get_name(0) == 'cells')
		{
			const int i = var.get_index(0);
			CPCell@ cell = @cells[i];
			
			if(name == 'x')
				cell.x -= x;
			else if(name == 'y')
				cell.y -= y;
		}
	}
	
	void activate(controllable@ c)
	{
		if(!has_apple)
		{
			if(c.type_name() == 'hittable_apple')
			{
				has_apple = true;
			}
		}
		
		if(!has_player)
		{
			if(c.player_index() != -1)
			{
				has_player = true;
			}
		}
		
		if(has_apple && has_player)
		{
			entity@ cp = create_entity('check_point');
			cp.set_xy(x, y);
			cp.face(facing >= 0 ? 1 : -1);
			varstruct@ vars = cp.vars();
			vararray@ area = cp.vars().get_var('trigger_area').get_array();
			area.resize(cells.length);
			
			const float tx = - 96 / 2.0;
			const float ty = - 96 / 2.0;
			for(uint i = 0; i < cells.length; i++)
			{
				CPCell@ cell = @cells[i];
				area.at(i).set_vec2(floor_int((cell.x - tx) / 96), floor_int((cell.y - ty) / 96));
			}
			
			script.g.add_entity(cp);
			
			script.g.remove_entity(self.as_entity());
		}
	}
	
	void editor_draw(float)
	{
		if(!script.debug_triggers)
			return;
		
		const float w = 8;
		script.g.draw_line_world(22, 22, x, y, x + 48 * facing, y, 4, 0xaaff00ff);
		script.g.draw_rectangle_world(22, 22, x - w, y - w, x + w, y + w, 0, 0xaaff00ff);
		
		const float tx = x - 96 / 2.0;
		const float ty = y - 96 / 2.0;
		for(uint i = 0; i < cells.length; i++)
		{
			CPCell@ cell = @cells[i];
			const float x1 = tx + floor((cell.x - tx + x) / 96) * 96;
			const float y1 = ty + floor((cell.y - ty + y) / 96) * 96;
			script.g.draw_rectangle_world(22, 21, x1, y1, x1 + 96, y1 + 96, 0, 0x55ff00ff);
		}
	}
	
}

class CPCell
{
	
	[position,mode:world,layer:19,y:y] float x;
	[hidden] float y;
	
}
