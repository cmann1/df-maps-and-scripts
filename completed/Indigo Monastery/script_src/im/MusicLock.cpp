#include '../common/SpriteBatch.cpp';
#include '../common/Sprite.cpp';
#include '../common/Easing.cpp'
#include '../common/Prop.cpp'

enum MusicLockState
{
	Off,
	PowerUp,
	On,
	UnlockStart,
	PlayMelody,
	UnlockRumble,
	PanToDoor,
	Wait,
}

class MusicLock : trigger_base, callback_base
{
	
	script@ script;
	scene@ g;
	scripttrigger@ self;
	
	// Bell order: cs3 e3 a3 cs4 e4 fs4 b2 ds3 gs3 b3 ds4 gs4
	// Code: e4,a3,cs4,e3,a3,-,cs3,-,ds3,gs3,b3,ds4,cs4,-,b3,-,cs4
	array<string> code = {'e4','a3','cs4','e3','a3','-','cs3','-','ds3','gs3','b3','ds4','-','cs4','b3','-','cs4'};
	int code_progress = 0;
	
	[hidden] MusicLockState state = Off;
	
	[hidden] float t;
	[hidden] float key_rotation = 0;
	[hidden] float key_scale = 1;
	[hidden] float key_offset_x = 0;
	[hidden] float key_offset_y = 0;
	[hidden] uint on_glow_alpha = 0;
	
	[position,mode:world,layer:19,y:on_glow_y] float on_glow_x;
	[hidden] float on_glow_y;
	
	[entity] int text_trigger_id;
	[text] string bell_ring_event = '';
	
	float state_timer = 0;
	float state_timer_max = 0;
	
	[text] float beam_speed = 240;
	[text] array<ProgressLocation> progresses;
	
	[position,mode:world,layer:19,y:door_y] float door_x;
	[hidden] float door_y;
	[text] float door_screen_height = 1080;
	[text] float camera_shake_amount = 30;
	[text] float camera_shake_duration = 180;
	
	[position,mode:world,layer:19,y:gear1_y] float gear1_x;
	[hidden] float gear1_y;
	[position,mode:world,layer:19,y:gear2_y] float gear2_x;
	[hidden] float gear2_y;
	Prop@ gear1;
	Prop@ gear2;
	
	array<ScriptCamera@> cameras;
	
	float note_life;
	float note_life_time = 180;
	float note_wait_time = 50;
	
	int num_players;
	
	SpriteBatch on_spr(
		array<string>={'props3','backdrops_3','props3','backdrops_3','props3','backdrops_3','props3','backdrops_3','props3','backdrops_3',},
		array<int>={20,24,20,24,20,24,20,24,20,24,},
		array<float>={-72,-12,-11,32,-10,14,-12,-4,-19.7043,108.778,},
		array<float>={0.319497,0.319497,0.12015,0.12015,0.12015,0.12015,0.12015,0.12015,0.319497,0.319497,},
		array<float>={0,0,0,0,-90,},
		array<uint>={0,0,0,0,0,});
	
	MusicLock()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		add_broadcast_receiver('lock_bell_ring', this, 'on_bell_ring');
		add_broadcast_receiver('music_lock_play_complete', this, 'on_play_complete');
		add_broadcast_receiver('main_door_open_complete', this, 'on_main_door_open_complete');
		
		if(state >= UnlockStart)
		{
			code_progress = int(code.size());
			note_life = note_life_time;
		}
		
		const bool active = state >= UnlockStart;
		for(int i = 0, count = int(progresses.size()); i < count; i++)
			progresses[i].init(beam_speed, active);
		
		num_players = int(num_cameras());
	}
	
	void on_bell_ring(string id, message@ msg)
	{
		if(state != On) return;
		
		const string note = msg.get_string('note');
		int new_progress = 0;
//		new_progress = int(code.size());
 		
		if(note == code[code_progress])
		{
			new_progress = code_progress + 1;
			note_life = note_life_time;
		}
		
		if(new_progress != code_progress)
		{
			code_progress = new_progress;
//			puts(' >> ' + code_progress);
			
			if(code_progress >= int(code.size()))
			{
				state = UnlockStart;
				
				for(int i = num_players - 1; i >= 0; i--)
				{
					camera@ cam = get_camera(i);
					ScriptCamera@ script_camera = ScriptCamera(cam);
					cameras.insertLast(script_camera);
				}
				
				script.players_disabled = true;
				
				state_timer = 0;
				state_timer_max = 50;
				
				for(int i = 0, count = int(progresses.size()); i < count; i++)
					progresses[i].activated_state = 1;
			}
		}
		
		note_life = note_life_time;
		
		if(state == On && code[code_progress] == '-')
		{
			note_life += note_wait_time;
			code_progress++;
		}
	}
	
	void on_play_complete(string id, message@ msg)
	{
		if(state  == PlayMelody)
			state_timer_max = state_timer + 30;
	}
	
	void on_main_door_open_complete(string id, message@ msg)
	{
		for(int i = num_players - 1; i >= 0; i--)
		{
			if(uint(i) < cameras.size())
				cameras[i].release();
		}
		
		script.players_disabled = false;
	}
	
	
	void create_puff(float x , float y, int w, int h, int time=1, int layer=16, int sub_layer=12)
	{
		entity@ emitter = create_emitter(CleansedFb1, x, y, w, h, layer, sub_layer);
		g.add_entity(emitter);
		remove_timer(emitter, time);
	}
	
	void activate(controllable@ e)
	{
		if(e.player_index() == -1)
			return;
		
		if(state == Off && script.vars.get_bool('has_machine_key'))
		{
			state = PowerUp;
			t = 0;
		}
	}
	
	void step()
	{
		if(state == PowerUp)
		{
			if(t < 20)
			{
				key_scale = 1 + t / 20 * 0.1;
			}
			else if(t == 30)
			{
				key_scale = 1;
				create_puff(self.x(), self.y(), 48 * 4, 48 * 2);
				create_puff(self.x(), self.y(), 48 * 2, 48 * 4);
				g.play_sound('sfx_knight_block', self.x(), self.y(), 1, false, true);
			}
			else if(t >= 60 && t < 100)
			{
				if(t == 60)
				{
					audio@ a = g.play_sound('sfx_door_stone_open', self.x(), self.y(), 1, false, true);
					a.time_scale(0.65);
				}
				
				if((key_rotation += 1.5) >= 90)
				{
					key_rotation = 90;
					t = 99;
				}
				else
				{
					t = 61;
				}
			}
			else if(t >= 100 && t <= 130)
			{
				const float factor = 1 - ((t - 100) / 30);
				const float shake_amount = 2 * factor;
				key_offset_x = rand_range(-shake_amount, shake_amount);
				key_offset_y = rand_range(-shake_amount, shake_amount);
				on_glow_alpha = uint(255 - 255 * factor) << 24;
				
				if(t == 100)
				{
					create_puff(self.x(), self.y(), 48 * 2, 48 * 2);
				}
				else if(t == 130)
				{
					state = On;
					entity@ text_trigger = entity_by_id(text_trigger_id);
					if(@text_trigger != null)
						g.remove_entity(text_trigger);
				}
			}

			t++;
		}
		
		else if(state >= On)
		{
			if(state == On)
			{
				if(note_life > 0 && --note_life <= 0)
					code_progress = 0;
			}
			
			else if(state == UnlockStart)
			{
				if(state_timer >= state_timer_max)
				{
					for(int i = num_players - 1; i >= 0; i--)
						cameras[i].shake(camera_shake_amount, camera_shake_duration);
					
					get_gear_props();
					
					state = PlayMelody;
					state_timer = 0;
					state_timer_max = 99999;
				}
			}
			
			else if(state == PlayMelody)
			{
				if(state_timer == 5)
				{
					create_puff(gear1_x, gear1_y - 24, 48 * 2, 48 * 2, 1, 16, 5);
					create_puff(gear2_x, gear2_y - 24, 48 * 2, 48 * 2, 1, 16, 5);
				}
				else if(state_timer == 50)
				{
					message@ msg = create_message();
					msg.set_string('id', 'music_lock');
					msg.set_int('positional', 0);
					msg.set_string('song', 'e4,a3,cs4,e3,a3,-,cs3,-,ds3,gs3,b3,ds4,cs4,-,b3,-,cs4');
					broadcast_message('music_machine_play', msg);
				}
				
				if(@gear1 != null)
					gear1.rotation(gear1.p.rotation() + 1);
				if(@gear2 != null)
					gear2.rotation(gear2.p.rotation() - 1);
				
				if(state_timer >= state_timer_max)
				{
					state = UnlockRumble;
					state_timer = 0;
					state_timer_max = camera_shake_duration;
					g.play_script_stream('rumble', 0, self.x(), self.y(), false, 1).positional(true);
					g.play_script_stream('lock_open_strings', 0, self.x(), self.y(), false, 1);
				}
			}
				
			else if(state == UnlockRumble)
			{
				for(int i = num_players - 1; i >= 0; i--)
					cameras[i].step();
					
				if(@gear1 != null)
					gear1.rotation(gear1.p.rotation() + 1);
				if(@gear2 != null)
					gear2.rotation(gear2.p.rotation() - 1);
				
				if(state_timer >= state_timer_max)
				{
					state = PanToDoor;
					state_timer = 0;
					state_timer_max = 99999;
					
					for(int i = num_players - 1; i >= 0; i--)
					{
						cameras[i].set_target(door_x, door_y);
					}
				}
			}
			
			else if(state == PanToDoor)
			{
				bool busy_panning = false;
				for(int i = num_players - 1; i >= 0; i--)
				{
					cameras[i].step();
					if(cameras[i].has_target)
						busy_panning = true;
				}
				
				if(@gear1 != null)
					gear1.rotation(gear1.p.rotation() + 1);
				if(@gear2 != null)
					gear2.rotation(gear2.p.rotation() - 1);
				
				if(!busy_panning)
				{
					broadcast_message('main_door_unlock', create_message());
					state = Wait;
				}
			}
			
			for(int i = 0, count = int(progresses.size()); i < count; i++)
				progresses[i].step(note_life, note_life_time);
		}
		
		state_timer++;
	}
	
	void get_gear_props()
	{
		int count = g.get_prop_collision(gear1_y - 1, gear1_y + 1, gear1_x - 1, gear1_x + 1);
		for(int i = 0; i < count; i++)
		{
			prop@ p = g.get_prop_collision_index(i);
			if(p.prop_set() == 3 and p.prop_group() == 27 && p.prop_index() == 11)
			{
				@gear1 = Prop(p);
				break;
			}
		}
		
		count = g.get_prop_collision(gear2_y - 1, gear2_y + 1, gear2_x - 1, gear2_x + 1);
		for(int i = 0; i < count; i++)
		{
			prop@ p = g.get_prop_collision_index(i);
			if(p.prop_set() == 3 and p.prop_group() == 27 && p.prop_index() == 11)
			{
				@gear2 = Prop(p);
				break;
			}
		}
	}
	
	void draw(float sub_frame)
	{
		if(state >= PowerUp)
		{
			float x = floor(self.x() / 48) * 48 + 24 + key_offset_x;
			float y = floor(self.y() / 48) * 48 + 24 + key_offset_y;
			script.music_lock_key_spr.draw(18, 1, x, y, key_rotation, key_scale);
			
			x = floor(on_glow_x / 48) * 48;
			y = floor(on_glow_y / 48) * 48;
			on_spr.draw_colour(x, y, on_glow_alpha | 0xFFFFFF);
			g.draw_rectangle_world(19, 2, x - 10, y - 10, x + 68, y + 68, 0, on_glow_alpha | 0xFFFFFF);
		}
		
		if(state >= On)
		{
			for(int i = int(progresses.size()) - 1; i >= 0; i--)
				progresses[i].draw(sub_frame, g);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		for(int i = int(progresses.size()) - 1; i >= 0; i--)
			progresses[i].editor_draw(sub_frame, g);
	}
	
}

class ScriptCamera
{
	
	camera@ cam;
	float start_x;
	float start_y;
	float current_x;
	float current_y;
	float x;
	float y;
	float scale_x;
	float scale_y;
	float shake_force;
	float shake_duration;
	float shake_timer;
	float target_x;
	float target_y;
	bool has_target = false;
	float pan_speed = 1500;
	float vel_x = 0, vel_y = 0;
	float acc = 1;
	float dec_distance = 350;
	
	ScriptCamera(){}
	
	ScriptCamera(camera@ cam)
	{
		@this.cam = cam;
		start_x = current_x = x = cam.x();
		start_y = current_y = y = cam.y();
		cam.script_camera(true);
	}
	
	void shake(float force, float duration)
	{
		shake_force = force;
		shake_duration = shake_timer = duration;
	}
	
	void set_target(float x, float y)
	{
		target_x = x;
		target_y = y;
		has_target = true;
	}
	
	void release()
	{
		cam.script_camera(false);
		reset_camera(cam.player());
	}
	
	void step()
	{
		if(has_target)
		{
			const float dx = target_x - x;
			const float dy = target_y - y;
			const float dist = magnitude(dx, dy);
			const float nx = dist == 0 ? 0 : dx / dist;
			const float ny = dist == 0 ? 0 : dy / dist;
			const float target_speed = max(0, min(pan_speed, dist / dec_distance * pan_speed)) * DT + DT;
			
			if(dist <= target_speed)
			{
				x = target_x;
				y = target_y;
				has_target = false;
			}
			else
			{
				vel_x += nx * acc;
				vel_y += ny * acc;
				const float speed = magnitude(vel_x, vel_y);
				
				if(speed > target_speed)
				{
					vel_x = vel_x / speed * target_speed;
					vel_y = vel_y / speed * target_speed;
				}
				
				x += vel_x;
				y += vel_y;
			}
		}
		else
		{
			vel_x = 0;
			vel_y = 0;
		}
		
		if(current_x != x)
			current_x += (x - current_x) * 0.5;
		if(current_y != y)
			current_y += (y - current_y) * 0.5;
		
		cam.x(current_x);
		cam.y(current_y);
		
		if(shake_timer > 0)
		{
			float shake_amount = (shake_timer / shake_duration) * shake_force;
			shake_amount = rand_range(-shake_amount, shake_amount);
			
			cam.x(current_x + rand_range(-shake_amount, shake_amount));
			cam.y(current_y + rand_range(-shake_amount, shake_amount));
			
			shake_timer--;
		}
	}
	
}

class ProgressLocation
{
	
	[position,mode:world,layer:19,y:y] float x;
	[hidden] float y;
	
	[text] array<ProgressLocationBeam> beams;
	
	float target_progress = 0;
	float progress = 0;
	float change_factor = 0;
	int activated_state = 0;
	float activated_t = 0;
	
	array<bool> beams_completed;
	float beam_speed;
	
	Sprite progress_glow_spr('props3', 'backdrops_3', 0.25, 0.5);
	
	ProgressLocation(){}
	
	void init(float beam_speed, bool active)
	{
		this.beam_speed = beam_speed / 60;
		
		if(active)
		{
			activated_state = 1;
		}
		
		beams_completed.resize(beams.size());
		
		for(int i = int(beams.size()) - 1; i >= 0; i--)
			beams[i].init();
	}
	
	void step(float note_life, float note_life_time)
	{
		target_progress = min(1, note_life / note_life_time);
		
		if(progress != target_progress)
		{
			if(change_factor == 0)
			change_factor = rand_range(2, 3) * DT;
			
			progress += (target_progress - progress) * change_factor;
			
			if(abs(progress - target_progress) < 0.01)
			{
				progress = target_progress;
				change_factor = 0;
			}
		}
		
		if(activated_state == 1)
		{
			for(int i = int(beams.size()) - 1; i >= 0; i--)
			{
				ProgressLocationBeam@ beam = beams[i];
				
				if(beam.progress < beam.length && (beam.after == -1 || beams_completed[beam.after]))
				{
					beam.progress += beam_speed;
					
					if(beam.progress >= beam.length)
					{
						beam.progress = beam.length;
						beams_completed[i] = true;
					}
				}
			}
			
			activated_t++;
		}
	}
	
	void draw(float sub_frame, scene@ g)
	{
		if(progress == 0) return;
		
		const uint colour = (uint(min(1, progress) * 255) << 24) | 0xFFFFFF;
		const float height = y - 96 * min(1, progress);
		progress_glow_spr.draw_world(20, 24, 1, 1, x, y, -90, 0.15 * progress, 0.4, colour);
		g.draw_rectangle_world(19, 2, x - 48, height, x + 48, y + 10, 0, colour);
		progress_glow_spr.draw_world(19, 2, 1, 1, x, height + 5, -90, 0.02, 0.4, colour);
		
		if(activated_state == 1)
		{
			for(int i = int(beams.size()) - 1; i >= 0; i--)
			{
				ProgressLocationBeam@ beam = beams[i];
				if(beam.progress > 0)
				{
					const float t = beam.progress / beam.length;
					g.draw_line(20, 24, beam.x1, beam.y1, beam.x1 + beam.dx * t, beam.y1 + beam.dy * t, 10, 0xFFEAD0FF);
				}
				
			}
		}
		else if(activated_state == 2)
		{
		}
	}
	
	void editor_draw(float sub_frame, scene@ g)
	{
		draw_dot(g, 22, 22, x, y, 5, 0x770000FF);
		
		for(int i = int(beams.size()) - 1; i >= 0; i--)
		{
			ProgressLocationBeam@ beam = beams[i];
			draw_arrow(g, 22, 22, beam.x1, beam.y1, beam.x2, beam.y2, 2, 6, 1, 0x99FF0000);
		}
	}
}

class ProgressLocationBeam
{
	
	[text] int after = -1;
	[position,mode:world,layer:19,y:y1] float x1;
	[hidden] float y1;
	[position,mode:world,layer:19,y:y2] float x2;
	[hidden] float y2;
	
	float dx, dy;
	float length;
	float progress = 0;
	
	ProgressLocationBeam(){}
	
	void init()
	{
		dx = x2 - x1;
		dy = y2 - y1;
		length = sqrt(dx * dx + dy * dy);
	}
	
}