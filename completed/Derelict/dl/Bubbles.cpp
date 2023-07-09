class Bubbles : trigger_base
{
	
	[persist] float frequency = 0.5;
	float volume = 1.5;
	
	script@ script;
	scripttrigger@ self;
	
	float t;
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = script;
		@this.self = self;
		
		self.radius(0);
		
		self.editor_colour_inactive(0xff518e37);
		self.editor_colour_active(0xffff7777);
		
		t_rand();
	}
	
	void step()
	{
		t -= DT;
		
		if(t <= 0)
		{
			t_rand();
			float volume = this.volume;
			while(volume > 0)
			{
				audio@ snd = script.g.play_sound(
					'sfx_slime_light_overlay_' + rand_range(1, 6), self.x(), self.y(),
					volume * rand_range(0.75, 1.0), false, true);
				snd.time_scale(rand_range(0.5, 1.5));
				volume -= 1;
			}
		}
	}
	
	void editor_step()
	{
		step();
	}
	
	private void t_rand()
	{
		t += rand_range(frequency * 0.65, frequency * 1.35);
	}
	
}
