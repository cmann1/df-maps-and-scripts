#include '../lib/phys/springs/constraints/RopeCollisionConstraint.cpp';
#include '../lib/phys/springs/CachedTileProvider.cpp';

#include 'settings/RopeSettings.cpp';
#include 'ConstraintTracker.cpp';
#include 'IRopeAttachment.cpp';

class Rope :
	trigger_base,
	LifecycleEntity, ConstraintTracker, ITileProvider
{
	
	[persist] bool chain;
	
	[entity] uint entity1_id;
	[entity] uint entity2_id;
	[persist] bool anchor_self;
	[position,mode:world,layer:19,y:anchor_y] float anchor_x;
	[hidden] float anchor_y;
	[persist] float length;
	[persist] float thickness = 8;
	[persist] float segment_length = 48;
	[persist] bool tile_collisions;
	[persist] bool passthu;
	[position,mode:world,layer:19,y:passthu_y1,tiles] int passthu_x1;
	[hidden] int passthu_y1;
	[position,mode:world,layer:19,y:passthu_y2,tiles] int passthu_x2;
	[hidden] int passthu_y2;
	
	[hidden] array<float> points;
	
	scripttrigger@ self;
	sprites@ spr;
	
	bool initialised_attachments;
	bool draw_self = true;
	IRopeAttachment@ attach1;
	IRopeAttachment@ attach2;
	Particle@ p1;
	Particle@ p2;
	float real_segment_length;
	
	CachedTileProvider tile_provider;
	
	string get_identifier() const override { return 'Rope' + self.id(); }
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.self = self;
		@this.script = script;
		@spring_system = script.spring_system;
		trigger_view_events = true;
		
		self.radius(0);
		
		if(!script.is_playing)
			return;
		
		@spr = create_sprites();
		spr.add_sprite_set('script');
		
		@tile_provider.g = script.g;
		tile_provider.frame = -1;
		
		if(anchor_self && anchor_x == 0 && anchor_y == 0)
		{
			anchor_x = self.x();
			anchor_y = self.y();
		}
	}
	
	void on_activate() override
	{
		if(!initialised_attachments)
		{
			@attach1 = entity1_id != 0 ? IRopeAttachment::get(entity_by_id(entity1_id)) : null;
			@attach2 = entity2_id != 0 ? IRopeAttachment::get(entity_by_id(entity2_id)) : null;
			
			if(length <= 0)
			{
				if(@attach1 == null && @attach2 == null)
				{
					script.g.remove_entity(self.as_entity());
					return;
				}
				
				float x1, y1, x2, y2;
				get_end_points(x1, y1, x2, y2);
				length = distance(x1, y1, x2, y2);
			}
			
			initialised_attachments = true;
		}
		
		if(create_constraints())
		{
			do_create_constraints();
		}
		
		// Hack: Create the constraints above so that linking with blocks works properly
		// but then immediatelly remove them so they can be activated instead when entering the view
		on_leave_view();
	}
	
	void on_deactivate()
	{
		store_positions();
	}
	
	void on_enter_view() override
	{
		if(create_constraints())
		{
			do_create_constraints();
		}
		
		make_static(false);
		update_end_points();
	}
	
	void on_leave_view() override
	{
		remove_constraints();
		// Make particles static when deactivating so that still active constraints
		// won't pull on them
		make_static(true);
	}
	
	void on_destroy() override
	{
		remove_constraints();
	}
	
	void on_checkpoint_save() override
	{
		// This rope hasn't been stepped, so no need to store positions
		if(tile_provider.frame < 0)
			return;
		
		store_positions();
	}
	
	private void store_positions()
	{
		points.resize(particles.length * 2);
		
		for(uint i = 0; i < particles.length; i++)
		{
			Particle@ p = @particles[i];
			points[i * 2] = p.x;
			points[i * 2 + 1] = p.y;
		}
	}
	
	private void get_end_points(float &out x1, float &out y1, float &out x2, float &out y2)
	{
		// Just choose some defaults for testing if one of the attachments isn't set
		if(@attach1 != null)
		{
			attach1.get_attachment_point(x1, y1);
			if(@attach2 == null && !anchor_self)
			{
				x2 = x1;
				y2 = y1 + length;
			}
		}
		if(@attach2 != null)
		{
			attach2.get_attachment_point(x2, y2);
			if(@attach1 == null && !anchor_self)
			{
				x1 = x2;
				y1 = y2 - length;
			}
		}
		
		if(anchor_self)
		{
			const bool use_anchors = anchor_x != 0 || anchor_y != 0;
			if(@attach1 == null)
			{
				x1 = use_anchors ? anchor_x : self.x();
				y1 = use_anchors ? anchor_y : self.y();
			}
			else if(@attach2 == null)
			{
				x2 = use_anchors ? anchor_x : self.x();
				y2 = use_anchors ? anchor_y : self.y();
			}
		}
		
		if(@attach1 == null && @attach2 == null)
		{
			x1 = self.x();
			y1 = self.y();
			x2 = x1;
			y2 = y1 + length;
		}
	}
	
	private void do_create_constraints()
	{
		if(length <= 0)
			return;
		
		float x1, y1, x2, y2;
		get_end_points(x1, y1, x2, y2);
		
		float nx = x2 - x1;
		float ny = y2 - y1;
		const float l = sqrt(nx * nx + ny * ny);
		nx /= l;
		ny /= l;
		const int num_segments = int(max(ceil(length / this.segment_length), 2.0));
		real_segment_length = length / (num_segments - 1);
		Particle@ p0 = null;
		const bool has_positions = points.length == uint(num_segments * 2);
		int pi = 0;
		
		RopeCollisionConstraint@ self_collsions = tile_collisions ? RopeCollisionConstraint() : null;
		if(tile_collisions)
		{
			self_collsions.strength = 0.01;
			track_constraint(self_collsions);
			script.spring_system.add_constraint(self_collsions);
		}
		
		for(int i = 0; i < num_segments; i++)
		{
			Particle@ p;
			//const bool add_collision = (i > 0 || @attach1 == null) && (i < num_segments - 1 || @attach2 == null);
			const bool add_collision = true;
			
			float px, py;
			
			if(i == num_segments - 1)
			{
				px = x2;
				py = y2;
			}
			else if(!has_positions)
			{
				const float f = length * (i / float(num_segments - 1));
				px = x1 + nx * f;
				py = y1 + ny * f;
			}
			else
			{
				px = points[pi++];
				py = points[pi++];
			}
			
			@p = add_particle(px, py, add_collision);
			
			if(@p0 != null)
			{
				add_constraint(p0, p);
			}
			
			if(tile_collisions)
			{
				self_collsions.particles.insertLast(p);
			}
			
			@p0 = p;
		}
		
		@p1 = particles[0];
		@p2 = particles[particles.length - 1];
		
		update_end_points();
		
		if(@attach1 !=  null)
			attach1.attach(this, p1);
		if(@attach2 !=  null)
			attach2.attach(this, p2);
	}
	
	private void update_end_points()
	{
		if(!anchor_self)
			return;
		
		if(@attach1 == null)
			p1.is_static = true;
		if(@attach2 == null)
			p2.is_static = true;
	}
	
	private Particle@ add_particle(const float px, const float py, const bool add_collision)
	{
		Particle@ p = Particle(px, py);
		p.radius = thickness;
		particles.insertLast(p);
		spring_system.add_particle(p);
		
		if(add_collision && tile_collisions)
		{
			TileConstraint@ tc = TileConstraint(this, p, Rope::SurfaceFriction);
			track_constraint(tc);
			spring_system.add_constraint(tc);
		}
		
		return p;
	}
	
	private void add_constraint(Particle@ p1, Particle@ p2)
	{
		DistanceConstraint@ d = DistanceConstraint(p1, p2, Rope::Stiffness, Rope::Damping, real_segment_length);
		track_constraint(d);
		spring_system.add_constraint(d);
	}
	
	void step()
	{
		init_lifecycle();
		
		if(@p1 == null)
			return;
		
		if(_in_view)
		{
			tile_provider.frame = script.frame;
			
			float x, y;
			
			if(@attach2 != null)
			{
				attach2.get_position(x, y);
			}
			else if(@attach1 != null)
			{
				attach1.get_position(x, y);
			}
			else
			{
				x = (p1.x + p2.x) * 0.5;
				y = (p1.y + p2.y) * 0.5;
			}
			
			self.set_xy(x, y);
		}
	}
	
	void draw(float sub_frame)
	{
		if(!_active && script.is_playing)
			return;
		
		if(!draw_self)
			return;
		if(particles.length == 0)
			return;
		
		float sw = Rope::SprW;
		float sy = Rope::SprH;
		float angle;
		float rotation_offset = 0;
		string spr_name = 'rope_joint';
		
		// Draw joints first, then rope segments above those
		for(int j = 0; j < 2; j++)
		{
			if(chain && j == 0)
				continue;
			
			Particle@ p1 = particles[particles.length - 1];
			float p1x = lerp(p1.prev_x, p1.x, sub_frame);
			float p1y = lerp(p1.prev_y, p1.y, sub_frame);
			
			for(int end = int(particles.length) - 2, i = end; i >= 0; i--)
			{
				Particle@ p2 = p1;
				const float p2x = p1x;
				const float p2y = p1y;
				
				@p1 = @particles[i];
				p1x = lerp(p1.prev_x, p1.x, sub_frame);
				p1y = lerp(p1.prev_y, p1.y, sub_frame);
				
				float nx = p2x - p1x;
				float ny = p2y - p1y;
				const float l = sqrt(nx * nx + ny * ny);
				
				if(l <= 0)
					continue;
				
				if(chain && j == 1)
				{
					const int spr_index = int(round(abs(sin(i * 999999) * 2)) + 1);
					spr_name = 'chain_link_' + spr_index;
					switch(spr_index)
					{
						case 1: sw = Rope::Chain1SprW; sy = Rope::Chain1SprH; rotation_offset = -2; break;
						case 2: sw = Rope::Chain2SprW; sy = Rope::Chain2SprH; rotation_offset = 3; break;
						case 3: sw = Rope::Chain3SprW; sy = Rope::Chain3SprH; rotation_offset = 2; break;
					}
				}
				
				const float scale = thickness / sy;
				const float sprw = sw * scale;
				const int count = chain
					? int(max(ceil(real_segment_length / sprw) - 1, 1.0))
					: 1;
				
				angle = atan2(ny, nx) * RAD2DEG + rotation_offset;
				
				for(int k = count - 1; k >= 0; k--)
				{
					const float st = float(k) / count;
					spr.draw_world(
						StoneBlock::Layer, StoneBlock::SubLayer, spr_name, 0, 0,
						p1x + nx * st,
						p1y + ny * st,
						angle,
						j == 0 ? scale : l / count / sw,
						scale,
						0xffffffff);
				}
				
				if(!chain && j == 0 && i == end)
				{
					spr.draw_world(
						StoneBlock::Layer, StoneBlock::SubLayer, 'rope_joint', 0, 0,
						p2x, p2y, angle, thickness / sy, thickness / sy, 0xffffffff);
				}
			}
			
			spr_name = 'rope';
		}
		
		draw_knot(p1, attach1, sub_frame);
		draw_knot(p2, attach2, sub_frame);
	}
	
	private void draw_knot(Particle@ p, IRopeAttachment@ attachment, const float sub_frame)
	{
		if(@attachment == null || chain)
			return;
		
		const float sy = thickness / Rope::SprH * Rope::KnotScale;
		
		spr.draw_world(
			StoneBlock::Layer, StoneBlock::SubLayer, 'rope_joint', 0, 0,
			lerp(p.prev_x, p.x, sub_frame),
			lerp(p.prev_y, p.y, sub_frame),
			attachment.attachment_rotation,
			sy, sy, 0xffffffff);
	}
	
	void editor_draw(float sub_frame)
	{
		if(passthu)
		{
			script.g.draw_rectangle_world(22, 22,
				passthu_x1 * 48, passthu_y1 * 48,
				passthu_x2 * 48, passthu_y2 * 48,
				0, 0x55ff0000);
		}
		
		if(anchor_self && (anchor_x != 0 || anchor_y != 0))
		{
			script.g.draw_line_world(22, 22,
				anchor_x, anchor_y, self.x(), self.y(),
				2, 0xff0b5390);
			script.g.draw_rectangle_world(22, 22,
				anchor_x - 5, anchor_y - 5, anchor_x + 5, anchor_y + 5,
				0, 0xff0b5390);
		}
	}
	
	TileData@ get_tile(const int tx, const int ty) override
	{
		if(passthu && tx >= passthu_x1 && tx < passthu_x2 && ty >= passthu_y1 && ty < passthu_y2)
			return  null;
		
		return tile_provider.get_tile(tx, ty);
	}
	
	void detach(IRopeAttachment@ attachment)
	{
		if(@attachment == @attach1)
		{
			entity1_id = 0;
			@attach1 = null;
		}
		else if(@attachment == @attach2)
		{
			entity2_id = 0;
			@attach2 = null;
		}
	}
	
}
