class Vehicle
{
	[hidden] float loc_x;
	[hidden] float loc_y;
	[hidden] float vel_x = 0;
	[hidden] float vel_y = 0;
	[hidden] float acc_x = 0;
	[hidden] float acc_y = 0;
	
	[hidden] float r = 20;
	[hidden] float maxforce = 0.01;
	[hidden] float maxspeed = 1;
	
	[hidden] float prev_x = 0;
	[hidden] float prev_y = 0;
	
	void update()
	{
		prev_x = loc_x;
		prev_y = loc_y;
		
		vel_x += acc_x;
		vel_y += acc_y;
		vec2_limit(vel_x, vel_y, maxspeed, vel_x, vel_y);
		loc_x += vel_x;
		loc_y += vel_y;
		acc_x = 0;
		acc_y = 0;
	}
	
	void apply_force(float force_x, float force_y)
	{
		acc_x += force_x;
		acc_y += force_y;
	}
	
	void seek(float target_x, float target_y)
	{
		float desired_x = target_x - loc_x;
		float desired_y = target_y - loc_y;
		normalize(desired_x, desired_y, desired_x, desired_y);
		desired_x *= maxspeed;
		desired_y *= maxspeed;
		
		float steer_x = desired_x - vel_x;
		float steer_y = desired_y - vel_y;
		vec2_limit(steer_x, steer_y, maxforce, steer_x, steer_y);
		apply_force(steer_x, steer_y);
	}
	
	void arrive(float target_x, float target_y) {
		float desired_x = target_x - loc_x;
		float desired_y = target_y - loc_y;

		float d = magnitude(desired_x, desired_y);
		normalize(desired_x, desired_y, desired_x, desired_y);
		if(d < r)
		{
			float m = map(d, 0, r, 0, maxspeed);
			desired_x *= m;
			desired_y *= m;
		}
		else
		{
			desired_x *= maxspeed;
			desired_y *= maxspeed;
		}

		float steer_x = desired_x - vel_x;
		float steer_y = desired_y - vel_y;
		vec2_limit(steer_x, steer_y, maxforce, steer_x, steer_y);
		apply_force(steer_x, steer_y);
	}

}