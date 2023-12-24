#include '../lib/props/Prop.cpp';
#include '../lib/phys/springs/constraints/CirclesCollisionConstraint.cpp';

#include 'settings/BreakableFloorSettings.cpp';
#include 'DebrisParticle.cpp';

class BreakableFloor : trigger_base
{
	
	[position,mode:world,layer:19,y:y1,tiles,round:down] int x1;
	[hidden] int y1;
	[position,mode:world,layer:19,y:y2,tiles,round:up] int x2;
	[hidden] int y2;
	[option,0:Right,1:Down,2:Left,3:Up]
	int direction = 1;
	
	[hidden] bool destroyed = false;
	
	script@ script;
	scripttrigger@ self;
	CachedTileProvider tile_provider;
	array<TileConstraint@> tile_constraints;
	CirclesCollisionConstraint circles_constraint;
	
	private array<DebrisParticle@> debris;
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.self = self;
		@this.script = script;
		
		if(destroyed)
		{
			script.g.remove_entity(self.as_entity());
			return;
		}
		
		self.radius(0);
	}
	
	void step()
	{
		if(destroyed)
		{
			tile_provider.frame = script.frame;
			
			for(int i = int(debris.length) - 1; i >= 0; i--)
			{
				if(!debris[i].step(i, @debris))
				{
					circles_constraint.particles.removeAt(circles_constraint.particles.findByRef(debris[i]));
					
					script.spring_system.remove_particle(debris[i]);
					@debris[i] = debris[int(debris.length) - 1];
					debris.resize(int(debris.length) - 1);
					
					script.spring_system.remove_constraint(tile_constraints[i]);
					@tile_constraints[i] = tile_constraints[int(tile_constraints.length) - 1];
					tile_constraints.resize(int(tile_constraints.length) - 1);
				}
			}
			
			if(debris.length == 0)
			{
				script.g.remove_entity(self.as_entity());
				script.spring_system.remove_constraint(@circles_constraint);
			}
			
			return;
		}
		
		const float rx1 = x1 * 48;
		const float ry1 = y1 * 48;
		const float rx2 = x2 * 48;
		const float ry2 = y2 * 48;
		
		const float o = 24;
		int i = script.g.get_entity_collision(
			ry1 - o, ry2 + o, rx1 - o, rx2 + o,
			StoneBlock::ColType);
		
		while(--i >= 0)
		{
			scriptenemy@ se = script.g.get_scriptenemy_collision_index(i);
			if(@se == null)
				continue;
			StoneBlock@ sb = cast<StoneBlock@>(se.get_object());
			if(@sb == null)
				continue;
			
			for(int j = 0; j < sb.num_heavy_land_contacts; j += 3)
			{
				const float cx = sb.heavy_land_contacts[j];
				const float cy = sb.heavy_land_contacts[j + 1];
				const float cf = sb.heavy_land_contacts[j + 2];
				
				if(cx > rx1 - 2 && cy > ry1 - 2 && cx < rx2 + 2 && cy < ry2 + 2)
				{
					destroy(cx, cy);
					return;
				}
			}
		}
	}
	
	void destroy(const float cx, const float cy)
	{
		destroyed = true;
		
		const float rx1 = x1 * 48;
		const float ry1 = y1 * 48;
		const float rx2 = x2 * 48;
		const float ry2 = y2 * 48;
		
		// Remove emitters
		entity@ e = emitter_by_id(script.g, rx1, ry1, rx2, ry2, BreakableFloor::RemoveEmitterId);
		if(@e != null)
		{
			script.g.remove_entity(e);
		}
		
		// Remove shadow props - sun or moon props on layer 19.4
		const array<uint>@ remove_props = @BreakableFloor::RemoveProps;
		int i = script.g.get_prop_collision(ry1, ry2, rx1, rx2);
		while(--i >= 0)
		{
			prop@ p = script.g.get_prop_collision_index(i);
			
			for(uint j = 0; j < remove_props.length; j += 5)
			{
				if(p.layer() != remove_props[j + 0] || p.sub_layer() != remove_props[j + 1])
					continue;
				if(p.prop_set() != remove_props[j + 2] || p.prop_group() != remove_props[j + 3] || p.prop_index() != remove_props[j + 4])
					continue;
				
				// Make sure the centre of the prop is inside of the remove rect
				Prop pr(p, 0.5, 0.5, script.g);
				if(pr.anchor_x > rx1 && pr.anchor_y > ry1 && pr.anchor_x < rx2 && pr.anchor_y < ry2)
				{
					script.g.remove_prop(p);
				}
			}
		}
		
		// Remove tiles
		for(int tx = x1; tx < x2; tx++)
		{
			for(int ty = y1; ty < y2; ty++)
			{
				script.g.set_tile(tx, ty, 19, false, 0, 0, 0, 0);
				script.g.set_tile(tx, ty, 17, false, 0, 0, 0, 0);
				script.g.set_tile_filth(tx, ty, 0, 0, 0, 0, true, true);
			}
		}
		
		// Add particles burst
		int w = int(rx2 - rx1);
		int h = int(ry2 - ry1);
		if(direction % 2 != 0)
		{
			int t = w;
			w = h;
			h = t;
		}
		script.add_emitter_burst(
			BreakableFloor::EmitterId,
			BreakableFloor::EmitterLayer, BreakableFloor::EmitterSubLayer,
			(rx1 + rx2) * 0.5, (ry1 + ry2) * 0.5,
			w, h, direction * 90, 0.5);
		
		// Create debris
		
		@tile_provider.g = script.g;
		script.spring_system.add_constraint(@circles_constraint);
		
		for(int tx = x1; tx < x2; tx++)
		{
			for(int ty = y1; ty < y2; ty++)
			{
				string spr_group;
				string spr_name;
				float scale;
				
				if(frand() <= BreakableFloor::BlockDebrisChance)
				{
					spr_group = 'props1';
					spr_name = 'buildingblocks_8';
					scale = rand_range(BreakableFloor::DebrisScaleMin, BreakableFloor::DebrisScaleMax);
				}
				else if(frand() >= 0.5)
				{
					spr_group = 'props2';
					spr_name = 'boulders_1';
					scale = rand_range(BreakableFloor::Boulder1DebrisScaleMin, BreakableFloor::Boulder1DebrisScaleMax);
				}
				else
				{
					spr_group = 'props2';
					spr_name = 'boulders_2';
					scale = rand_range(BreakableFloor::Boulder2DebrisScaleMin, BreakableFloor::Boulder2DebrisScaleMax);
				}
				
				const float px = tx * 48 + 24;
				const float py = ty * 48 + 24;
				float dx = px - cx;
				float dy = py - cy;
				const float dist = sqrt(dx * dx + dy * dy);
				const float force = map(dist, 48, BreakableFloor::DebrisMaxDistForce, BreakableFloor::DebrisForce, 0);
				dx = dx / dist * force;
				dy = dy / dist * force;
				DebrisParticle p(
					px + rand_range(-BreakableFloor::DebrisOffset, BreakableFloor::DebrisOffset),
					py + rand_range(-BreakableFloor::DebrisOffset, BreakableFloor::DebrisOffset),
					rand_range(-BreakableFloor::DebrisRot, BreakableFloor::DebrisRot),
					dx, dy,
					rand_range(-BreakableFloor::DebrisRotVel, BreakableFloor::DebrisRotVel),
					BreakableFloor::DebrisFric,
					BreakableFloor::DebrisRotFric,
					rand_range(BreakableFloor::DebrisLifeMin, BreakableFloor::DebrisLifeMax),
					spr_group, spr_name,
					0.5, 0.5,
					BreakableFloor::DebrisLayer, BreakableFloor::DebrisSubLayer,
					0, 0, scale, scale);
				p.radius = 24;
				circles_constraint.particles.insertLast(p);
				debris.insertLast(@p);
				script.spring_system.add_particle(p);
				
				TileConstraint@ tc = script.spring_system.add_constraint(
					TileConstraint(tile_provider, p, BreakableFloor::DebrisFric));
				tile_constraints.insertLast(tc);
			}
		}
	}
	
	void draw(float sub_frame)
	{
		if(!destroyed)
			return;
		
		for(uint i = 0; i < debris.length; i++)
		{
			debris[i].draw(sub_frame);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		script.g.draw_rectangle_world(22, 22,
			x1 * 48, y1 * 48,
			x2 * 48, y2 * 48,
			0, 0x55ff0000);
	}
	
}
