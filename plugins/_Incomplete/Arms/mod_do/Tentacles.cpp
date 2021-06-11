#include '../common/States.cpp'
#include '../common/sprite_group.cpp'
#include 'Arm.cpp'

class Tentacles
{
	
	scene@ g;
	int player_index;
	controllable@ player = null;
	dustman@ dm = null;
	camera@ cam;
	string character = '';
	
	sprite_group body_spr;
	
	Arm arm_tl(-1, -4);
	Arm arm_tr( 1, -4);
	Arm arm_bl(-1, 1);
	Arm arm_br( 1, 1);
	array<Arm@> arms(4);
	
	private float ox = 0;
	private float oy = -58;
	private float arm_x = 18;
	private float arm_y = 18;
	
	float move_speed = 3500;
	
	float prev_x = 0;
	float prev_y = 0;
	float x = 0;
	float y = 0;
	
	float fall_accel;
	float hover_accel;
	
	bool requires_init = true;
	
	Tentacles()
	{
		@g = get_scene();
	}
	
	void update_player(int player_index, controllable@ player)
	{
		this.player_index = player_index;
		@this.player = player;
		@dm = player is null ? null : player.as_dustman();
		character = dm is null ? '' : dm.character();
		@cam = get_camera(player_index);
		
		if(dm !is null)
		{
			fall_accel = dm.fall_accel();
			hover_accel = dm.hover_accel();
		}
		
		x = prev_x = player.x() + ox;
		y = prev_y = player.y() + oy;
		
		arm_tl.init(player, dm, x - arm_x, y - arm_y);
		arm_tr.init(player, dm, x + arm_x, y - arm_y);
		arm_bl.init(player, dm, x - arm_x, y + arm_y);
		arm_br.init(player, dm, x + arm_x, y + arm_y);
		@arms[0] = @arm_tl;
		@arms[1] = @arm_tr;
		@arms[2] = @arm_bl;
		@arms[3] = @arm_br;
		
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
		arm_tl.reset();
		arm_tr.reset();
		arm_bl.reset();
		arm_br.reset();
	}
	
	void step()
	{
		int x_intent = player.x_intent();
		int y_intent = player.y_intent();
		
		if(player.taunt_intent() != 0)
		{
			cam.controller_mode(1);
			bool moving_arm = false;
			bool has_lock = false;
			for(int i = 0; i < 4; i++)
			{
				Arm@ arm = arms[i];
				
				has_lock = has_lock or arm.locked;
				
				int intent;
				switch(i)
				{
					case 0: intent = player.jump_intent(); break;
					case 1: intent = player.light_intent(); break;
					case 2: intent = player.dash_intent(); break;
					case 3: intent = player.heavy_intent(); break;
				}
				
				if(intent != 0)
				{
					arm.locked = false;
					arm.has_target = true;
					arm.target_tx += x_intent * arm.move_speed * DT;
					arm.target_ty += y_intent * arm.move_speed * DT;
					float dx = arm.target_tx - arm.x;
					float dy = arm.target_ty - arm.y;
					float dist = dx * dx + dy * dy;
					if(dist > arm.max_reach * arm.max_reach)
					{
						dist = sqrt(dist);
						dx /= dist;
						dy /= dist;
						arm.target_tx = arm.x + dx * arm.max_reach;
						arm.target_ty = arm.y + dy * arm.max_reach;
					}
					
					moving_arm = true;
				}
				else
				{
					arm.has_target = false;
				}
			}
			
			player.dash_intent(0);
			player.jump_intent(0);
			player.heavy_intent(0);
			player.light_intent(0);
			player.x_intent(0);
			player.y_intent(0);
			
			if(!moving_arm and has_lock)
			{
				dm.fall_accel(0);
				dm.hover_accel (0);
				player.set_speed_xy(
					player.x_speed() * 0.9 + x_intent * move_speed * DT,
					player.y_speed() * 0.9 + y_intent * move_speed * DT
				);
			}
			
			if(!has_lock)
			{
				dm.fall_accel(fall_accel);
				dm.hover_accel(hover_accel);
			}
		}
		else
		{
			cam.controller_mode(0);
			dm.fall_accel(fall_accel);
			dm.hover_accel(hover_accel);
			for(int i = 0; i < 4; i++)
			{
				if(arms[i].found_target) puts('found_target = false;');
				arms[i].has_target = false;
				arms[i].found_target = false;
			}
		}
		
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
//			
//			y -= 24;
//		}
		
		arm_tl.step(x - arm_x, y - arm_y);
		arm_tr.step(x + arm_x, y - arm_y);
		arm_bl.step(x - arm_x, y + arm_y);
		arm_br.step(x + arm_x, y + arm_y);
		
		float pl_x = player.x();
		float pl_y = player.y();
		bool pull = false;
		for(int i = 0; i < 4; i++)
		{
			Arm@ arm = arms[i];
			if(!arm.locked) continue;
//			has_lock = true;
			float dx = arm.target_x - arm.x;
			float dy = arm.target_y - arm.y;
			float dist = dx * dx + dy * dy;
			if(dist > arm.max_reach * arm.max_reach)
			{
				dist = sqrt(dist);
				dx /= dist;
				dy /= dist;
				float px = arm.x;
				float py = arm.y;
				arm.x = arm.target_x - dx * arm.max_reach;
				arm.y = arm.target_y - dy * arm.max_reach;
				player.x(player.x() + (arm.x - px));
				player.y(player.y() + (arm.y - py));
				float angle = (player.direction() - 90) * DEG2RAD;
				float rx = cos(angle);
				float ry = sin(angle);
				pull = dot(rx, ry, dx, dy) < -0.5;
			}
		}
		
		if(pull)
		{
			player.set_speed_xy(player.x_speed() * 0.85, player.y_speed() * 0.85);
		}
		
//		if(has_lock)
//		{
//			puts(y_intent != 0 ? max(abs(player.y_speed()), move_speed_y * DT) * y_intent : player.y_speed());
//			puts(y_intent * move_speed_y * DT);
//			puts(dm.hover_fall_threshold ());
//			player.set_speed_xy(
//				player.x_speed() + x_intent * move_speed_x * DT,
//				player.y_speed() + y_intent * move_speed_x * DT - dm.hover_fall_threshold() * DT
//			);
//			puts(player.y_speed());
//		}
	}
	
	void draw(float sub_frame)
	{
		float px = lerp(prev_x, x, sub_frame);
		float py = lerp(prev_y, y, sub_frame);
		
		arm_tl.draw(sub_frame);
		arm_tr.draw(sub_frame);
		arm_bl.draw(sub_frame);
		arm_br.draw(sub_frame);
		body_spr.draw(19, 19, px, py, 0, player.scale());
	}
	
}