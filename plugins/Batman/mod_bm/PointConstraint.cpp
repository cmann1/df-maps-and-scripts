class PointConstraint : Constraint{
	Particle@ particle;
	float x;
	float y;
	
	PointConstraint(Particle@ particle, float x, float y){
		@this.particle = particle;
		this.x = x;
		this.y = y;
	}
	
	void Resolve(){
		particle.x = x;
		particle.y = y;
	}
}