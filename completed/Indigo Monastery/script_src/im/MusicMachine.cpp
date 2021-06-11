class MusicMachine : enemy_base, callback_base
{
	
	script@ script;
	scene@ g;
	scriptenemy@ self;
	
	[text] string machine_id = '';
	[entity] int text_trigger_id;
	[text] int bpm = 60;
	[text] float volume = 1;
	[text] bool play_sound = true;
	
	float t = 0;
	int current_song_index = -1;
	int current_song_progress;
	int current_song_size;
	array<string>@ current_song;
	float ticks_per_beat;
	bool positional = true;
	
	MusicMachine()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scriptenemy@ self)
	{
		@this.script = s;
		@this.self = self;
		
		self.on_hurt_callback(this, 'on_hurt', 0);
		self.auto_physics(false);
		
		ticks_per_beat = 60.0 / bpm * 60;
		
		add_broadcast_receiver('music_machine_play', this, 'on_play_request');
	}
	
	void on_play_request(string id, message@ msg)
	{
		if(machine_id != ''  && msg.get_string('id') != machine_id)
			return;
		
		if(msg.has_int('positional'))
			positional = msg.get_int('positional') != 0;
		
		this.play_song(msg.get_string('song'));
	}
	
	void on_hurt(controllable@ attacked, controllable@ attacker, hitbox@ attack_hitbox, int arg)
	{
		if(machine_id != '')
			return;
		
		if(text_trigger_id != 0)
		{
			entity@ txt = entity_by_id(text_trigger_id);
			if(@txt != null)
				play_song(txt.vars().get_var("text_string").get_string());
		}
	}
	
	void play_song(string song)
	{
		current_song_index = 0;
		current_song_progress = 0;
		@current_song = array<string> = {};
		int song_chr_index = -1;
		int index = 0;
		
		for(int i = 0, count = int(song.size()); i < count; i++)
		{
			string chr = ' ';
			chr[0] = song[i];
			
			if(chr == ' ')
				continue;
			if(chr == ',' || chr == '\n')
			{
				index++;
				continue;
			}
			
			{
				if(song_chr_index  != index)
				{
					current_song.insertLast(chr);
					song_chr_index  = index = current_song.size() - 1;
				}
				else
					current_song[index] += chr;
			}
		}
		
		current_song_size = int(current_song.size());
		t = ticks_per_beat;
	}
	
	void step()
	{
		if(current_song_index >= 0 && t++ >= ticks_per_beat)
		{
			do
			{
				string note = current_song[current_song_progress++];
				if(note != '-' && play_sound)
				{
					audio@ ring_sound = g.play_script_stream('bell_' + note, 0, self.x(), self.y(), false, volume);
					if(positional)
						ring_sound.positional(true);
				}
				
				if(note != '-' and machine_id != '')
				{
					message@ msg = create_message();
					msg.set_string('id', machine_id);
					msg.set_string('note', note);
					msg.set_int('particles', 1);
					broadcast_message(machine_id + '_play_note', msg);
				}
				
				if(current_song_progress < current_song_size && current_song[current_song_progress] == '+')
					current_song_progress++;
				else
					break;
			}
			while(true);
			
			if(current_song_progress >= current_song_size)
			{
				current_song_index = -1;
				
				if(machine_id != '')
				{
					message@ msg = create_message();
					broadcast_message(machine_id + '_play_complete', msg);
				}
			}
			
			t  = 0;
		}
	}
	
//	void draw(float sub_frame)
//	{
//		const float size = 20;
//		g.draw_rectangle_world(18, 0, self.x() - size, self.y() - size, self.x() + size, self.y() + size, 45, 0x88a2e9ff);
//	}
	
	void editor_draw(float sub_frame)
	{
		
		const float size = 20;
		g.draw_rectangle_world(18, 0, self.x() - size, self.y() - size, self.x() + size, self.y() + size, 45, 0x88a2e9ff);
		
//		draw(sub_frame);
		
		if(text_trigger_id != 0)
		{
			entity@ txt = entity_by_id(text_trigger_id);
			if(@txt != null)
				draw_arrow(g, 22, 22, self.x(), self.y(), txt.x(), txt.y(), 4, 20, 0.5, 0x99FF0000);
		}
	}
	
}