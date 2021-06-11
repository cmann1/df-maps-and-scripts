#include "../common/SAT.cpp"
#include "../common/math.cpp"
#include "../common/sprite_group.cpp"
#include "../common/ColType.cpp"

class Pokerball : enemy_base, callback_base
{
	
	scene@ g;
	script@ script;
	scriptenemy@ self;
	controllable@ self_controllable;
	
	int player_index = 0;
	
	sprite_group@ sprite = null;
	
	int layer = 18;
	int sub_layer = 11;
	
	audio@ hit_sfx = null;
	audio@ bounce_sfx = null;
	
	[text] int life = 300;
	
	[text] float radius = 18;
	[text] float density = 0.5;
	[text] float attack_force = 350;
	[hidden] float inv_mass = -1;
	
	[hidden] float x = 0;
	[hidden] float y = 0;
	[hidden] float rotation = 0;
	
	[hidden] float vel_x = 0;
	[hidden] float vel_y = 0;
	[hidden] float rotation_vel = 0;
	
	[text] float gravity_x = 0;
	[text] float gravity_y = 15;
	[hidden] float gravity_nx = 0;
	[hidden] float gravity_ny = 1;

	[text] float drag = 0.999;
	[text] float friction = 0.99;
	[text] float restitution = 0.5;
	
	Circle@ shape = Circle(0, 0, radius);
	
	Pokerball()
	{
		@g = get_scene();
		
		rotation_vel = (frand() * 2 - 1) * 15;
		rotation = rand() % 360;
	}
	
	void catch_pokerman(entity@ e)
	{
		if(!script.catch_pokerman(player_index, e)) return;
		
		rectangle@ rect = e.as_controllable().collision_rect();
		Fx@ fx = spawn_fx(e.x() + rect.left() + rect.get_width() * 0.5, e.y() + rect.top() + rect.get_height() * 0.5, "dustworth", "docleanse");
		fx.start_frame = fx.frame = 7;
		g.remove_entity(self.as_entity());
	}
	
	void on_add()
	{
		x = self.x();
		y = self.y();
	}
	
	void init(script@ script, scriptenemy@ self)
	{
		@this.script = @script;
		@this.self = @self;
		@this.self_controllable = @self.as_controllable();
		
		self.auto_physics(false);
		self.as_controllable().on_hurt_callback(this, "on_hurt", 0);
		
		self.base_rectangle(-radius, radius, -radius, radius);
		self.hit_rectangle(-radius, radius, -radius, radius);
		
		inv_mass = 1 / (PI * (radius * radius) * density);
		
		x = self.x();
		y = self.y();
		
		if(@sprite == null)
		{
			@sprite = sprite_group();
			
			float scale = 0.8;
			
			// Border
			sprite.add_sprite("props4", "machinery_18",
				0.5, 0.5, 0, 0, 0,
				scale, scale, 0xFFFF0000, 0, 0,
				layer, sub_layer);
			// Inside
			sprite.add_sprite("props4", "machinery_18",
				0.5, 0.5, 0, 0, 0,
				scale * 0.4, scale * 0.4, 0xFF000000, 0, 0,
				layer, sub_layer);
			// Line
			sprite.add_sprite("props4", "machinery_6",
				0.5, 0.5, 0, 0, 0,
				scale * 0.32, scale * 0.32, 0xFF000000, 0, 0,
				layer, sub_layer);
			// Dot
			sprite.add_sprite("props4", "machinery_18",
				0.5, 0.5, 0, 0, 0,
				scale * 0.33, scale * 0.33, 0xFFFFFFFF, 0, 0,
				layer, sub_layer);
		}
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
		
		const int enemy_collisions = g.get_entity_collision(y - radius, y + radius, x - radius, x + radius, COL_TYPE_ENEMY);
		for(int i = 0; i < enemy_collisions; i++)
		{
			entity@ e = g.get_entity_collision_index(i);
			if(e.is_same(self_controllable)) continue;
			
			scriptenemy@ s = e.as_scriptenemy();
			Pokerball@ ball = @s != null ? cast<Pokerball>(s.get_object()) : null;
			if(@ball != null) continue;
			
			catch_pokerman(e);
			
			return;
		}
		
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
				}
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
			gravity_factor = dot(collision_nx, collision_ny, -gravity_nx, -gravity_ny);
			if(gravity_factor < 0) gravity_factor = 0;
			
			// Fake some angular velocity
			rotation_vel = -dot(vel_x, vel_y, collision_ny, -collision_nx) * 0.02;
			
			float speed = magnitude(vel_x, vel_y);
			float vnx = speed == 0.0 ? 1.0 : vel_x / speed;
			float vny = speed == 0.0 ? 0.0 : vel_y / speed;
			float impact = floor(penetration * 5) * 0.1;
//			float impact = round((vel_x * vel_x + vel_y * vel_y) * 0.00001) * 0.1;
			if(impact > 0 and (@bounce_sfx == null or !bounce_sfx.is_playing()))
			{
				const int num = rand() % 3 + 1;
//				g.play_sound("sfx_footstep_stone_" + num, x, y, impact, false, true);
				@bounce_sfx = g.play_sound("sfx_footstep_stone_" + num, x, y, impact, false, true);
			}
			
			// Calculate bounce velocity
			const float vn_nn = dot(vel_x, vel_y, collision_nx, collision_ny) / dot(collision_nx, collision_ny, collision_nx, collision_ny);
			const float ux = vn_nn * collision_nx;
			const float uy = vn_nn * collision_ny;
			const float wx = vel_x - ux;
			const float wy = vel_y - uy;
			
			vel_x = friction * wx - restitution * ux;
			vel_y = friction * wy - restitution * uy;
			
			rotation_vel *= friction;
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
					float i_f = (-(1.0f + restitution) * vn) / (inv_mass + ball.inv_mass);
					float impulse_x = mtd_nx * i_f;
					float impulse_y = mtd_ny * i_f;
					
					// Fake some angular velocity
					rotation_vel = -dot(vel_x, vel_y, mtd_ny, -mtd_nx) * 0.02;
					
					// change in momentum
					vel_x += impulse_x * inv_mass;
					vel_y += impulse_y * inv_mass;
					ball.vel_x -= impulse_x * ball.inv_mass;
					ball.vel_y -= impulse_y * ball.inv_mass;
					
					rotation_vel *= friction;
					
					if(vn < -100 and (@hit_sfx == null or !hit_sfx.is_playing()))
					{
//						const float impact = magnitude(mtd_x, mtd_y) * 0.5;
						const float impact = abs(vn) * 0.007;
						const int num = rand() % 3 + 1;
						@hit_sfx = g.play_sound("sfx_footstep_stone_" + num, x, y, impact, false, true);
					}
				}
			}
		}
		
		vel_x += gravity_x * gravity_factor;
		vel_y += gravity_y * gravity_factor;
		
		vel_x *= drag;
		vel_y *= drag;
		
		rotation += rotation_vel;
		
		self.x(x);
		self.y(y);
		
		if(life-- <= 0)
		{
			g.remove_entity(self.as_entity());
			Fx@ fx = spawn_fx(x, y, "dustkid", "dkcleanse", 0, 15, rand() % 360);
			fx.end_frame = 3.9;
		}
	}
	
	void draw(float sub_frame){
		sprite.draw(layer, sub_layer,
			x + vel_x * (sub_frame / 60),
			y + vel_y * (sub_frame / 60),
			rotation + (rotation_vel / DT) * (sub_frame / 60),
			radius / 18);
	}
	
	void editor_draw(float sub_frame){
		draw(sub_frame);
	}
	
}