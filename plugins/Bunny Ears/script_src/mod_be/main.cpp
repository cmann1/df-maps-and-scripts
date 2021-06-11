#include '../lib/embed_utils.cpp'
#include '../lib/std.cpp'
#include '../lib/phys/springs/SpringSystem.cpp'
#include 'embed_sprites.cpp'
#include 'Bunny.cpp'
#include 'EasterEgg.cpp'

const float EAR_SPACING = 4;
const float EAR_BASE_ANGLE_MIN = -10;
const float EAR_BASE_ANGLE_MAX = -5;
const float EAR_SEGMENT_ANGLE_MIN = -25;
const float EAR_SEGMENT_ANGLE_MAX = -20;
const float EAR_STIFFNESS = 1;
const float EAR_DAMPING = 0.5;
const float EAR_ANGLE_STIFFNESS = 0.075;
const float EAR_ANGLE_DAMPING = 0.01;
const float EAR_BASE_ANGLE_STIFFNESS = 0.25;
const float EAR_BASE_ANGLE_DAMPING = 0.1;
const float EAR_LENGTH = 35;
const int NUM_EAR_JOINTS = 4;

const int BASE_WIDTH = 6;
const int END_WIDTH = 12;
const int TIP_WIDTH = 4;
const int TIP_HEIGHT = 4;
const uint LIGHT_CLR = 0xffd9d9d9;
const uint DARK_CLR = 0xff8b8b8b;

class script
{
	
	scene@ g;
	camera@ cam;
	fog_setting@ fog;
	
	SpringSystem spring_system;
	
	int num_bunnies;
	array<Bunny> bunnies;
	
	bool needs_init = true;
	
	array<float> easter_egg_queue(50);
	uint easter_egg_queue_size;
	
	script()
	{
		@g = get_scene();
		num_bunnies = num_cameras();
		bunnies.resize(num_bunnies);
		
		@cam = get_active_camera();
		@fog = cam.get_fog();
		
		for(int i = 0; i < num_bunnies; i++)
		{
			bunnies[i].init(i, @spring_system);
		}
	}
	
	void build_sprites(message@ msg)
	{
		embed_build_sprites(msg);
	}
	
	void entity_on_add(entity@ e)
	{
		const string name = e.type_name();
		//puts('on_add', name);
		
		if(
			name == 'entity_cleansed' ||
			name == 'entity_cleansed_full' ||
			name == 'entity_cleansed_walk')
		{
			g.remove_entity(e);
			add_easter_egg(e);
		}
	}
	
	void entity_on_remove(entity@ e)
	{
		const string name = e.type_name();
		//puts('on_remove', name);
		
		if(
			name == 'enemy_tutorial_square' ||
			name == 'enemy_tutorial_hexagon' ||
			name == 'enemy_trash_ball')
		{
			add_easter_egg(e);
		}
	}
	
	void add_easter_egg(entity@ e)
	{
		if(easter_egg_queue_size >= easter_egg_queue.length)
		{
			easter_egg_queue.resize(easter_egg_queue.length + 25);
		}
		
		rectangle@ r = e.base_rectangle();
		easter_egg_queue[easter_egg_queue_size++] = e.x() + (r.left() + r.right()) * 0.5;
		easter_egg_queue[easter_egg_queue_size++] = e.y() + (r.top() + r.bottom()) * 0.5;
	}
	
	void checkpoint_load()
	{
		for(int i = 0; i < num_bunnies; i++)
		{
			bunnies[i].checkpoint_load();
		}
		
		update_apple();
	}
	
	void update_apple()
	{
		entity@ apple = create_entity('hittable_apple');
		sprites@ spr = apple.get_sprites();
		spr.add_sprite_set('script');
		apple.set_sprites(spr);
	}
	
	void step(int entities)
	{
		if(needs_init)
		{
			update_apple();
			needs_init = false;
		}
		
		for(int i = 0; i < num_bunnies; i++)
		{
			bunnies[i].step();
		}
		
		if(easter_egg_queue_size > 0)
		{
			for(uint i = 0; i < easter_egg_queue_size; i += 2)
			{
				EasterEgg@ egg = EasterEgg();
				scriptenemy@ e = create_scriptenemy(@egg);
				e.set_xy(easter_egg_queue[i], easter_egg_queue[i + 1]);
				g.add_entity(e.as_entity(), false);
			}
			
			easter_egg_queue_size = 0;
		}
	}
	
	void step_post(int entities)
	{
		@cam = get_active_camera();
		@fog = cam.get_fog();
		
		for(int i = 0; i < num_bunnies; i++)
		{
			bunnies[i].step_post(fog);
		}
	}
	
	void draw(float sub_frame)
	{
		for(int i = 0; i < num_bunnies; i++)
		{
			bunnies[i].draw(sub_frame);
		}
	}
	
}
