#include "../common/math.cpp"
#include "../common/ColType.cpp"
#include "../common/Sprite.cpp"

class Pokerball : enemy_base, callback_base
{
	scene@ g;
	script@ script;
	scriptenemy@ self;
	
	uint id;
	int life = 1500;
	float radius = 18;
	float density = 1;
	float inv_mass;
	bool requires_init = true;
	
	float x;
	float y;
	float rotation;
	
	float vel_x;
	float vel_y;
	float rotation_vel = 0;
	
	Circle@ shape = Circle(0, 0, radius);
	
	sprites@ sprite;
	Sprite@ outer_sprite;
	Sprite@ line_sprite;

	Pokerball(uint id, float x=0, float y=0, float vel_x=0, float vel_y=0)
	{
		@g = get_scene();
		this.id = id;
		this.x = x;
		this.y = y;
		this.vel_x = vel_x;
		this.vel_y = vel_y;
		
		@outer_sprite = Sprite("props4", "machinery_18");
		@line_sprite = Sprite("props4", "machinery_6");
		
		rotation_vel = (frand() * 2 - 1) * 15;
		rotation = rand() % 360;
		
		float f = frand();
		if(f > 0.75)
		{
			radius += (rand() % 60);
			shape.radius = radius;
		}
		
		inv_mass = 1 / (PI * (radius * radius) * density);
	}

	void init(script@ script, scriptenemy@ self)
	{
		@this.script = @script;
		@this.self = @self;
		
		self.auto_physics(false);
		self.as_controllable().on_hurt_callback(this, "on_hurt", 0);
	}
	
	void add_to_scene()
	{
		if(!requires_init) return;
		
		self.base_rectangle(-radius, radius, -radius, radius);
		self.hit_rectangle(-radius, radius, -radius, radius);
		
		requires_init = false;
	}
	
	void on_hurt(controllable@ self, controllable@ other, hitbox@ hb, int arg)
	{
		float angle = hb.attack_dir() / 180.0 * PI;
		vel_x = sin(angle) * hb.attack_strength() * 800 * inv_mass;
		vel_y = -cos(angle) * hb.attack_strength() * 800 * inv_mass;
	}

	void step()
	{
		x += vel_x * DT;
		y += vel_y * DT;
		shape.x = x;
		shape.y = y;
		
		int x1 = int(floor((x - radius) / 48));
		int y1 = int(floor((y - radius) / 48));
		int x2 = int(floor((x + radius) / 48));
		int y2 = int(floor((y + radius) / 48));
		CollisionResponse response;
		float penetration = 0;
		float collision_nx = 0;
		float collision_ny = 0;
		int collision_count = 0;
		float gravity_factor = 1;
		
		for(int tile_x = x1; tile_x <= x2; tile_x++)
		{
			for(int tile_y = y1; tile_y <= y2; tile_y++)
			{
				response.reset();
				bool col = test_tile_circle(g, tile_x, tile_y, shape, @response);
				if(col)
				{
					// Hack: Ignore collisions pointing in the same direction this ball is moving or collisions that are too large
					float d = dot(vel_x, vel_y, response.overlapN_x, response.overlapN_y);
					if(d <= 0 and response.overlap <= radius * 2)
					{
						// Hack: Give collisions with an edge's vertices less priority than collisions with an edge's face
						//       Gives better results when colliding with corners of tiles
						float t = response.type * 4 + 0.5;
						collision_nx += response.overlapN_x * response.overlap * t;
						collision_ny += response.overlapN_y * response.overlap * t;
						penetration += response.overlap;
						collision_count++;
					}
					
					const float w = 4;
					const int frames = 0;
					const int layer = 20;
					const int sub_layer = 20;
					const float nl = response.overlap > 48 ? response.overlap : 48;
					script.debug.line(shape.x, shape.y, shape.x + response.overlapN_x * nl, shape.y + response.overlapN_y * nl, layer, sub_layer, frames, 2, 0x99FFFFFF);
					script.debug.line(shape.x, shape.y, shape.x + response.overlapN_x * response.overlap, shape.y + response.overlapN_y * response.overlap, layer, sub_layer + 1, frames, 1, 0x99000000);
					script.debug.line(response.x1, response.y1, response.x2, response.y2, layer, sub_layer - 1, frames, 1, 0xFF0000FF);
				}
				
				script.debug.outline_tile(tile_x, tile_y, 19, 19, 0, 1, col ? 0xFF00FFFF : 0xAAFF0000);
			}
		}
		
		if(collision_count > 0 and (collision_nx != 0 or collision_ny != 0))
		{
			collision_nx /= collision_count;
			collision_ny /= collision_count;
			normalize(collision_nx, collision_ny, collision_nx, collision_ny);
			penetration /= collision_count;
			
			x += collision_nx * penetration;
			y += collision_ny * penetration;
			
			// Hack: Applying less gravity when resting on a surface helps jittering when a ball is stuck between two surfaces
			gravity_factor = dot(collision_nx, collision_ny, -GRAVITY_NX, -GRAVITY_NY);
			if(gravity_factor < 0) gravity_factor = 0;
			
			const float w = 4;
			const int frames = 0;
			const int layer = 20;
			const int sub_layer = 20;
			const float nl = penetration > 48 ? penetration : 48;
			script.debug.line(shape.x, shape.y, shape.x + collision_nx * nl, shape.y + collision_ny * nl, layer, sub_layer, frames, 2, 0xFFFFFFFF);
			script.debug.line(shape.x, shape.y, shape.x + collision_nx * penetration, shape.y + collision_ny * penetration, layer, sub_layer + 1, frames, 1, 0xFF000000);
			
			// Fake some angular velocity
			rotation_vel = -dot(vel_x, vel_y, collision_ny, -collision_nx) * 0.02;
			
			// Calculate bounce velocity
			const float vn_nn = dot(vel_x, vel_y, collision_nx, collision_ny) / dot(collision_nx, collision_ny, collision_nx, collision_ny);
			const float ux = vn_nn * collision_nx;
			const float uy = vn_nn * collision_ny;
			const float wx = vel_x - ux;
			const float wy = vel_y - uy;
			
			vel_x = FRICTION * wx - RESTITUTION * ux;
			vel_y = FRICTION * wy - RESTITUTION * uy;
			
			rotation_vel *= FRICTION;
		}
		
		if(BALL_BALL_COLLISION)
		{
			int count = g.get_entity_collision(y - radius, y + radius, x - radius, x + radius, COL_TYPE_ENEMY);
			for(int i = 0; i < count; i++)
			{
				entity@ e = g.get_entity_collision_index(uint(i));
				scriptenemy@ s = e.as_scriptenemy();
				Pokerball@ ball = @s != null ? cast<Pokerball>(s.get_object()) : null;
				if(@ball != null and @ball != @this)
				{
					
					// get the mtd
					float delta_x = x - ball.x;
					float delta_y = y - ball.y;
					float d = magnitude(delta_x, delta_y);
					if(d == 0)
					{
						delta_y = 1;
						d = 1;
					}
					
					if(d > this.radius + ball.radius) continue;
					
					// minimum translation distance to push balls apart after intersecting
					float f = (radius + ball.radius - d) / d;
					if(f == 0) f = 0.1;
					float mtd_x = delta_x * f;
					float mtd_y = delta_y * f;
					float mtd_nx;
					float mtd_ny;
					normalize(mtd_x, mtd_y, mtd_nx, mtd_ny);
					
					// impact speed
					float v_x = vel_x - ball.vel_x;
					float v_y = vel_y - ball.vel_y;
					float vn = dot(v_x, v_y, mtd_nx, mtd_ny);
					
					// sphere intersecting but moving away from each other already
					if (vn > 0.0) continue;

					// push-pull them apart based off their mass
					float im1 = inv_mass / (inv_mass + ball.inv_mass);
					float im2 = ball.inv_mass / (inv_mass + ball.inv_mass);
					x += mtd_x * im1;
					y += mtd_y * im1;
					ball.x -= mtd_x * im2;
					ball.y -= mtd_y * im2;
					
					// collision impulse
					float i_f = (-(1.0f + RESTITUTION) * vn) / (inv_mass + ball.inv_mass);
					float impulse_x = mtd_nx * i_f;
					float impulse_y = mtd_ny * i_f;
					
					// Fake some angular velocity
					rotation_vel = -dot(vel_x, vel_y, mtd_ny, -mtd_nx) * 0.02;
					
					// change in momentum
					vel_x += impulse_x * inv_mass;
					vel_y += impulse_y * inv_mass;
					ball.vel_x -= impulse_x * ball.inv_mass;
					ball.vel_y -= impulse_y * ball.inv_mass;
					
					rotation_vel *= FRICTION;
				}
			}
		}
		
		vel_x += GRAVITY_X * gravity_factor;
		vel_y += GRAVITY_Y * gravity_factor;
		
		vel_x *= DRAG;
		vel_y *= DRAG;
		
		rotation += rotation_vel;
		
		self.x(x);
		self.y(y);
		
		if(--life <= 0)
		{
			g.remove_entity(self.as_entity());
		}
	}
	
	void draw(float sub_frame){
		const int layer = 18;
		const int sub_layer = 19;
		float scale = 0.8 * (radius / 18);
		
		outer_sprite.draw_world(layer, sub_layer,
				0, 0, x, y, rotation,
				scale, scale, 0xFFFF0000);
		
		outer_sprite.draw_world(layer, sub_layer,
				0, 0, x, y, rotation,
				scale * 0.4, scale * 0.4, 0xFF000000);
		
		line_sprite.draw_world(layer, sub_layer,
				0, 0, x, y, rotation,
				scale * 0.32, scale * 0.32, 0xFF000000);
		
		outer_sprite.draw_world(layer, sub_layer,
				0, 0, x, y, rotation,
				scale * 0.33, scale * 0.33, 0xFFFFFFFF);
	}
	
}