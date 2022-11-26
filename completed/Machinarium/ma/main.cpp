#include '../lib/drawing/common.cpp';

#include 'Tentacle.cpp';

class script
{
	
	scene@ g;
	camera@ cam;
	bool is_playing;
	
	float view_x1, view_y1;
	float view_x2, view_y2;
	float time_scale = 1;
	
	bool large_prism_removed;
	array<entity@> filth_balls;
	int filth_ball_count;
	
	script()
	{
		@g = get_scene();
		@cam = get_active_camera();
	}
	
	void on_level_start()
	{
		is_playing = true;
	}
	
	void entity_on_add(entity@ e)
	{
		if(large_prism_removed && e.type_name() == 'filth_ball')
		{
			filth_balls.insertLast(e);
			filth_ball_count++;
			//g.remove_entity(e);
			large_prism_removed = false;
		}
	}

	void entity_on_remove(entity@ e)
	{
		if(e.type_name() == 'enemy_tutorial_hexagon')
		{
			large_prism_removed = true;
		}
	}
	
	void step(int)
	{
		init_cam();
		
		if(filth_ball_count > 0)
		{
			for(int i = 0; i < filth_ball_count; i++)
			{
				g.remove_entity(filth_balls[i]);
			}
			
			filth_balls.resize(0);
			filth_ball_count = 0;
		}
	}
	
	void editor_step()
	{
		init_cam();
	}
	
	//void draw(float)
	//{
	//	outline_rect(g, 22, 22, view_x1, view_y1, view_x2, view_y2, 1, 0xffff0000);
	//}
	//
	//void editor_draw(float)
	//{
	//	outline_rect(g, 22, 22, view_x1, view_y1, view_x2, view_y2, 1, 0xffff0000);
	//}
	
	void init_cam()
	{
		@cam = get_active_camera();
		float left1, top1, width1, height1;
		float left2, top2, width2, height2;
		cam.get_layer_draw_rect(0, 19, left1, top1, width1, height1);
		cam.get_layer_draw_rect(1, 19, left2, top2, width2, height2);
		view_x1 = min(left1, left2);
		view_y1 = min(top1, top2);
		view_x2 = max(left1 + width1, left2 + width2);
		view_y2 = max(top1 + height1, top2 + height2);
		
		time_scale = g.time_warp();
	}
	
}
