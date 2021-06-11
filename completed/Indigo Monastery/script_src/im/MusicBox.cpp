#include '../common/AnimatedSpriteBatch.cpp';
#include '../common/SpriteBatch.cpp';

class MusicBox : trigger_base, callback_base
{
	
	script@ script;
	scene@ g;
	scripttrigger@ self;
	
	[hidden] bool prev_completed = false;
	[hidden] bool completed = false;
	[entity] int text_1;
	[entity] int text_2;
	
	[position,mode:world,layer:19,y:music_box_y] float music_box_x;
	[hidden] float music_box_y;
	[position,mode:world,layer:19,y:notes_y] float notes_x;
	[hidden] float notes_y;
	[text] int notes_layer = 16;
	[text] int notes_sublayer = 20;
	
	[hidden] bool player_actived = false;
	audio@ music_box_sound = null;
	int cooldown = 60;
	int cooldown_min = 50;
	int cooldown_max = 240;
	
	[text] float scale = 1.5;
	
	sprite_group in_progress_spr(
		array<string>={'props6','npc_2','props1','decoration_6','props1','buildingblocks_10','props1','buildingblocks_10','props1','buildingblocks_10','props1','buildingblocks_10','props1','books_8','props1','books_8','props1','books_8','props1','books_8','props1','books_8','props1','lighting_2','props1','lighting_2','props3','sidewalk_11','props3','sidewalk_11','props3','sidewalk_9','props3','sidewalk_5','props3','sidewalk_5','props3','sidewalk_5','props3','sidewalk_5','props3','sidewalk_9',},
		array<int>={16,19,16,19,16,19,16,19,16,19,16,19,16,19,16,19,16,19,16,19,16,19,16,13,16,13,16,13,16,13,16,19,16,13,16,19,16,19,16,19,16,19,},
		array<float>={0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,},
		array<float>={76.995,-14.5025,77.0676,15.9697,61.9148,-4.10741,53.9148,-3.10741,14.9566,-3.13009,15.9566,-5.13009,24.5495,-10,29.5495,-10,34.5495,-10,39.5495,-10,43.5495,-10,45.7594,-4.14879,24.0141,-8.86428,38.3949,-19.503,92.3949,-4.50301,44.6762,-4.38171,29.5363,-14.9394,64.6971,0.777818,69.6971,-0.222182,71.6971,0.777818,86.6762,-4.38171,},
		array<float>={0,180,69.9994,69.9994,90,90,90,90,90,90,90,249.999,90,0,0,0,99.9976,90,90,90,0,},
		array<float>={0.195928,0.195928,0.195928,0.195928,0.0867249,0.0867249,0.0867249,0.0867249,0.0867249,0.0867249,0.0867249,0.0867249,0.271442,0.271442,0.271442,0.271442,0.271442,0.271442,0.271442,0.271442,0.271442,0.271442,0.271442,0.271442,0.271442,0.271442,0.102078,0.102078,0.102078,0.102078,0.0625984,0.0625984,0.141421,0.141421,0.141421,0.141421,0.141421,0.141421,0.141421,0.141421,0.0625984,0.0625984,},
		array<uint>={0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,},
		array<uint>={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
		array<uint>={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,});
	sprite_group completed_spr(
		array<string>={'props1','books_8','props1','books_8','props1','books_8','props1','books_8','props1','books_8','props1','decoration_7','props1','lighting_2','props1','lighting_2','props1','lighting_3','props1','lighting_3','props1','books_8','props1','books_8',},
		array<int>={16,12,16,12,16,12,16,12,16,12,16,6,16,12,16,12,16,12,16,12,16,13,16,13,},
		array<float>={0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,},
		array<float>={36.6994,-10,42.6994,-10,48.6994,-10,54.6994,-10,60.6994,-10,48.6226,-3.46062,59.8643,-3.98586,37.1357,-3.98586,43.1335,-3.66595,53.8658,-3.66545,41,-2.54949,56,-2.54949,},
		array<float>={90,90,90,90,90,180,180,180,149.996,209.998,0,0,},
		array<float>={0.319497,0.319497,0.319497,0.319497,0.319497,0.319497,0.319497,0.319497,0.319497,0.319497,0.166458,0.166458,0.271442,0.271442,-0.271442,0.271442,0.271442,0.271442,-0.271442,0.271442,0.271442,0.271442,-0.271442,0.271442,},
		array<uint>={0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,},
		array<uint>={0,0,0,0,0,0,0,0,0,0,0,0,},
		array<uint>={0,0,0,0,0,0,0,0,0,0,0,0,});
	sprite_group ballerina_spr(
		array<string>={'props6','npc_2',},
		array<int>={16,12,},
		array<float>={0.5,0.5,},
		array<float>={48.995,-31.5025,},
		array<float>={0,},
		array<float>={0.195928,0.195928,},
		array<uint>={0xFFFFFFFF,},
		array<uint>={0,},
		array<uint>={0,});
	
	simple_transform@ ballerina_transform;
	int ballerina_anim_length;
	float ballerina_frame = 0;
	float ballerina_fps = 12;
	
	sprites@ note_spr;
	array<FloatingNote@> notes;
	
	float song_t = 0;
	float song_interval = 40;
	
	MusicBox()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		if(script.in_game)
		{
			script.vars.listen('main_door_open', this, 'on_main_door_unlock');
			bool new_completed = script.vars.get_bool('main_door_open');
			if(new_completed != completed)
				on_main_door_unlock('', null);
			
			sprites@ spr = cast<sprites>(ballerina_spr.sprite_map['props6']);
			ballerina_anim_length = spr.get_animation_length('npc_2');
			@ballerina_transform = ballerina_spr.sprite_transforms[0];
		}
		
		@note_spr = create_sprites();
		note_spr.add_sprite_set('script');
	}
	
	void on_main_door_unlock(string id, message@ msg)
	{
		completed = script.vars.get_bool('main_door_open');
	}
	
	void activate(controllable@ e)
	{
		if(e.player_index() == -1)
			return;
		
		if(completed != prev_completed)
		{
			entity@ t1 = entity_by_id(text_1);
			entity@ t2 = entity_by_id(text_2);
			if(t1 !is null && t2 !is null)
			{
				const float x = t1.x();
				const float y = t1.y();
				t1.set_xy(t2.x(), t2.y());
				t2.set_xy(x, y);
			}
			
			prev_completed = completed;
			
			if(completed)
			{
				cooldown = 60;
				if(music_box_sound !is null)
				{
					music_box_sound.stop();
					@music_box_sound = null;
				}
			}
		}
		
		player_actived = true;
	}
	
	void step()
	{
		bool spawn_note = false;
		
		if(player_actived)
		{
			if(cooldown == 0 && (music_box_sound is null or !music_box_sound.is_playing()))
			{
				@music_box_sound = g.play_script_stream(
					completed ? 'music_box' : 'mb_sample' + rand_range(1, 3),
					0, music_box_x, music_box_y, false, 1);
				music_box_sound.positional(true);
				cooldown = rand_range(cooldown_min, cooldown_max);
				spawn_note = true;
				song_t = 0;
			}
			
			if(cooldown > 0 && (music_box_sound is null or !music_box_sound.is_playing()))
				cooldown--;
			
			player_actived = false;
		}
		
		if(completed)
		{
			ballerina_frame += ballerina_fps / 60;
			ballerina_transform.frame = int(ballerina_frame) % ballerina_anim_length;
			
			if(music_box_sound !is null and music_box_sound.is_playing())
				if(song_t++ >= song_interval)
				{
					spawn_note = true;
					song_t = 0;
				}
		}
		
		if(spawn_note)
		{
			notes.insertLast(FloatingNote(notes_x, notes_y));
		}
		
		for(int i = int(notes.size()) - 1; i >= 0; i--)
		{
			if(!notes[i].step())
				notes.removeAt(i);
		}
	}
	
	void draw(float sub_frame)
	{
		if(completed)
		{
			completed_spr.draw(-1, -1, music_box_x, music_box_y, 0, scale);
			ballerina_spr.draw(-1, -1, music_box_x, music_box_y, 0, scale);
		}
		else
		{
			in_progress_spr.draw(-1, -1, music_box_x, music_box_y, 0, scale);
		}
		
		for(int i = int(notes.size()) - 1; i >= 0; i--)
		{
			notes[i].draw(note_spr, notes_layer, notes_sublayer);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		
		entity@ t = entity_by_id(text_1);
		if(t !is null)
			draw_arrow(g, 22, 22, self.x(), self.y(), t.x(), t.y(), 2, 10, 0.5, 0xFF00FF00);
		@t = entity_by_id(text_2);
		if(t !is null)
			draw_arrow(g, 22, 22, self.x(), self.y(), t.x(), t.y(), 2, 10, 0.5, 0xFF0000FF);
		
		draw_dot(g, 22, 22, music_box_x, music_box_y, 5, 0x88FF0000, 0);
		draw_dot(g, 22, 22, notes_x, notes_y, 5, 0x8800FF00, 0);
	}
	
}

class FloatingNote
{
	
	float x, y;
	string sprite_name;
	float rotation;
	float vel_x, vel_y, vel_rotation;
	float life = 0;
	float life_max;
	float alpha = 0;
	float scale = 0.75;
	
	FloatingNote(){}
	
	FloatingNote(float x, float y)
	{
		this.x = x;
		this.y = y;
		
		sprite_name = 'note_' + rand_range(1, 2);
		rotation = rand_range(-20, 20);
		vel_y = rand_range(-20.0, -100.0);
		vel_x = rand_range(-40.0, 40.0);
		vel_rotation = rand_range(-40.0, 40.0);
		life_max = rand_range(60, 220);
	}
	
	bool step()
	{
		x += vel_x * DT;
		y += vel_y * DT;
		rotation += vel_rotation * DT;
		
		vel_x *= 0.99;
		vel_y *= 0.99;
		vel_rotation *= 0.99;
		
		if(life++ >= life_max)
		{
			alpha -= 0.05;
			if(alpha <= 0)
				return false;
		}
		else if(alpha < 1)
		{
			alpha += 0.05;
		}
		
		return true;
	}
	
	void draw(sprites@ spr, int layer, int sublayer)
	{
		const uint colour_alpha = uint(alpha * 255) << 24;
		spr.draw_world(layer, sublayer, sprite_name, 0, 0, x, y, rotation, scale, scale, colour_alpha | 0xFFFFFF);
	}
	
}