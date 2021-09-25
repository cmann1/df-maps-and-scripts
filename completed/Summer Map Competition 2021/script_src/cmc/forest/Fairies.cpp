#include '../../lib/utils/colour.cpp';
#include '../../lib/enums/ColType.cpp';

#include '../Location.cpp';
#include 'Fairy.cpp';

class Fairies : trigger_base
{
	
	script@ script;
	scripttrigger@ self;
	
	[persist] int count = 4;
	[persist] float wander_x;
	[persist] float wander_y;
	[persist] int layer = 11;
	[persist] int sub_layer = 5;
	[persist] float scale = 1;
	[persist] bool check_player;
	[persist] bool playful = true;
	[slider,min:0,max:1] float alpha = 1;
	[persist] array<Location> hide_pos;
	
	array<Fairy> fairies;
	array<controllable@> players;
	
	float x, y;
	bool in_view = true;
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = script;
		@this.self= self;
		
		self.radius(0);
		self.editor_colour_inactive(0xffdddddd);
		self.editor_colour_active(0xffffffff);
		self.editor_colour_circle(0xffdddddd);
		self.editor_handle_size(5);
		
		x = self.x();
		y = self.y();
		
		//if(!script.is_playing)
		//	return;
		
		init_faires();
	}
	
	private void init_faires()
	{
		fairies.resize(count);
		
		for(int i = 0; i < count; i++)
		{
			Fairy@ fairy = @fairies[i];
			fairy.init(script,
				i,
				layer, sub_layer,
				alpha, scale,
				self.x(), self.y(), wander_x, wander_y);
		}
	}
	
	void step()
	{
		in_view = script.in_view(
			x - wander_x - 200, y - wander_y - 200,
			x + wander_x + 200, y + wander_y + 200);
		if(!in_view)
			return;
		
		if(check_player && script.is_playing)
		{
			players.resize(0);
			
			int e_count = script.g.get_entity_collision(
				self.y() - (wander_y + Fairy::DetectRadius + Fairy::IdleRangeMax),
				self.y() + (wander_y + Fairy::DetectRadius + Fairy::IdleRangeMax),
				self.x() - (wander_x + Fairy::DetectRadius + Fairy::IdleRangeMax),
				self.x() + (wander_x + Fairy::DetectRadius + Fairy::IdleRangeMax),
				ColType::Player);
			
			while(--e_count >= 0)
			{
				controllable@ c = script.g.get_controllable_collision_index(e_count);
				
				if(@c != null && c.player_index() != -1)
				{
					players.insertLast(c);
				}
			}
		}
		
		for(int i = 0; i < count; i++)
		{
			//if(i != 3) continue;
			fairies[i].step(this);
		}
	}
	
	void editor_step()
	{
		if(count != int(fairies.length))
		{
			init_faires();
		}
		
		step();
	}
	
	void draw(float sub_frame)
	{
		if(!in_view || !script.should_draw(
			x - wander_x - 100, y - wander_y - 100,
			x + wander_x + 100, y + wander_y + 100))
			return;
		
		for(int i = 0; i < count; i++)
		{
			//if(i != 3) continue;
			fairies[i].draw(sub_frame);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		
		for(uint i = 0; i < hide_pos.length; i++)
		{
			Location@ pos = @hide_pos[i];
			float x, y;
			script.layer_position(pos.x, pos.y, layer, 22, x, y);
			//draw_line(script.g, 22, 22, self.x(), self.y(), x, y, 1, 0x55ff0000);
			draw_dot(script.g, 22, 22, x, y, 3, 0x55ff4400, 45);
		}
		
		//outline_rect(script.g, 22, 24,
		//	x - wander_x, y - wander_y,
		//	x + wander_x, y + wander_y,
		//	2, 0x22ffffff);
		outline_rect(script.g, layer, 24,
			x - wander_x, y - wander_y,
			x + wander_x, y + wander_y,
			2, 0x22ffffff);
	}
	
}
