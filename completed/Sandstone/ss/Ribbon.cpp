class Ribbon : trigger_base, LifecycleEntity, ConstraintTracker
{
	
	[persist] int segments = 4;
	[persist] float segment_length = 48;
	[tooltip:'The rate at which the direction\nand strength change']
	float wind_rate = 0.2;
	[persit] float wind_multiplier = 1;
	[persit] float gravity_multiplier = 0.75;
	[persit] float entity_force_multiplier = 1;
	[persit] float dir_force = 0;
	[angle] float dir_angle = 0;
	[persit] float air_friction = 4;
	[persist] bool tile_collisions;
	[persist] bool self_collisions;
	[persit] float thickness = 12;
	[persit] float trim_space = 5;
	[persit] float trim_size = 2;
	[persit] int layer = 18;
	[persit] int sublayer = 9;
	[colour] uint light_clr = Player::ScarfLightClr;
	[colour] uint dark_clr = Player::ScarfDarkClr;
	[colour] uint trim_clr = Player::ScarfTrimClr;
	
	scripttrigger@ self;
	Particle@ p1;
	Particle@ p2;
	CachedTileProvider@ tile_provider;
	RopeCollisionConstraint@ rope_constraint;
	
	float create_sin_size = 60;
	float base_thickness = 1;
	float x;
	float y;
	float prev_x;
	float prev_y;
	float x1, y1, x2, y2;
	bool visible = true;
	bool ready;
	float glow_alpha;
	uint glow_clr = Player::ScarfTrimGlowClr;
	Sprite glow_spr('props3', 'backdrops_3');
	int glow_layer = layer;
	int glow_sublayer = sublayer;
	entity@ glow_emitter;
	bool draw_end;
	
	private float wind_t;
	private float wind_x_ratio;
	private float wind_range;
	
	string get_identifier() const override { return 'Ribbon' + self.id(); }
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.self = self;
		@this.script = script;
		trigger_view_events = true;
		
		self.radius(0);
		self.editor_handle_size(5);
		self.editor_colour_active(0xffff5555);
		self.editor_colour_inactive(0xffaa5555);
		
		if(!script.is_playing)
			return;
		
		@spring_system = script.spring_system;
		
		@tile_provider = CachedTileProvider();
		@tile_provider.g = script.g;
		
		x = prev_x = self.x();
		y = prev_y = self.y();
		x1 = x;
		y1 = y;
		x2 = x;
		y2 = y;
	}
	
	void init_xy(const float x, const float y)
	{
		self.set_xy(x, y);
		this.x = prev_x = x;
		this.y = prev_y = y;
		x1 = x;
		y1 = y;
		x2 = x;
		y2 = y;
	}
	
	void set_xy(const float x, const float y)
	{
		if(@p1 == null)
			return;
		
		prev_x = this.x;
		prev_y = this.y;
		this.x = x;
		this.y = y;
		
		if(check_teleport())
		{
			prev_x += this.x - prev_x;
			prev_y += this.y - prev_y;
		}
		
		p1.prev_x = p1.x;
		p1.prev_y = p1.y;
		p1.x = x;
		p1.y = y;
		self.set_xy(x, y);
	}
	
	void on_activate() override
	{
		const int index = script.ribbons.findByRef(this);
		if(index == -1)
		{
			script.ribbons.insertLast(this);
		}
	}
	
	void on_deactivate() override
	{
		const int index = script.ribbons.findByRef(this);
		if(index == -1)
		{
			script.ribbons.removeAt(index);
		}
	}
	
	void on_enter_view() override
	{
		if(create_constraints())
		{
			do_create();
		}
		
		make_static(false);
		p1.is_static = true;
		script.boxes_constraint.add(particles);
	}
	
	void on_leave_view() override
	{
		remove_constraints();
		// Make particles static when deactivating so that still active constraints
		// won't pull on them
		make_static(true);
		script.boxes_constraint.remove(particles);
	}
	
	void on_destroy() override
	{
		remove_constraints();
	}
	
	private void do_create()
	{
		x = self.x();
		y = self.y();
		
		@rope_constraint = self_collisions ? RopeCollisionConstraint() : null;
		if(self_collisions)
		{
			rope_constraint.strength = 0.05;
			track_constraint(rope_constraint);
			script.spring_system.add_constraint(rope_constraint);
		}
		
		Particle@ p0 = null;
		
		for(int i = 0; i < segments; i++)
		{
			float px = segment_length * i;
			float py = sin((x + segment_length * i) * 0.02) * create_sin_size * (float(i) / segments);
			
			if(dir_force != 0)
			{
				rotate(px, py, (dir_angle - 90) * DEG2RAD, px, py);
			}
			
			// Create along a sin wave so that it looks more natural when spawning
			Particle@ p = create_particle(x + px, y + py);
			
			if(@p0 != null)
			{
				create_constraint(p0, p);
			}
			
			@p0 = p;
		}
		
		@p1 = particles[0];
		@p2 = particles[particles.length - 1];
		p1.is_static = true;
		ready = true;
	}
	
	private Particle@ create_particle(const float x, const float y)
	{
		Particle@ p = Particle(x, y);
		p.air_friction = air_friction;
		p.radius = 10;
		particles.insertLast(p);
		spring_system.add_particle(p);
		
		if(self_collisions)
		{
			rope_constraint.particles.insertLast(p);
		}
		
		if(tile_collisions)
		{
			TileConstraint@ tc = TileConstraint(tile_provider, p, 10);
			track_constraint(tc);
			spring_system.add_constraint(tc);
		}
		
		return p;
	}
	
	private DistanceConstraint@ create_constraint(Particle@ p0, Particle@ p1, const float length=-1)
	{
		DistanceConstraint@ d = DistanceConstraint(p0, p1, 0.75, 0.01, length < 0 ? segment_length : length);
		constraints.insertLast(d);
		spring_system.add_constraint(d);
		
		return d;
	}
	
	void step()
	{
		init_lifecycle();
		
		if(@p1 == null)
			return;
		
		prev_x = x;
		prev_y = y;
		x = self.x();
		y = self.y();
		
		check_teleport();
		
		if(glow_alpha > 0)
		{
			if(@glow_emitter == null)
			{
				@glow_emitter = create_emitter(Player::ScarfGetEmitterId,
					p2.x, p2.y,
					16, int(thickness * 2),
					glow_layer, glow_sublayer, int(end_angle() * RAD2DEG)
				);
				script.g.add_entity(glow_emitter, false);
			}
			
			if(@glow_emitter != null)
			{
				glow_emitter.set_xy(p2.x, p2.y);
				glow_emitter.vars().get_var('e_rotation').set_int32(int(end_angle() * RAD2DEG));
			}
		}
		else if(@glow_emitter != null)
		{
			script.g.remove_entity(glow_emitter);
			@glow_emitter = null;
		}
		
		if(!_in_view)
			return;
		
		tile_provider.frame = script.frame;
		
		const bool has_wind = script.wind_strength > 0 && wind_multiplier != 0;
		if(has_wind)
		{
			wind_step();
		}
		
		x1 = x;
		y1 = y;
		x2 = x1;
		y2 = y1;
		
		for(uint i = 0; i < particles.length; i++)
		{
			Particle@ p = particles[i];
			
			if(p.x < x1) x1 = p.x;
			if(p.y < y1) y1 = p.y;
			if(p.x > x1) x2 = p.x;
			if(p.y > y1) y2 = p.y;
			
			if(p.is_static)
				continue;
			
			if(has_wind)
			{
				float wx, wy;
				calc_wind(p.x, p.y, wx, wy, 0.25 + (float(i) / particles.length) * 0.5);
				p.impulse_x += wx;
				p.impulse_y += wy;
			}
			
			if(gravity_multiplier != 1)
			{
				p.impulse_y -= spring_system.gravity.y * (1 - gravity_multiplier);
			}
			
			if(dir_force != 0)
			{
				p.impulse_x += cos((dir_angle - 90) * DEG2RAD) * dir_force;
				p.impulse_y += sin((dir_angle - 90) * DEG2RAD) * dir_force;
			}
		}
		
		int i = script.g.get_entity_collision(y1, y2, x1, x2, ColType::Hitbox);
		while(--i >= 0)
		{
			apply_attack_force(script.g.get_hitbox_collision_index(i));
		}
		
		if(entity_force_multiplier > 0)
		{
			i = script.g.get_entity_collision(y1, y2, x1, x2, ColType::Hittable);
			while(--i >= 0)
			{
				controllable@ c = script.g.get_controllable_collision_index(i);
				if(@c == null)
					return;
				
				const float fx = c.x_speed() * 6 * entity_force_multiplier;
				const float fy = c.y_speed() * 6 * entity_force_multiplier;
				
				rectangle@ r = c.base_rectangle();
				const float x = c.x();
				const float y = c.y();
				const float hb_x1 = x + r.left();
				const float hb_y1 = y + r.top();
				const float hb_x2 = x + r.right();
				const float hb_y2 = y + r.bottom();
				
				//script.debug.rect(
				//	22, 22,
				//	hb_x1, hb_y1, hb_x2, hb_y2,
				//	0, 1, 0xffff0000, true, 60);
				
				for(uint j = 0; j < particles.length; j++)
				{
					Particle@ p = particles[j];
					
					if(p.is_static)
						continue;
					if(p.x < hb_x1 || p.x > hb_x2 || p.y < hb_y1 || p.y > hb_y2)
						continue;
					
					p.impulse_x += fx;
					p.impulse_y += fy;
				}
			}
		}
	}
	
	void apply_attack_force(hitbox@ hb)
	{
		if(@hb == null || hb.hit_outcome() < 1 || hb.hit_outcome() > 3)
			return;
		
		const float force = (hb.attack_strength() * script.attack_force * 20) / thickness ;
		
		const float angle = (hb.attack_dir() - 90) * DEG2RAD;
		const float fx = cos(angle) * force;
		const float fy = sin(angle) * force;
		
		rectangle@ r = hb.base_rectangle();
		const float x = hb.x();
		const float y = hb.y();
		const float hb_x1 = x + r.left();
		const float hb_y1 = y + r.top();
		const float hb_x2 = x + r.right();
		const float hb_y2 = y + r.bottom();
		
		//script.debug.rect(
		//	22, 22,
		//	hb_x1, hb_y1, hb_x2, hb_y2,
		//	0, 1, 0xffff0000, true, 60);
		
		for(uint j = 0; j < particles.length; j++)
		{
			Particle@ p = particles[j];
			
			if(p.is_static)
				continue;
			if(p.x < hb_x1 || p.x > hb_x2 || p.y < hb_y1 || p.y > hb_y2)
				continue;
			
			p.impulse_x += fx;
			p.impulse_y += fy;
			
			//script.debug.line(
			//	22, 22,
			//	p.x, p.y, p.x + fx/force*48, p.y + fy/force*48,
			//	2, 0xffff0000, true, 60);
			//script.debug.dot(
			//	22, 22,
			//	p.x, p.y,
			//	3, 0xffff0000, 45, true, 60);
		}
	}
	
	bool check_teleport()
	{
		const float dx = x - prev_x;
		const float dy = y - prev_y;
		
		if((dx * dx + dy * dy) <= 200 * 200)
			return false;
		
		offset(dx, dy);
		return true;
	}
	
	void draw(float sub_frame)
	{
		if(!visible)
			return;
		if(!_in_view && script.is_playing)
			return;
		
		const int num_particles = particles.length;
		if(num_particles == 0)
			return;
		
		Particle@ p1 = null;
		Particle@ p2 = particles[0];
		Particle@ p3 = particles[1];
		float t1x, t1y;
		float t2x = lerp(p2.prev_x, p2.x, sub_frame);
		float t2y = lerp(p2.prev_y, p2.y, sub_frame);
		float t3x = lerp(p3.prev_x, p3.x, sub_frame);
		float t3y = lerp(p3.prev_y, p3.y, sub_frame);
		float p2_angle;
		float p3_angle = atan2(p3.y - p2.y, p3.x - p2.x);
		
		float x1, y1;
		float x2 = cos(p3_angle + PI * 0.5) * base_thickness;
		float y2 = sin(p3_angle + PI * 0.5) * base_thickness;
		float angle;
		
		if(script.debug_wind)
		{
			wind_step();
		}
		
		const uint trim_clr = glow_alpha > 0
			? colour::lerp(this.trim_clr, Player::ScarfTrimGlowClr, glow_alpha)
			: this.trim_clr;
		
		for(int i = 1; i < num_particles; i++)
		{
			@p1 = p2;
			@p2 = p3;
			@p3 = (i + 1) < num_particles ? @particles[i + 1] : null;
			
			t1x = t2x;
			t1y = t2y;
			t2x = t3x;
			t2y = t3y;
			
			p2_angle = p3_angle;
			
			if(@p3 != null)
			{
				t3x = lerp(p3.prev_x, p3.x, sub_frame);
				t3y = lerp(p3.prev_y, p3.y, sub_frame);
				p3_angle = atan2(t3y - t2y, t3x - t2x);
				angle = p2_angle + shortest_angle(p2_angle, p3_angle) * 0.5;
			}
			else
			{
				angle = p2_angle;
			}
			
			x1 = x2;
			y1 = y2;
			x2 = cos(angle + PI * 0.5);
			y2 = sin(angle + PI * 0.5);
			
			script.g.draw_quad_world(layer, sublayer, false,
				t1x - x1 * thickness, t1y - y1 * thickness,
				t1x + x1 * thickness, t1y + y1 * thickness,
				t2x + x2 * thickness, t2y + y2 * thickness,
				t2x - x2 * thickness, t2y - y2 * thickness,
				light_clr, dark_clr, dark_clr, light_clr);
			// Trim
			script.g.draw_quad_world(layer, sublayer, false,
				t1x - x1 * (thickness - trim_space), t1y - y1 * (thickness - trim_space),
				t1x - x1 * (thickness - trim_space - trim_size), t1y - y1 * (thickness - trim_space - trim_size),
				t2x - x2 * (thickness - trim_space - trim_size), t2y - y2 * (thickness - trim_space - trim_size),
				t2x - x2 * (thickness - trim_space), t2y - y2 * (thickness - trim_space),
				trim_clr, trim_clr, trim_clr, trim_clr);
			script.g.draw_quad_world(layer, sublayer, false,
				t1x + x1 * (thickness - trim_space), t1y + y1 * (thickness - trim_space),
				t1x + x1 * (thickness - trim_space - trim_size), t1y + y1 * (thickness - trim_space - trim_size),
				t2x + x2 * (thickness - trim_space - trim_size), t2y + y2 * (thickness - trim_space - trim_size),
				t2x + x2 * (thickness - trim_space), t2y + y2 * (thickness - trim_space),
				trim_clr, trim_clr, trim_clr, trim_clr);
			
			if(script.debug_wind)
			{
				float wx, wy;
				calc_wind(p2.x, p2.y, wx, wy, 0.25 + (float(i) / particles.length) * 0.5);
				script.g.draw_line_world(22, 22,
					p2.x, p2.y,
					p2.x + wx / (script.wind_strength * wind_multiplier) * 50,
					p2.y + wy / (script.wind_strength * wind_multiplier) * 50,
					2, 0xffff0000);
			}
		}
		
		if(draw_end || glow_alpha > 0)
		{
			const uint clr = glow_alpha > 0
				? draw_end
					? colour::lerp(trim_clr, this.glow_clr | 0xff000000, glow_alpha)
					: (this.glow_clr & 0xffffff) | (uint(round(glow_alpha * 255)) << 24)
				: trim_clr;
			const float end_thickness = trim_size * 1.5;
			const float p1x1 = t1x - x1 * thickness;
			const float p1y1 = t1y - y1 * thickness;
			const float p1x2 = t1x + x1 * thickness;
			const float p1y2 = t1y + y1 * thickness;
			const float p2x1 = t2x - x2 * thickness;
			const float p2y1 = t2y - y2 * thickness;
			const float p2x2 = t2x + x2 * thickness;
			const float p2y2 = t2y + y2 * thickness;
			float nx1, ny1, nx2, ny2;
			normalize(p1x1 - p2x1, p1y1 - p2y1, nx1, ny1);
			normalize(p1x2 - p2x2, p1y2 - p2y2, nx2, ny2);
			script.g.draw_quad_world(layer, sublayer, false,
				p2x1, p2y1,
				p2x2, p2y2,
				p2x2 + nx2 * end_thickness, p2y2 + ny2 * end_thickness,
				p2x1 + nx1 * end_thickness, p2y1 + ny1 * end_thickness,
				clr, clr, clr, clr);
			
			if(glow_alpha > 0)
			{
				const float scale = thickness / 10.0;
				glow_spr.draw(glow_layer, glow_sublayer, 0, 0,
					lerp(p2.prev_x, p2.x, sub_frame),
					lerp(p2.prev_y, p2.y, sub_frame),
					angle * RAD2DEG - 90,
					0.05 * scale, 0.05 * scale, clr);
			}
		}
	}
	
	void editor_draw(float sub_frame)
	{
		if(script.is_playing)
			return;
		
		const float x = self.x();
		const float y = self.y();
		
		for(int i = 1; i < segments; i++)
		{
			const float t1 = float(i - 1) / (segments - 1);
			const float t2 = float(i) / (segments - 1);
			float x1 = (i - 1) * segment_length;
			float y1 = sin((i - 1) * 5) * 30 * t1;
			float x2 = (i) * segment_length;
			float y2 = sin((i) * 5) * 30 * t2;
			float px = 0;
			float py = 1;
			
			if(dir_force != 0)
			{
				rotate(x1, y1, (dir_angle - 90) * DEG2RAD, x1, y1);
				rotate(x2, y2, (dir_angle - 90) * DEG2RAD, x2, y2);
				rotate(px, py, (dir_angle - 90) * DEG2RAD, px, py);
			}
			
			script.g.draw_quad_world(layer, sublayer, false,
				x + x1 - px * thickness, y + y1 - py * thickness,
				x + x1 + px * thickness, y + y1 + py * thickness,
				x + x2 + px * thickness, y + y2 + py * thickness,
				x + x2 - px * thickness, y + y2 - py * thickness,
				light_clr, dark_clr, dark_clr, light_clr);
			script.g.draw_line_world(layer, sublayer,
				x + x1 - px * (thickness - 5), y + y1 - py * (thickness - 5),
				x + x2 - px * (thickness - 5), y + y2 - py * (thickness - 5),
				2, trim_clr);
			script.g.draw_line_world(layer, sublayer,
				x + x1 + px * (thickness - 5), y + y1 + py * (thickness - 5),
				x + x2 + px * (thickness - 5), y + y2 + py * (thickness - 5),
				2, trim_clr);
		}
		
		if(script.debug_wind && self.editor_selected())
		{
			draw_wind_grid();
		}
	}
	
	void offset(const float dx, const float dy)
	{
		for(uint i = 0; i < particles.length; i++)
		{
			Particle@ p = particles[i];
			p.x += dx;
			p.y += dy;
			p.prev_x += dx;
			p.prev_y += dy;
		}
	}
	
	void update_segment_length(const float new_length)
	{
		if(new_length == segment_length)
			return;
		
		for(uint i = 0; i < constraints.length; i++)
		{
			DistanceConstraint@ dc = cast<DistanceConstraint@>(constraints[i]);
			if(@dc == null)
				continue;
			
			dc.rest_length = new_length;
		}
		
		segment_length = new_length;
	}
	
	array<Constraint@>@ get_constraints()
	{
		return @constraints;
	}
	
	void save_positions(array<float>@ positions)
	{
		if(particles.length == 0)
			return;
		
		positions.resize((particles.length - 1) * 2);
		int j = 0;
		
		for(uint i = 1; i < particles.length; i++)
		{
			Particle@ p = @particles[i];
			positions[j++] = p.x - p1.x;
			positions[j++] = p.y - p1.y;
		}
	}
	
	void load_positions(array<float>@ positions)
	{
		if(positions.length != (particles.length - 1) * 2)
			return;
		
		int j = 0;
		Particle@ p0 = @particles[0];
		
		bool found_collision = false;
		float collision_x = 0;
		float collision_y = 0;
		
		for(uint i = 1; i < particles.length; i++)
		{
			Particle@ p = @particles[i];
			
			if(found_collision)
			{
				p.x = p.prev_x = collision_x;
				p.y = p.prev_y = collision_y;
				continue;
			}
			
			p.x = p.prev_x = p1.x + positions[j++];
			p.y = p.prev_y = p1.y + positions[j++];
			
			script.g.ray_cast_tiles_ex(
				p0.x, p0.y, p.x, p.y,
				script.collision_layer,
				script.ray);
			
			if(script.ray.hit())
			{
				found_collision = true;
				collision_x = script.ray.hit_x();
				collision_y = script.ray.hit_y();
			}
			
			@p0 = p;
		}
	}
	
	float end_angle()
	{
		if(particles.length <= 1)
			return 0;
		
		Particle@ p0 = particles[particles.length - 2];
		return atan2(p2.y - p0.y, p2.x - p0.x);
	}
	
	DistanceConstraint@ extend(float new_length=-1)
	{
		if(new_length < 0)
		{
			new_length = segment_length;
		}
		
		Particle@ p0 = particles[particles.length - 2];
		float dx = p2.x - p0.x;
		float dy = p2.y - p0.y;
		const float length = magnitude(dx, dy);
		dx /= length;
		dy /= length;
		Particle@ p = create_particle(p2.x + dx * new_length, p2.y + dy * new_length);
		DistanceConstraint@ d = create_constraint(p2, p, new_length);
		script.boxes_constraint.add(p);
		
		@p2 = p;
		return d;
	}
	
	private void draw_wind_grid()
	{
		script.frame++;
		wind_step();
		
		const float spacing = 24;
		
		for(float x = self.x() - 500; x <= self.x() + 500; x += spacing)
		{
			for(float y = self.y() - 500; y <= self.y() + 500; y += spacing)
			{
				float wx, wy;
				calc_wind(x, y, wx, wy);
				script.g.draw_line_world(
					22, 22, x, y,
					x + wx / (script.wind_strength * wind_multiplier) * 50,
					y + wy / (script.wind_strength * wind_multiplier) * 50,
					2, 0xffff0000);
			}
		}
	}
	
	private void wind_step()
	{
		wind_t = script.frame * wind_rate;
		// Make the x period fluctuate over time
		wind_x_ratio = (sin(wind_t * 0.01) * 0.5 + 0.5) * 0.002 + 0.002;
	}
	
	private void calc_wind(
		float x, float y, float &out out_x, float &out out_y,
		const float dir_multiplier=1,
		const float str_multiplier=1)
	{
		x *= 0.5;
		y *= 0.75;
		const float wind_range = PI * 0.1 + (sin(script.frame * 0.05 + x * 0.01) * 0.5 + 0.5) * PI * 0.2 * dir_multiplier;
		
		const float dir = sin(x * wind_x_ratio + y * 0.005 + wind_t * 0.04) * wind_range;
		const float strength = (sin((wind_t * 2 + x) * 0.0231 + (y - wind_t * 4) * 0.0841) * 0.5 + 0.5) * script.wind_strength * wind_multiplier * str_multiplier;
		out_x = cos(dir) * strength;
		out_y = sin(dir) * strength;
		
		//const float dir = sin(x * wind_x_ratio + y * 0.005 + wind_t * 0.04) * wind_range;
		//const float strength = (sin(wind_t * 0.01 + x * 0.0231 + y * 0.0841) * 0.5 + 0.5) * script.wind_strength * wind_multiplier * str_multiplier;
		//out_x = cos(dir) * strength;
		//out_y = sin(dir) * strength;
	}
	
}
