#include '../../lib/enums/ColType.cpp';
#include '../../lib/enums/EntityState.cpp';
#include '../../lib/tiles/EdgeFlags.cpp';
#include '../../lib/tiles/TileShape.cpp';

class Door : trigger_base, callback_base, IInitHandler
{
	
	[position,mode:world,layer:19,y:y1] float x1;
	[hidden] float y1;
	[position,mode:world,layer:19,y:y2] float x2;
	[hidden] float y2;
	[persist] string name = 'door1';
	[persist] float time = 0;
	[persist] int count = 1;
	[persist] string sound = '';
	[persist] int sprite_set = 1;
	[persist] int sprite_tile = 19;
	[persist] int sprite_palette = 1;
	[persist] int layer = 19;
	[persist] int sub_layer = 12;
	[persist] int emitter_id = -1;
	[persist] int splash_emitter_id = -1;
	[persist] bool alternate_palette = true;
	[persist] bool death_zone = false;
	[persist] bool global = false;
	
	[hidden] bool open;
	[hidden] int current_count;
	
	script@ script;
	scripttrigger@ self;
	
	entity@ death_zone_e;
	entity@ emitter;
	entity@ splash_emitter;
	
	float tx1, ty1;
	float tx2, ty2;
	float mx, my;
	float sx, sy;
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = script;
		@this.self = self;
		
		self.radius(0);
		self.editor_handle_size(5);
		
		script.init_subscribe(this);
		calc_box();
		
		if(!script.is_playing)
			return;
		
		update();
	}
	
	void init() override
	{
		if(!script.is_playing)
			return;
		
		if(global)
		{
			open = script.get_global(name);
		}
		
		update_tiles();
		script.messages.add_listener(name, MessageHandler(on_lever));
	}
	
	private void calc_box()
	{
		tx1 = floor(min(x1, x2) / 48) * 48;
		ty1 = floor(min(y1, y2) / 48) * 48;
		tx2 = ceil(max(x1, x2) / 48) * 48;
		ty2 = ceil(max(y1, y2) / 48) * 48;
		mx = (tx1 + tx2) * 0.5;
		my = (ty1 + ty2) * 0.5;
		sx = (tx2 - tx1);
		sy = (ty2 - ty1);
	}
	
	void step()
	{
		if(death_zone && !open)
		{
			int i = script.g.get_entity_collision(
				ty1, ty2, tx1 + 10, tx2 - 10,
				ColType::Hittable);
			
			while(--i >= 0)
			{
				dustman@ dm = script.g.get_dustman_collision_index(i);
				if(@dm != null)
				{
					if(!dm.dead())
					{
						dm.stun(0, 0);
					}
					
					dm.kill(false);
					continue;
				}
				
				controllable@ c = script.g.get_controllable_collision_index(i);
				if(@c != null)
				{
					script.g.remove_entity(c.as_entity());
					continue;
				}
			}
		}
	}
	
	void editor_draw(float sub_frame)
	{
		outline_rect(script.g, 22, 22, tx1, ty1, tx2, ty2, 1 * script.ed_zoom, 0xffff3333);
		
		if(death_zone)
		{
			outline_rect(script.g, 22, 22, tx1 + 10, ty1, tx2 - 10, ty2, 1 * script.ed_zoom, 0x55ff3333);
		}
	}
	
	private void update()
	{
		if(open && @emitter != null)
		{
			script.g.remove_entity(emitter);
			@emitter = null;
		}
		
		if(open && @splash_emitter != null)
		{
			script.g.remove_entity(splash_emitter);
			@splash_emitter = null;
		}
		
		calc_box();
		
		if(emitter_id != -1 && !open && @emitter == null)
		{
			@emitter = create_emitter(emitter_id,
				mx, my, int(sx - 20), int(sy - 20),
				layer, sub_layer, 0);
			script.g.add_entity(emitter, false);
		}
		if(splash_emitter_id != -1 && !open && @splash_emitter == null)
		{
			@splash_emitter = create_emitter(splash_emitter_id,
				mx, ty2, int(tx2 - tx1) + 30, 24,
				layer, sub_layer, 0);
			script.g.add_entity(splash_emitter, false);
		}
	}
	
	private void update_tiles()
	{
		const int tx1 = floor_int(this.x1 / 48);
		const int ty1 = floor_int(this.y1 / 48);
		const int tx2 = floor_int(this.x2 / 48);
		const int ty2 = floor_int(this.y2 / 48);
		tileinfo@ tile = create_tileinfo();
		tile.type(TileShape::Full);
		tile.solid(!open);
		
		tile.sprite_set(sprite_set);
		tile.sprite_tile(sprite_tile);
		tile.sprite_palette(sprite_palette);
		
		for(int x = tx1; x <= tx2; x++)
		{
			if(alternate_palette)
			{
				tile.sprite_palette(1 + x % 2);
			}
			
			for(int y = ty1; y <= ty2; y++)
			{
				const bool col_top = !(!open && y == ty1 && is_touching(x, y - 1, layer));
				const bool col_bottom = !(!open && y == ty2 && is_touching(x, y + 1, layer));
				const bool col_left = !(!open && x == tx1 && is_touching(x - 1, y, layer));
				const bool col_right = !(!open && x == tx2 && is_touching(x + 1, y, layer));
				tile.edge_top(col_top && y == ty1 ? Collision : Clear);
				tile.edge_bottom(col_bottom && y == ty2 ? Collision : Clear);
				tile.edge_left(col_left && x == tx1 ? Collision : Clear);
				tile.edge_right(col_right && x == tx2 ? Collision : Clear);
				script.g.set_tile(x, y, layer, tile, open);
			}
		}
	}
	
	private bool is_touching(const int x, const int y, const int layer)
	{
		tileinfo@ tile = script.g.get_tile(x, y, layer);
		return tile.type() == TileShape::Full && tile.solid();
	}
	
	private void on_lever(const string &in id, message@ msg)
	{
		if(msg.get_string('type') == 'door')
			return;
		
		current_count = clamp(current_count + msg.get_int('count'), 0, count);
		
		if(open)
		{
			if(current_count < count)
			{
				open = false;
				update_tiles();
				update();
			}
		}
		else if(current_count == count || count < 0)
		{
			open = true;
			
			if(time > 0)
			{
				Lever::trigger(script, name, 0, time, 0, 'door');
			}
			
			update_tiles();
			update();
			
			if(sound != '')
			{
				audio@ snd = script.g.play_sound(
					sound, self.x(), self.y(),
					rand_range(0.9, 1.0), false, true);
				snd.time_scale(rand_range(0.9, 1.1));
			}
			
			if(count < 0 || time < 0)
			{
				script.g.remove_entity(self.as_entity());
			}
		}
	}
	
	void editor_var_changed(var_info@ var)
	{
		const string name = var.name;
		
		if(name == 'x1' || name == 'y1' || name == 'x2' || name == 'y2')
		{
			calc_box();
		}
	}
	
}
