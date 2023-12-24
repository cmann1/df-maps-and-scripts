class ConstraintTracker
{
	
	protected SpringSystem@ spring_system;
	protected array<Particle@> particles;
	protected array<Constraint@> constraints;
	protected array<TileConstraint@> collision_constraints;
	protected bool created;
	protected bool constraints_added;
	
	void track_constraint(Constraint@ c)
	{
		constraints.insertLast(c);
	}
	
	protected bool create_constraints()
	{
		if(!created)
		{
			created = true;
			constraints_added = true;
			return true;
		}
		
		if(!constraints_added)
		{
			for(uint i = 0; i < particles.length; i++)
			{
				spring_system.add_particle(particles[i]);
			}
			for(uint i = 0; i < constraints.length; i++)
			{
				spring_system.add_constraint(constraints[i]);
			}
			
			constraints_added = true;
		}
		
		return false;
	}
	
	protected void remove_constraints()
	{
		if(!created || !constraints_added)
			return;
		
		for(uint i = 0; i < particles.length; i++)
		{
			spring_system.remove_particle(particles[i]);
		}
		for(uint i = 0; i < constraints.length; i++)
		{
			spring_system.remove_constraint(constraints[i]);
		}
		
		constraints_added = false;
	}
	
	protected void make_static(const bool is_static=true)
	{
		for(uint i = 0; i < particles.length; i++)
		{
			particles[i].is_static = is_static;
		}
	}
	
}
