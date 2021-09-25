#include '../../lib/math/Line.cpp';
#include '../../lib/utils/colour.cpp';
#include '../../lib/enums/ColType.cpp';
#include '../../lib/enums/EntityState.cpp';

namespace Arc
{
	
	const float SprWidth = 31;
	const float Spr1Height = 3;
	const float Spr2Height = 7;
	
	const float Velocity = 15;
	const float ThicknessFactorMin = 0.8;
	const float ThicknessFactorMax = 1.5;
	
	const float BranchScale = 0.5;
	const float BranchThicknessFactor = 0.9;
	const float BranchLengthFactorMin = 0.3;
	const float BranchLengthFactorMax = 0.5;
	const float BranchAngleMin = 5 * DEG2RAD;
	const float BranchAngleMax = 80 * DEG2RAD;
	
	const float FadeTime = 0.05;
	
	float velocity(const float scale=1)
	{
		return rand_range(-Velocity, Velocity) * scale;
	}
	
	string sprite_index(const float thickness, float &out height)
	{
		const int size = thickness < 3 ? 1 : 2;
		height = size == 1 ? Spr1Height : Spr2Height;
		return 'arc' + size + rand_range(1, 4);
	}
	
	Segment@ build_segments(const float x1, const float y1, const float x2, const float y2, const float division_factor)
	{
		const float dx = x2 - x1;
		const float dy = y2 - y1;
		const float length = magnitude(dx, dy);
		
		if(length == 0)
			return null;
		
		const int divisions = clamp(int(length / (48 * division_factor)), 3, 5);
		const float nx = -dy / length;
		const float ny =  dx / length;
		const float spread = clamp(length / (500 * division_factor) * 96, 32.0, 96.0 * division_factor);
		
		float p0x = x1;
		float p0y = y1;
		float total_length = 0;
		Segment@ first_segment = null;
		Segment@ prev_segment = null;
		
		for(int i = 0; i < divisions; i++)
		{
			const float t = i / (divisions - 1.0);
			float p1x = x1 + dx * t;
			float p1y = y1 + dy * t;
			// Map from 0-1 to 0-1-0
			const float t2 = 1 - abs((t - 0.5) * 2);
			const float spread_mult = (0.5 + frand() * 0.5) * (frand() > 0.5 ? 1 : -1);
			p1x += nx * spread * t2 * spread_mult;
			p1y += ny * spread * t2 * spread_mult;
			
			Segment@ s = Segment();
			s.x = p1x;
			s.y = p1y;
			s.dx = p1x - p0x;
			s.dy = p1y - p0y;
			s.base_length = total_length;
			s.length = distance(p0x, p0y, p1x, p1y);
			
			if(@prev_segment != null)
			{
				@prev_segment.next = s;
			}
			else
			{
				@first_segment = s;
			}
			
			total_length += s.length;
			
			p0x = p1x;
			p0y = p1y;
			@prev_segment = s;
		}
		
		first_segment.base_length = total_length;
		return first_segment;
	}
	
	class Segment
	{
		float x;
		float y;
		float dx;
		float dy;
		float base_length;
		float length;
		Segment@ next;
	}
	
}

class Arc
{
	
	script@ script;
	
	float x1;
	float y1;
	float x2;
	float y2;
	float max_life = 0.5;
	float life = 0.5;
	int layer;
	int sub_layer;
	float resolution;
	float thickness;
	uint colour;
	
	DamageType kill_player;
	DamageType kill_enemy;
	DamageType kill_apple;
	int player_stun = 48;
	int enemy_stun = 48;
	int apple_stun = 48;
	
	int points_size = 32;
	int num_points;
	array<ArcPoint> points(points_size);
	Arc@ parent;
	ArcPoint@ anchor;
	
	sprites@ spr;
	
	Arc(script@ script)
	{
		@this.script = script;
		
		@spr = create_sprites();
		spr.add_sprite_set('script');
	}
	
	void reset()
	{
		@parent = null;
		@anchor = null;
		kill_player = Off;
		kill_enemy = Off;
		kill_apple = Off;
	}
	
	void initialise(ArcSource@ src, ArcContactArea@ end, Coil@ coil)
	{
		const float scale = @parent == null ? 1.0 : Arc::BranchScale;
		
		if(@parent == null)
		{
			max_life = rand_range(src.life_min, src.life_max) * end.life_multiplier;
			life = max_life;
			
			layer = src.layer;
			sub_layer = src.sub_layer_max >= 0
				? rand_range(src.sub_layer_min, src.sub_layer_max)
				: src.sub_layer_min;
			
			resolution = src.resolution;
			colour = colour::lerp(src.colour1, src.colour2, frand());
		}
		else
		{
			max_life = parent.max_life;
			life = max_life;
			
			layer = parent.layer;
			sub_layer = parent.sub_layer;
			
			resolution = parent.resolution * scale;
			colour = parent.colour;
		}
		
		Arc::Segment@ base_segment = Arc::build_segments(x1, y1, x2, y2, scale);
		if(@base_segment == null)
			return;
		
		const float length = base_segment.base_length;
		num_points = ceil_int(length / (@parent == null ? src.resolution : resolution) + 1.0);
		
		if(num_points <= 1)
			return;
		
		while(num_points >= points_size)
		{
			points.resize(points_size *= 2);
		}
		
		const float dx = x2 - x1;
		const float dy = y2 - y1;
		thickness = @parent == null
			? rand_range(src.thickness_min, src.thickness_max)
			: anchor.thickness * Arc::BranchThicknessFactor;
		const bool grounded = @end !=  null && end.grounded;
		
		Arc::Segment@ segment = base_segment.next;
		
		for(int i = 0; i < num_points; i++)
		{
			const float t = float(i) / (num_points - 1);
			const float t2 = float(i) / (num_points);
			
			const float p_distance = length * t;
			while(p_distance > segment.base_length + segment.length && @segment.next != null)
			{
				@segment = segment.next;
			}
			
			ArcPoint@ p = @points[i];
			const float segment_t = 1 - ((p_distance) - segment.base_length) / segment.length;
			p.x = segment.x - segment.dx * segment_t;
			p.y = segment.y - segment.dy * segment_t;
			
			if(i > 0 && (!grounded || i < num_points - 1))
			{
				p.x += Arc::velocity(scale);
				p.y += Arc::velocity(scale);
			}
			p.prev_x = p.x;
			p.prev_y = p.y;
			
			p.thickness = thickness * (grounded ? 1.0 : 1.0 - t2) *
				rand_range(Arc::ThicknessFactorMin, Arc::ThicknessFactorMax);
			p.spr_index = Arc::sprite_index(p.thickness, p.spr_height);
			p.vel_x = Arc::velocity(scale);
			p.vel_y = Arc::velocity(scale);
			if(@end != null)
			{
				p.vel_x /= end.life_multiplier;
				p.vel_y /= end.life_multiplier;
			}
		}
		
		points[0].is_static = true;
		points[num_points - 1].is_static = grounded;
		
		if(@end != null && end.emitter_id > 0 && script.is_playing)
		{
			const float l = magnitude(dx, dy);
			script.add_emitter_burst(
				end.emitter_id,
				layer, sub_layer,
				x2 - dx / l * 15,
				y2 - dy / l * 15,
				30, 30, 0, 0.25, 1);
		}
		
		if(@parent != null)
			return;
		
		const float angle = atan2(dy, dx);
		
		const int num_branches = rand_range(0, src.branches + 1);
		for(int i = 0; i < num_branches; i++)
		{
			Arc@ arc = coil.add_arc();
			if(@arc == null)
				break;
			
			const int anchor_index = rand_range(1, floor_int(num_points * 0.8));
			
			@arc.parent = @this;
			@arc.anchor = points[anchor_index];
			// Reduce the length a little, closer to the parent end
			const float l_t = map(1 - (anchor_index / (num_points * 0.8)), 0, 1, 0.5, 1);
			const float l = length * rand_range(Arc::BranchLengthFactorMin, Arc::BranchLengthFactorMax) * l_t;
			const float a = angle + rand_range(Arc::BranchAngleMin, Arc::BranchAngleMax) *
				(frand() > 0.5 ? -1 : 1);
			arc.x1 = arc.anchor.x;
			arc.y1 = arc.anchor.y;
			arc.x2 = arc.x1 + cos(a) * l;
			arc.y2 = arc.y1 + sin(a) * l;
			arc.initialise(null, null, coil);
		}
		
		if(script.is_playing && src.spark_volume > 0)
		{
			script.g.play_sound('sfx_sparks_' + rand_range(1, 12),
				x1 + dx * 0.5, y1 + dy * 0.5,
				src.spark_volume * rand_range(0.9, 1.0), false, true);
		}
	}
	
	bool step()
	{
		life -= DT;
		
		if(life < 0)
		{
			life = 0;
			return false;
		}
		
		const float ox = @anchor != null ? anchor.x - anchor.prev_x : 0;
		const float oy = @anchor != null ? anchor.y - anchor.prev_y : 0;
		
		for(int i = 0; i < num_points; i++)
		{
			ArcPoint@ p = @points[i];
			p.prev_x = p.x;
			p.prev_y = p.y;
			
			p.x += ox;
			p.y += oy;
			
			if(p.is_static)
				continue;
			
			p.x += p.vel_x * DT;
			p.y += p.vel_y * DT;
		}
		
		if(script.is_playing && (kill_player != Off || kill_enemy != Off || kill_apple != Off))
		{
			//draw_line(script.g, 22, 22, x1, y1, x2, y2, 2, 0xffff0000);
			const float padding = thickness + 5;
			int i = script.g.get_entity_collision(
				min(y1, y2) - padding,
				max(y1, y2) + padding,
				min(x1, x2) - padding,
				max(x1, x2) + padding,
				ColType::Hittable);
			
			while(--i >= 0)
			{
				controllable@ c = script.g.get_controllable_collision_index(i);
				if(@c == null)
					continue;
				
				float stun_force = 0;
				DamageType damage = Off;
				
				if(c.player_index() != -1)
				{
					if(kill_player == Off)
						continue;
					
					damage = kill_player;
					stun_force = player_stun;
				}
				const string type = c.type_name();
				if(type == 'hittable_apple')
				{
					if(kill_apple == Off)
						continue;
					
					damage = kill_apple;
					stun_force = apple_stun;
				}
				if(type.substr(0, 6) == 'enemy_')
				{
					if(kill_enemy == Off)
						continue;
					
					damage = kill_enemy;
					stun_force = enemy_stun;
				}
				
				rectangle@r = c.base_rectangle();
				float _;
				if(!line_aabb_intersection(
					x1, y1, x2, y2,
					c.x() + r.left(),
					c.y() + r.top(),
					c.x() + r.right(),
					c.y() + r.bottom(),
					_, _)
				)
					continue;
				
				if(
					(damage == Stun || damage == KillAndStun) &&
					(c.state() < EntityState::Stun || c.state() > EntityState::StunGround))
				{
					const float mx = c.x() + r.left() + r.width * 0.5;
					const float my = c.y() + r.top() + r.height * 0.5;
					Line line(x1, y1, x2, y2);
					float lx, ly;
					line.closest_point(mx, my, lx, ly);
					const float dx = mx - lx;
					const float dy = my - ly;
					const float l = sqrt(dx * dx + dy * dy);
					c.stun(dx / l * stun_force, dy / l * stun_force);
				}
				
				if(damage == Kill || damage == KillAndStun)
				{
					dustman@ dm = c.as_dustman();
					if(@dm != null)
					{
						dm.kill(false);
						continue;
					}
					
					script.g.remove_entity(c.as_entity());
				}
			}
		}
		
		return true;
	}
	
	void power_off()
	{
		if(life > Arc::FadeTime)
		{
			life = Arc::FadeTime;
		}
	}
	
	void draw(float sub_frame)
	{
		//if(@parent == null)
		//draw_line(script.g, 21, 22, x1, y1, x2, y2, 2, 0x99ff0000);
		
		const float alpha = clamp01(lerp(life + DT, life, sub_frame) / Arc::FadeTime);
		const uint clr = multiply_alpha(colour, alpha);
		
		ArcPoint@ p0 = @points[0];
		float p0x = lerp(p0.prev_x, p0.x, sub_frame);
		float p0y = lerp(p0.prev_y, p0.y, sub_frame);
		
		for(int i = 1; i < num_points; i++)
		{
			ArcPoint@ p = @points[i];
			const float px = lerp(p.prev_x, p.x, sub_frame);
			const float py = lerp(p.prev_y, p.y, sub_frame);
			const float l = distance(p0x, p0y, px, py);
			const float a = atan2(py - p0y, px - p0x) * RAD2DEG;
			spr.draw_world(
				layer, sub_layer, p.spr_index, 0, 0, p0x, p0y, a,
				l / Arc::SprWidth, p.thickness / p.spr_height, clr);
			
			p0x = px;
			p0y = py;
		}
	}
	
}

class ArcPoint
{
	
	float prev_x;
	float prev_y;
	float x;
	float y;
	float vel_x;
	float vel_y;
	float thickness;
	bool is_static;
	string spr_index;
	float spr_height;
	
}
