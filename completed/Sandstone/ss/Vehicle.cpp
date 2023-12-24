#include '../lib/math/math.cpp';

class Vehicle
{
	
	float x;
	float y;
	float prev_x;
	float prev_y;
	float vel_x = 0;
	float vel_y = 0;
	float acc_x = 0;
	float acc_y = 0;
	
	float arrive_dist = 20;
	float in_sight_dist = 100;
	float too_close_dist = 10;
	float max_force = 0.01;
	float max_speed = 1;
	
	float time_scale = 1;
	
	void update()
	{
		prev_x = x;
		prev_y = y;
		
		vel_x += acc_x;
		vel_y += acc_y;
		vec2_limit(vel_x, vel_y, max_speed, vel_x, vel_y);
		x += vel_x * time_scale;
		y += vel_y * time_scale;
		acc_x = 0;
		acc_y = 0;
	}
	
	void apply_force(const float force_x, const float force_y)
	{
		acc_x += force_x * time_scale;
		acc_y += force_y * time_scale;
	}
	
	void seek(const float target_x, const float target_y)
	{
		float desired_x = target_x - x;
		float desired_y = target_y - y;
		normalize(desired_x, desired_y, desired_x, desired_y);
		desired_x *= max_speed;
		desired_y *= max_speed;
		
		float steer_x = desired_x - vel_x;
		float steer_y = desired_y - vel_y;
		vec2_limit(steer_x, steer_y, max_force, steer_x, steer_y);
		apply_force(steer_x, steer_y);
	}
	
	void flee(const float target_x, const float target_y)
	{
		float desired_x = target_x - x;
		float desired_y = target_y - y;
		normalize(desired_x, desired_y, desired_x, desired_y);
		desired_x *= max_speed;
		desired_y *= max_speed;
		
		float steer_x = desired_x - vel_x;
		float steer_y = desired_y - vel_y;
		vec2_limit(steer_x, steer_y, max_force, steer_x, steer_y);
		apply_force(-steer_x, -steer_y);
	}
	
	void arrive(const float target_x, const float target_y)
	{
		float desired_x = target_x - x;
		float desired_y = target_y - y;
		float d = magnitude(desired_x, desired_y);
		normalize(desired_x, desired_y, desired_x, desired_y);
		
		if(d < arrive_dist)
		{
			d = map(d, 0.0, arrive_dist, 0.0, max_speed);
			desired_x *= d;
			desired_y *= d;
		}
		else
		{
			desired_x *= max_speed;
			desired_y *= max_speed;
		}

		float steer_x = desired_x - vel_x;
		float steer_y = desired_y - vel_y;
		vec2_limit(steer_x, steer_y, max_force, steer_x, steer_y);
		apply_force(steer_x, steer_y);
	}
	
	void avoid_tiles(script@ script)
	{
		const float angle = atan2(vel_y, vel_x);
		
		script.g.ray_cast_tiles_ex(
			x, y,
			x + cos(angle - 35 * DEG2RAD) * 48,
			y + sin(angle - 35 * DEG2RAD) * 48,
			script.collision_layer,
			script.ray);
		
		//script.g.draw_line_world(22, 22, x, y,
		//	x + cos(angle - 35 * DEG2RAD) * 48,
		//	y + sin(angle - 35 * DEG2RAD) * 48, 1, 0xff00ffff);
		//script.g.draw_line_world(22, 22, x, y,
		//	x + cos(angle + 35 * DEG2RAD) * 48,
		//	y + sin(angle + 35 * DEG2RAD) * 48, 1, 0xff00ffff);
		
		if(!script.ray.hit())
		{
			script.g.ray_cast_tiles_ex(
				x, y,
				x + cos(angle + 35 * DEG2RAD) * 48,
				y + sin(angle + 35 * DEG2RAD) * 48,
				script.collision_layer,
				script.ray);
			
			if(!script.ray.hit())
				return;
		}
		
		flee(script.ray.hit_x(), script.ray.hit_y());
		flee(script.ray.hit_x(), script.ray.hit_y());
	}
	
	void flock(array<Vehicle@>@ vehicles)
	{
		int in_sight_count = 0;
		float avg_vel_x = vel_x;
		float avg_vel_y = vel_y;
		float avg_pos_x = 0;
		float avg_pos_y = 0;
		
		for(uint i = 0; i < vehicles.length; i++)
		{
			Vehicle@ vehicle = @vehicles[i];
			
			if(@vehicle == @this || !in_sight(vehicle))
				continue;
			
			avg_vel_x += vehicle.vel_x;
			avg_vel_y += vehicle.vel_y;
			avg_pos_x += vehicle.x;
			avg_pos_y += vehicle.y;
			in_sight_count++;
			
			if(too_close(vehicle))
			{
				flee(vehicle.x, vehicle.y);
			}
		}
		
		if(in_sight_count > 0){
			avg_vel_x /= in_sight_count;
			avg_vel_y /= in_sight_count;
			avg_pos_x /= in_sight_count;
			avg_pos_y /= in_sight_count;
			seek(avg_pos_x, avg_pos_y);
			apply_force(avg_vel_x - vel_x, avg_vel_y - vel_y);
		}
	}
	
	bool in_sight(Vehicle@ vehicle)
	{
		if(dist_sqr(x, y, vehicle.x, vehicle.y) > in_sight_dist * in_sight_dist)
			return false;
		
		float heading_x, heading_y;
		normalize(vel_x, vel_y, heading_x, heading_y);
		const float diff_x = vehicle.x - x;
		const float diff_y = vehicle.y - y;
		
		return dot(diff_x, diff_y, heading_x, heading_y) >= 0;
	}
	
	bool too_close(Vehicle@ vehicle)
	{
		return dist_sqr(x, y, vehicle.x, vehicle.y) < too_close_dist * too_close_dist;
	}

}
