#include '../lib/debug/Debug.cpp';
#include '../lib/drawing/circle.cpp';
#include '../lib/easing/cubic.cpp';
#include '../lib/enums/GVB.cpp'; // REMOVE
#include '../lib/enums/Team.cpp';
#include '../lib/phys/springs/CachedTileProvider.cpp';
#include '../lib/phys/springs/SpringSystem.cpp';
#include '../lib/phys/springs/ITileProvider.cpp';

/// Failed first attempt at snake movement.
class Snake0 : enemy_base
{
	
	script@ script;
	scriptenemy@ self;
	
	SpringSystem spring_system;
	CachedTileProvider tile_provider;
	
	input_api@ input;
	
	protected array<Particle@> joints;
	protected Particle@ head;
	
	void init(script@ script, scriptenemy@ self)
	{
		@this.script = script;
		@this.self = self;
		
		self.auto_physics(false);
		self.team(Team::Cleaner);
		
		@tile_provider.g = script.g;
		tile_provider.frame = -1;
		
		spring_system.gravity.y = 96 * 9.81;
		spring_system.constraint_iterations = 3;
		
		//if(script.is_playing)
		{
			create();
		}
		
		if(script.is_playing)
		{
			controller_entity(0, self.as_controllable());
		}
		
		@input = get_input_api();
	}
	
	void create()
	{
		float x = self.x();
		float y = self.y();
		Particle@ p0;
		Particle@ pb;
		const float base_radius = 22;
		float radius_prev = base_radius;
		
		const int num_joints = 20;
		for(int i = 0; i < num_joints; i++)
		{
			const float t = float(i) / (num_joints - 1);
			Particle@ p = Particle(x, y);
			joints.insertLast(p);
			p.radius = lerp(5, base_radius, ease_in_cubic(1 - max(t - 0.65, 0.0)));
			spring_system.add_particle(p);
			
			x += (p.radius + radius_prev) * 0.75;
			radius_prev = p.radius;
			
			TileConstraint@ tc = TileConstraint(tile_provider, p, 10);
			spring_system.add_constraint(tc);
			
			if(@p0 != null)
			{
				DistanceConstraint@ dc = DistanceConstraint(p0, p, 0.98, 0.25);
				spring_system.add_constraint(dc);
				if(@pb != null)
				{
					AngularConstraint@ ac = AngularConstraint(p0, p, pb, -65, 65, 0.01, 0.01);
					spring_system.add_constraint(ac);
				}
			}
			
			@pb = p0;
			@p0 = p;
		}
		
		@head = joints[0];
	}
	
	void editor_step()
	{
		step();
	}
	
	void step()
	{
		const float strength_x = 8000;
		const float strength_y = 8000;
		float fx = 0;
		float fy = 0;
		if(@input != null)
		{
			if(input.key_check_gvb(GVB::UpArrow))
				fy -= strength_y;
			if(input.key_check_gvb(GVB::DownArrow))
				fy += strength_y;
			if(input.key_check_gvb(GVB::LeftArrow))
				fx -= strength_x;
			if(input.key_check_gvb(GVB::RightArrow))
				fx += strength_x;
		}
		else
		{
			const int move_x = self.x_intent();
			const int move_y = self.y_intent();
			if(move_y == -1)
				fy -= strength_y;
			else if(move_y == 1)
				fy += strength_y;
			if(move_x == -1)
				fx -= strength_x;
			else if(move_x == 1)
				fx += strength_x;
		}
		
		if(fx != 0 || fy != 0)
		{
			const int max_contacts = int(joints.length * 0.5);
			int contact_count = 0;
			for(int i = 1; i < max_contacts; i++)
			{
				Particle@ p = joints[i];
				const float t = float(i) / (max_contacts - 1);
				
				if(p.has_contact)
				{
					contact_count++;
					float ix, iy;
					project(fx, fy, -p.contact_ny, p.contact_nx, ix, iy);
					p.impulse_x += (ix / max_contacts) * 20;
					p.impulse_y += (iy / max_contacts) * 20;
				}
			}
			
			float mult = min(float(contact_count) / ((max_contacts - 1) * 0.25), 1.0);
			mult *= head.has_contact ? 5.0 : 1.0;
			mult = mult * 0.5 + 0.5;
			head.impulse_x += fx * mult;
			head.impulse_y += fy * mult;
		}
		
		tile_provider.frame++;
		spring_system.step(script.time_scale);
		
		if(script.is_playing)
		{
			self.set_xy(head.x, head.y);
			self.set_speed_xy((head.x - head.prev_x) / DT, (head.y - head.prev_y) / DT);
		}
	}
	
	void draw(float sub_frame)
	{
		for(uint i = 0; i < joints.length; i++)
		{
			Particle@ p = joints[i];
			drawing::circle(script.g, 22, 22, p.x, p.y, p.radius, 32, 1, 0x44ffffff);
		}
		
		spring_system.debug_draw(script.g);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		
		script.g.draw_rectangle_world(22, 22, self.x() - 24, self.y() - 24, self.x() + 24, self.y() + 24, 0, 0x44ffffff);
	}
	
}
