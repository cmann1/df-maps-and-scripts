#include "../common/Fx.cpp"
#include "../common/utils.cpp"
#include "../common/drawing_utils.cpp"
#include "../common/SAT.cpp"
#include "../common/Debug.cpp"

#include "../common/TilePolygons.cpp"
#include "Pokerball.cpp"

const float THROW_SPEED = 1400;
const float THROW_DIR_RANGE = 5;

const float GRAVITY_X = 0;
const float GRAVITY_Y = 30;
const float GRAVITY_NX = 0;
const float GRAVITY_NY = 1;

const float DRAG = 0.99;
const float FRICTION = 0.99;
const float RESTITUTION = 0.5;

const float DT = 1.0 / 60;

const bool BALL_BALL_COLLISION = true;

class script
{
//	void on_hurt(controllable@ a, controllable@ b, hitbox@ hb, int arg)
//	{
//		Pokerball@ ball = cast<Pokerball>(a.as_scriptenemy().get_object());
//		
//		float angle = hb.attack_dir() / 180.0 * PI;
//		ball.vel_x = sin(angle) * hb.attack_strength() * hb.damage() * 800 * ball.inv_mass;
//		ball.vel_y = -cos(angle) * hb.attack_strength() * hb.damage() * 800 * ball.inv_mass;
//	}
	
	scene@ g;
	controllable@ player;
	dustman@ dm;
	
	uint pokerball_id = 1;
	Debug@ debug;
	HelpBox@ help;
	
	script()
	{
		@g = get_scene();
		@debug = Debug(g);
		@help = HelpBox("Throw pokerball: Taunt + Directions");
		help.x = -790;
		help.y = -440;
	}
	
	void checkpoint_save()
	{
		
	}

	void checkpoint_load()
	{
		@player = null;
		@dm = null;
	}
	
	void entity_on_add(entity@ e)
	{
		// Do this because init is called before the entity is added to the scene
		scriptenemy@ s = e.as_scriptenemy();
		Pokerball@ ball = @s != null ? cast<Pokerball>(s.get_object()) : null;
		if(@ball != null)
		{
			ball.add_to_scene();
		}
	}
	
	void step(int entities)
	{
		if(@player == null)
		{
			entity@e = controller_entity(0);
			if(@e != null)
			{
				@player = e.as_controllable();
				@dm = e.as_dustman();
			}
		}
		else
		{
			if(player.taunt_intent() != 0)
			{
				float throw_dir = 0;
				// TODO: Uncomment randomness
				
				const int face_y = player.y_intent();
				const int face = player.x_intent() == 0 ? player.face() : player.x_intent();
				const float throw_speed = THROW_SPEED + frand() * (THROW_SPEED * 0.5);
//				const float throw_speed = THROW_SPEED;
				float vel_x;
				float vel_y;
				
				if(face_y == -1)
				{
					throw_dir = 45;
				}
				else if(face_y == 1)
				{
					throw_dir = 135;
				}
				else
				{
					throw_dir = 80;
				}
				
				throw_dir += -THROW_DIR_RANGE + frand() * (THROW_DIR_RANGE * 2);
				throw_dir *= face;
				
				vel_x = sin(throw_dir / 180 * PI) * throw_speed + player.x_speed();
				vel_y = -cos(throw_dir / 180 * PI) * throw_speed + player.y_speed();
				
				Pokerball@ pokerball = Pokerball(pokerball_id++, player.x(), player.y() - 60, vel_x, vel_y);
				scriptenemy@ pokerball_enemy = create_scriptenemy(@pokerball);
				pokerball_enemy.x(100);
				pokerball_enemy.y(100);
				g.add_entity(pokerball_enemy.as_entity(), true);
			}
			
			player.taunt_intent(0);
		}
		
		debug.step();
	}
	
	void draw(float sub_frame)
	{
		debug.draw();
		help.draw(g);
	}
	
//	void step_post(int entities) {}
//	void entity_on_add(entity@ e) {}
//	void entity_on_remove(entity@ e) {}
//	void move_cameras() {}
	
}