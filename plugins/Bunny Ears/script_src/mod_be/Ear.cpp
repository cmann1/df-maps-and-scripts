class Ear
{
	
	scene@ g;
	SpringSystem@ spring_system;
	
	Particle@ base;
	AngularConstraint@ base_angle;
	array<Particle@> joints;
	array<AngularConstraint@> angle_constraints;
	
	float rotation;
	float offset_x, offset_y;
	float rotation_offset;
	
	uint dark_clr, light_clr;
	
	void init(scene@ g, SpringSystem@ spring_system,
		const float offset_x, const float offset_y,
		const float rotation_offset)
	{
		@this.g = g;
		@this.spring_system = spring_system;
		
		this.offset_x = offset_x;
		this.offset_y = offset_y;
		this.rotation_offset = rotation_offset;
		
		const int num_joints = max(NUM_EAR_JOINTS, 2);
		const float segment_length = EAR_LENGTH / (num_joints - 1);
		joints.resize(num_joints);
		
		float x = 0;
		float y = 0;
		Particle@ prev_base_joint;
		Particle@ prev_joint;
		
		for(int i = 0; i < num_joints; i++)
		{
			Particle@ joint = spring_system.add_particle(x, y);
			@joints[i] = joint;
			
			if(@prev_joint != null)
			{
				spring_system.add_constraint(
					DistanceConstraint(
						prev_joint, joint,
						EAR_STIFFNESS, EAR_DAMPING));
				
				if(@prev_base_joint != null)
				{
					AngularConstraint@ ac = spring_system.add_constraint(
						AngularConstraint(
							prev_joint, joint, prev_base_joint,
							EAR_SEGMENT_ANGLE_MIN, EAR_SEGMENT_ANGLE_MAX,
							EAR_ANGLE_STIFFNESS, EAR_ANGLE_DAMPING));
					angle_constraints.insertLast(ac);
				}
			}
			else
			{
				joint.is_static = true;
			}
			
			@prev_base_joint = prev_joint;
			@prev_joint = joint;
			y -= segment_length;
		}
		
		@base = joints[0];
		Particle@ p2 = joints[1];
		@base_angle = spring_system.add_constraint(
			AngularConstraint(
				base, p2, null,
				-90 + EAR_BASE_ANGLE_MIN, -90 + EAR_BASE_ANGLE_MAX,
				EAR_BASE_ANGLE_STIFFNESS, EAR_BASE_ANGLE_DAMPING));
	}
	
	void calculate_base_pos(
		const float base_x, const float base_y,
		float &out x, float &out y)
	{
		rotate(offset_x, offset_y, (rotation + 90) * DEG2RAD, x, y);
		x += base_x;
		y += base_y;
	}
	
	void reset(float x, float y)
	{
		calculate_base_pos(x, y, x, y);
		
		offset(x - base.x, y - base.y);
	}
	
	void offset(const float dx, const float dy)
	{
		const int num_joints = int(joints.length);
		
		for(int i = 0; i < num_joints; i++)
		{
			Particle@ joint = joints[i];
			joint.x += dx;
			joint.y += dy;
			joint.prev_x += dx;
			joint.prev_y += dy;
		}
	}
	
	void step(const float base_x, const float base_y)
	{
		calculate_base_pos(base_x, base_y, base.x, base.y);
	}
	
	void update_rotation(const float rotation, const int face)
	{
		this.rotation = rotation;
		base_angle.set_range(
			rotation + (rotation_offset + EAR_BASE_ANGLE_MIN) * face,
			rotation + (rotation_offset + EAR_BASE_ANGLE_MAX) * face);
		
		for(uint i = 0; i < angle_constraints.length; i++)
		{
			angle_constraints[i].set_range(
				EAR_SEGMENT_ANGLE_MIN * face,
				EAR_SEGMENT_ANGLE_MAX * face);
		}
	}
	
	void update_colours(const uint dark_clr, const uint light_clr)
	{
		this.dark_clr = dark_clr;
		this.light_clr = light_clr;
	}
	
	void draw(const float sub_x, const float sub_y, const float head_angle)
	{
		const int num_joints = max(NUM_EAR_JOINTS, 2);
		
		Particle@ p1 = null;
		Particle@ p2 = joints[0];
		Particle@ p3 = joints[1];
		float p2_angle;
		float p3_angle = atan2(p3.y - p2.y, p3.x - p2.x);
		
		float x1, y1;
		float x2 = cos(head_angle + PI * 0.5) * BASE_WIDTH * 0.5;
		float y2 = sin(head_angle + PI * 0.5) * BASE_WIDTH * 0.5;
		float angle;
		
		for(int i = 1; i < num_joints; i++)
		{
			@p1 = p2;
			@p2 = p3;
			@p3 = (i + 1) < num_joints ? @joints[i + 1] : null;
			
			p2_angle = p3_angle;
			
			if(@p3 != null)
			{
				p3_angle = atan2(p3.y - p2.y, p3.x - p2.x);
				angle = p2_angle + shortest_angle(p2_angle, p3_angle) * 0.5;
			}
			else
			{
				angle = p2_angle;
			}
			
			x1 = x2;
			y1 = y2;
			const float t = float(i) / (num_joints - 1);
			const float node_width = lerp(BASE_WIDTH, END_WIDTH, t);
			x2 = cos(angle + PI * 0.5) * node_width * 0.5;
			y2 = sin(angle + PI * 0.5) * node_width * 0.5;
			
			g.draw_quad_world(18, 10, false,
				sub_x + p1.x - x1, sub_y + p1.y - y1,
				sub_x + p1.x + x1, sub_y + p1.y + y1,
				sub_x + p2.x + x2, sub_y + p2.y + y2,
				sub_x + p2.x - x2, sub_y + p2.y - y2,
				dark_clr, light_clr, light_clr, dark_clr);
		}
		
		// Tip
		x1 = x2;
		y1 = y2;
		x2 = cos(angle + PI * 0.5) * TIP_WIDTH * 0.5;
		y2 = sin(angle + PI * 0.5) * TIP_WIDTH * 0.5;
		const float x3 = cos(angle) * TIP_HEIGHT;
		const float y3 = sin(angle) * TIP_HEIGHT;
		g.draw_quad_world(18, 10, false,
			sub_x + p2.x - x1, sub_y + p2.y - y1,
			sub_x + p2.x + x1, sub_y + p2.y + y1,
			sub_x + p2.x + x2 + x3, sub_y + p2.y + y2 + y3,
			sub_x + p2.x - x2 + x3, sub_y + p2.y - y2 + y3,
			dark_clr, light_clr, light_clr, dark_clr);
	}
	
}
