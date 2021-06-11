#include 'Particle.cpp'
#include 'Force.cpp'
#include 'Constraint.cpp'
#include 'PointConstraint.cpp'
#include 'DistanceConstraint.cpp'
#include 'AngularConstraint.cpp'

class SpringSystem{
	int iterations = 5;
	
	Force gravity(0, 650);
	
	private array<Force@> forces;
	private int forceCount;
	private array<Particle@> particles;
	private int particleCount;
	private array<Constraint@> constraints;
	private int constraintCount;
	
	private float deltaPrev = DT;
	private float airFriction;
	
	SpringSystem(float airFriction = 0.85){
		SetDrag(airFriction);
	}
	
	void SetDrag(float airFriction){
		this.airFriction = airFriction;
	}
	
	Force@ AddForce(float x, float y){
		Force@ f = Force(x, y);
		forces.insertLast(f);
		forceCount++;
		return f;
	}
	
	Particle@ AddPartice(float x, float y){
		Particle@ particle = Particle(x, y);
		particles.insertLast(particle);
		particleCount++;
		return particle;
	}
	
	PointConstraint@ AddPointConstraint(Particle@ particle, float x, float y){
		PointConstraint@ constraint = PointConstraint(particle, x, y);
		constraints.insertLast(constraint);
		constraintCount++;
		return constraint;
	}
	
	DistanceConstraint@ AddDistanceConstraint(Particle@ p1, Particle@ p2, float distance=-1){
		DistanceConstraint@ constraint = DistanceConstraint(p1, p2, distance);
		constraints.insertLast(constraint);
		constraintCount++;
		return constraint;
	}
	
	AngularConstraint@ AddAngularConstraint(Particle@ p1, Particle@ p2, Particle@ p3, float min, float max, float stiffness=0.1){
		AngularConstraint@ constraint = AngularConstraint(p1, p2, p3, min * DEG2RAD, max * DEG2RAD, stiffness);
		constraints.insertLast(constraint);
		constraintCount++;
		return constraint;
	}
	
	void Update(const float delta){
		const float dt = DT * delta;
		
		// Accumulate forces
		for(int a = 0; a < particleCount; a++){
			Particle@ particle = particles[a];
			particle.forceX = gravity.x;
			particle.forceY = gravity.y;
			for(int b = 0; b < forceCount; b++){
				Force@ force = forces[b];
				particle.forceX += force.x;
				particle.forceY += force.y;
			}
			// Verlet
			particle.setPosition(
				particle.x + (particle.x - particle.oldX) * airFriction * (dt / deltaPrev) + particle.forceX * (dt * dt),
				particle.y + (particle.y - particle.oldY) * airFriction * (dt / deltaPrev) + particle.forceY * (dt * dt)
			);
		}
		
		// Satisfy constaints.
		for(int b = 0; b < iterations; b++){
			for(int a = 0; a < constraintCount; a++){
				constraints[a].Resolve();
			}
		}
		
		deltaPrev = dt;
	}
	
	void Draw(scene@ g, float ox, float oy){
		for(int a = 0; a < particleCount; a++){
			Particle@ particle = particles[a];
			
			g.draw_rectangle_world(21, 21, ox + particle.x-1, oy + particle.y-1, ox + particle.x+1, oy + particle.y+1, 0, 0xFFFF0000);
		}
		
		for(int a = 0; a < constraintCount; a++){
			constraints[a].Draw(g, ox, oy);
		}
	}
	
	void Shift(const float dx, const float dy){
		for(int a = 0; a < particleCount; a++){
			Particle@ particle = particles[a];
			particle.x += dx;
			particle.y += dy;
			particle.oldX += dx;
			particle.oldY += dy;
		}
	}
	
}
