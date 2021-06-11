#include '../common/sprite_group.cpp'
#include '../common/ColType.cpp'
#include '../common/Fx.cpp';
#include '../common/PropGroupNames.cpp';

class BellTrigger : trigger_base, callback_base
{
	
	script@ script;
	scene@ g;
	scripttrigger@ self;
	
	sprite_group spr;
	
	array<FadingAudio> fading_audio;
	
	float spr_offset_x = 0;
	float spr_offset_y = 0;
	float spr_offset_prev_x = 0;
	float spr_offset_prev_y = 0;
	
	float hitbox_left   = -50;
	float hitbox_top    = -40;
	float hitbox_right  = 50;
	float hitbox_bottom = 50;
	
	bool force_hit = false;
	int force_damage;
	float hit_timer = 0;
	float hit_timer_max = 20;
	float shake_amount = 3;
	float shake_timer = 0;
	float shake_timer_max = 350;
	float shake_swing = 0;
	float shake_swing_max = 13;
	float shake_swing_vel = 0;
	float shake_swing_gravity = 0.5;
	float swing_vel_max = 3;
	float swing_hit_force = 0.25;
	
	[hidden] bool has_been_hit = false;
	[text] bool golden = false;
	[angle] float rotation = 0;

	[option,default,a2,b2,c3,cs3,d3,ds3,e3,fs3,g3,gs3,a3,b3,c4,cs4,d4,ds4,e4,fs4,g4,gs4,a4,c5]
	string note = 'default';
	
	[text] string ring_event = '';
	[text] string hit_event = '';
	
	BellTrigger()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		if(script.in_game)
		{
//			removeFallbackProps();

			if(has_been_hit)
				create_bell_sprite();
			
			if(hit_event != '')
			{
				add_broadcast_receiver(hit_event, this, 'on_hit_event');
			}
		}
	}
	
	void create_bell_sprite()
	{
		spr.populate(
			array<string>={'props3','facade_12','props3','facade_11','props1','chains_5','props2','foliage_16','props3','facade_12','props1','foliage_18','props1','foliage_18','props1','foliage_18','props2','foliage_16','props2','foliage_16','props2','foliage_16','props2','foliage_16','props1','facade_2','props1','facade_2','props1','foliage_18','props1','facade_2','props1','foliage_6','props2','foliage_16','props1','foliage_18','props1','foliage_18','props1','foliage_18','props1','foliage_18','props1','foliage_18','props2','foliage_16','props2','foliage_16','props1','foliage_18','props1','foliage_18','props1','foliage_18','props1','foliage_18','props2','foliage_16','props2','foliage_16','props2','foliage_16','props1','foliage_18','props1','foliage_18','props2','foliage_16','props2','foliage_17','props2','foliage_16','props1','foliage_18','props2','foliage_16','props2','foliage_16','props1','foliage_18','props2','foliage_16','props2','foliage_16','props2','foliage_16','props1','foliage_18','props2','foliage_16','props2','foliage_16','props1','foliage_18','props1','foliage_18','props1','foliage_18','props2','foliage_16','props2','foliage_16','props2','foliage_16','props2','foliage_16','props1','foliage_18','props2','foliage_16','props1','foliage_18','props3','facade_11','props3','facade_11','props3','facade_11','props3','facade_12','props3','facade_12',},
			golden
				? array<int>={16,16,16,15,16,16,16,18,16,15,16,18,16,18,16,18,16,18,16,18,16,18,16,18,16,14,16,14,16,18,16,14,16,17,16,17,16,17,16,17,16,18,16,18,16,17,16,17,16,17,16,18,16,18,16,18,16,17,16,18,16,17,16,17,16,18,16,17,16,18,16,17,16,18,16,17,16,18,16,18,16,18,16,18,16,18,16,18,16,18,16,18,16,18,16,18,16,18,16,18,16,17,16,18,16,17,16,17,16,17,16,17,16,18,16,15,16,15,16,15,16,16,16,15,}
				: array<int>={12,16,12,15,12,16,12,18,12,15,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,14,12,14,12,18,12,14,12,17,12,17,12,17,12,17,12,18,12,18,12,17,12,17,12,17,12,18,12,18,12,18,12,17,12,18,12,17,12,17,12,18,12,17,12,18,12,17,12,18,12,17,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,17,12,18,12,17,12,17,12,17,12,17,12,18,12,15,12,15,12,15,12,16,12,15,},
			array<float>={0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,},
			array<float>={-57.0005,132.603,-27.5005,135.603,-1.00049,68.103,38.4995,139.603,42.9995,112.603,54.4995,141.603,9.18726,95.4023,14.8454,98.3417,-15.5005,97.603,-25.5005,91.603,-25.5005,94.603,16.4995,106.603,-2.50049,125.603,-2.50049,129.603,10.4995,111.603,-2.50049,135.603,-20.0005,122.603,-47.5005,126.603,-49.8042,111.538,-46.8464,126.341,23.4995,136.603,-16.5005,135.603,-46.0005,126.103,-47.5005,133.603,-49.5005,137.603,-56.5005,134.603,25.4995,141.603,-12.5005,141.603,-47.5005,140.603,-16.5005,86.603,-37.5005,61.603,-24.5005,66.603,9.99951,71.103,-31.8464,73.341,-15.5005,79.603,-34.7973,88.1456,37.4995,108.603,39.4995,115.603,44.4995,136.603,32.4995,134.603,40.9086,67.5877,38.4995,70.603,40.4995,80.603,42.4995,92.603,42.9995,86.103,37.4995,96.603,33.4995,62.603,52.4995,134.603,-0.500489,25.603,-7.26247,30.4489,-8.50049,30.603,22.4995,49.603,-20.5005,47.603,-29.5005,51.603,-22.3518,55.001,-36.5005,58.603,29.3513,51.0016,24.4995,134.603,37.4995,135.603,0.499512,135.603,53.9995,132.603,-42.0005,109.603,},
			array<float>={0,0,0,0,90,0,299.998,259.997,0,0,0,0,0,0,0,0,90,0,339.999,99.9976,0,180,90,0,0,180,0,0,0,0,0,0,270,99.9976,0,279.998,0,0,0,0,69.9994,0,0,0,90,0,0,0,180,9.99756,0,0,0,0,139.999,0,39.9957,0,0,0,0,90,},
			array<float>={1,1,1,1,1,1,1,1,-1,1,1,1,1,1,-1,1,1,1,1,1,1,1,1,1,1,1,1,1,-1,1,1,1,1,1,1,1,1,1,1,1,1,-1,1,1,1,-1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,-1,1,1,1,1,1,1,1,-1,1,1,1,1,1,-1,-1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,-1,1,1,-1,1,1,1,-1,1,1,1,},
			array<uint>={0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,},
			array<uint>={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
			array<uint>={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,}
		);
	}
	
	void removeFallbackProps()
	{
		create_bell_sprite();
		
		dictionary sprite_map;
		for(int i = 0, count = int(spr.sprite_sets.size()); i < count; i++)
		{
			simple_transform@ tr = @spr.sprite_transforms[i];
			sprite_map[spr.sprite_sets[i] + '_' + spr.sprite_names[i] + '_' + tr.layer + '.' + tr.sub_layer] = true;
		}
		
		const float x = self.x();
		const float y = self.y();
		int count = g.get_prop_collision(
			y + hitbox_top - 20,  y + hitbox_bottom + 10,
			x + hitbox_left - 10, x + hitbox_right + 10);
		
		for(int i = 0; i < count; i++)
		{
			prop@ p = g.get_prop_collision_index(i);
			string sprite_set, sprite_name;
			sprite_from_prop(p, sprite_set, sprite_name);
			
			if(sprite_map.exists(sprite_set + '_' + sprite_name + '_' + p.layer() + '.' + p.sub_layer()))
				g.remove_prop(p);
		}
	}
	
	void on_hit_event(string id, message@ msg)
	{
		if(msg.get_string('note') == note)
		{
			force_hit = true;
			
			force_damage = msg.has_int('damage')
				? msg.get_int('damage')
				: 3;
			swing(msg.has_int('direction')
				? msg.get_int('direction')
				: rand_range(-180, 180), force_damage);
			
			if(msg.has_int('particles') && msg.get_int('particles') == 1)
			{
				entity@ emitter = create_emitter(KingZoneBlue, self.x(), self.y(), 48, 48, 18, 12);
				g.add_entity(emitter);
				remove_timer(emitter, 10);
			}
		}
	}
	
	void swing(int direction, int damage)
	{
		if(rotation != 0) return;
		
		shake_swing_vel += sgn(direction) * -damage * swing_hit_force;
		if(shake_swing_vel < -swing_vel_max) shake_swing_vel = -swing_vel_max;
		else if(shake_swing_vel > swing_vel_max) shake_swing_vel = swing_vel_max;
	}
	
	void step()
	{
		for(int i = int(fading_audio.size()) - 1; i >= 0; i--)
		{
			FadingAudio@ a = fading_audio[i];
			if(a.current_fade_time-- <= 0 or !a.sound.is_playing())
			{
				a.sound.stop();
				fading_audio.removeAt(i);
				continue;
			}
			
			a.sound.volume(a.current_fade_time / float(a.fade_time) * a.max_volume);
		}
		
		if(hit_timer > 0)
		{
			hit_timer--;
		}
		
		if(shake_timer > 0)
		{
			if(--shake_timer == 0)
			{
				spr_offset_x = spr_offset_prev_x = 0;
				spr_offset_y = spr_offset_prev_y = 0;
			}
			else
			{
				spr_offset_prev_x = spr_offset_x;
				spr_offset_prev_y = spr_offset_y;
				const float shake_factor = shake_timer / shake_timer_max * shake_amount;
				spr_offset_x = rand_range(-shake_factor, shake_factor);
//				spr_offset_y = rand_range(-shake_factor, shake_factor);
			}
		}
		
		const float x = self.x();
		const float y = self.y();
		int count = g.get_entity_collision(
			y + hitbox_top, y + hitbox_bottom,
			x + hitbox_left, x + hitbox_right,
			COL_TYPE_HITBOX
		);
		
		int hit_damage = 0;
		int hit_fx_framerate;
		int attack_dir;
		float hit_fx_x, hit_fx_y;
		bool has_impact = false;
		string hit_fx;
		
		if(force_hit)
		{
			hit_damage = force_damage;
			hit_timer = 0;
			force_hit = false;
		}
		else
		{
			for(int i = 0; i < count; i++)
			{
				hitbox@ hit = g.get_entity_collision_index(i).as_hitbox();
				if(hit is null) continue;
				controllable@ owner = hit.owner();
				
				if(hit.triggered() && hit.state_timer() == hit.activate_time())
				{
					if(hit.damage() > hit_damage)
					{
						hit_damage = hit.damage();
						hit_fx = hit.attack_effect();
						hit_fx_framerate = hit.effect_frame_rate();
						attack_dir = hit.attack_dir();
						rectangle@ r = hit.base_rectangle();
						hit_fx_x = hit.x() + r.left() + r.get_width() * 0.5;
						hit_fx_y = hit.y() + r.top() + r.get_height() * 0.5;
						
						swing(hit.attack_dir(), hit_damage);
					}
					
					has_impact = true;
				}
			}
		}
		
		if(hit_damage > 0)
		{
			if(!has_been_hit)
			{
				removeFallbackProps();
				has_been_hit = true;
			}
			
			const float multiplier = hit_damage == 1 ? 0.65 : 1;
			
			if(hit_timer == 0)
			{
				string sound_name = '';
				
				if(note == 'default')
					sound_name = hit_damage == 1 ? 'bell_02_light' : 'bell_02';
				else
					sound_name = 'bell_' + note;
				
				if(sound_name != '')
				{
					audio@ ring_sound = g.play_script_stream(sound_name, 0, self.x(), self.y(), false, multiplier);
					ring_sound.positional(true);
					
					if(note != 'default' && hit_damage == 1)
						fading_audio.insertLast(FadingAudio(ring_sound, 2.5));
				}
				hit_timer = hit_timer_max * (hit_damage == 1 ? 0.5 : 1.0);
				
				if(has_impact)
				{
					const string impact = hit_damage < 3 ? 'light' : 'heavy';
					g.play_sound('sfx_impact_' + impact + '_' + rand_range(1, 3), x, y, 1, false, true);
				}
				
				spawn_fx(hit_fx_x, hit_fx_y, 'effects', hit_fx, 0, hit_fx_framerate, attack_dir - 90, 1, 1);
				
				if(ring_event != '')
				{
					message@ msg = create_message();
					msg.set_string('note', note);
					broadcast_message(ring_event, msg);
				}
			}
			
			shake_timer = shake_timer_max * multiplier;
		}
		
		if(rotation == 0)
		{
			shake_swing += shake_swing_vel;
			if(shake_swing < 0)
				shake_swing_vel += shake_swing_gravity * DT;
			else if(shake_swing > 0)
				shake_swing_vel -= shake_swing_gravity * DT;
			if(shake_swing > shake_swing_max)
			{
				if(shake_swing_vel > 0) shake_swing_vel = 0;
			}
			else if(shake_swing < -shake_swing_max)
			{
				if(shake_swing_vel < 0) shake_swing_vel = 0;
			}
			shake_swing_vel *= 0.98;
		}
	}
	
	void draw_collision()
	{
		const float x = self.x();
		const float y = self.y();
		
		g.draw_rectangle_world(22, 22, 
			x + hitbox_left, y + hitbox_top,
			x + hitbox_right, y + hitbox_bottom,
			0, 0x66FFFFFF
		);
	}
	
	void draw(float sub_frame)
	{
		const float x = self.x();
		const float y = self.y();
		
		float sprite_anchor_x = 0;
		float sprite_anchor_y = -96;
		
		if(rotation != 0)
			rotate(sprite_anchor_x, sprite_anchor_y, rotation * DEG2RAD, sprite_anchor_x, sprite_anchor_y);
		
		const float sprite_x = x + sprite_anchor_x + lerp(spr_offset_prev_x, spr_offset_x, sub_frame);
		const float sprite_y = y + sprite_anchor_y + lerp(spr_offset_prev_y, spr_offset_y, sub_frame);
		spr.draw(-1, -1, sprite_x, sprite_y, rotation + shake_swing, 1);
	}
	
	void editor_draw(float sub_frame)
	{
//		if(self.editor_selected())
			draw(sub_frame);
		if(self.editor_selected())
			draw_collision();
	}
	
}

class FadingAudio
{
	
	audio@ sound;
	int fade_time;
	int current_fade_time;
	float max_volume;
	
	FadingAudio(){}
	
	FadingAudio(audio@ sound, float fade_seconds)
	{
		@this.sound = sound;
		this.fade_time = current_fade_time = int(fade_seconds * 60);
		max_volume = sound.volume();
	}
	
}