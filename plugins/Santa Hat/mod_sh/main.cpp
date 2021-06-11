#include '../common-old/utils.cpp'
#include 'embeds.cpp'
#include 'SpringSystem.cpp'
#include 'Hat.cpp'
#include '../common-old/Debug.cpp'

class script
{
	
	scene@ g;
	camera@ cam;
	fog_setting@ fog;
	
	array<Hat@> hats;
	int num_hats = 0;
	
	script()
	{
		@g = get_scene();
		//g.time_warp(1);
		
		num_hats = num_cameras();
		hats.resize(num_hats);
		
		for(int i = 0; i < num_hats; i++)
		{
			@hats[i] = Hat(i);
			@hats[i].s = this;
		}
	}
	
	void build_sprites(message@ msg)
	{
		embed_build_sprites(msg);
	}
	
	void checkpoint_load()
	{
		for(int i = 0; i < num_hats; i++)
		{
			@hats[i].player = null;
			@hats[i].dm = null;
		}
	}
	
	void step(int entities)
	{
		@cam = get_active_camera();
		@fog = cam.get_fog();
		
		for(int i = 0; i < num_hats; i++)
		{
			hats[i].step();
		}
	}
	
	void step_post(int entities)
	{
		for(int i = 0; i < num_hats; i++)
		{
			hats[i].step_post();
		}
	}
	
	void draw(float sub_frame)
	{
		for(int i = 0; i < num_hats; i++)
		{
			hats[i].draw(sub_frame);
		}
	}
	
}
