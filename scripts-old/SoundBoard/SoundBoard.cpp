#include "common/utils.cpp"
#include "common/drawing_utils.cpp"
#include "SoundBoard_SoundNames.cpp"
#include "SoundBoard_Sound.cpp"

class script
{
	scene@ g;
	controllable@ player;
	dustman@ dm;
	
	camera@ cam = null;
	
	float camera_width = 1920;
	float camera_height = 1080;
	float view_height = camera_height * 0.75; // < less than the camera height because of combo metre
	string font_name = "ProximaNovaReg";
	uint font_size = 36;
	float font_scale = 0.75;
	
	float placement_border = 48;
	float placement_padding_x = 48;
	float placement_padding_y = 24;
	float placement_group_spacing = 72;
	float placement_line_height = 0;
	
	float col_size = 0;
	float row_size = 0;
	uint col_count = 0;
	uint row_count = 0;
	
	float scroll_x = 0;
	float scroll_y = 0;
	float max_scroll_x = 0;
	float max_scroll_y = 0;
	int scroll_delay = 10;
	int scroll_delay_timer = 0;
	
	float time_scale = 1;
	float time_scale_step = 0.01;
	
	int audio_loop_delay = 30;
	audio@ current_audio = null;
	int current_audio_timer = -1;
	Sound@ current_sound = null;
	int current_sound_index = -1;
	
	array<Sound@> sounds = {};
	array<array<Sound@>> sound_grid(10, array<Sound@>(10));
	
	textfield@ help_text;
	textfield@ time_scale_text;
	bool require_init = true;
	
	script()
	{
		@g = get_scene();
		
		@help_text = create_textfield();
		help_text.text(
			"Select: Up/Down Left/Right\n" +
			"PageUp/Down: Heavy + Up/Down\n" +
			"Time Scale: Light + Up/Down\n" +
			"Replay: Taunt"
		);
		help_text.align_horizontal(-1);
		help_text.align_vertical(1);
		
		@time_scale_text = create_textfield();
		time_scale_text.align_horizontal(-1);
		time_scale_text.align_vertical(1);
		update_time_scale_text();
		
		g.disable_score_overlay(true);
		
//		cam.x(camera_width/2);
//		cam.y(camera_height/2);
	}
	
	void update_time_scale_text()
	{
		time_scale_text.text("Time Scale: " + formatFloat(time_scale, "", 0, 2));
	}
	
	void checkpoint_save()
	{
		
	}

	void checkpoint_load()
	{
		@player = null;
		@dm = null;
	}
	
	void init()
	{
		if(!require_init) return;
		
//		add_sounds(0, AUDIO_AMB_NAMES);
//		add_sounds(1, AUDIO_MUSIC_NAMES);
		add_sounds(0, AUDIO_SFX_NAMES);
		
		float placement_x = placement_border;
		float placement_y = placement_border;
		
		col_count = uint(floor(camera_width / col_size));
		row_count = uint(ceil(sounds.length() / col_count)) + 1;
		array<array<Sound@>> sound_grid(row_count, array<Sound@>(col_count));
		
		col_size = camera_width / col_count;
		
		for(uint r = 0; r < row_count; r++)
		{
			array<Sound@>@ sound_row = sound_grid[r];
			for(uint c = 0; c < col_count; c++)
			{
				@sound_row[c] = null;
			}
		}
		
		uint sound_count = sounds.length();
		uint current_group = 0;
		int col_index = -1;
		uint row_index = 0;
		for(uint i = 0; i < sound_count; i++)
		{
			Sound@ sound = sounds[i];
			
			if(sound.group_index != current_group or ++col_index >= int(col_count))
			{
				placement_x = placement_border;
				placement_y += row_size + placement_padding_y;
				if(sound.group_index != current_group) placement_y += placement_group_spacing;
				current_group = sound.group_index;
				col_index = 0;
				row_index++;
			}
			
			sound.x = placement_x;
			sound.y = placement_y;
			sound.col = col_index;
			sound.row = row_index;
			
			if(row_index >= sound_grid.length()) sound_grid.insertLast(array<Sound@>(col_count));
			@sound_grid[row_index][col_index] = sound;
			
			placement_x += col_size;
		}
		
		max_scroll_y = (placement_y + row_size + placement_border) - view_height;
		this.sound_grid = sound_grid;
				
		select(sounds[0]);
		require_init = false;
	}
	
	void add_sounds(uint group_index, const array<array<string>>@ sound_list)
	{
		uint group_count = sound_list.length();
		for(uint i = 0; i  < group_count; i++)
		{
			const array<string>@ name_list = sound_list[i];
			uint sound_count = name_list.length();
			
			for(uint j = 0; j  < sound_count; j++)
			{
				string sound_name = name_list[j];
				Sound@ sound = Sound(group_index, sound_name, font_name, font_size, font_scale);
				sounds.insertLast(sound);
				
				float width = sound.width;
				float height = sound.height;
				if(width > col_size) col_size = width;
				if(height > row_size) row_size = height;
			}
		}
	}
	
	void select(Sound@ sound)
	{
		if(@current_sound == @sound) return;
		
		if(@current_sound != null)
		{
			current_sound.selected = false;
		}
		
		if(@current_audio != null)
		{
			current_audio.stop();
		}
		
		@current_sound = sound;
		
		if(@current_sound != null)
		{
			current_sound.selected = true;
			current_sound_index = sounds.find(@current_sound);
			play_current_sound();
			
			if(sound.y - placement_border < scroll_y)
			{
				scroll_y = sound.y - placement_border;
			}
			else if(sound.y + sound.height > scroll_y + view_height)
			{
				scroll_y = sound.y + sound.height - view_height;
			}
		}
		else{
			current_sound_index = -1;
		}
	}
	
	void play_current_sound()
	{
		if(@current_sound == null) return;
		
		if(@current_audio != null) current_audio.stop();
		
		@current_audio = g.play_sound(current_sound.name, 0, 0, 1, false, false);
		current_audio_timer = audio_loop_delay;
		if(@current_audio == null)
		{
			@current_audio = g.play_stream(current_sound.name, 1, 0, 0, false, 1);
		}
		
		if(time_scale != 1 and @current_audio != null)
		{
			current_audio.time_scale(time_scale);
		}
	}
	
	void set_scroll(float x, float y)
	{
		if(x < 0) x = 0;
		if(y < 0) y = 0;
		if(x > max_scroll_x) x = max_scroll_x;
		if(y > max_scroll_y) y = max_scroll_y;
		
		scroll_x = x;
		scroll_y = y;
	}
	
	void step(int entities)
	{
		if(@player == null)
		{
			entity@e = controller_entity(0);
			if(@e != null)
			{
				@player = e.as_controllable();
				@dm = e.as_dustman();
			}
			
			@cam = get_camera(0);
			cam.script_camera(true);
			
			if(require_init)
			{
				init();
			}
		}
		else
		{
			if(player.y_intent() != 0 or player.x_intent() != 0)
			{
				if(scroll_delay_timer <= 0)
				{
					if(player.light_intent() != 0)
					{
						time_scale -= time_scale_step * player.y_intent();
						if(time_scale < time_scale_step) time_scale = time_scale_step;
						update_time_scale_text();
					}
					else
					{
						int x_dir = player.x_intent();
						int y_dir = x_dir == 0 ? player.y_intent() : 0;
						
						if(player.heavy_intent() != 0)
						{
							y_dir *= int(view_height / (row_size + placement_padding_y));
						}
						
						int sound_col = current_sound.col + x_dir;
						int sound_row = current_sound.row + y_dir;
						
						if(sound_col < 0)
						{
							sound_col = int(col_count - 1);
							sound_row--;
						}
						else if(sound_col >= int(col_count))
						{
							sound_col = 0;
							sound_row++;
						}

						if(sound_col < 0) sound_col = 0;
						if(sound_col >= int(col_count)) sound_col = col_count - 1;
						if(sound_row < 0) sound_row = 0;
						if(sound_row >= int(row_count)) sound_row = row_count - 1;
						
						array<Sound@>@ sound_grid_row = sound_grid[sound_row];
						Sound@ next_sound = sound_grid_row[sound_col];
						while(@next_sound == null)
						{
							if(--sound_col < 0) break;
							@next_sound = sound_grid_row[sound_col];
						}
						
						select(next_sound);
					}
					
					scroll_delay_timer = scroll_delay;
				}
			}
			else{
				scroll_delay_timer = 0;
			}
			
			if(player.taunt_intent() != 0)
			{
				play_current_sound();
			}

			if(@current_audio != null)
			{
				if(!current_audio.is_playing())
				{
					if(current_audio_timer-- <= 0)
					{
						play_current_sound();
					}
				}
			}
			
			scroll_delay_timer--;
		}
	}
	
	void move_cameras()
	{
		if(@cam != null)
		{
			cam.x(camera_width/2);
			cam.y(camera_height/2);
		}
	}
	
	void draw(float sub_frame)
	{
		uint sound_count = sounds.length();
		for(uint i = 0; i < sound_count; i++)
		{
			sounds[i].draw(g, scroll_x, scroll_y);
		}
		
		draw_text_box(g, help_text, 0, 430, 0.6, 10);
		draw_text_box(g, time_scale_text, -700, 430, 0.6, 10);
	}
	
	void draw_text_box(scene@ g, textfield@ txt, float tx, float ty, float scale, float padding)
	{
		const float tw = txt.text_width() * scale;
		const float th = txt.text_height() * scale;
		
		g.draw_glass_hud(19, 19,
			tx - tw/2 - padding, ty - th - padding,
			tx + tw/2 + padding, ty + padding,
			0, 0x00000000);
		
		g.draw_rectangle_hud(19, 19,
			tx - tw/2 - padding, ty - th - padding,
			tx + tw/2 + padding, ty + padding,
			0, 0xBB000000);
		
		txt.draw_hud(20, 20, tx - tw/2, ty, scale, scale, 0);
	}
	
	void step_post(int entities) {}
	void entity_on_add(entity@ e) {}
	void entity_on_remove(entity@ e) {}
	
	
}