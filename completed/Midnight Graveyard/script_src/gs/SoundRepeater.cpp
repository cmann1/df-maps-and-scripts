class SoundRepeater : trigger_base, callback_base
{
	
	script@ script;
	scene@ g;
	scripttrigger@ self;
	
	[text] string sound_name = "";
	[text] int name_max = 1;
	[text] uint max_playing = 1;
	[slider,min:0,max:1] uint volume_min = 1;
	[slider,min:0,max:1] uint volume_max = 1;
	
	array<audio@> currrently_playing;
	
	SoundRepeater()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
	}
	
	void step()
	{
		for(int i = int(currrently_playing.length() - 1); i >= 0; i--)
		{
			if(@currrently_playing[i] == null or !currrently_playing[i].is_playing())
			{
				currrently_playing.removeAt(i);
			}
		}
		
		if(currrently_playing.length() < max_playing and (rand() % 20) >= 15)
		{
			currrently_playing.insertLast(g.play_sound(sound_name + "_" + (rand() % name_max + 1), self.x(), self.y(), volume_min + (volume_max - volume_min) * frand(), false, true));
		}
	}
	
}