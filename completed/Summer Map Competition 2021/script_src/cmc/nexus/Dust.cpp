#include '../../lib/enums/EmitterId.cpp';
#include '../../lib/emitters/common.cpp';
#include '../../lib/props/Prop.cpp';
#include '../../lib/tiles/common.cpp';
#include '../../lib/tiles/get_tile_edge_points.cpp';
#include '../../lib/tiles/TileEdge.cpp';
#include '../../lib/tiles/EdgeFlags.cpp';

const int DustGlowSpriteLayer = 19;
const int DustGlowSpriteSubLayer = 4;
const int DustGlowSpriteSet = 2;
const int DustGlowSpriteGroup = 22;
const int DustGlowSpriteIndex = 4;

class Dust : trigger_base
{
	
	[position,mode:world,layer:19,y:y1] float x1;
	[hidden] float y1;
	[position,mode:world,layer:19,y:y2] float x2;
	[hidden] float y2;
	[persist] string name = 'flowers1';
	[persist] int id = 0;
	
	[hidden] array<bool> collected;
	[hidden] int collected_count;
	
	script@ script;
	scripttrigger@ self;
	
	array<TileDust> dust;
	float ex1, ey1, ex2, ey2;
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = script;
		@this.self = self;
		
		self.radius(0);
		self.editor_handle_size(7);
		self.editor_colour_inactive(0xff88ff88);
		self.editor_colour_active(0xff44ff44);
		self.editor_colour_circle(0xff88ff88);
		
		update_tiles();
	}
	
	private void update_tiles()
	{
		const int tx1 = floor_int(min(x1, x2) / 48);
		const int ty1 = floor_int(min(y1, y2) / 48);
		const int tx2 = floor_int(max(x1, x2) / 48);
		const int ty2 = floor_int(max(y1, y2) / 48);
		
		dust.resize(0);
		
		for(int tx = tx1; tx <= tx2; tx++)
		{
			for(int ty = ty1; ty <= ty2; ty++)
			{
				tileinfo@ tile = script.g.get_tile(tx, ty);
				if(!tile.solid())
					continue;
				
				const int shape = tile.type();
				const float x = tx * 48;
				const float y = ty * 48;
				for(int side = TileEdge::Top; side <= TileEdge::Right; side++)
				{
					const uint8 edge = get_tile_edge(tile, side);
					if(edge & EdgeFlags::Collision == 0)
						continue;
					
					float ex1, ey1, ex2, ey2;
					const bool is_valid = get_edge_points(shape, side, ex1, ey1, ex2, ey2, x, y);
					if(!is_valid)
						continue;
					
					TileDust d;
					d.tx = tx;
					d.ty = ty;
					d.x1 = ex1;
					d.y1 = ey1;
					d.x2 = ex2;
					d.y2 = ey2;
					d.mx = (ex1 + ex2) * 0.5;
					d.my = (ey1 + ey2) * 0.5;
					d.angle = atan2(ey2 - ey1, ex2 - ex1);
					d.length = distance(ex1, ey1, ex2, ey2);
					d.edge = TileEdge(side);
					dust.insertLast(d);
					
					if(dust.length == 0)
					{
						this.ex1 = this.ex2 = d.mx;
						this.ey1 = this.ey2 = d.my;
					}
					else
					{
						this.ex1 = min(this.ex1, d.mx);
						this.ey1 = min(this.ey1, d.my);
						this.ex2 = max(this.ex2, d.mx);
						this.ey2 = max(this.ey2, d.my);
					}
				}
			}
		}
		
		if(collected.length != dust.length)
		{
			collected.resize(dust.length);
			for(uint i = 0; i < dust.length; i++)
			{
				collected[i] = false;
			}
			
			collected_count = 0;
		}
		
		if(script.is_playing)
		{
			for(uint i = 0; i < dust.length; i++)
			{
				if(collected[i])
					continue;
				
				dust[i].add_emitter(script);
			}
		}
	}
	
	void step()
	{
		int i = script.g.get_entity_collision(
			ey1, ey2, ex1, ex2,
			ColType::Hitbox);
		
		while(--i >= 0)
		{
			hitbox@ hb = script.g.get_hitbox_collision_index(i);
			if(@hb == null)
				continue;
			if(!hb.triggered() || hb .state_timer() != hb.activate_time())
				continue;
			controllable@ c = hb.owner();
			if(@c == null || c.player_index() == -1)
				continue;
			
			rectangle@ r = hb.base_rectangle();
			float hb_x1 = hb.x() + r.left();
			float hb_y1 = hb.y() + r.top();
			float hb_x2 = hb.x() + r.right();
			float hb_y2 = hb.y() + r.bottom();
			
			for(uint j = 0; j < dust.length; j++)
			{
				if(collected[j])
					continue;
				
				TileDust@ d = @dust[j];
				if(d.mx < hb_x1 || d.mx > hb_x2 || d.my < hb_y1 || d.my > hb_y2)
					continue;
				
				d.remove_emitter(script);
				d.remove_glow_prop(script);
				collected[j] = true;
				collected_count++;
				
				if(collected_count == int(dust.length))
				{
					message@ msg = create_message();
					msg.set_int('id', id);
					msg.set_string('type', 'dust');
					msg.set_int('count', 1);
					script.messages.broadcast(name, msg);
					script.g.remove_entity(self.as_entity());
					break;
				}
			}
		}
	}
	
	//void draw(float sub_frame)
	//{
	//}
	
	void editor_draw(float sub_frame)
	{
		//draw(sub_frame);
		
		for(uint i = 0; i < dust.length; i++)
		{
			if(collected[i])
				continue;
			
			TileDust@ d = @dust[i];
			draw_line(script.g, 19, 19, d.x1, d.y1, d.x2, d.y2, 2 * script.ed_zoom, 0x88ff0000);
			draw_dot(script.g, 19, 19, d.mx, d.my, 3 * script.ed_zoom, 0xffff5500, 45);
		}
		
		outline_rect_inside(script.g, 21, 21,
			floor(min(x1, x2) / 48) * 48 - 5,
			floor(min(y1, y2) / 48) * 48 - 5,
			ceil(max(x1, x2) / 48) * 48 + 5,
			ceil(max(y1, y2) / 48) * 48 + 5,
			2 * script.ed_zoom, 0x8844ff44);
	}
	
	void editor_var_changed(var_info@ var)
	{
		const string name = var.name;
		if(name == 'x1' || name == 'y1' || name == 'x2' || name == 'y2')
		{
			update_tiles();
		}
	}
	
}

class TileDust
{
	
	int tx;
	int ty;
	float x1;
	float y1;
	float x2;
	float y2;
	float mx;
	float my;
	float angle;
	float length;
	TileEdge edge;
	entity@ emitter;
	
	void add_emitter(script@ script)
	{
		if(@emitter != null)
			return;
		
		@emitter = create_emitter(EmitterId::SlimeBarrel, mx, my, 6, int(length), 19, 14, int(angle * RAD2DEG) - 90);
		script.g.add_entity(emitter, false);
	}
	
	void remove_emitter(script@ script)
	{
		if(@emitter == null)
			return;
		
		script.add_emitter_burst(
			EmitterId::PolygonsGroundDestroy,
			19, 14,
			mx, my,
			6, int(length), int(angle * RAD2DEG) - 90, 0.25, 1);
		
		script.g.remove_entity(emitter);
		@emitter = null;
	}
	
	void remove_glow_prop(script@ script)
	{
		int i = script.g.get_prop_collision(y1, y2, x2, x2);
		prop@ closest;
		float closest_dist = 9999999;
		
		while(i-- > 0)
		{
			prop@ p = script.g.get_prop_collision_index(i);
			
			if(p.layer() != DustGlowSpriteLayer || p.sub_layer() != DustGlowSpriteSubLayer)
				continue;
			if(p.prop_set() != DustGlowSpriteSet || p.prop_group() != DustGlowSpriteGroup || p.prop_index() != DustGlowSpriteIndex)
				continue;
			
			Prop pr(p, 0.5, 0.5, script.g);
			const float dist = dist_sqr(mx, my, pr.anchor_x, pr.anchor_y);
			if(dist < closest_dist)
			{
				closest_dist = dist;
				@closest = p;
			}
		}
		
		if(@closest != null)
		{
			script.g.remove_prop(closest);
		}
	}
	
}
