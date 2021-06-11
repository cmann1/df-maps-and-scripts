#include '../common/Sprite.cpp';
#include '../common/CameraRumble.cpp';

class CrystalPillar : trigger_base, callback_base
{
	
	script@ script;
	scene@ g;
	scripttrigger@ self;
	
	[text] float bpm1 = 105;
	[text] float bpm2 = 220;
	[text] float volume = 0.85;
	[text] float delay = 100;
	
	[text] array<CrystalLocation> crystals;
	[text] int crystal_layer = 14;
	[text] int crystal_sublayer = 19;
	[text] int crystal_sublayer2 = 24;
	
	array<string> song1 = {'e4','a3','cs4','e3','a3','-','cs3','-','ds3','gs3','b3','ds4','cs4','-','b3','-','cs4'};
	array<string> song2 = {
		'e4','a3','cs4','e3','a3','-','cs3','-','ds3','gs3','b3','ds4','cs4','-','b3','-','cs4',
		'-','-', '-','-', '-',
		'e4','+','a1_long','a3','cs4','e3','a3','-','cs3','-','ds3','+','e1_long','gs3','b3','ds4','cs4','-','b3','-','cs4','+','cs1_long','fs3','gs3','fs3','cs3','cs2','fs2','gs2','cs2_chord'
	};
	array<string>@ song;
	int song_size;
	
	int playing = 0;
	float t = 0;
	int song_progress;
	float ticks_per_beat;
	float ticks_per_beat1;
	float ticks_per_beat2;
	int current_crystal = -1;
	
	Sprite glow_spr('props3', 'backdrops_3');
	
	CrystalPillar()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		self.editor_colour_active(0xBBb88aff);
		self.editor_colour_inactive(0xBB883aff);
		
		add_broadcast_receiver('start_crystal_music', this, 'on_start_crystal_music');
		
		ticks_per_beat1 = 60.0 / bpm1 * 60;
		ticks_per_beat2 = 60.0 / bpm2 * 60;
	}
	
	void on_start_crystal_music(string id, message@ msg)
	{
		playing = 1;
		update_song(song1, ticks_per_beat1);
		
		g.play_script_stream('resonance', 0, self.x(), self.y(), false, volume).positional(true);
		g.play_script_stream('rumble', 0, self.x(), self.y(), false, 0.5).positional(true);
		
		add_camera_rumble(self.x(), self.y(), 10, 90);
		
		t = -120;
	}
	
	void update_song(array<string>@ new_song, float ticks_per_beat)
	{
		this.ticks_per_beat = ticks_per_beat;
		
		@song = @new_song;
		song_size = uint(song.size());
		current_crystal = -1;
	}
	
	void make_crystal_glow(int index)
	{
		CrystalLocation@ crystal = crystals[index];
		crystal.direction = 25;
		
		entity@ emitter = create_emitter(KingZoneBlue, crystal.x, crystal.y, 64, 64, 15, 12);
		g.add_entity(emitter);
		remove_timer(emitter, 10);
	}
	
	void step()
	{
		switch(playing)
		{
			case 1:
			case 3:
			{
				if(t++ >= ticks_per_beat)
				{
					do
					{
						string note = song[song_progress++];
						if(note != '-')
						{
							int new_crystal;
							do
							{
								new_crystal = rand_range(0, int(crystals.size()) - 1);
							}
							while(new_crystal == current_crystal);
							
							current_crystal = new_crystal;
							make_crystal_glow(current_crystal);
							CrystalLocation@ crystal = crystals[current_crystal];
							
							audio@ ring_sound = g.play_script_stream('crystal_' + note, 0, crystal.x, crystal.y, false, volume);
							ring_sound.positional(true);
						}
						
						if(song_progress < song_size && song[song_progress] == '+')
							song_progress++;
						else
							break;
					}
					while(true);
					
					if(song_progress >= song_size)
					{
						playing++;
						song_progress = 0;
					}
					
					t  = 0;
				}
			} break;
			case 2:
			{
				if(t++ >= delay)
				{
					t = 0;
					playing = 3;
					update_song(@song2, ticks_per_beat2);
				}
			} break;
		}
		
		for(int i = int(crystals.size()) - 1; i >= 0; i--)
			crystals[i].step();
	}
	
	void draw(float sub_frame)
	{
		for(int i = int(crystals.size()) - 1; i >= 0; i--)
			crystals[i].draw(g, @glow_spr, crystal_layer, crystal_sublayer, crystal_sublayer2);
	}
	
	void editor_draw(float sub_frame)
	{
		for(int i = int(crystals.size()) - 1; i >= 0; i--)
			crystals[i].editor_draw(g);
	}
	
}

class CrystalLocation
{
	
	[position,mode:world,layer:19,y:y] float x;
	[hidden] float y;
	[angle] float angle;
	[text] float scale_x = 0.4;
	[text] float scale_y = 0.4;
	
	float direction = 0;
	float glow_amount = 0;
	float glow_max = 1;
	
	CrystalLocation(){}
	
	void step()
	{
		if(direction >= 1)
		{
			glow_amount += (glow_max - glow_amount) * 0.75;
			if(abs(glow_max - glow_amount) < 0.05)
				direction--;
		}
		else if(glow_amount > 0)
		{
			glow_amount += -glow_amount * 0.25;
			if(glow_amount < 0.05)
				glow_amount = 0;
		}
	}
	
	void draw(scene@ g, Sprite@ glow_spr, int layer, int sublayer, int sublayer2)
	{
		if(glow_amount > 0)
		{
			const uint alpha = uint(min(1, glow_amount) * 255) << 24;
			const float ox = cos((angle) * DEG2RAD) * 30;
			const float oy = sin((angle) * DEG2RAD) * 30;
			glow_spr.draw_world(layer, sublayer, 0, 0, x, y, angle, scale_x, scale_y, alpha | 0xFFFFFF);
			glow_spr.draw_world(layer, sublayer, 0, 0, x, y, angle, scale_x * 0.5, scale_y * 0.5, alpha | 0xFFFFFF);
			glow_spr.draw_world(layer, sublayer2, 0, 0, x - ox, y - oy, angle, scale_x * 0.5, scale_y * 0.75, alpha | 0xFFFFFF);
		}
	}
	
	void editor_draw(scene@ g)
	{
		const float rad = angle * DEG2RAD;
		const float length = 20;
		draw_dot(g, 22, 22, x, y, 5, 0x88FF0000, 0);
		g.draw_line_world(22, 22, x, y, x + cos(rad) * length, y + sin(rad) * length, 2, 0x88FF0000);
	}
	
}







