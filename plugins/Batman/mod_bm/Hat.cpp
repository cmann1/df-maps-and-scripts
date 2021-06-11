#include '../common-old/States.cpp'
#include '../common-old/drawing_utils.cpp'
#include 'hat_data.cpp'

class Hat
{
	
	scene@ g;
	int player_index;
	controllable@ player;
	sprites@ spr;
	
	bool draw_hat = true;
	bool is_virtual = false;
	string character = '';
	int char_index = -1;
	
	float player_x, player_y = 0;
	float prev_x, prev_y = 0;
	
	script@ s;
	
	float offset_x = 0;
	float offset_y = 0;
	float offset_angle = 0;
	bool has_head_angle = false;
	float hat_offset_x = 0;
	float hat_offset_y = 0;
	float hat_scale = 1;
	float cape_offset_x = -3;
	float cape_offset_y = 15;
	float cape_angle;
	float cape_speed_offset_angle;
	
	uint sprite_colour = 0xFFFFFFFF;
	uint red1 = 0xFF222222;
	uint red2 = 0xFF222222; //0xFF141414;
	
	SpringSystem spring_sys;
	Particle@ base;
	Particle@ p1;
	Particle@ p2;
	Particle@ end;
	PointConstraint@ player_attach;
	AngularConstraint@ base_angle;
	
	float head_rotation;
	float head_angle;
	float base_width = 7;
	float p1_width = 16;
	float p2_width = 23;
	float end_width = 30;
	
	Hat(int player_index)
	{
		@g = get_scene();
		this.player_index = player_index;
		
		@base = spring_sys.AddPartice(0, 0);
		@p1 = spring_sys.AddPartice(0, 0);
		@p2 = spring_sys.AddPartice(0, 0);
		@end = spring_sys.AddPartice(0, 0);
		@player_attach = spring_sys.AddPointConstraint(base, 0, 0);
		spring_sys.AddDistanceConstraint(base, p1, 22);
		spring_sys.AddDistanceConstraint(p1, p2, 22);
		spring_sys.AddDistanceConstraint(p2, end, 22);
		@base_angle = spring_sys.AddAngularConstraint(base, p1, null, 180, 360, 0.5);
		spring_sys.AddAngularConstraint(p1, p2, base, -30, 30);
		spring_sys.AddAngularConstraint(p2, end, p1, -40, 40);
	}
	
	float player_face()
	{
		return player.attack_state() != 0
			? player.attack_face()
			: player.face();
	}
	
	bool get_base(float &out out_x, float &out out_y, float &out out_angle, float &out out_angle_deg)
	{
		float ox = offset_x;
		float oy = offset_y;
		
		string sprite_name;
		uint frame;
		float face;
		get_sprite_info(sprite_name, frame, face);
		bool has_data = false;
		draw_hat = sprite_name != 'none';
		
		if(char_index != -1)
		{
			const int anim_index = ANIM_INDEX.exists(sprite_name) ? int(ANIM_INDEX[sprite_name]) : -1;
			if(anim_index != -1)
			{
				out_x = HAT_DATA[char_index][anim_index][frame * 3] * face;
				out_y = HAT_DATA[char_index][anim_index][frame * 3 + 1];
				out_angle = HAT_DATA[char_index][anim_index][frame * 3 + 2] * face;
				has_data = true;
			}
		}
		
		if(!has_data)
		{
			out_x = ox * face;
			out_y = oy;
		}
		else
		{
			if(hat_offset_x != 0 or hat_offset_y != 0)
			{
				float dx = hat_offset_x * face;
				float dy = hat_offset_y;
				rotate(dx, dy, out_angle, dx, dy);
				out_x += dx;
				out_y += dy;
			}
			out_angle +=  -PI * 0.5 + player.rotation() * DEG2RAD + PI;
			out_angle_deg = out_angle * RAD2DEG + offset_angle * player_face() + 180;
		}
		
		rotate(out_x, out_y, player.rotation() * DEG2RAD, out_x, out_y);
		out_x += player_x + player.draw_offset_x();
		out_y += player_y + player.draw_offset_y();
		
		return has_data;
	}
	
	void get_sprite_info(string &out sprite_name, uint &out frame, float &out face)
	{
		sprites@ spr = player.get_sprites();
		if(player.attack_state() == ATTACK_TYPE_IDLE)
		{
			sprite_name = player.sprite_index();
			frame = uint(player.state_timer());
			face = player.face();
		}
		else
		{
			sprite_name = player.attack_sprite_index();
			frame = uint(player.attack_timer());
			face = player.attack_face();
		}
		frame = frame % spr.get_animation_length(sprite_name);
	}
	
	void step()
	{
		if(player is null)
		{
//			puts('init player');
			entity@ e = controller_entity(player_index);
			@player = (@e != null ? e.as_controllable() : null);
			if(player !is null)
			{
				if(spr is null)
				{
					@spr = create_sprites();
					spr.add_sprite_set('script');
				}
				
//				player.time_warp(0.5);
				dustman@ dm = player.as_dustman();
				if(dm is null)
				{
					is_virtual = false;
					character = '';
				}
				else
				{
					character = dm.character();
					is_virtual = character.substr(0, 1) == 'v';
					if(is_virtual)
					{
						character = character.substr(1);
					}
				}
				
				if(character == '')
				{
					rectangle@ r = player.hit_rectangle();
					offset_x = r.left() + r.get_width() * 0.5;
					offset_y = r.top() + 10;
				}
				else
				{
					char_index = CHAR_INDICES.exists(character) ? int(CHAR_INDICES[character]) : -1;
					offset_x = 3;
					offset_y = -87;
					
					if(character == 'dustworth')
					{
						hat_offset_x = -3;
						hat_offset_y = -5;
						offset_angle = -5;
						hat_scale = 1;
					}
					else if(character == 'dustgirl')
					{
						hat_offset_x = -1;
						hat_offset_y = -5;
						hat_scale = 0.909;
					}
					else if(character == 'dustkid')
					{
						hat_offset_x = 1;
						hat_offset_y = -2;
						offset_angle = 35;
						hat_scale = 1;
					}
					else if(character == 'dustman')
					{
						hat_offset_x = 0;
						hat_offset_y = -3;
						offset_angle = 6;
						hat_scale = 0.95;
					}
					else
						hat_offset_x = hat_offset_y = 0;
				}
				
				const uint alpha = is_virtual ? 0x88000000 : 0xFF000000;
				sprite_colour = alpha | (sprite_colour & 0xFFFFFF);
				red1 = alpha | (red1 & 0xFFFFFF);
				red2 = alpha | (red2 & 0xFFFFFF);
				
				player_x = player.x();
				player_y = player.y();
				prev_x = player.prev_x();
				prev_y = player.prev_y();
				
				const float face = -5 * player.face();
				float bx, by;
				has_head_angle = get_base(bx, by, head_angle, head_rotation);
				base.reset(bx, by);
				p1.reset(base.x + face, base.y - 15);
				p2.reset(p1.x + face, p1.y - 10);
				end.reset(p2.x + face, p2.y - 10);
			}
		}
		
		if(player !is null)
		{
			player_x = player.x();
			player_y = player.y();
			prev_x = player.prev_x();
			prev_y = player.prev_y();
		}
	}
	
	void step_post()
	{
		if(player !is null)
		{
			float angle;
			has_head_angle = get_base(player_attach.x, player_attach.y, head_angle, head_rotation);
			
			if(!has_head_angle)
			{
				head_rotation = ((player.state() >= ST_ROOF_GRAB and player.state() <= ST_ROOF_RUN) ? 90 : -90) + player.rotation() + 180;
				head_angle = head_rotation * DEG2RAD;
			}
			
			cape_speed_offset_angle = min(1, abs(player.x_speed()) / 512) * player_face() * 55;
			cape_angle = head_angle + cape_speed_offset_angle * DEG2RAD;
			
			const float range = 20 * DEG2RAD;
			base_angle.SetRange(cape_angle - range, cape_angle + range);
			
			spring_sys.Update();
		}
	}
	
	void draw(float sub_frame)
	{
		if(player is null or !draw_hat) return;
		
		float dx = lerp(prev_x, player_x, sub_frame) - prev_x;
		float dy = lerp(prev_y, player_y, sub_frame) - prev_y;
		
//		draw_dot(g, 22, 22, player_attach.x + dx, player_attach.y + dy, 1.5, 0xFF000000, 45);
//		draw_dot(g, 22, 22, base.x + dx, base.y + dy, 1.5, 0xFF00FF00, 45);

		float cox = cape_offset_x;
		float coy = cape_offset_y;
		rotate(cox, coy, cape_speed_offset_angle * DEG2RAD * player_face(), cox, coy);
		
		dx += player_attach.x - base.x;
		dy += player_attach.y - base.y;
		dx += cox * player_face();
		dy += coy;
		
		float base_angle = atan2(p1.y - base.y, p1.x - base.x);
		float mid_angle = atan2(p2.y - p1.y, p2.x - p1.x);
		float end_angle = atan2(end.y - p2.y, end.x - p2.x);
		
		float b2m = base_angle + short_angle(base_angle, mid_angle) * 0.5;
		float m2end = mid_angle + short_angle(mid_angle, end_angle) * 0.5;
		
		float base_x = -sin(head_angle) * base_width;
		float base_y =  cos(head_angle) * base_width;
		float p1_x = -sin(b2m) * p1_width;
		float p1_y =  cos(b2m) * p1_width;
		float p2_x = -sin(m2end) * p2_width;
		float p2_y =  cos(m2end) * p2_width;
		float end2_x = -sin(m2end) * end_width;
		float end2_y =  cos(m2end) * end_width;
		const float ba1x = dx + base.x - base_x, ba1y = dy + base.y - base_y;
		const float ba2x = dx + base.x + base_x, ba2y = dy + base.y + base_y;
		const float p11x = dx + p1.x - p1_x, p11y = dy + p1.y - p1_y;
		const float p12x = dx + p1.x + p1_x, p12y = dy + p1.y + p1_y;
		const float p21x = dx + p2.x - p2_x, p21y = dy + p2.y - p2_y;
		const float p22x = dx + p2.x + p2_x, p22y = dy + p2.y + p2_y;
		const float endx1 = dx + end.x - end2_x, endy1 = dy + end.y - end2_y;
		const float endx2 = dx + end.x + end2_x, endy2 = dy + end.y + end2_y;
		
		const int cape_layer = 18;
		const int cape_sublayer = 9;
		g.draw_quad_world(cape_layer, cape_sublayer, false,
			ba1x, ba1y, ba2x, ba2y,
			p12x, p12y, p11x, p11y,
			red2, red1, red1, red2);
		g.draw_quad_world(cape_layer, cape_sublayer, false,
			p11x, p11y, p12x, p12y,
			p22x, p22y, p21x, p21y,
			red2, red1, red1, red2);
		g.draw_quad_world(cape_layer, cape_sublayer, false,
			p21x, p21y, p22x, p22y,
			endx2, endy2, endx1, endy1,
			red2, red1, red1, red2);
		
		spr.draw_world(cape_layer, cape_sublayer, 'cape', 0, 0, dx + end.x, dy + end.y, -90 + m2end * RAD2DEG, 1, 1, sprite_colour);
		
		spr.draw_world(18, 11, 'bat_ears', 0, 0, dx - cox * player_face() + base.x, dy - coy + base.y, head_rotation + 90, player_face() * hat_scale, hat_scale, sprite_colour);
		
//		g.draw_line_world(21, 21, base.x, base.y, base.x+cos(head_angle)*48, base.y+sin(head_angle)*48, 1, 0xFF0000FF);
//		spring_sys.Draw(g, dx, dy);
	}
	
}