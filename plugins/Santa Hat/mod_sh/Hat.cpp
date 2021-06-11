#include '../common-old/States.cpp'
#include '../common-old/drawing_utils.cpp'
#include 'hat_data.cpp'

const uint HAT_RED_1 = 0xFFb8010f;
const uint HAT_RED_2 = 0xFF94000b;

class Hat
{
	
	scene@ g;
	int player_index;
	fog_setting@ fog;
	camera@ cam;
	controllable@ player;
	dustman@ dm;
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
	bool has_head_angle = false;
	float hat_offset_x = 0;
	float hat_offset_y = 0;
	
	uint sprite_colour = 0xFFFFFFFF;
	uint red1 = 0xFFb8010f;
	uint red2 = 0xFF94000b;
	
	SpringSystem spring_sys;
	Particle@ base;
	Particle@ p1;
	Particle@ p2;
	Particle@ bob;
	PointConstraint@ player_attach;
	AngularConstraint@ base_angle;
	
	float head_rotation;
	float head_angle;
	float base_width = 13;
	float p1_width = 8;
	float p2_width = 4;
	
	Hat(int player_index)
	{
		@g = get_scene();
		this.player_index = player_index;
		@cam = get_camera(player_index);
		
		@base = spring_sys.AddPartice(0, 0);
		@p1 = spring_sys.AddPartice(0, 0);
		@p2 = spring_sys.AddPartice(0, 0);
		@bob = spring_sys.AddPartice(0, 0);
		@player_attach = spring_sys.AddPointConstraint(base, 0, 0);
		spring_sys.AddDistanceConstraint(base, p1, 15);
		spring_sys.AddDistanceConstraint(p1, p2, 10);
		spring_sys.AddDistanceConstraint(p2, bob, 10);
		@base_angle = spring_sys.AddAngularConstraint(base, p1, null, -110, -70, 0.5);
		spring_sys.AddAngularConstraint(p1, p2, base, -30, 30);
		spring_sys.AddAngularConstraint(p2, bob, p1, -40, 40);
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
		out_x += player_x;
		out_y += player_y;
		
		if(player.attack_state() == ATTACK_TYPE_IDLE)
		{
			out_x += player.draw_offset_x();
			out_y += player.draw_offset_y();
		}
		
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
		
		// No clue why, but get_animation_length can return 0?
		// Unable to reproduce
		
//		if(spr.get_animation_length(sprite_name) == 0)
//		{
//			puts('-----------------------------------------------------');
//			puts('--- spr.get_animation_length(sprite_name) == 0');
//			puts(' sprite_name: ' + sprite_name);
//			puts(' frame: ' + frame);
//		}
		
		frame = spr.get_animation_length(sprite_name) != 0
			? frame % spr.get_animation_length(sprite_name)
			: 0;
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
				@dm = player.as_dustman();
				
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
				bob.reset(p2.x + face, p2.y - 10);
			}
		}
		
		if(player !is null)
		{
			const uint layer_colour = s.fog.colour(18, 10);
			const float layer_perc = s.fog.percent(18, 10);
			int lr, lg, lb, la;
			int r1r, r1g, r1b, r1a;
			int r2r, r2g, r2b, r2a;
			int_to_rgba(s.fog.colour(18, 10), lr, lg, lb, la);
			int_to_rgba(HAT_RED_1, r1r, r1g, r1b, r1a);
			int_to_rgba(HAT_RED_2, r2r, r2g, r2b, r2a);
			
			const uint alpha = is_virtual ? 0xaa000000 : 0xFF000000;
			sprite_colour = alpha | (sprite_colour & 0xFFFFFF);
			red1 = rgba(lerpi(lr, r1r, 1.0 - layer_perc), lerpi(lg, r1g, 1.0 - layer_perc), lerpi(lb, r1b, 1.0 - layer_perc), ((alpha >> 24) * lerpi(la, r1a, 1.0 - layer_perc)) / 255);
			red2 = rgba(lerpi(lr, r2r, 1.0 - layer_perc), lerpi(lg, r2g, 1.0 - layer_perc), lerpi(lb, r2b, 1.0 - layer_perc), ((alpha >> 24) * lerpi(la, r2a, 1.0 - layer_perc)) / 255);
			
			player_x = player.x();
			player_y = player.y();
			
			const float dx = player_x - prev_x;
			const float dy = player_y - prev_y;
			
			if((dx * dx + dy * dy) > 200 * 200)
			{
				spring_sys.Shift(dx, dy);
			}
			
			prev_x = player.prev_x();
			prev_y = player.prev_y();
		}
	}
	
	void update_character()
	{
		const string old_character = character;
		
		if(@dm == null)
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
		
		if(old_character == character)
			return;
		
		if(character == '')
		{
			rectangle@ r = player.hit_rectangle();
			offset_x = r.left() + r.get_width() * 0.5;
			offset_y = r.top() + 10;
			char_index = -1;
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
			{
				hat_offset_x = hat_offset_y = 0;
			}
		}
	}
	
	void step_post()
	{
		if(player !is null)
		{
//			if(player.taunt_intent() != 0)
//			{
//				g.time_warp(g.time_warp() == 1 ? 0.05 : 1);
//			}
			
			update_character();
			
			float angle;
			has_head_angle = get_base(player_attach.x, player_attach.y, head_angle, head_rotation);
			
			if(!has_head_angle)
			{
				head_rotation = ((player.state() >= ST_ROOF_GRAB and player.state() <= ST_ROOF_RUN) ? 90 : -90) + player.rotation();
				head_angle = head_rotation * DEG2RAD;
			}
			const float offset = 5;
			base_angle.SetRange(
				head_angle - (25 - offset) * player.face() * DEG2RAD,
				head_angle + (15 - offset) * player.face() * DEG2RAD);
			
			spring_sys.Update(g.time_warp());
		}
	}
	
	void draw(float sub_frame)
	{
		if(player is null or !draw_hat) return;
		
		float dx = lerp(prev_x, player_x, sub_frame) - prev_x;
		float dy = lerp(prev_y, player_y, sub_frame) - prev_y;
		
//		draw_dot(g, 22, 22, player_attach.x + dx, player_attach.y + dy, 1.5, 0xFF000000, 45);
//		draw_dot(g, 22, 22, base.x + dx, base.y + dy, 1.5, 0xFF00FF00, 45);
		
		dx += player_attach.x - base.x;
		dy += player_attach.y - base.y;
		
		float base_angle = atan2(p1.y - base.y, p1.x - base.x);
		float mid_angle = atan2(p2.y - p1.y, p2.x - p1.x);
		float bob_angle = atan2(bob.y - p2.y, bob.x - p2.x);
		
		float b2m = base_angle + short_angle(base_angle, mid_angle) * 0.5;
		float m2bob = mid_angle + short_angle(mid_angle, bob_angle) * 0.5;
		
		float base_x = -sin(head_angle) * base_width;
		float base_y =  cos(head_angle) * base_width;
		float p1_x = -sin(b2m) * p1_width;
		float p1_y =  cos(b2m) * p1_width;
		float p2_x = -sin(m2bob) * p2_width;
		float p2_y =  cos(m2bob) * p2_width;
		const float ba1x = dx + base.x - base_x, ba1y = dy + base.y - base_y;
		const float ba2x = dx + base.x + base_x, ba2y = dy + base.y + base_y;
		const float p11x = dx + p1.x - p1_x, p11y = dy + p1.y - p1_y;
		const float p12x = dx + p1.x + p1_x, p12y = dy + p1.y + p1_y;
		const float p21x = dx + p2.x - p2_x, p21y = dy + p2.y - p2_y;
		const float p22x = dx + p2.x + p2_x, p22y = dy + p2.y + p2_y;
		const float box = dx + bob.x, boy = dy + bob.y;
		
		g.draw_quad_world(18, 10, false,
			ba1x, ba1y, ba2x, ba2y,
			p12x, p12y, p11x, p11y,
			red2, red1, red1, red2);
		g.draw_quad_world(18, 10, false,
			p11x, p11y, p12x, p12y,
			p22x, p22y, p21x, p21y,
			red2, red1, red1, red2);
		g.draw_quad_world(18, 10, false,
			p21x, p21y, p22x, p22y,
			box, boy, box, boy,
			red2, red1, red1, red2);
		
		spr.draw_world(18, 10, 'hat_rim', 0, 0, dx + base.x, dy + base.y, head_rotation + 90, player.face(), 1, sprite_colour);
		spr.draw_world(18, 10, 'hat_bob', 0, 0, dx + bob.x, dy + bob.y, bob_angle * RAD2DEG, -1, 1, sprite_colour);
		
//		g.draw_line_world(21, 21, base.x, base.y, base.x+cos(head_angle)*48, base.y+sin(head_angle)*48, 1, 0xFF0000FF);
//		spring_sys.Draw(g, dx, dy);
	}
	
	uint rgba(int r, int g, int b, int a = 255)
	{
		return (uint(a) << 24) + (uint(r) << 16) + (uint(g) << 8) + uint(b);
	}
	
	void int_to_rgba(uint colour, int &out r, int &out g, int &out b, int &out a)
	{
		a = (colour >> 24) & 0xFF;
		r = (colour >> 16) & 0xFF;
		g = (colour >> 8) & 0xFF;
		b = (colour) & 0xFF;
	}
	
	int lerpi(int a, int b, float x)
	{
		return int(a * (1.0 - x) + b * x);
	}
	
}
