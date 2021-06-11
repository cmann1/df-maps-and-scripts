#include "../common/Sprite.cpp"

class Clouds
{
	
	scene@ g;
	int cloud_count = 40;
	array<Cloud> clouds(cloud_count);
	
	float min_x = -30000;
	float max_x =  80000;
	float min_y = -16000;
	float max_y =  16000;
	
	Clouds()
	{
		@g = get_scene();
		
		for(int i = 0; i < cloud_count; i++)
		{
			Cloud@ cloud = clouds[i];
			cloud.init(min_x, max_x, min_y, max_y);
		}
	}
	
	void step()
	{
		for(int i = 0; i < cloud_count; i++)
		{
			clouds[i].step(min_x, max_x, min_y, max_y);
		}
	}
	
	void draw(float sub_frame)
	{
		for(int i = 0; i < cloud_count; i++)
		{
			clouds[i].draw(g, sub_frame);
		}
	}
	
}

class Cloud
{
	
	float min_speed = 150;
	float max_speed = 550;
	float prev_x;
	float x;
	float y;
	float speed;
	int sub_layer;
	Sprite spr("props1", "backdrops_" + ((rand() % 3) + 1));
	
	void init(float min_x, float max_x, float min_y, float max_y)
	{
		prev_x = x = min_x + (max_x - min_x) * frand();
		y = min_y + (max_y - min_y) * frand();
		speed = min_speed + (max_speed - min_speed) * frand();
		sub_layer = 19 + rand() % 2;
	}
	
	void reset(float min_x, float max_x, float min_y, float max_y)
	{
		prev_x = x = min_x - (max_x - min_x) * frand() * 0.25;
		y = min_y + (max_y - min_y) * frand();
		speed = min_speed + (max_speed - min_speed) * frand();
	}
	
	void step(float min_x, float max_x, float min_y, float max_y)
	{
		prev_x = x;
		x += speed * DT;
		
		if(x > max_x)
		{
			reset(min_x, max_x, min_y, max_y);
		}
	}
	
	void draw(scene@ g, float sub_frame)
	{
		spr.draw_world(1, sub_layer, 0, 0, lerp(prev_x, x, sub_frame), y);
	}
	
}