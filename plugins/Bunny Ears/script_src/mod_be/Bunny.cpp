#include '../lib/enums/AttackType.cpp';
#include '../lib/enums/EntityState.cpp';
#include '../lib/entities/player_head_data.cpp';
#include '../lib/entities/utils.cpp';
#include '../lib/math/math.cpp';
#include '../lib/utils/colour.cpp';
#include 'Ear.cpp';

class Bunny
{
	
	scene@ g;
	int player_index;
	SpringSystem@ spring_system;
	controllable@ player;
	dustman@ dm;
	int player_face = 1;
	float player_x, player_y;
	float prev_x, prev_y;
	
	bool draw_ears = true;
	bool is_virtual = false;
	string character = '';
	int char_index = -1;
	
	
	float offset_x, offset_y;
	bool has_head_angle = false;
	float head_offset_x, head_offset_y;
	float head_offset_rotation;
	float head_rotation;
	float head_x, head_y;
	bool has_head_data;
	
	Ear ear_left;
	Ear ear_right;
	
	void init(const int player_index, SpringSystem@ spring_system)
	{
		@g = get_scene();
		this.player_index = player_index;
		@this.spring_system = spring_system;
		
		ear_left.init(@g, @spring_system, -EAR_SPACING, 0, -16);
		ear_right.init(@g, @spring_system, EAR_SPACING, 0,  12);
	}
	
	void checkpoint_load()
	{
		@player = null;
		@dm = null;
		character = '';
		char_index = -1;
	}
	
	void init_player()
	{
		if(@player != null)
			return;
		
		entity@ e = controller_entity(player_index);
		@player = (@e != null ? e.as_controllable() : null);
		
		if(@player == null)
			return;
		
		@dm = player.as_dustman();
		
		player_x = player.x();
		player_y = player.y();
		prev_x = player.prev_x();
		prev_y = player.prev_y();
		
		update_character();
	}
	
	void update_character()
	{
		const string old_character = character;
		const int old_char_index = char_index;
		
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
		
		char_index = CHAR_INDICES.exists(character)
			? int(CHAR_INDICES[character])
			: -1;
		
		if(char_index == -1)
		{
			character = '';
		}
		
		if(old_character == character)
			return;
		
		if(character != '')
		{
			offset_x = 3;
			offset_y = -87;
			
			if(character == 'dustworth')
			{
				head_offset_x =  2;
				head_offset_y = -3;
				head_offset_rotation = 5;
			}
			else if(character == 'dustgirl')
			{
				head_offset_x = 0;
				head_offset_y = -5;
				head_offset_rotation = 0;
			}
			else if(character == 'dustkid')
			{
				head_offset_x = 2;
				head_offset_y = 2;
				head_offset_rotation = 25;
			}
			else if(character == 'dustman')
			{
				head_offset_x = 0;
				head_offset_y = -5;
				head_offset_rotation = 5;
			}
		}
		else
		{
			//rectangle@ r = player.hit_rectangle();
			//offset_x = r.left() + r.get_width() * 0.5;
			//offset_y = r.top() + 10;
			
			head_offset_x = head_offset_y = 0;
			head_offset_rotation = 0;
			char_index = -1;
		}
		
		if(old_char_index == -1 && char_index != -1)
		{
			calculate_head_base();
			ear_left.reset(head_x, head_y);
			ear_right.reset(head_x, head_y);
			update_ears_rotation();
		}
	}
	
	void calculate_head_base()
	{
		float ox = offset_x;
		float oy = offset_y;
		
		string sprite_name;
		uint frame;
		get_entity_sprite_info(player, sprite_name, frame, player_face);
		has_head_data = false;
		draw_ears = sprite_name != 'none';
		
		if(char_index != -1)
		{
			const int anim_index = ANIM_INDEX.exists(sprite_name)
				? int(ANIM_INDEX[sprite_name]) : -1;
			
			if(anim_index != -1)
			{
				const array<array<float>>@ head_data = @HEAD_DATA[char_index];
				const array<float>@ anim_data = @head_data[anim_index];
				head_x = anim_data[frame * 3] * player_face;
				head_y = anim_data[frame * 3 + 1];
				head_rotation = anim_data[frame * 3 + 2] * player_face * RAD2DEG;
				has_head_data = true;
			}
		}
		
		if(!has_head_data)
		{
			head_x = ox * player_face;
			head_y = oy;
			
			head_rotation = 
				player.state() >= EntityState::RoofGrab and
				player.state() <= EntityState::RoofRun
					? 90 : -90;
			head_rotation += player.rotation();
		}
		else
		{
			if(head_offset_x != 0 or head_offset_y != 0)
			{
				float dx = head_offset_x * player_face;
				float dy = head_offset_y;
				rotate(dx, dy, head_rotation * DEG2RAD, dx, dy);
				head_x += dx;
				head_y += dy;
			}
			
			head_rotation += -90 + player.rotation();
		}
		
		rotate(head_x, head_y, player.rotation() * DEG2RAD, head_x, head_y);
		head_x += player_x;
		head_y += player_y;
		
		if(player.attack_state() == AttackType::Idle)
		{
			head_x += player.draw_offset_x();
			head_y += player.draw_offset_y();
		}
	}
	
	void update_fog(fog_setting@ fog)
	{
		int lr, lg, lb, la;
		int c1r, c1g, c1b, c1a;
		int c2r, c2g, c2b, c2a;
		int_to_rgba(fog.colour(18, 10), lr, lg, lb, la);
		int_to_rgba(DARK_CLR,  c1r, c1g, c1b, c1a);
		int_to_rgba(LIGHT_CLR, c2r, c2g, c2b, c2a);
		
		const float layer_perc = 1 - fog.percent(18, 10);
		const float alpha = is_virtual ? 0xaa : 0xff;
		//sprite_colour = alpha | (sprite_colour & 0xffffff);
		const uint dark_clr = rgba(
			int(lerp(lr, c1r, layer_perc)),
			int(lerp(lg, c1g, layer_perc)),
			int(lerp(lb, c1b, layer_perc)),
			int((alpha * lerp(la, c1a, layer_perc)) / 255));
		const uint light_clr = rgba(
			int(lerp(lr, c2r, layer_perc)),
			int(lerp(lg, c2g, layer_perc)),
			int(lerp(lb, c2b, layer_perc)),
			int((alpha * lerp(la, c2a, layer_perc)) / 255));
		
		ear_left.update_colours(dark_clr, light_clr);
		ear_right.update_colours(dark_clr, light_clr);
	}
	
	void check_teleport()
	{
		const float dx = player_x - prev_x;
		const float dy = player_y - prev_y;
		
		if((dx * dx + dy * dy) <= 200 * 200)
			return;
		
		ear_left.offset(dx, dy);
		ear_right.offset(dx, dy);
	}
	
	void update_ears_rotation()
	{
		const float rot = head_rotation + head_offset_rotation * player_face;
		ear_left.update_rotation(rot, player_face);
		ear_right.update_rotation(rot, player_face);
	}
	
	void step()
	{
		if(@player == null)
		{
			init_player();
		}
		
		if(@player != null)
		{
			player_x = player.x();
			player_y = player.y();
			
			check_teleport();
			
			prev_x = player.prev_x();
			prev_y = player.prev_y();
		}
	}
	
	void step_post(fog_setting@ fog)
	{
		if(@player == null)
			return;
		
		update_character();
		calculate_head_base();
		update_ears_rotation();
		
		ear_left.offset_x = -EAR_SPACING * player_face;
		ear_right.offset_x = EAR_SPACING * player_face;
		ear_left.step(head_x, head_y);
		ear_right.step(head_x, head_y);
		
		if(char_index == -1)
			return;
		
		update_fog(fog);
		spring_system.step(g.time_warp());
	}
	
	void draw(const float subframe)
	{
		if(player is null || !draw_ears || char_index == -1)
			return;
		
		const float dx = lerp(prev_x, player_x, subframe) - prev_x;
		const float dy = lerp(prev_y, player_y, subframe) - prev_y;
		
		ear_left.draw(dx, dy, head_rotation * DEG2RAD);
		ear_right.draw(dx, dy, head_rotation * DEG2RAD);
		//spring_system.debug_draw(g);
	}
	
}
