#include "../common/drawing_utils.cpp"
#include "../common/math.cpp"

class BoxAfterImage : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	float prev_x = 0;
	float prev_y = 0;
	
	float size = 1;
	float rotation = 1;
	float life_max = 10;
	float life = life_max;
	
	BoxAfterImage()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
	}
	
	void step()
	{
		if(--life == 0)
		{
			g.remove_entity(self.as_entity());
		}
	}
	
	void draw(float sub_frame)
	{
		const float x = lerp(prev_x, self.x(), sub_frame);
		const float y = lerp(prev_y, self.y(), sub_frame);
		const uint alpha = uint(((life - 1 * sub_frame) / life_max) * 0x99);
		g.draw_rectangle_world(18, 9, x - size, y - size, x + size, y + size, rotation, (alpha << 24) + 0xFFFFFF);
	}
	
}

class CornerBoostFx : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	float size = 1;
	uint colour = 0;
	float life_max = 20;
	float life = life_max;
	
	CornerBoostFx()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		colour = colour & 0xFFFFFF;
	}
	
	void step()
	{
		if(--life == 0)
		{
			g.remove_entity(self.as_entity());
		}
	}
	
	void draw(float sub_frame)
	{
		const float t = (life - 1 * sub_frame) / life_max;
		const uint alpha = uint(t * 0xFF);
		const float size = this.size + 30 * (1 - t);
		outline_rect(g,
			self.x() - size, self.y() - size,
			self.x() + size, self.y() + size, 18, 9, 3, (alpha << 24) + colour);
	}
	
}

class AttackFx : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	float life_max = 8;
	float expansion = 0.3;
	
	float size = 1;
	uint colour = 0;
	float life = life_max;
	controllable@ e = null;
	
	AttackFx()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
	}
	
	void step()
	{
		if(--life == 0 or @e == null)
		{
			g.remove_entity(self.as_entity());
			return;
		}
		
		self.x(e.x());
		self.y(e.y());
	}
	
	void draw(float sub_frame)
	{
		if(@e == null) return;
		
		const float x = lerp(e.prev_x(), e.x(), sub_frame);
		const float y = lerp(e.prev_y(), e.y(), sub_frame);
		const float sub_life = (life - 1 * sub_frame);
		
		const uint colour = 0xAAFFFFFF;
		const float t = sub_life / life_max;
		float rad = size * 0.9 + size * expansion * (1 - t);
		const float s = 0.5;
		const float h = 0.9;
		const float rad_inner = rad * s;
		
		const float lt_x = x - rad_inner;
		const float lt_y = y - rad * h;
		const float l_x = x - rad;
		const float l_y = y;
		const float lb_x = x - rad_inner;
		const float lb_y = y + rad * h;
		
		const float rt_x = x + rad_inner;
		const float rt_y = y - rad * h;
		const float r_x = x + rad;
		const float r_y = y;
		const float rb_x = x + rad_inner;
		const float rb_y = y + rad * h;
		
		const float gap = 0.65;
		
		// Top
		g.draw_line_world(18, 9,
			lt_x, lt_y,
			rt_x, rt_y, 8, 0x88FFFFFF);
		// Bottom
		g.draw_line_world(18, 9,
			lb_x, lb_y,
			rb_x, rb_y, 8, 0x88FFFFFF);
		
		// Left top
		g.draw_line_world(18, 9,
			l_x, l_y,
			l_x + (lt_x - l_x) * gap, l_y + (lt_y - l_y) * gap, 8, 0x88FFFFFF);
		// Left bottom
		g.draw_line_world(18, 9,
			l_x, l_y,
			l_x + (lb_x - l_x) * gap, l_y + (lb_y - l_y) * gap, 8, 0x88FFFFFF);
		// Right top
		g.draw_line_world(18, 9,
			r_x, r_y,
			r_x + (rt_x - r_x) * gap, r_y + (rt_y - r_y) * gap, 8, 0x88FFFFFF);
		// Right bottom
		g.draw_line_world(18, 9,
			r_x, r_y,
			r_x + (rb_x - r_x) * gap, r_y + (rb_y - r_y) * gap, 8, 0x88FFFFFF);
		
		const uint colour2 = (uint(0x88 * (1 - t)) << 24) + 0xFFFFFF;
		const float p = 0.75;
		g.draw_quad_world(18, 9, false,
			x - rad * p, y,
			x + rad * p, y,
			x + rad * p * s, y - rad * h * p,
			x - rad * p * s, y - rad * h * p,
			colour2, colour2, colour2, colour2);
		g.draw_quad_world(18, 9, false,
			x - rad * p, y,
			x + rad * p, y,
			x + rad * p * s, y + rad * h * p,
			x - rad * p * s, y + rad * h * p,
			colour2, colour2, colour2, colour2);
	}
	
}

class DeathFx : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	int num_particles = 30;
	float inner_radius = 40;
	float outer_radius = 300;
	float start_size = 15;
	float end_size = 10;
	float life_max = 50;
	float life = life_max;
	float fade_time = 10;
	
	DeathFx()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
	}
	
	void step()
	{
		if(--life == 0)
		{
			g.remove_entity(self.as_entity());
			return;
		}
	}
	
	void draw(float sub_frame)
	{
		const float x = self.x();
		const float y = self.y();
		const float sub_life = (life - 1 * sub_frame);
		const float t = sub_life / life_max;
		const float size = start_size + (end_size - start_size) * (1 - t);
		const float radius = inner_radius + (outer_radius - inner_radius) * (1 - t);
		
		uint colour = uint(min(1, sub_life / fade_time) * 0xFF) << 24 | 0xFFFFFF;
		
		for(int i = 0; i < num_particles; i++)
		{
			float angle = (i / 30.0) * PI2;
			rect(g,
				x + cos(angle) * radius, y + sin(angle) * radius,
				size, size,
				int(abs(sub_life * 0.25)) % 2 == 0 ? 0 : PI * 0.25,
				19, 24, 2, colour);
		}
	}
	
}

class SpawnFx : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	array<float> particles(15 * 4);
	float speed = 500;
	float start_size = 10;
	float end_size = 1;
	float life_max = 40;
	float life = life_max;
	float fade_time = 10;
	
	SpawnFx()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		
		for(uint i = 0; i < particles.length(); i += 5)
		{
			const float p_speed = speed + (frand() - 0.5) * speed * 0.75;
			const float angle = frand() * PI2;
			const float dx = cos(angle);
			const float dy = sin(angle);
			const float d = rand() % 20;
			particles[i] = dx * d;
			particles[i + 1] = dy * d;
			particles[i + 2] = dx * p_speed * DT;
			particles[i + 3] = dy * p_speed * DT;
			particles[i + 4] = frand();
		}
	}
	
	void step()
	{
		for(uint i = 0; i < particles.length(); i += 5)
		{
			particles[i] += particles[i + 2];
			particles[i + 1] += particles[i + 3];
		}
		
		if(--life == 0)
		{
			g.remove_entity(self.as_entity());
			return;
		}
	}
	
	void draw(float sub_frame)
	{
		const float x = self.x();
		const float y = self.y();
		const float sub_life = (life - 1 * sub_frame);
		const float t = sub_life / life_max;
		const uint colour = uint(min(1, sub_life / fade_time) * 0xBB) << 24 | 0xFFFFFF;

		for(uint i = 0; i < particles.length(); i += 5)
		{
			float phase = ((1 - t) + particles[i + 4]) * 2;
			phase = phase - floor(phase);
			const float size = start_size + (end_size - start_size) * phase;
			const float px = x + particles[i];
			const float py = y + particles[i + 1];
			g.draw_rectangle_world(19, 24,
				px - size, py - size, px + size, py + size,
				45, colour);
		}
		
	}
	
}

class FastFallFx : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	float life_max = 24;
	float expansion = 0.3;
	
	float size = 1;
	uint colour = 0;
	float life = life_max;
	
	FastFallFx()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
	}
	
	void step()
	{
		if(--life == 0)
		{
			g.remove_entity(self.as_entity());
			return;
		}
	}
	
	void draw(float sub_frame)
	{
		const bool alt = life < life_max * 0.5;
		const float x = self.x();
		const float y = self.y();
		const float sub_life = (life - 1 * sub_frame);
		const float t = alt ? (sub_life) / (life_max * 0.5) : sub_life / (life_max * 0.5);
		const float thickness = 1.5;
		
		const float size = 10 * (1 - t);
		
		float angle = alt ? 0 : PI * 0.25;
		for(float i = 0; i < 4; i++)
		{
			const float cx = x + cos(angle) * size;
			const float cy = y + sin(angle) * size;
			
			g.draw_rectangle_world(18, 9, cx - size * 0.5, cy - thickness, cx + size * 0.5, cy + thickness, angle * RAD2DEG, 0xFFFFFFFF);
			
			angle += PI * 0.5;
		}
	}
	
}