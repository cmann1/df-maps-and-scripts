#include '../lib/utils/colour.cpp';

#include 'Vehicle.cpp';
#include 'Location.cpp';

class Bird
{
	
	int index;
	script@ script;
	Vehicle vehicle;
	BirdState state = Idle;
	float target_x;
	float target_y;
	float dir_x;
	float dir_y;
	Player@ target;
	bool anticipate;
	float anticipate_x;
	float anticipate_y;
	bool is_default = true;
	float anticipate_timer;
	
	array<Location> segments(Flock::BirdSegmentLength);
	Location base;
	
	Sprite shadow_spr('props3', 'backdrops_3');
	raycast@ ray;
	
	bool do_glow;
	float glow = 0;
	
	void init(script@ script, const float x, const float y)
	{
		@this.script = script;
		vehicle.x = vehicle.prev_x = x;
		vehicle.y = vehicle.prev_y = y;
		reset_vehicle(1);
		
		const float angle = rand_range(-PI, PI);
		const float dx = cos(angle) * Flock::BirdSegmentLength;
		const float dy = sin(angle) * Flock::BirdSegmentLength;
		float px = x + dx;
		float py = y + dy;
		for(int i = 0; i < Flock::BirdSegments; i++)
		{
			Location@ p = @segments[i];
			p.x = p.prev_x = px;
			p.y = p.prev_y = py;
			px += dx;
			py += dy;
		}
	}
	
	void reset_vehicle(const float m=0.05)
	{
		vehicle.max_speed += (Flock::MaxSpeed - vehicle.max_speed) * m;
		vehicle.max_force += (Flock::MaxForce - vehicle.max_force) * m;
		vehicle.arrive_dist += (Flock::ArriveDist - vehicle.arrive_dist) * m;
		
		if(
			approximately(vehicle.max_speed, Flock::MaxSpeed) &&
			approximately(vehicle.max_force, Flock::MaxForce) &&
			approximately(vehicle.arrive_dist, Flock::ArriveDist)
		)
		{
			is_default = true;
		}
	}
	
	bool arrived()
	{
		return dist_sqr(vehicle.x, vehicle.y, target_x, target_y) <= vehicle.arrive_dist * vehicle.arrive_dist;
	}
	
	void step()
	{
		if(!is_default)
		{
			vehicle.avoid_tiles(script);
			
			float dx = target_x - vehicle.x;
			float dy = target_y - vehicle.y;
			normalize(dx, dy, dx, dy);
			@ray = script.g.ray_cast_tiles(vehicle.x, vehicle.y, target_x + dx * 48, target_y + dy * 48, ray);
			if(ray.hit())
			{
				target_x = ray.hit_x() - dx * 48;
				target_y = ray.hit_y() - dy * 48;
			}
		}
		
		vehicle.update();
		
		Location@ p0 = @base;
		p0.x = vehicle.x;
		p0.y = vehicle.y;
		p0.prev_x = vehicle.prev_x;
		p0.prev_y = vehicle.prev_y;
		
		for(int i = 0; i < Flock::BirdSegments; i++)
		{
			Location@ p1 = @segments[i];
			p1.prev_x = p1.x;
			p1.prev_y = p1.y;
			
			float dx = p1.x - p0.x;
			float dy = p1.y - p0.y;
			float dist = sqrt(dx * dx + dy * dy);
			
			if(dist > 0)
			{
				dx *= 0.9 + lerp(0.1, 0, vehicle.time_scale);
				dy *= 0.9 + lerp(0.1, 0, vehicle.time_scale);
				dist *= 0.9 + lerp(0.1, 0, vehicle.time_scale);
				p1.x = p0.x + dx;
				p1.y = p0.y + dy;
				
				if(dist > Flock::BirdSegmentLength)
				{
					p1.x = p0.x + dx / dist * Flock::BirdSegmentLength;
					p1.y = p0.y + dy / dist * Flock::BirdSegmentLength;
				}
			}
			
			@p0 = p1;
		}
		
		if(@target != null)
		{
			do_glow = false;
			if(glow < 1)
			{
				glow = min(glow + DT / Flock::GlowInSpeed, 1.0);
				if(do_glow && @target == null && glow == 1)
				{
					do_glow = false;
				}
			}
		}
		else if(do_glow)
		{
			if(glow < 0)
			{
				glow += DT;
			}
			else if(glow < 2)
			{
				glow = min(glow + DT / Flock::GlowInSpeed, 2.0);
				if(glow == 2)
				{
					do_glow = false;
				}
			}
		}
		else if(glow > 0)
		{
			glow = max(glow - DT / Flock::GlowOutSpeed, 0.0);
		}
	}
	
	void draw(scene@ g, const float sub_frame)
	{
		//g.draw_rectangle_world(22, 22, vehicle.x-2, vehicle.y-2,vehicle.x+2,vehicle.y+2, 0, 0xffff0000);
		//g.draw_line_world(22, 22, vehicle.x, vehicle.y, vehicle.x + vehicle.vel_x * 10, vehicle.y + vehicle.vel_y * 10, 1, 0xaaff0000);
		//g.draw_line_world(22, 22, target_x, target_y, vehicle.x, vehicle.y, 1, 0xaaffff00);
		
		Location@ p0;
		Location@ p1 = @base;
		Location@ p2 = @segments[0];
		
		p1.sx = lerp(p1.prev_x, p1.x, sub_frame);
		p1.sy = lerp(p1.prev_y, p1.y, sub_frame);
		p2.sx = lerp(p2.prev_x, p2.x, sub_frame);
		p2.sy = lerp(p2.prev_y, p2.y, sub_frame);
		
		float angle1 = 0;
		float angle2 = atan2(p2.sy - p1.sy, p2.sx - p1.sx);
		float angle = angle2;
		float prev_angle;
		
		const float size = Flock::BirdSize;
		const float trim_space = Flock::TrimSpace;
		const float trim_size = Flock::TrimSize;
		const uint trim_clr = glow > 0
			? colour::lerp(Player::ScarfTrimClr, Flock::GlowClr, clamp01(glow))
			: Player::ScarfTrimClr;
		
		for(int i = 0; i < Flock::BirdSegments; i++)
		{
			@p0 = @p1;
			@p1 = @p2;
			@p2 = (i + 1) < Flock::BirdSegments ? @segments[i + 1] : null;
			//g.draw_line_world(22, 22, p0.x, p0.y, p.x, p.y, 1, 0xaaff00ff);
			//g.draw_rectangle_world(22, 22, p.x-2, p.y-2, p.x+2, p.y+2, 0, 0xaaff0000);
			
			angle1 = angle2;
			
			if(@p2 != null)
			{
				p2.sx = lerp(p2.prev_x, p2.x, sub_frame);
				p2.sy = lerp(p2.prev_y, p2.y, sub_frame);
				angle2 = atan2(p2.sy - p1.sy, p2.sx - p1.sx);
			}
			
			prev_angle = angle;
			angle = angle1 + shortest_angle(angle1, angle2) * 0.5;
			
			const float nx0 = -sin(prev_angle);
			const float ny0 =  cos(prev_angle);
			const float nx1 = -sin(angle);
			const float ny1 =  cos(angle);
			
			g.draw_quad_world(Flock::Layer, Flock::SubLayer, false,
				p0.sx - nx0*size, p0.sy - ny0*size,
				p0.sx + nx0*size, p0.sy + ny0*size,
				p1.sx + nx1*size, p1.sy + ny1*size,
				p1.sx - nx1*size, p1.sy - ny1*size,
				Player::ScarfLightClr, Player::ScarfDarkClr, Player::ScarfDarkClr, Player::ScarfLightClr);
			// Trim
			g.draw_quad_world(Flock::Layer, Flock::SubLayer, false,
				p0.sx - nx0*(size - trim_space), p0.sy - ny0*(size - trim_space),
				p0.sx - nx0*(size - trim_space - trim_size), p0.sy - ny0*(size - trim_space - trim_size),
				p1.sx - nx1*(size - trim_space - trim_size), p1.sy - ny1*(size - trim_space - trim_size),
				p1.sx - nx1*(size - trim_space), p1.sy - ny1*(size - trim_space),
				trim_clr, trim_clr, trim_clr, trim_clr);
			g.draw_quad_world(Flock::Layer, Flock::SubLayer, false,
				p0.sx + nx0*(size - trim_space), p0.sy + ny0*(size - trim_space),
				p0.sx + nx0*(size - trim_space - trim_size), p0.sy + ny0*(size - trim_space - trim_size),
				p1.sx + nx1*(size - trim_space - trim_size), p1.sy + ny1*(size - trim_space - trim_size),
				p1.sx + nx1*(size - trim_space), p1.sy + ny1*(size - trim_space),
				trim_clr, trim_clr, trim_clr, trim_clr);
		}
		
		@p1 = @base;
		@p2 = @segments[Flock::BirdSegments - 1];
		const float dx = p2.sx - p1.sx;
		const float dy = p2.sy - p1.sy;
		angle = atan2(dy, dx);
		const float length = sqrt(dx * dx + dy * dy);
		shadow_spr.draw(
			Flock::Layer, Flock::ShadowSubLayer, 0, 0,
			(p1.x + p2.x) * 0.5,
			(p1.y + p2.y) * 0.5,
			angle * RAD2DEG,
			(length + 20) / shadow_spr.sprite_width, (Flock::BirdSize + 10) / shadow_spr.sprite_height * 2,
			Flock::ShadowClr);
		
		if(glow > 0)
		{
			shadow_spr.draw(
				Flock::Layer, Flock::GlowSubLayer, 0, 0,
				(p1.x + p2.x) * 0.5,
				(p1.y + p2.y) * 0.5,
				angle * RAD2DEG,
				(length + 20) / shadow_spr.sprite_width, (Flock::BirdSize + 10) / shadow_spr.sprite_height * 2,
				0xffffff | (round_int(clamp01(glow) * 255) << 24));
		}
	}
	
}

enum BirdState
{
	
	Idle,
	GiveScarf,
	JumpPause,
	JumpAnticipate,
	Jump,
	
}
