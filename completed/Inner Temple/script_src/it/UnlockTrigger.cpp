#include '../common/PropGroupNames.cpp';
#include 'PropGrower.cpp';

class UnlockTrigger : trigger_base, callback_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	array<array<int>> water_tiles = {
		{71,-51,0, 71,-50,0, 71,-49,0, 71,-48,0, 71,-47,0, 71,-46,0, 71,-45,0, 71,-44,0, 71,-43,0, 71,-42,0, 71,-41,0, 71,-40,0, 71,-39,0, 71,-38,0, 71,-37,0, 71,-36,0, 71,-35,0, 71,-34,0, 71,-33,0, 71,-32,0, 71,-31,0, 71,-30,0, 71,-29,0, 71,-28,0, 71,-27,0, 71,-26,0, 71,-25,0, 71,-24,0, 71,-23,0, 71,-22,0, 71,-21,0, 72,-21,0, 70,-21,0, 69,-21,0, 68,-21,0, 67,-21,0, 66,-21,20, 66,-20,0, 65,-20,0, 64,-20,0, 63,-20,0, 62,-20,0, 61,-20,0, 60,-20,0, 59,-20,0},
		{93,-51,0, 93,-50,0, 93,-49,0, 93,-48,0, 93,-47,0, 93,-46,0, 93,-45,0, 93,-44,0, 93,-43,0, 93,-42,0, 93,-41,0, 93,-40,0, 93,-39,0, 93,-38,0, 93,-37,0, 93,-36,0, 93,-35,0, 93,-34,0, 93,-33,0, 93,-32,0, 93,-31,0, 93,-30,0, 93,-29,0, 93,-28,0, 93,-27,0, 93,-26,0, 93,-25,0, 93,-24,0, 93,-23,0, 93,-22,0, 93,-21,0, 94,-21,0, 95,-21,0, 96,-21,0, 97,-21,0, 98,-21,0, 92,-21,0},
	};
	[hidden] array<uint> water_tile_index = {0, 0};
	int water_tile_count;
	
	[hidden] int t = 0;
	
	array<EmitterStruct> emitter_events = {
		EmitterStruct(59, 42, 3404, -1004, 30, 12, 15, 12),
		EmitterStruct(59, 42, 3462, -1006, 21, 19, 15, 12),
		EmitterStruct(59, 42, 4461, -1013, 30, 22, 15, 12),
		EmitterStruct(59, 42, 4519, -1006, 16, 23, 15, 12),
		
		EmitterStruct(61, 42, 3352, -898, 231, 36, 14, 12),
		EmitterStruct(61, 42, 4576, -903, 326, 46, 14, 12),
		
		EmitterStruct(65, 42, 3217, -988, 19, 18, 15, 12),
		EmitterStruct(65, 42, 3184, -955, 22, 16, 15, 12),
		
		EmitterStruct(63, 42, 3030, -859, 319, 28, 14, 12),
	};
	[hidden] int emitter_event_index = 0;
	[hidden] int emitter_event_count = 0;
	
	array<PropStruct> prop_events = {
		PropStruct(20, 3292, -1771, 270, 1, 1, 3, 22, 3, 0, 15, 19),
		PropStruct(20, 4385, -1779, 270, 1, 1, 3, 22, 3, 0, 15, 19),
		
		PropStruct(59, 3403, -1056, 19.9951, 1, 1, 3, 22, 3, 0, 15, 19),
		PropStruct(59, 4022, -830, 339.999, 1, 1, 3, 22, 3, 0, 15, 19),
		
		PropStruct(65, 3055, -983, 19.9951, 1, 1, 3, 22, 3, 0, 15, 19),
	};
	[hidden] int prop_event_index = 0;
	[hidden] int prop_event_count = 0;
	[text] float prop_fade_frames = 15;
	[hidden] array<int> prop_fade_in;
	
	[entity] int apple_id;
	[entity] int ambient_id;
	[text] int period = 2;
	
	[position,layer:19,y:foliage_y] float foliage_x;
	[hidden] float foliage_y;
	
	[position,layer:19,y:wfall1_y] float wfall1_x;
	[hidden] float wfall1_y;
	[position,layer:19,y:wfall2_y] float wfall2_x;
	[hidden] float wfall2_y;
	[position,layer:19,y:wfall_y] float wfall_x;
	[hidden] float wfall_y;
	entity@ wfall1 = null;
	entity@ wfall2 = null;
	float wfall1_top, wfall2_top;
	[hidden] int wfall1_id = 0;
	[hidden] int wfall2_id = 0;
	[hidden] float wfall_t = 0;
	
	[hidden] bool active = false;
	
	UnlockTrigger()
	{
		@g = get_scene();
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.self = @self;
		add_broadcast_receiver('UNLOCK', this, 'on_unlock');
		water_tile_count = int(water_tiles.length());
		emitter_event_count = int(emitter_events.length());
		prop_event_count = int(prop_events.length());
		
		for(uint i = 0; i < prop_fade_in.length(); i++)
		{
			prop_events[prop_fade_in[i]].init();
		}
	}
	
	void on_unlock(string id, message@ msg)
	{
		active = true;
		
		entity@ amb = entity_by_id(ambient_id);
		if(amb !is null)
		{
			camera@ cam = get_active_camera();
			amb.set_xy(cam.x(), cam.y());
		}
	}
	
	void step()
	{
		if(!active) return;
		
		bool busy = false;
		
		for(int i = int(prop_fade_in.length()) - 1; i >= 0; i--)
		{
			PropStruct@ p = @prop_events[prop_fade_in[i]];
			if(p.t + prop_fade_frames <= t)
			{
				prop@ pr = create_prop(p.prop_set, p.prop_group, p.prop_index, p.layer, p.sub_layer, p.x, p.y, p.rotation);
				g.add_prop(pr);
				prop_fade_in.removeAt(i);
				continue;
			}
			
			busy = true;
		}
		
		if(t % period == 0)
		{
			for(int i = 0; i < water_tile_count; i++)
			{
				array<int>@ tiles = water_tiles[i];
				if(water_tile_index[i] < tiles.length())
				{
					const int j = water_tile_index[i];
					
					int tx = tiles[j], ty = tiles[j + 1];
					int type = tiles[j + 2];
					tileinfo@ tile = g.get_tile(tx, ty, 14);
					tile.solid(true);
					tile.type(type);
					tile.sprite_set(1);
					tile.sprite_tile(20);
					tile.sprite_palette(0);
					g.set_tile(tx, ty, 14, tile, true);
					
					water_tile_index[i] += 3;
					busy = true;
				}
			}
		}
		else
		{
			busy = true;
		}
		
		if(wfall_t < 1)
		{
			if(wfall1 is null)
			{
				if(wfall1_id == 0)
				{
					@wfall1 = create_emitter(49, wfall1_x, wfall1_y, 10, 10, 14, 12);
					g.add_entity(wfall1);
					wfall1_id = wfall1.id();
					@wfall2 = create_emitter(49, wfall2_x, wfall2_y, 10, 10, 14, 12);
					g.add_entity(wfall2);
					wfall2_id = wfall2.id();
				}
				else
				{
					@wfall1 = entity_by_id(wfall1_id);
					@wfall2 = entity_by_id(wfall2_id);
				}
				wfall1_top = wfall1.y();
				wfall2_top = wfall2.y();
			}
			else
			{
				set_emitter_height(wfall1, wfall1_top, int((wfall_y - wfall1_top) * wfall_t));
				set_emitter_height(wfall2, wfall2_top, int((wfall_y - wfall2_top) * wfall_t));
				wfall_t += DT;
			}
			
			busy = true;
		}
		
		while(emitter_event_index < emitter_event_count)
		{
			if(emitter_events[emitter_event_index].t > t)
				break;
			
			if(emitter_events[emitter_event_index].t == t)
			{
				g.add_entity(emitter_events[emitter_event_index].create());
			}
			
			emitter_event_index++;
		}
		
		
		while(prop_event_index < prop_event_count)
		{
			PropStruct@ prop_event = @prop_events[prop_event_index];
			
			if(prop_event.t > t)
				break;
			
			if(prop_event.t == t)
			{
				prop_fade_in.insertLast(prop_event_index);
				prop_event.init();
				busy = true;
			}
			
			prop_event_index++;
		}
		
		if(!busy)
		{
			PropGrower grower;
			grower.apple_id = apple_id;
			grower.apple_x = self.x();
			grower.apple_y = self.y();
			scripttrigger@ st = create_scripttrigger(grower);
			st.set_xy(floor(foliage_x / 48) * 48, floor(foliage_y / 48) * 48);
			g.add_entity(st.as_entity());
			
			g.remove_entity(self.as_entity());
		}
		
		t++;
	}
	
	void draw(float sub_frame)
	{
		if(!active) return;
		
		for(uint i = 0; i < prop_fade_in.length(); i++)
		{
			PropStruct@ p = @prop_events[prop_fade_in[i]];
			const float alpha = (t - p.t) / prop_fade_frames;
			const uint a = uint(0xFF * alpha) << 24;
			p.spr.draw_world(p.layer, p.sub_layer, p.sprite_name, 0, p.palette, p.x, p.y, p.rotation, p.scale_x, p.scale_y, a + 0xFFFFFF);
		}
	}
	
	void set_emitter_height(entity@ emitter, float y, int height)
	{
		g.remove_entity(emitter);
		varstruct@ vars = emitter.vars();
		vars.get_var("height").set_int32(height);
		g.add_entity(emitter);
		emitter.y(y + height * 0.5);
	}
	
}

class EmitterStruct
{
	
	int t;
	int id;
	float x, y;
	int width, height;
	int layer, sub_layer;
	
	EmitterStruct(){}
	
	EmitterStruct(int t, int id, float x, float y, int width, int height, int layer, int sub_layer)
	{
		this.t = t;
		this.id = id;
		this.x = x;
		this.y = y;
		this.width = width;
		this.height = height;
		this.layer = layer;
		this.sub_layer = sub_layer;
	}
	
	entity@ create()
	{
		return create_emitter(id, x, y, width, height, layer, sub_layer);
	}
	
}

class PropStruct
{
	
	int t;
	float x, y;
	float rotation;
	float scale_x, scale_y;
	uint prop_set, prop_group, prop_index;
	uint palette;
	uint layer, sub_layer;
	
	sprites@ spr;
	string sprite_name;
	
	PropStruct(){}
	
	PropStruct(int t, float x, float y, float rotation, float scale_x, float scale_y, uint prop_set, uint prop_group, uint prop_index, uint palette, uint layer, uint sub_layer)
	{
		this.t = t;
		this.x = x;
		this.y = y;
		this.rotation = rotation;
		this.scale_x = scale_x;
		this.scale_y = scale_y;
		this.prop_set = prop_set;
		this.prop_group = prop_group;
		this.prop_index = prop_index;
		this.palette = palette;
		this.layer = layer;
		this.sub_layer = sub_layer;
	}
	
	void init()
	{
		@spr = create_sprites();
		string sprite_set;
		sprite_from_prop(prop_set, prop_group, prop_index, sprite_set, sprite_name);
		spr.add_sprite_set(sprite_set);
	}
	
}