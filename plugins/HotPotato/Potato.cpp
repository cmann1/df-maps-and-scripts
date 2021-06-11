#include "../common/SAT.cpp"
#include "../common/sprite_group.cpp"
#include "../common/Sprite.cpp"
#include "../common/Fx.cpp"

class Potato : enemy_base, callback_base
{
	scene@ g;
	script@ script;
	scriptenemy@ self;
	controllable@ e;
	
	Circle@ shape = Circle(0, 0, radius);
	Sprite sprite("beachball", "beachball", 0.5, 0.5);
	float sprite_scale = 1;
	
	Player@ player = null;
	
	bool _in_world = false;
	bool is_dead = false;
	float max_life = 220;
	float life = max_life;
	
	[text] float radius = 30;
	[text] float density = 0.1;
	[text] float attack_force = 350;
	[hidden] float _inv_mass;
	
	[hidden] float x = 0;
	[hidden] float y = 0;
	[hidden] float rotation = 0;
	
	[hidden] float vel_x = 0;
	[hidden] float vel_y = 0;
	[hidden] float rotation_vel = 0;
	
	[text] float gravity_x = 0;
	[text] float gravity_y = 15;
	[hidden] float _gravity_nx = 0;
	[hidden] float _gravity_ny = 1;

	[text] float drag = 0.999;
	[text] float friction = 0.999;
	[text] float restitution = 0.6;
	
	Potato()
	{
		@g = get_scene();
	}
	
	Potato(float x, float y)
	{
		@g = get_scene();
		
		this.x = x;
		this.y = y;
	}
	
	bool pickup(Player@ player)
	{
		if(@this.player != @player)
		{
			@this.player = player;
			
			life = max_life;
			
			vel_x = 0;
			vel_y = 0;
			rotation_vel = 0;
			rotation = 0;
			_in_world = false;
			g.remove_entity(self.as_entity());
			
			return true;
		}
		
		return false;
	}
	
	void drop(float x, float y, float vel_x=0, float vel_y=0)
	{
		self.x(this.x = x);
		self.y(this.y = y);
		
		this.vel_x = vel_x;
		this.vel_y = vel_y;
		
		g.add_entity(self.as_entity());
	}
	
	void init(script@ script, scriptenemy@ self)
	{
		@this.script = @script;
		@this.self = @self;
		@e = self.as_controllable();
		
		self.auto_physics(false);
		self.as_controllable().on_hurt_callback(this, "on_hurt", 0);
//		self.team(2);
		
		float r = radius;
		self.base_rectangle(-r, r, -r, r);
		self.hit_rectangle(-r, r, -r, r);
		shape.radius = radius;
		sprite_scale = radius / 53;
		
		normalize(gravity_x, gravity_y, _gravity_nx, _gravity_ny);
		_inv_mass = 1 / (PI * (radius * radius) * density);
		
		self.x(x);
		self.y(y);
	}
	
	void on_add()
	{
		_in_world = true;
		is_dead = false;
		x = self.x();
		y = self.y();
	}
	
	void on_remove()
	{
		if(_in_world)
		{
			_in_world = false;
			broadcast_message("ball_dead", create_message());
			
			is_dead = true;
			spawn_fx(x, y, "dustkid", "dkcleanse", 0, 20, rand() % 360, 1, 1, 0xDDFFBBBB);
			audio@ sfx = g.play_sound("sfx_door_land", x, y, 1, false, true);
			if(@sfx != null)
			{
				sfx.time_scale(1.5);
			}
		}
	}
	
	void on_hurt(controllable@ self, controllable@ other, hitbox@ hb, int arg)
	{
		float angle = hb.attack_dir() * DEG2RAD;
		vel_x = sin(angle) * hb.attack_strength() * attack_force * _inv_mass + other.x_speed();
		vel_y = -cos(angle) * hb.attack_strength() * attack_force * _inv_mass + other.y_speed();
	}
	
	void step()
	{
		x += vel_x * DT;
		y += vel_y * DT;
		shape.x = x;
		shape.y = y;
		
		float penetration = 0;
		float collision_nx = 0;
		float collision_ny = 0;
		int collision_count = 0;
		
		CollisionResponse response;
		float gravity_factor = 1;
		
		int x1 = int(floor((x - radius) * PIXEL2TILE));
		int y1 = int(floor((y - radius) * PIXEL2TILE));
		int x2 = int(floor((x + radius) * PIXEL2TILE));
		int y2 = int(floor((y + radius) * PIXEL2TILE));
		
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
			gravity_factor = dot(collision_nx, collision_ny, -_gravity_nx, -_gravity_ny);
			if(gravity_factor < 0) gravity_factor = 0;
			
			// Fake some angular velocity
			rotation_vel = -dot(vel_x, vel_y, collision_ny, -collision_nx) * 0.02;
			
			float speed = magnitude(vel_x, vel_y);
			float vnx = speed == 0.0 ? 1.0 : vel_x / speed;
			float vny = speed == 0.0 ? 0.0 : vel_y / speed;
			float impact = floor(penetration * 5) * 0.1;
			//float impact = round((vel_x * vel_x + vel_y * vel_y) * 0.00001) * 0.1;
			if(impact > 0)
			{
//				g.play_sound("sfx_barrel_land", x, y, impact, false, true);
				g.play_sound("sfx_bear_footstep", x, y, impact, false, true);
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
		}
	}
	
	void draw(float sub_frame)
	{
		sprite.draw_world(18, 11, 0, 0,
			x + vel_x * (sub_frame / 60),
			y + vel_y * (sub_frame / 60),
			rotation + (rotation_vel / DT) * (sub_frame / 60), 1, 1);
	}
	
	void editor_draw(float sub_frame)
	{
		x = self.x();
		y = self.y();
		draw(sub_frame);
	}
	
}