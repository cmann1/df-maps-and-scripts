#include '../common/States.cpp'
#include '../common/drawing_utils.cpp'
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
	
	bool flip_for_facing = true;
	int frame_count = 0;
	float fps_step = 1;
	float t = 0;
	
	float player_x, player_y = 0;
	float prev_x, prev_y = 0;
	
	float head_x = 0, head_y = 0;
	float offset_x = 0;
	float offset_y = 0;
	bool has_head_angle = false;
	float hat_offset_x = 0;
	float hat_offset_y = 0;
	
	uint sprite_colour = 0xFFFFFFFF;
	
	float head_rotation;
	float head_angle;
	
	Hat(int player_index)
	{
		@g = get_scene();
		this.player_index = player_index;
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
			out_angle +=  -PI * 0.5 + player.rotation() * DEG2RAD;
			out_angle_deg = out_angle * RAD2DEG;
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
						hat_offset_x = -2;
						hat_offset_y = -3;
					}
					else if(character == 'dustgirl')
					{
						hat_offset_x = -2;
						hat_offset_y = 0;
					}
					else if(character == 'dustkid')
					{
						hat_offset_x = -3;
						hat_offset_y = 0;
					}
					else if(character == 'dustman')
					{
						hat_offset_x = 0;
						hat_offset_y = -3;
					}
					else
						hat_offset_x = hat_offset_y = 0;
				}
			}
		}
		
		if(player !is null)
		{
			player_x = player.x();
			player_y = player.y();
			prev_x = player.prev_x();
			prev_y = player.prev_y();
			
			if(frame_count > 0) t += fps_step;
		}
	}
	
	void step_post()
	{
		if(player !is null)
		{
			float angle;
			has_head_angle = get_base(head_x, head_y, head_angle, head_rotation);
			
			if(!has_head_angle)
			{
				head_rotation = ((player.state() >= ST_ROOF_GRAB and player.state() <= ST_ROOF_RUN) ? 90 : -90) + player.rotation();
				head_angle = head_rotation * DEG2RAD;
			}
		}
	}
	
	void draw(float sub_frame)
	{
		if(player is null or !draw_hat) return;
		
		float dx = lerp(prev_x, player_x, sub_frame) - prev_x;
		float dy = lerp(prev_y, player_y, sub_frame) - prev_y;
		
		const string sprite_name = 'mod_ha_hat' + (frame_count > 0 ? '_' + int((t % frame_count) + 1) : '');
//		puts(sprite_name);
		spr.draw_world(18, 11, sprite_name, 0, 0, dx + head_x, dy + head_y, head_rotation + 90, flip_for_facing ? player.face() : 1, 1, sprite_colour);
		
//		draw_dot(g, 22, 22, dx + head_x, dy + head_y, 1.5, 0xFF000000, 45);
//		g.draw_line_world(21, 21, base.x, base.y, base.x+cos(head_angle)*48, base.y+sin(head_angle)*48, 1, 0xFF0000FF);
//		spring_sys.Draw(g, dx, dy);
	}
	
}