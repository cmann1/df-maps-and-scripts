#include '../lib/drawing/SpriteGroup.cpp';
#include '../lib/phys/springs/CachedTileProvider.cpp';

#include 'settings/StoneBlockSettings.cpp';
#include 'ConstraintTracker.cpp';

class StoneBlock :
	enemy_base, callback_base,
	LifecycleEntity, ConstraintTracker, IRopeAttachment, ICollisionEntity,
	ICollideableBox
{
	
	[option,0:Large,1:Small,2:Pillar,3:PillarChunk,4:HeadLarge,5:HeadSmall,6:CrateLarge,7:CreateSmall,8:Barrel]
	int type = StoneBlock::TypeLarge;
	[persist] float phys_spacing = 34;
	[persist] float scale = 1;
	[persist] bool show_anchor;
	[persist] bool tile_collisions = false;
	[persist] bool box_collisions = false;
	[persist] float attack_force = 1;
	/** Hack to pull block towards pull_x when it's below pull_y, to prevent a certain pillar from block up a hole the player needs to get through. */
	[persist] float pull_force = 0;
	[position,mode:world,layer:19,y:pull_y] float pull_x;
	[hidden] float pull_y;
	
	int id;
	scriptenemy@ self;
	float x;
	float y;
	float rotation;
	float prev_x;
	float prev_y;
	float prev_rotation;
	float current_rect_size_x = -100;
	float current_rect_size_y = -100;
	float current_rect_frame = -100;
	
	float size_x;
	float size_y;
	float mass = 1;
	float attachment_offset;
	int segments_x, segments_y;
	int num_particles;
	
	SpriteGroup@ shadow_spr;
	SpriteGroup@ attachment_spr;
	SpriteGroup@ block_spr;
	SpriteGroup@ markings_spr;
	float marking_alpha = 0.25;
	
	Rope@ rope;
	Particle@ attached_particle;
	
	// The top left and bottom corners
	private Particle@ p1;
	private Particle@ p2;
	
	private int heavy_land_timer;
	float heavy_land_force;
	float heavy_land_x;
	float heavy_land_y;
	int num_heavy_land_contacts;
	int size_heavy_land_contacts = 3 * 4;
	array<float> heavy_land_contacts(size_heavy_land_contacts);
	
	array<DistanceConstraint@> attachments;
	
	CollisionEdge edge_roof;
	CollisionEdge edge_ground;
	CollisionEdge edge_left;
	CollisionEdge edge_right;
	bool has_contact;
	array<float> particle_contacts;
	array<Particle@> edge_particles;
	CachedTileProvider tile_provider;
	
	string get_identifier() const override { return 'StoneBlock' + self.id(); }
	
	int get_box_id() const { return id; }
	
	void init(script@ script, scriptenemy@ self)
	{
		@this.self = self;
		@this.script = script;
		@spring_system = script.spring_system;
		id = self.id();
		@tile_provider.g = script.g;
		trigger_view_events = true;
		
		x = self.x();
		y = self.y();
		rotation = self.rotation();
		prev_x = x;
		prev_y = y;
		prev_rotation = rotation;
		
		StoneBlock::set_sprites(script.spr_cache, this);
		StoneBlock::set_base_size(this);
		
		self.auto_physics(false);
		
		if(!script.is_playing)
			return;
		
		self.base_collision().collision_type(StoneBlock::ColType);
		self.on_hurt_callback(this, 'on_hurt', 0);
		
		@edge_roof.entity = this;
		@edge_ground.entity = this;
		@edge_left.entity = this;
		@edge_right.entity = this;
		edge_roof.surface_type = 'stone';
		edge_ground.surface_type = 'stone';
		edge_left.surface_type = 'stone';
		edge_right.surface_type = 'stone';
	}
	
	void on_create() override
	{
		if(create_constraints())
		{
			do_create_constraints(false);
			constraints_added = false;
		}
	}
	
	void on_activate() override
	{
		update_collision_edges();
		
		script.add_collision_edge(@edge_roof);
		script.add_collision_edge(@edge_ground);
		script.add_collision_edge(@edge_left);
		script.add_collision_edge(@edge_right);
	}
	
	void on_deactivate() override
	{
		script.remove_collision_edge(@edge_roof);
		script.remove_collision_edge(@edge_ground);
		script.remove_collision_edge(@edge_left);
		script.remove_collision_edge(@edge_right);
		
		edge_roof.clear();
		edge_ground.clear();
		edge_left.clear();
		edge_right.clear();
	}
	
	void on_enter_view() override
	{
		if(create_constraints())
		{
			do_create_constraints();
		}
		
		make_static(false);
		
		script.boxes_constraint.add(this);
		
		if(box_collisions)
		{
			script.boxes_constraint.add(edge_particles);
		}
	}
	
	void on_leave_view() override
	{
		// Make particles static when deactivating so that still active constraints
		// won't pull on them.
		make_static(true);
		
		remove_constraints();
		script.boxes_constraint.remove(this);
		
		if(box_collisions)
		{
			script.boxes_constraint.remove(edge_particles);
		}
	}
	
	void on_destroy() override
	{
		remove_constraints();
	}
	
	private void do_create_constraints(const bool add_to_system=true)
	{
		if(phys_spacing <= 0)
			return;
		
		const float sx = size_x * scale;
		const float sy = size_y * scale;
		
		const float spacing = max(4.0, phys_spacing);
		segments_x = clamp(round_int((sx * 2) / spacing), 3, StoneBlock::MaxSegments);
		segments_y = clamp(round_int((sy * 2) / spacing), 3, StoneBlock::MaxSegments);
		num_particles = segments_x * segments_y;
		particles.resize(num_particles);
		particle_contacts.resize(num_particles);
		
		const float x = self.x();
		const float y = self.y();
		const float angle = self.rotation() * DEG2RAD;
		
		for(int iy = 0; iy < segments_y; iy++)
		{
			const float py = -sy + iy / (segments_y - 1.0) * sy * 2;
			
			for(int ix = 0; ix < segments_x; ix++)
			{
				const float px = -sx + ix / (segments_x - 1.0) * sx * 2;
				float gx, gy;
				rotate(px, py, angle, gx, gy);
				
				const int p_idx = idx(ix, iy);
				Particle@ p = Particle(x + gx, y + gy);
				p.body_id = id;
				p.local_index = p_idx;
				@particles[p_idx] = p;
				
				if(add_to_system)
				{
					spring_system.add_particle(p);
				}
				
				
				// Only add the outer layer for box and tile collisions
				if((ix <= 0 || ix >= segments_x - 1) || (iy <= 0 || iy >= segments_y - 1))
				{
					edge_particles.insertLast(p);
					
					if(tile_collisions)
					{
						TileConstraint@ tc = TileConstraint(tile_provider, p, StoneBlock::SurfaceFriction);
						constraints.insertLast(tc);
						collision_constraints.insertLast(tc);
						
						if(add_to_system)
						{
							spring_system.add_constraint(tc);
						}
					}
				}
				
				if(ix > 0)
				{
					add_contraint(particles[idx(ix - 1, iy)], p, add_to_system);
				}
				
				if(iy > 0)
				{
					add_contraint(particles[idx(ix, iy - 1)], p, add_to_system);
					
					if(ix < segments_x - 1)
					{
						add_contraint(particles[idx(ix + 1, iy - 1)], p, add_to_system);
					}
				}
				
				if(ix > 0 && iy > 0)
				{
					add_contraint(particles[idx(ix - 1, iy - 1)], p, add_to_system);
				}
			}
		}
		
		for(int ix = 0; ix < segments_x; ix++)
		{
			add_contraint(
				particles[idx(ix, 0)],
				particles[idx(ix, segments_y - 1)], add_to_system);
		}
		for(int iy = 0; iy < segments_y; iy++)
		{
			add_contraint(
				particles[idx(0, iy)],
				particles[idx(segments_x - 1, iy)], add_to_system);
		}
		
		@p1 = particles[0];
		@p2 = particles[num_particles - 1];
		
		if(@rope != null)
		{
			this.attach(rope, attached_particle);
		}
	}
	
	private void add_contraint(Particle@ p1, Particle@ p2, const bool add_to_system=true)
	{
		DistanceConstraint@ d = DistanceConstraint(
			p1, p2,
			StoneBlock::ConstraintStiffness, StoneBlock::ConstraintDamping);
		constraints.insertLast(d);
		
		if(add_to_system)
		{
			spring_system.add_constraint(d);
		}
	}
	
	private int idx(const int x, const int y)
	{
		return y * segments_x + x;
	}
	
	private void update_collision_edges()
	{
		const float x = self.x();
		const float y = self.y();
		const float angle = self.rotation() * DEG2RAD;
		
		float x1 = -size_x * scale;
		float y1 = -size_y * scale;
		rotate(x1, y1, angle, x1, y1);
		float x2 = +size_x * scale;
		float y2 = -size_y * scale;
		rotate(x2, y2, angle, x2, y2);
		float x3 = +size_x * scale;
		float y3 = +size_y * scale;
		rotate(x3, y3, angle, x3, y3);
		float x4 = -size_x * scale;
		float y4 = +size_y * scale;
		rotate(x4, y4, angle, x4, y4);
		
		edge_roof.x1 = x + x1;
		edge_roof.y1 = y + y1;
		edge_roof.x2 = x + x2;
		edge_roof.y2 = y + y2;
		edge_right.x1 = x + x2;
		edge_right.y1 = y + y2;
		edge_right.x2 = x + x3;
		edge_right.y2 = y + y3;
		edge_ground.x1 = x + x3;
		edge_ground.y1 = y + y3;
		edge_ground.x2 = x + x4;
		edge_ground.y2 = y + y4;
		edge_left.x1 = x + x4;
		edge_left.y1 = y + y4;
		edge_left.x2 = x + x1;
		edge_left.y2 = y + y1;
		
		edge_roof.update();
		edge_ground.update();
		edge_left.update();
		edge_right.update();
	}
	
	private void add_contact_weight()
	{
		if(!_in_view)
			return;
		
		for(SideType side = SideType::Left; side <= SideType::Ground; side++)
		{
			CollisionEdge@ edge;
			bool is_hor_edge;
			
			switch(side)
			{
				case SideType::Left:   @edge = @edge_left;   is_hor_edge = false; break;
				case SideType::Right:  @edge = @edge_right;  is_hor_edge = false; break;
				case SideType::Roof:   @edge = @edge_roof;   is_hor_edge = true; break;
				case SideType::Ground: @edge = @edge_ground; is_hor_edge = true; break;
			}
			
			const float sx = size_x * scale;
			const float sy = size_y * scale;
			const float grid_x = (sx * 2) / (segments_x - 1);
			const float grid_y = (sy * 2) / (segments_y - 1);
			
			for(int i = edge.num_contacts - 1; i >= 0; i--)
			{
				EdgeContact@ e = @edge.contacts[i];
				if(!e.has_contact)
					continue;
				
				rectangle@ r = e.data.c.collision_rect();
				const float mass = r.get_width() * r.get_height() * StoneBlock::ContactMass;
				const float spread = (is_hor_edge ? r.get_width() : r.get_height());
				
				float local_x, local_y;
				rotate(e.x - x, e.y - y, -rotation * DEG2RAD, local_x, local_y);
				
				const int mx = clamp(round_int((local_x + sx) / grid_x), 0, segments_x - 1);
				const int my = clamp(round_int((local_y + sy) / grid_y), 0, segments_y - 1);
				const int spread_x = min(round_int(spread / grid_x), (segments_x - 1) / 2);
				const int spread_y = min(round_int(spread / grid_y), (segments_y - 1) / 2);
				const int px1 = max(mx - spread_x, 0);
				const int py1 = max(my - spread_y, 0);
				const int px2 = min(mx + spread_x, segments_x - 1);
				const int py2 = min(my + spread_y, segments_y - 1);
				
				const int spread_count = (px2 - px1 + 1) * (py2 - py1 + 1);
				const float fx = (e.speed_x * StoneBlock::ContactVelocityMultipler) / this.mass / spread_count;
				const float fy =
					(e.speed_y * StoneBlock::ContactVelocityMultipler) / this.mass / spread_count +
					mass / spread_count;
				
				for(int px = px1; px <= px2; px++)
				{
					for(int py = py1; py <= py2; py++)
					{
						Particle@ p = particles[idx(px, py)];
						//script.debug.dot(22, 24, p.x, p.y, 3, 0xffffffff, 45);
						
						p.impulse_x += fx * (1 - abs(px - mx) / max(spread_x, 1));
						p.impulse_y += fy * (1 - abs(py - my) / max(spread_y, 1));
					}
				}
				
				e.speed_x = 0;
				e.speed_y = 0;
			}
		}
	}
	
	private void update_contacts()
	{
		float land_force = 0;
		float land_x = 0;
		float land_y = 0;
		int land_contacts = 0;
		
		heavy_land_force = 0;
		heavy_land_x = 0;
		heavy_land_y = 0;
		num_heavy_land_contacts = 0;
		
		bool found_contact = false;
		
		for(uint i = 0; i < edge_particles.length; i++)
		{
			Particle@ p = edge_particles[i];
			
			if(!p.has_contact)
			{
				particle_contacts[p.local_index] = 0;
				continue;
			}
			
			const float force = magnitude(p.contact_vx, p.contact_vy);
			
			if(
				heavy_land_timer == 0 &&
				force * mass >= StoneBlock::HeavyLandMin &&
				particle_contacts[p.local_index] <= 0)
			{
				if(num_heavy_land_contacts + 3 >= size_heavy_land_contacts)
					heavy_land_contacts.resize(size_heavy_land_contacts += 3);
				
				heavy_land_contacts[num_heavy_land_contacts++] = p.x;
				heavy_land_contacts[num_heavy_land_contacts++] = p.y;
				heavy_land_contacts[num_heavy_land_contacts++] = force * mass;
				
				heavy_land_x += p.x;
				heavy_land_y += p.y;
				
				if(force * mass > heavy_land_force)
				{
					heavy_land_force = force * mass;
				}
			}
			
			if(
				force >= StoneBlock::LandMin && force < StoneBlock::HeavyLandMin &&
				particle_contacts[p.local_index] <= 0)
			{
				land_x += p.x;
				land_y += p.y;
				land_contacts++;
				
				if(force > land_force)
				{
					land_force = force;
				}
			}
			
			int emitter_id = force >= StoneBlock::HeavyLandMin && !has_contact
				? StoneBlock::HeavyCollisionEmitterId
				: force >= StoneBlock::LandMin
					? StoneBlock::CollisionEmitterId : -1;
			
			if(emitter_id != -1)
			{
				script.add_emitter_burst(
					emitter_id,
					StoneBlock::Layer, StoneBlock::CollisionEmitterSubLayer,
					p.x, p.y,
					10, 10, atan2(p.contact_nx, p.contact_ny) * RAD2DEG + 90);
			}
			
			found_contact = true;
			particle_contacts[p.local_index]++;
		}
		
		if(land_contacts > 0)
		{
			heavy_land_timer = 5;
			land_force = map_clamped(land_force,
				StoneBlock::LandMin, StoneBlock::HeavyLandMin, 0.25, 1);
			
			land_x /= land_contacts;
			land_y /= land_contacts;
			
			audio@ snd = script.g.play_sound(
				'sfx_stone_land', land_x, land_y,
				map_clamped(land_force, StoneBlock::LandMin, StoneBlock::HeavyLandMin, 0.15, 1), false, true);
			snd.time_scale(rand_range(0.9, 1.1));
		}
		
		if(heavy_land_timer == 0 && !has_contact)
		{
			if(num_heavy_land_contacts > 0)
			{
				heavy_land_timer = 5;
				const float force_t = map_clamped(heavy_land_force,
					StoneBlock::HeavyLandMin, StoneBlock::HeavyLandMax, 0.05, 1);
				
				heavy_land_x /= (num_heavy_land_contacts / 3);
				heavy_land_y /= (num_heavy_land_contacts / 3);
				script.cam.add_screen_shake(
					heavy_land_x, heavy_land_y,
					180 * max(force_t, 0.25), StoneBlock::HeavyLandShake);
				
				for(int i = 0; i < 2; i++)
				{
					script.g.play_sound(
						'sfx_land_generic_heavy', heavy_land_x, heavy_land_y,
						force_t, false, true);
				}
			}
		}
		else if(heavy_land_timer > 0)
		{
			heavy_land_timer--;
		}
		
		has_contact = found_contact;
	}
	
	void on_hurt(controllable@ attacked, controllable@ attacker, hitbox@ attack_hitbox, int arg)
	{
		if(StoneBlock::AttackForceMultiplier * script.attack_force * attack_force <= 0)
			return;
		
		rectangle@ r = attack_hitbox.base_rectangle();
		const float x = attack_hitbox.x();
		const float y = attack_hitbox.y();
		const float x1 = x + r.left();
		const float y1 = y + r.top();
		const float x2 = x + r.right();
		const float y2 = y + r.bottom();
		
		//script.debug.rect(
		//	22, 22,
		//	x1, y1, x2, y2,
		//	0, 1, 0x88ff0000, true, 30);
		//script.debug.dot(
		//	22, 22,
		//	attack_hitbox.x(), attack_hitbox.y(),
		//	3, 0xffff0000, 45, true, 30);
		
		// Supers have a force=0 and damage=10000
		const float force = ((attack_hitbox.damage() == 10000
			? 1000.0
			: attack_hitbox.attack_strength()) * StoneBlock::AttackForceMultiplier * script.attack_force * attack_force) / mass;
		
		const float angle = (attack_hitbox.attack_dir() - 90) * DEG2RAD;
		const float fx = cos(angle) * force;
		const float fy = sin(angle) * force;
		//script.debug.line(
		//	22, 22,
		//	attack_hitbox.x(), attack_hitbox.y(),
		//	attack_hitbox.x() + cos(angle) * 96,
		//	attack_hitbox.y() + sin(angle) * 96,
		//	2, 0xffff0000, true, 30);
		
		for(uint i = 0; i < particles.length; i++)
		{
			Particle@ p = @particles[i];
			
			if(p.x <= x1 || p.x >= x2 || p.y <= y1 || p.y >= y2)
				continue;
			
			p.impulse_x += fx;
			p.impulse_y += fy;
		}
	}
	
	void step()
	{
		init_lifecycle();
		
		if(@p1 == null)
			return;
		
		tile_provider.frame = script.frame;
		
		const float x = (p1.x + p2.x) * 0.5;
		const float y = (p1.y + p2.y) * 0.5;
		self.set_xy(x, y);
		float dx = p2.x - p1.x;
		float dy = p2.y - p1.y;
		const float angle = atan2(dy, dx) - atan2(size_y, size_x);
		self.rotation(angle * RAD2DEG);
		
		update_collision_edges();
		add_contact_weight();
		update_contacts();
		
		prev_x = this.x;
		prev_y = this.y;
		prev_rotation = this.rotation;
		
		this.x = self.x();
		this.y = self.y();
		rotation = self.rotation();
		
		update_rect();
		
		if(pull_force > 0 && (pull_x != 0 || pull_y != 0) && x < pull_x && y > pull_y)
		{
			for(uint i = 0; i < edge_particles.length; i++)
			{
				edge_particles[i].impulse_x += pull_force;
			}
		}
	}
	
	void editor_step()
	{
		prev_x = x = self.x();
		prev_y = y = self.y();
		prev_rotation = rotation = self.rotation();
	}
	
	void draw(float sub_frame)
	{
		if(_in_view || !script.is_playing)
		{
			const float x = lerp(prev_x, this.x, sub_frame);
			const float y = lerp(prev_y, this.y, sub_frame);
			const float rotation = lerp_angle_degrees(prev_rotation, this.rotation, sub_frame);
			
			if(@shadow_spr != null)
				shadow_spr.draw(x, y, rotation, scale);
			if(show_anchor && @attachment_spr != null)
				attachment_spr.draw(x, y, rotation, scale);
			block_spr.draw(x, y, rotation, scale);
			if(@markings_spr != null)
				markings_spr.draw(x, y, rotation, scale, 0, 0, marking_alpha);
		}
		
		if(@rope != null)
		{
			// Make sure the rope draws above the block it is attached to
			rope.draw_self = true;
			rope.draw(sub_frame);
			rope.draw_self = false;
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		
		float x, y;
		get_attachment_point(x, y);
		script.g.draw_rectangle_world(22, 22, x - 2, y - 2, x + 2, y + 2, 45, 0xffff0000);
		
		if(pull_force > 0 && (pull_x != 0 || pull_y != 0))
		{
			draw_dot(script.g, 22, 22, pull_x, pull_y, 4, 0xffff0000, 45);
		}
	}
	
	void get_attachment_point(float &out x, float &out y) const override
	{
		const float angle = (rotation - 90) * DEG2RAD;
		const float dx = cos(angle);
		const float dy = sin(angle);
		
		x = this.x + dx * (size_y + (show_anchor ? attachment_offset : 0)) * scale;
		y = this.y + dy * (size_y + (show_anchor ? attachment_offset : 0)) * scale;
	}
	
	float get_attachment_rotation() const override
	{
		return rotation;
	}
	
	void get_position(float &out x, float &out y) const override
	{
		x = this.x;
		y = this.y;
	}
	
	void attach(Rope@ rope, Particle@ p) override
	{
		if(attachments.length > 0)
			return;
		
		if(particles.length == 0)
		{
			@this.rope = rope;
			@attached_particle = p;
			return;
		}
		
		// The middle two/three particles
		const int s1 = segments_x / 2 - 1;
		const int s2 = (segments_x - 1) / 2 + 1;
		
		for(int i = s1; i <= s2; i++)
		{
			DistanceConstraint@ attachment = DistanceConstraint(p, particles[i]);
			attachments.insertLast(attachment);
			track_constraint(attachment);
			
			if(constraints_added)
			{
				script.spring_system.add_constraint(attachment);
			}
		}
		
		// Also constraint to opposite corners to be more stable
		DistanceConstraint@ attachment = DistanceConstraint(p, particles[idx(0, segments_y - 1)]);
		attachments.insertLast(attachment);
		track_constraint(attachment);
		
		if(constraints_added)
		{
			script.spring_system.add_constraint(attachment);
		}
		
		@attachment = DistanceConstraint(p, particles[idx(segments_x - 1, segments_y - 1)]);
		attachments.insertLast(attachment);
		track_constraint(attachment);
		
		if(constraints_added)
		{
			script.spring_system.add_constraint(attachment);
		}
		
		@this.rope = rope;
		@attached_particle = p;
		rope.draw_self = false;
	}
	
	void get_box_properties(float &out x, float &out y, float &out hw, float &out hh, float &out rotation)
	{
		x = this.x;
		y = this.y;
		hw = size_x * scale;
		hh = size_y * scale;
		rotation = this.rotation;
	}
	
	void editor_var_changed(var_info@ var)
	{
		const string path = var.get_path(true);
		
		if(path == 'type')
		{
			StoneBlock::set_sprites(script.spr_cache, this);
			StoneBlock::set_base_size(this);
		}
		else if(path == 'scale')
		{
			scale = abs(scale);
			StoneBlock::set_base_size(this);
		}
	}
	
	void update_rect()
	{
		// Width and height are the same so bounding box should always be ok
		// regardless of rotation
		// But make sure to update at least once
		if(size_x == size_y && current_rect_size_x > 0)
			return;
		// Don't update every frame
		if(abs(current_rect_frame - script.frame) < 5)
			return;
		
		const float c = abs(cos(rotation * DEG2RAD));
		const float sw = max(lerp(size_y, size_x, c) * scale, 5.0);
		const float sh = max(lerp(size_y, size_x, 1 - c) * scale, 5.0);
		
		// Only update when it has change significantly
		if(abs(sw - current_rect_size_x) < 2 && abs(sh - current_rect_size_y) < 2)
			return;
		
		self.base_rectangle(-sh, sh, -sw, sw);
		self.hit_rectangle(-sh, sh, -sw, sw);
		
		current_rect_size_x = sw;
		current_rect_size_y = sh;
		current_rect_frame = script.frame;
	}
	
}
