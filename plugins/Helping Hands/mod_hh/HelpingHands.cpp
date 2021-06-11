#include '../common/States.cpp'
#include '../common/sprite_group.cpp'
#include 'Arm.cpp'

class HelpingHands
{
	
	scene@ g;
	controllable@ player = null;
	dustman@ dm = null;
	string character = '';
	
	sprite_group body_spr;
	
	Arm arm_left(-1);
	Arm arm_right(1);
	
	private float ox = 0;
	private float oy = -58;
	private float arm_x = 18;
	private float arm_y = -2;
	
	float prev_x = 0;
	float prev_y = 0;
	float x = 0;
	float y = 0;
	
	bool requires_init = true;
	
	HelpingHands()
	{
		@g = get_scene();
	}
	
	void update_player(controllable@ player)
	{
		@this.player = player;
		@dm = player is null ? null : player.as_dustman();
		character = dm is null ? '' : dm.character();
		
		x = prev_x = player.x() + ox;
		y = prev_y = player.y() + oy;
		
		float lx = x - arm_x;
		float ly = y - arm_y;
		float rx = x + arm_x;
		float ry = y - arm_y;
		arm_left.init(player, dm, lx, ly);
		arm_right.init(player, dm, rx, ry);
		
		if(requires_init)
		{
			int sl = 9;
			
			uint colour = dm is null or dm.character().substr(0, 1) != 'v' ? 0xFFFFFFFF : 0x99FFFFFF;
			
			body_spr.add_sprite('props4', 'machinery_11', 0.5, 0.5, 14.3546, -20.271, 90, 0.141421, -0.141421, colour, 0, 0, 18, sl - 2);
			body_spr.add_sprite('props4', 'machinery_11', 0.5, 0.5, -14.3546, -20.271, 90, 0.141421, 0.141421, colour, 0, 0, 18, sl - 2);
			body_spr.add_sprite('props4', 'machinery_18', 0.5, 0.5, -0.0527802, -23.2413, 0, 0.37606, 0.37606, colour, 0, 0, 18, sl - 1);
			body_spr.add_sprite('props4', 'machinery_10', 0.5, 0.5, 1.74029, 12.1694, 270, 0.195928, 0.195928, colour, 0, 0, 18, sl - 1);
			body_spr.add_sprite('props4', 'machinery_9', 0.5, 0.5, -15.7046, -1.19485, 90, 0.521001, 0.521001, colour, 0, 1, 18, sl);
			body_spr.add_sprite('props4', 'machinery_9', 0.5, 0.5, 17.2954, -1.19485, 90, 0.521001, 0.521001, colour, 0, 1, 18, sl);
			body_spr.add_sprite('props4', 'machinery_2', 0.5, 0.5, -7.39025, 0.431606, 90, 0.271442, -0.271442, colour, 0, 1, 18, sl);
			body_spr.add_sprite('props4', 'machinery_2', 0.5, 0.5, 9.39025, 0.431599, 90, 0.271442, 0.271442, colour, 0, 1, 18, sl);
			
			requires_init = false;
		}
	}
	
	void reset()
	{
		arm_left.reset();
		arm_right.reset();
	}
	
	void step()
	{
		prev_x = x;
		prev_y = y;
		x = player.x() + ox;
		y = player.y() + oy;
		
//		if(character == 'dustkid') y += 10;
//		const int state = player.state();
//		if(state == ST_ROOF_GRAB or state == ST_ROOF_GRAB_IDLE or state == ST_ROOF_RUN)
//		{
//			const float ox = player.draw_offset_x();
//			if(ox != 0)
//			{
//				x += 16 * sgn(ox);
//			}
			
//			y -= 24;
//		}
		
		float lx = x - arm_x;
		float ly = y - arm_y;
		float rx = x + arm_x;
		float ry = y - arm_y;
		arm_left.step(lx, ly);
		arm_right.step(rx, ry);
	}
	
	void draw(float sub_frame)
	{
		float px = lerp(prev_x, x, sub_frame);
		float py = lerp(prev_y, y, sub_frame);
		
		arm_left.draw(sub_frame);
		arm_right.draw(sub_frame);
		body_spr.draw(19, 19, px, py, 0, player.scale());
		
//		g.draw_rectangle_world(21, 21, px-6,py-6,px+6,py+6, 0, 0xFFFF0000);
		
//		py -= 2;
//		px -= 18;
//		g.draw_rectangle_world(21, 21, px-3,py-3,px+3,py+3, 0, 0xFFFFFFFF);
//		px += 18 * 2;
//		g.draw_rectangle_world(21, 21, px-3,py-3,px+3,py+3, 0, 0xFFFFFFFF);
		
//		px+=player.draw_offset_x();
//		py+=player.draw_offset_y();
//		g.draw_rectangle_world(21, 21, px-6,py-6,px+6,py+6, 0, 0xFF0000FF);
		
		
	}
	
}