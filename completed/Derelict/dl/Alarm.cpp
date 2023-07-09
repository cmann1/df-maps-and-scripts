class Alarm : Light
{
	
	[hidden] bool activated;
	[hidden] float alarm_snd_timer;
	
	private float frame;
	
	private Sprite glow_spr('props3', 'backdrops_3', 0.5, 0.5);
	private Sprite beam_spr('props3', 'backdrops_4', 0.17, 1.05);
	private int glow_spr_sub_layer = 12;
	private int beam_spr_sub_layer = 6;
	private float glow_spr_scale = 0.45;
	private float beam_spr_scale = 1.2;
	private int beam_spr_rot = 85;
	
	private float rotate_speed = 0.06;
	private float light_offset = 50;
	private float activate_speed = 4;
	
	private float activated_t;
	private uint alarm_clr;
	private uint alarm_off_clr = 0xffffffff;
	private float alarm_sound_time = 24 * 60;
	private float alarm_sound_fade_pitch_time = 0.5 * 60;
	private float alarm_sound_fade_pitch_min = 0.25;
	private float alarm_sound_fade_volume_time = 1 * 60;
	
	private canvas@ c;
	private audio@ alarm_snd;
	
	private MessageHandler@ on_security_updated_cb;
	
	void init(script@ s, scripttrigger@ self) override
	{
		Light::init(s, self);
		
		self.editor_colour_inactive(0xffff7777);
		self.editor_colour_circle(0xffff7777);
		self.editor_colour_active(0xffffaaaa);
		
		activated_t = activated ? 1 : 0;
		alarm_clr = activated ? 0xffffffff : 0x00ffffff;
		
		is_on = activated;
		
		if(activated)
		{
			@c = create_canvas(false, 17, beam_spr_sub_layer);
		}
	}
	
	void on_add() override
	{
		if(!script.is_playing)
			return;
		
		Light::on_add();
		
		if(!activated)
		{
			if(@on_security_updated_cb == null)
			{
				@on_security_updated_cb = MessageHandler(on_security_updated);
			}
			
			script.messages.add_listener('security_updated', on_security_updated_cb);
		}
		
		if(activated && alarm_snd_timer > 0)
		{
			if(script.security_level > 1)
			{
				play_alarm_sound();
			}
			else
			{
				alarm_snd_timer = 0;
			}
		}
	}
	
	void on_remove() override
	{
		if(!script.is_playing)
			return;
		
		Light::on_remove();
		
		if(@on_security_updated_cb != null)
		{
			script.messages.remove_listener('security_updated', on_security_updated_cb);
		}
		
		if(@alarm_snd != null)
		{
			alarm_snd.stop();
			@alarm_snd = null;
		}
	}
	
	void activate(controllable@ c) override
	{
		if(activated)
		{
			EntityOutlinerSource::activate_enter_exit(c);
		}
	}
	
	void step() override
	{
		EntityOutlinerSource::step();
		
		frame += script.time_scale;
		
		if(alarm_snd_timer > 0)
		{
			alarm_snd_timer -= script.time_scale;
			
			if(alarm_snd_timer <= alarm_sound_fade_pitch_time)
			{
				const float t = alarm_snd_timer / (alarm_sound_fade_pitch_time);
				alarm_snd.time_scale(lerp(alarm_sound_fade_pitch_min, 1, t));
			}
			if(alarm_snd_timer <= alarm_sound_fade_volume_time)
			{
				const float t = alarm_snd_timer / (alarm_sound_fade_volume_time);
				alarm_snd.volume(t);
			}
			
			if(alarm_snd_timer <= 0)
			{
				alarm_snd_timer = 0;
				alarm_snd.stop();
				@alarm_snd = null;
			}
		}
		
		if(activated && activated_t < 1)
		{
			activated_t = min(activated_t + activate_speed * DT, 1.0);
			alarm_clr = uint(activated_t * 255) << 24 | 0xffffff;
			alarm_off_clr = uint((1 - activated_t) * 255) << 24 | 0xffffff;
		}
	}
	
	void draw(float sub_frame)
	{
		if(activated && activated_t < 1)
		{
			script.sprites.AlarmOff.draw(src_x, src_y, alarm_off_clr);
		}
		
		SpriteBatch@ spr = activated ? @script.sprites.AlarmOn : @script.sprites.AlarmOff;
		spr.draw(src_x, src_y, activated ? alarm_clr : alarm_off_clr);
		
		if(activated)
		{
			script.sprites.AlarmLightGlow.draw(src_x, src_y, alarm_clr);
			
			const float t = sin(frame * rotate_speed);
			
			c.reset();
			c.translate(src_x, src_y + light_offset);
			c.scale(t, 1.0);
			
			for(int i = -1; i <= 1; i += 2)
			{
				c.push();
				c.scale(i, 1);
				c.sub_layer(beam_spr_sub_layer);
				beam_spr.draw(c, 0, 0, 0, 0, beam_spr_rot, beam_spr_scale, beam_spr_scale, alarm_clr);
				c.scale(0.5, 0.5);
				c.sub_layer(beam_spr_sub_layer + 1);
				beam_spr.draw(c, 0, 0, 0, 0, beam_spr_rot, beam_spr_scale, beam_spr_scale, alarm_clr);
				c.pop();
				
				glow_spr.draw(
					17, glow_spr_sub_layer, 0, 0,
					src_x + glow_spr.sprite_width * 0.35 * glow_spr_scale * t * i,
					src_y + light_offset, 0,
					glow_spr_scale * lerp(0.5, 1.0, abs(t)), glow_spr_scale, alarm_clr);
			}
		}
	}
	
	void editor_draw(float sub_frame) override
	{
		draw(sub_frame);
		
		Light::editor_draw(sub_frame);
	}
	
	private void on_security_updated(const string &in event, message@ msg)
	{
		if(activated)
			return;
		
		is_on = true;
		activated = true;
		script.messages.remove_listener('security_updated', on_security_updated_cb);
		@on_security_updated_cb = null;
		
		play_alarm_sound();
		
		if(@c == null)
		{
			@c = create_canvas(false, 17, beam_spr_sub_layer);
		}
	}
	
	private void play_alarm_sound()
	{
		if(@alarm_snd != null)
			return;
		
		@alarm_snd = script.g.play_script_stream('alarm', 0, self.x(), self.y(), true, 0.0);
		alarm_snd.positional(true);
		// Setting the volume to 0 when creating prevents far away script sounds from playing briefly
		alarm_snd.volume(1.0);
		
		if(alarm_snd_timer == 0)
		{
			alarm_snd_timer = alarm_sound_time;
		}
	}
	
	protected float get_source_centre_y() override
	{
		return Light::get_source_centre_y() + light_offset;
	}
	
}
