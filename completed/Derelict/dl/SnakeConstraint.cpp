#include '../lib/phys/springs/constraints/Constraint.cpp';

/// Special constraint the pushes apart each pair of snake joints separated by another joint.
/// This helps prevent the entire snake folding in on itself giving it some more body.
/// This can also help prevent joints folding when entering tight spaces, which can cause the snake to get stuck.
class SnakeConstraint : Constraint
{
	
	float stiffness = 0.02;
	array<Particle@> particle_pairs;
	
	void resolve(const float time_scale, const int iteration) override
	{
		for(uint i = 0, count = particle_pairs.length; i < count; i += 2)
		{
			Particle@ p1 = particle_pairs[i];
			Particle@ p2 = particle_pairs[i + 1];
			
			float dx = p2.x - p1.x;
			float dy = p2.y - p1.y;
			float dist = dx * dx + dy * dy;
			
			if(dist < (p1.radius + p2.radius) * (p1.radius + p2.radius))
			{
				dist = sqrt(dist);
				float resting_ratio = dist != 0
					? ((p1.radius + p2.radius) - dist) / dist
					: (p1.radius + p2.radius);
				resting_ratio = resting_ratio * stiffness;
				dx *= resting_ratio;
				dy *= resting_ratio;
				p1.x -= dx;
				p1.y -= dy;
				p2.x += dx;
				p2.y += dy;
			}
		}
	}
	
}
