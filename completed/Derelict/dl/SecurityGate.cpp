#include '../lib/drawing/common.cpp';
#include '../lib/props/common.cpp';

#include 'IOpenable.cpp';

const float SecurityGateHandleSize = 10;

class SecurityGate : trigger_base, IOpenable
{
	
	[persist|tooltip:"This gate will close when the global security\nlevel is higher than or equal to this.\nSet to -1 to only activate by switches."]
	int level = 1;
	[persist|tooltip:"Only activate when global level is ==, instead of >="]
	bool exact_level;
	[persist] bool reversed;
	[persist|tooltip:"If true doesn't overwrite existing tiles."]
	bool empty_only;
	[option,None,SecurityActivate,SwitchActive,Opened,Closed,Any|tooltip:"Will remove this trigger after\nbeing activated in this way."]
	int once = 0;
	[persist|tooltip:"If true, this gate will not respond to security\nlevel changes until it has been activated\nby a switch."]
	bool inactive;
	[persist] array<SecurityBlock> blocks;
	[persist] bool closed = false;
	[option,None,SecurityActivate,SwitchActive,Both] int play_sounds = 2;
	[option,None,Slide,Unlock]
	int open_sound = 1;
	[option,None,Land,Parry,Springball]
	int close_sound = 1;
	[persist] int force_sound = -1;
	
	[hidden] int current_level = -1;
	
	private DLScript@ script;
	private scripttrigger@ self;
	
	private MessageHandler@ on_security_updated_cb;
	
	private bool selected;
	private int drag_i = -1;
	private int hover_i = -1;
	private float drag_ox, drag_oy;
	private float drag_sx, drag_sy;
	private int hover_handle = 0;
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		self.editor_colour_inactive(0xffffff77);
		self.editor_colour_circle(0xffffff77);
		self.editor_colour_active(0xffff7777);
	}
	
	void on_add()
	{
		if(!script.is_playing)
			return;
		
		bool requires_activate = false;
		
		if(level != -1)
		{
			if(@on_security_updated_cb == null)
			{
				@on_security_updated_cb = MessageHandler(on_security_updated);
			}
			
			script.messages.add_listener('security_updated', on_security_updated_cb);
			
			if(current_level != script.security_level)
			{
				current_level = script.security_level;
				closed = calc_is_closed();
				requires_activate = script.security_level > 0;
			}
		}
		
		if(requires_activate || closed)
		{
			activate(closed, false, true);
			
			if(once == (!closed ? 3 : 4))
			{
				script.g.remove_entity(self.as_entity());
			}
		}
	}
	
	void on_remove()
	{
		if(!script.is_playing)
			return;
		
		if(@on_security_updated_cb != null)
		{
			script.messages.remove_listener('security_updated', on_security_updated_cb);
		}
	}
	
	void editor_step()
	{
		if(drag_i != -1 && !script.input.key_check_gvb(GVB::LeftClick))
		{
			drag_i = -1;
		}
		
		if(drag_i != -1)
		{
			SecurityBlock@ block = @blocks[drag_i];
			const float mx = script.input.mouse_x_world(19);
			const float my = script.input.mouse_y_world(19);
			
			if(hover_handle == -1)
			{
				block.x1 = mx - drag_ox;
				block.y1 = my - drag_oy;
				block.x2 = block.x1 + drag_sx * 48;
				block.y2 = block.y1 + drag_sy * 48;
				self.editor_sync_vars_menu();
				return;
			}
			
			if(hover_handle == 0 || hover_handle == 3)
				block.x1 = mx;
			else if(hover_handle == 1 || hover_handle == 2)
				block.x2 = mx;
			if(hover_handle == 0 || hover_handle == 1)
				block.y1 = my;
			else if(hover_handle == 2 || hover_handle == 3)
				block.y2 = my;
			
			self.editor_sync_vars_menu();
			return;
		}
		
		selected = self.editor_selected();
		hover_i = -1;
		hover_handle = -1;
		
		if(!selected)
			return;
		if(!script.input.key_check_gvb(GVB::Shift) || script.input.key_check_gvb(GVB::Space))
			return;
		
		const float mx = script.input.mouse_x_world(19);
		const float my = script.input.mouse_y_world(19);
		
		for(int i = int(blocks.length) - 1; i >= 0; i--)
		{
			SecurityBlock@ block = @blocks[i];
			const float x1 = floor(block.x1 / 48) * 48;
			const float y1 = floor(block.y1 / 48) * 48;
			const float x2 = ceil(block.x2 / 48) * 48;
			const float y2 = ceil(block.y2 / 48) * 48;
			
			if(mx >= x1 && mx <= x2 && my >= y1 && my <= y2)
			{
				hover_i = i;
				
				if(mx - x1 <= SecurityGateHandleSize)
				{
					if(my - y1 <= SecurityGateHandleSize)
						hover_handle = 0;
					else if(my >= y2 - SecurityGateHandleSize)
						hover_handle = 3;
						
				}
				else if(mx >= x2 - SecurityGateHandleSize)
				{
					if(my - y1 <= SecurityGateHandleSize)
						hover_handle = 1;
					else if(my >= y2 - SecurityGateHandleSize)
						hover_handle = 2;
				}
				
				break;
			}
		}
		
		if(hover_i != -1)
		{
			// Delete a block
			if(script.input.key_check_pressed_gvb(GVB::RightClick))
			{
				blocks.removeAt(hover_i);
				hover_i = -1;
				self.editor_sync_vars_menu();
				return;
			}
			
			// Drag a block
			if(script.input.key_check_pressed_gvb(GVB::LeftClick))
			{
				SecurityBlock@ block = @blocks[hover_i];
				drag_i = hover_i;
				drag_sx = ceil(block.x2 / 48) - floor(block.x1 / 48) - 1;
				drag_sy = ceil(block.y2 / 48) - floor(block.y1 / 48) - 1;
				if(hover_handle == -1 || hover_handle == 0 || hover_handle == 3)
					drag_ox = mx - block.x1;
				if(hover_handle == -1 || hover_handle == 0 || hover_handle == 1)
					drag_oy = my - block.y1;
				if(hover_handle == 1 || hover_handle == 2)
					drag_ox = mx - block.x2;
				if(hover_handle == 2 || hover_handle == 3)
					drag_oy = my - block.y2;
				return;
			}
			
			const int mouse_state = script.input.mouse_state();
			const int scroll_dir = mouse_state & 0x1 != 0 ? -1 : mouse_state & 0x2 != 0 ? 1 : 0;
			if(scroll_dir != 0)
			{
				SecurityBlock@ block = @blocks[hover_i];
				const int ii = block.tile_colour;
				block.tile_colour += scroll_dir;
				block.tile_colour = (4 + (block.tile_colour % 4)) % 4;
				self.editor_sync_vars_menu();
			}
		}
		// Add a block
		else if(script.input.key_check_pressed_gvb(GVB::LeftClick))
		{
			SecurityBlock@ block = SecurityBlock();
			block.x1 = mx;
			block.y1 = my;
			block.x2 = mx;
			block.y2 = my;
			blocks.insertLast(block);
			self.editor_sync_vars_menu();
			
			drag_i = blocks.length - 1;
			hover_i = drag_i;
			hover_handle = 2;
			drag_ox = 0;
			drag_oy = 0;
		}
	}
	
	void editor_draw(float)
	{
		if(!script.debug_triggers)
			return;
		
		const float x = self.x();
		const float y = self.y();
		
		for(uint i = 0; i < blocks.length; i++)
		{
			SecurityBlock@ block = @blocks[i];
			
			uint tile_clr = 0;
			switch(block.tile_colour)
			{
				case 0: tile_clr = 0xff476c44; break;
				case 1: tile_clr = 0xff42484d; break;
				case 2: tile_clr = 0xff98a3a3; break;
				case 3: tile_clr = 0xff1c1c1b; break;
			}
			
			const bool hovered = selected && hover_i == int(i);
			const float x1 = floor(block.x1 / 48) * 48;
			const float y1 = floor(block.y1 / 48) * 48;
			const float x2 = ceil(block.x2 / 48) * 48;
			const float y2 = ceil(block.y2 / 48) * 48;
			script.g.draw_rectangle_world(22, 22, x1 + 5, y1 + 5, x1 + 26, y1 + 26, 0, 0x55ffff77);
			script.g.draw_rectangle_world(22, 22, x1 + 8, y1 + 8, x1 + 23, y1 + 23, 0, tile_clr);
			outline_rect(script.g, 22, 22, x1, y1, x2, y2, 2, hovered ? 0x77ffff77 : 0x55ffff77);
			script.g.draw_line_world(22, 22, x, y, x1, y1, 4, selected ? 0x55ffff77 : 0x22ffff77);
			
			if(selected)
			{
				if(hovered)
				{
					script.g.draw_rectangle_world(22, 21, x1, y1, x2, y2, 0, 0x33ffff77);
				}
				
				// Corner handles
				if(hovered && hover_handle != -1)
				{
					if(hover_handle == 0)
						script.g.draw_rectangle_world(22, 22, x1, y1, x1 + SecurityGateHandleSize, y1 + SecurityGateHandleSize, 0, 0x77ffff77);
					else if(hover_handle == 1)
						script.g.draw_rectangle_world(22, 22, x2 - SecurityGateHandleSize, y1, x2, y1 + SecurityGateHandleSize, 0, 0x77ffff77);
					else if(hover_handle == 2)
						script.g.draw_rectangle_world(22, 22, x2 - SecurityGateHandleSize, y2 - SecurityGateHandleSize, x2, y2, 0, 0x77ffff77);
					else if(hover_handle == 3)
						script.g.draw_rectangle_world(22, 22, x1, y2 - SecurityGateHandleSize, x1 + SecurityGateHandleSize, y2, 0, 0x77ffff77);
				}
			}
		}
	}
	
	bool is_closed
	{
		get const override { return  closed; }
	}
	
	bool is_alive
	{
		get const override { return !self.destroyed(); }
	}
	
	void open(const bool open=true) override
	{
		if(closed == !open)
			return;
		
		activate(!open, play_sounds == 2 || play_sounds == 3);
		inactive = false;
		
		if(once == 2 || once == 5 || once == (!closed ? 3 : 4))
		{
			script.g.remove_entity(self.as_entity());
		}
	}
	
	void toggle() override
	{
		open(closed);
	}
	
	private bool calc_is_closed()
	{
		const bool r = exact_level ? (current_level == level) : (current_level >= level);
		return reversed ? !r : r;
	}
	
	private void on_security_updated(const string &in event, message@ msg)
	{
		if(inactive)
			return;
		if(level == -1 || current_level == script.security_level)
			return;
		
		current_level = script.security_level;
		activate(calc_is_closed(), play_sounds == 1 || play_sounds == 3);
		
		if(
			(!exact_level || script.security_level == level) &&
			(once == 1 || once == 5 || once == (!closed ? 3 : 4)))
		{
			script.g.remove_entity(self.as_entity());
		}
	}
	
	private void activate(const bool new_closed, const bool play_sounds, const bool force=false)
	{
		if(!force && new_closed == closed)
			return;
		
		closed = new_closed;
		
		tileinfo@ tile = create_tileinfo();
		tile.sprite_set(4);
		tile.sprite_tile(1);
		
		for(uint i = 0; i < blocks.length; i++)
		{
			SecurityBlock@ block = @blocks[i];
			
			tile.solid(closed);
			tile.sprite_palette(block.tile_colour);
			
			const int x1 = floor_int(block.x1 / 48);
			const int y1 = floor_int(block.y1 / 48);
			const int x2 = ceil_int(block.x2 / 48);
			const int y2 = ceil_int(block.y2 / 48);
			
			for(int x = x1; x < x2; x++)
			{
				for(int y = y1; y < y2; y++)
				{
					if(empty_only)
					{
						tileinfo@ t = script.g.get_tile(x, y);
						
						if(!closed
							? t.sprite_set() != 4 || t.sprite_tile() != 1 || t.sprite_palette() != uint(block.tile_colour)
							: t.solid())
							continue;
					}
					
					script.g.set_tile(x, y, 19, tile, true);
				}
			}
			
			// -- Add/remove a light prop
			
			if(block.light_prop != 0)
			{
				const float mid_x = (x1 * 48 + x2 * 48) * 0.5;
				const float mid_y = (y1 * 48 + y2 * 48) * 0.5;
				const int prop_index = block.light_prop == 1 ? 19 : 18;
				
				if(!closed)
				{
					int j = script.g.get_prop_collision(mid_y - 48, mid_y + 48, mid_x - 48, mid_x + 48);
					while(--j >= 0)
					{
						prop@ p = script.g.get_prop_collision_index(j);
						if(p.prop_set() != 4 || p.prop_group() != 28)
							continue;
						if(p.layer() != 19 || p.sub_layer() != 11)
							continue;
						if(int(p.prop_index()) != prop_index)
							continue;
						
						script.g.remove_prop(p);
					}
				}
				else
				{
					float ox, oy;
					Sprite spr('props4', 'machinery_' + prop_index);
					spr.real_position(mid_x, mid_y, 0, ox, oy);
					prop@ p = create_prop(4, 28, prop_index, ox, oy, 19, 11, 0);
					script.g.add_prop(p);
				}
			}
			
			// -- Update the side/top/bottom tiles afterwards so the "gate" tiles are behind them.
			
			if(block.update_top_colour != -1)
			{
				if(block.update_top_colour != -1)
				{
					tile.solid(true);
					tile.sprite_palette(block.update_top_colour);
				}
				
				int y = y1 - 1;
				for(int j = 0; j < 2; j++)
				{
					for(int x = x1; x < x2; x++)
					{
						tileinfo@ ct = script.g.get_tile(x, y);
						if(!ct.solid())
							continue;
						
						script.g.set_tile(x, y, 19, block.update_top_colour == -2 ? ct : tile, true);
					}
					
					y = y2;
				}
			}
			
			if(block.update_sides_colour != -1)
			{
				if(block.update_sides_colour != -1)
				{
					tile.solid(true);
					tile.sprite_palette(block.update_sides_colour);
				}
				
				int x = x1 - 1;
				for(int j = 0; j < 2; j++)
				{
					for(int y = y1; y < y2; y++)
					{
						tileinfo@ ct = script.g.get_tile(x, y);
						if(!ct.solid())
							continue;
						
						script.g.set_tile(x, y, 19, block.update_sides_colour == -2 ? ct : tile, true);
					}
					
					x = x2;
				}
			}
		}
		
		if(play_sounds && !force)
		{
			// Find the centre of all blocks and play the sound from there instead of the trigger position.
			// Because blocks could be inside unloaded chunks, which could cause the block to not open/close when triggered.
			// To avoid this some triggers are moved outside of the map so the blocks will always be in loaded chunks,
			// and playing the sounds from there wouldn't sound right.
			float centre_x = 0;
			float centre_y = 0;
			int count = 0;
			
			for(uint i = 0; i < blocks.length; i++)
			{
				if(force_sound != -1 && int(i) != force_sound)
					continue;
				
				SecurityBlock@ block = @blocks[i];
				const float x1 = floor(block.x1 / 48) * 48;
				const float y1 = floor(block.y1 / 48) * 48;
				const float x2 = ceil(block.x2 / 48) * 48;
				const float y2 = ceil(block.y2 / 48) * 48;
				
				centre_x += (x1 + x2) * 0.5;
				centre_y += (y1 + y2) * 0.5;
				count++;
			}
			
			centre_x = count > 0 ? centre_x / count : self.x();
			centre_y = count > 0 ? centre_y / count : self.y();
			
			if(!closed && open_sound != 0)
			{
				audio@ snd = script.g.play_sound(
					open_sound == 1 ? 'sfx_door_lab' : 'sfx_door_unlock',
					centre_x, centre_y, 1, false, true);
				snd.time_scale(rand_range(0.9, 1.1));
			}
			
			if(closed && close_sound != 0)
			{
				audio@ snd = script.g.play_sound(
					close_sound == 1 ? 'sfx_trashcan_land'
						: close_sound == 2 ? 'sfx_impact_parry' : 'sfx_springball_land_light',
					centre_x, centre_y, 1, false, true);
				snd.time_scale(rand_range(0.9, 1.1));
			}
		}
	}
	
}

class SecurityBlock
{
	
	[option,Green,Silver,White,Black]
	int tile_colour = 1;
	[option,-1:None,Green,Silver,White,Black,-2:Keep]
	int update_top_colour = -1;
	[option,-1:None,Green,Silver,White,Black,-2:Keep]
	int update_sides_colour = -1;
	[option,None,Small,Big] int light_prop = 0;
	[position,mode:world,layer:19,y:y1] float x1;
	[hidden] float y1;
	[position,mode:world,layer:19,y:y2] float x2;
	[hidden] float y2;
	
}
