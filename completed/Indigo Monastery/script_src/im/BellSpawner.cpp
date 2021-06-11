class BellSpawner : trigger_base
{
	script@ script;
	scene@ g;
	scripttrigger@ self;
	
	[text] array<BellSpawnLocation> locations;
	[text] string ring_event = '';
	[text] string hit_event = '';
	
	BellSpawner()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		self.editor_colour_active(0xFF56ff5c);
		self.editor_colour_inactive(0xFF56ff5c);
		self.editor_colour_circle(0xFF56ff5c);
	}
	
	void activate(controllable@ e)
	{
		if(e.player_index() == -1)
			return;
		
		for(int i = int(locations.size()) - 1; i >= 0; i--)
		{
			BellSpawnLocation@ loc = @locations[i];
			BellTrigger bell();
			bell.note = loc.note;
			bell.golden = loc.golden;
			bell.rotation = loc.rotation;
			bell.ring_event = ring_event;
			bell.hit_event = hit_event;
			
			scripttrigger@ st = create_scripttrigger(@bell);
			st.set_xy(loc.x, loc.y);
			g.add_entity(st.as_entity(), true);
		}
		
		g.remove_entity(self.as_entity());
	}
	
	void editor_draw(float sub_frame)
	{
		for(int i = int(locations.size()) - 1; i >= 0; i--)
		{
			BellSpawnLocation@ loc = @locations[i];
			draw_dot(g, 22, 22, loc.x, loc.y, 5, 0x880000FF, 45);
		}
	}
	
}

class BellSpawnLocation
{
	
	[position,mode:world,layer:19,y:y] float x;
	[hidden] float y;
	
	[option,default,a2,b2,c3,cs3,d3,ds3,e3,fs3,g3,gs3,a3,b3,c4,cs4,d4,ds4,e4,fs4,g4,gs4,a4,c5]
	string note = 'default';
	[text] bool golden = false;
	[angle] float rotation = 0;
	
	BellSpawnLocation(){}
	
}