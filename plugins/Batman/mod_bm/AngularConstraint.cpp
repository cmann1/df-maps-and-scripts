class AngularConstraint : Constraint
{
	
	float stiffness;
	Particle@ particle1;
	Particle@ particle2;
	Particle@ particle3;
	float min, max;
	float base_angle = 0;
	float offset_angle = 0;
	
	AngularConstraint(Particle@ particle1, Particle@ particle2, Particle@ particle3, float min, float max, float stiffness=0.1){
		@this.particle1 = particle1;
		@this.particle2 = particle2;
		@this.particle3 = particle3;
		SetRange(min, max);
		this.stiffness = stiffness;
	}
	
	void Resolve(){
		float dx = particle2.x - particle1.x;
		float dy = particle2.y - particle1.y;
		float angle1 = atan2(dy, dx);
		base_angle = particle3 is null ? 0 : atan2(particle1.y - particle3.y, particle1.x - particle3.x);
		
		float angle = short_angle(base_angle, angle1);
		offset_angle = 0;
		if(angle < min)
			offset_angle = min - angle;
		else if(angle > max)
			offset_angle = max - angle;
		
		float min_dx = short_angle(angle1, base_angle + min);
		float max_dx = short_angle(angle1, base_angle + max);
		offset_angle = 0;
		if(min_dx > 0 and max_dx < 0)
			offset_angle = abs(max_dx) < min_dx ? max_dx : min_dx;
		else if(min_dx > 0)
			offset_angle = min_dx;
		else if(max_dx < 0)
			offset_angle = max_dx;
		
		if(offset_angle != 0)
		{
			
			const float damping = 0.005;
			float da = stiffness * offset_angle - damping * offset_angle;
			
//			da *= 0.35;
			dx *= 0.5;
			dy *= 0.5;
			float mx = particle1.x + dx;
			float my = particle1.y + dy;
//			float dx2, dy2;
//			rotate(dx, dy, -da, dx2, dy2);
			rotate(dx, dy, da, dx, dy);
			particle2.x = mx + dx;
			particle2.y = my + dy;
			particle1.x = mx - dx;
			particle1.y = my - dy;
//			particle2.x = particle1.x + dx;
//			particle2.y = particle1.y + dy;
//			particle1.x = particle2.x + dx2;
//			particle1.y = particle2.y + dy2;
		}
	}
	
	void SetRange(float min, float max)
	{
//		min = min - PI2 * floor((min + PI) / PI2);
//		max = max - PI2 * floor((max + PI) / PI2);
		this.min = min < max ? min : max;
		this.max = max > min ? max : min;
	}
	
//	void Draw(scene@ g, float ox, float oy)
//	{
//		float x = ox + particle1.x;
//		float y = oy + particle1.y;
//		float ba = particle3 is null ? (min + max)*0.5 : base_angle;
//		g.draw_line_world(21, 21, x, y, x+cos(ba)*48, y+sin(ba)*48, 1, 0xFF00FF00);
//		g.draw_line_world(21, 21, x, y, x+cos(base_angle+min)*48, y+sin(base_angle+min)*48, 1, 0xFF000000);
//		g.draw_line_world(21, 21, x, y, x+cos(base_angle+max)*48, y+sin(base_angle+max)*48, 1, 0xFFFFFFFF);
//		g.draw_line_world(21, 21, x, y, x+cos(ba+offset_angle)*48, y+sin(ba+offset_angle)*48, 1, 0xFF00FFFF);
//	}
	
}