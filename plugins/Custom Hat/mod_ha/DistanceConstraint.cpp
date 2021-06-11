class DistanceConstraint : Constraint{
	Particle@ particle1;
	Particle@ particle2;
	float restLength;
	
	DistanceConstraint(Particle@ p1, Particle@ p2, float d=-1){
		@particle1 = p1;
		@particle2 = p2;
		restLength = d == -1 ? distance(p1.x, p1.y, p2.x, p2.y) : d;
	}
	
	void Resolve(){
		float dx = particle2.x - particle1.x;
		float dy = particle2.y - particle1.y;
		/*float deltaLength = Math.sqrt(dx*dx + dy*dy);
		float deltaLength2 = dx*dx + dy*dy;
		float ratio = (deltaLength - restLength)/deltaLength;
		particle1.x += dx * 0.5 * ratio;
		particle1.y += dy * 0.5 * ratio;
		particle2.x -= dx * 0.5 * ratio;
		particle2.y -= dy * 0.5 * ratio;*/
		
		const float r = restLength * restLength / (dx * dx + dy * dy + restLength * restLength) - 0.5;
		dx *= r;
		dy *= r;
		particle1.x -= dx;
		particle1.y -= dy;
		particle2.x += dx;
		particle2.y += dy;
	}
	
	void Draw(scene@ g, float ox, float oy){
		g.draw_line_world(21, 21, ox + particle1.x, oy + particle1.y, ox + particle2.x, oy + particle2.y, 1, 0xFFFF0000);
	}
	
}
