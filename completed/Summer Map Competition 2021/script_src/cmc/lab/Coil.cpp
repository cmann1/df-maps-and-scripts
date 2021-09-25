#include '../../lib/drawing/SpriteGroup.cpp';

#include 'ArcContact.cpp';
#include 'ArcSource.cpp';
#include 'ArcEndPoint.cpp';
#include 'ArcContactList.cpp';
#include 'Arc.cpp';
#include 'DamageType.cpp';

class Coil : trigger_base
{
	
	// I had to remove the annotations from fields marked with "LAB UNUSED" to try and help with
	// checkpoint lag spikes in the cmclab map
	
	[persist] array<ArcSource> sources(1);
	[persist] array<ArcEndPoint> contacts(1);
	[persist|tooltip:'When on, arcs will spawn independently from each source.\nWhen off, a random source will be chosen and the timing will be\nbased on the first source frequency'] /* LAB UNUSED */
	bool independent_sources = true;
	[persist] bool on = true;
	[option,0:Off,1:Stun,2:Kill,3:KillAndStun] /* LAB UNUSED */
	DamageType kill_player = Off;
	[option,0:Off,1:Stun,2:Kill,3:KillAndStun] /* LAB UNUSED */
	DamageType kill_enemy = Off;
	[option,0:Off,1:Stun,2:Kill,3:KillAndStun] /* LAB UNUSED */
	DamageType kill_apple = Off;
	[persist] /* LAB UNUSED */
	int player_stun = 48;
	[persist] /* LAB UNUSED */
	int enemy_stun = 48;
	[persist] /* LAB UNUSED */
	int apple_stun = 48;
	
	script@ script;
	scripttrigger@ self;
	
	ArcContactList sourcesList;
	ArcContactList contactsList;
	
	private int arcs_size = 8;
	private int num_arcs = 0;
	private array<Arc@> arcs(arcs_size);
	
	SpriteRectangle bounds;
	bool in_view = true;
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = script;
		@this.self= self;
		
		self.radius(0);
		self.editor_colour_inactive(0xff4277ec);
		self.editor_colour_active(0xff729eff);
		self.editor_colour_circle(0xff4277ec);
		self.editor_handle_size(7);
		
		@sourcesList.script = script;
		@contactsList.script = script;
		sourcesList.initialise(@sources);
		contactsList.initialise(@contacts);
		
		for(int i = 0; i < arcs_size; i++)
		{
			@arcs[i] = Arc(script);
		}
		
		update_bounds();
		prime();
	}
	
	private void update_bounds()
	{
		bounds.left = bounds.right = self.x();
		bounds.top = bounds.bottom = self.y();
		
		for(uint i = 0; i < sources.length; i++)
		{
			add_contact_bounds(@sources[i]);
		}
		
		for(uint i = 0; i < contacts.length; i++)
		{
			add_contact_bounds(@contacts[i]);
		}
		
		const float padding = 100;
		bounds.left -= padding;
		bounds.top -= padding;
		bounds.right += padding;
		bounds.bottom += padding;
	}
	
	private void add_contact_bounds(ArcContact@ contact)
	{
		bounds.left = min(bounds.left, contact.x1);
		bounds.top = min(bounds.top, contact.y1);
		bounds.right = max(bounds.right, contact.x1);
		bounds.bottom = max(bounds.bottom, contact.y1);
		switch(contact.type)
		{
			case ArcContact::Point:
				bounds.left = min(bounds.left, contact.x1 - contact.radius);
				bounds.top = min(bounds.top, contact.y1 - contact.radius);
				bounds.right = max(bounds.right, contact.x1 + contact.radius);
				bounds.bottom = max(bounds.bottom, contact.y1 + contact.radius);
				break;
			case ArcContact::Line:
			case ArcContact::Rect:
			case ArcContact::Tiles:
				bounds.left = min(bounds.left, contact.x2);
				bounds.top = min(bounds.top, contact.y2);
				bounds.right = max(bounds.right, contact.x2);
				bounds.bottom = max(bounds.bottom, contact.y2);
				break;
		}
	}
	
	void step()
	{
		in_view = script.in_view(bounds.left, bounds.top, bounds.right, bounds.bottom);
		if(!in_view)
			return;
		
		for(int i = num_arcs - 1; i >= 0; i--)
		{
			if(!arcs[i].step())
			{
				Arc@ t = arcs[i];
				@arcs[i] = arcs[num_arcs - 1];
				@arcs[--num_arcs] = t;
			}
		}
		
		if(sources.length == 0 || !on)
			return;
		
		if(!independent_sources)
		{
			try_spawn_arc(@sources[0]);
		}
		else
		{
			for(uint i = 0; i < sources.length; i++)
			{
				try_spawn_arc(@sources[i]);
			}
		}
	}
	
	void editor_step()
	{
		step();
	}
	
	void draw(float sub_frame)
	{
		if(!in_view || !script.should_draw(bounds.left, bounds.top, bounds.right, bounds.bottom))
			return;
		
		for(int i = 0; i < num_arcs; i++)
		{
			arcs[i].draw(sub_frame);
		}
		
		//outline_rect(script.g, 22, 24, bounds.left, bounds.top, bounds.right, bounds.bottom, 2, 0x22ffffff);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		
		if(!self.editor_selected())
			return;
		
		for(uint i = 0; i < sources.length; i++)
		{
			sources[i].editor_draw(true);
		}
		for(uint i = 0; i < contacts.length; i++)
		{
			contacts[i].editor_draw(false);
		}
	}
	
	void editor_var_changed(var_info@ var)
	{
		const string base_name = var.get_name(0);
		
		if(base_name == 'sources')
			sourcesList.initialise(@sources);
		else if(base_name == 'contacts')
			contactsList.initialise(@contacts);
		
		update_bounds();
	}
	
	void prime()
	{
		for(uint i = 0; i < sources.length; i++)
		{
			sources[i].prime();
			break;
		}
	}
	
	void power_off()
	{
		on = false;
		
		for(int i = 0; i < num_arcs; i++)
		{
			arcs[i].power_off();
		}
	}
	
	Arc@ add_arc()
	{
		if(num_arcs > 1000)
			return null;
		
		if(num_arcs >= arcs_size)
		{
			const int s = arcs_size;
			arcs_size *= 2;
			arcs.resize(arcs_size);
			
			for(int i = s; i < arcs_size; i++)
			{
				@arcs[i] = Arc(script);
			}
		}
		
		Arc@ arc = arcs[num_arcs++];
		arc.reset();
		
		return arc;
	}
	
	private void try_spawn_arc(ArcSource@ src)
	{
		if(!src.try_spawn_arc())
			return;
		
		Arc@ arc = add_arc();
		arc.kill_player = kill_player;
		arc.kill_enemy = kill_enemy;
		arc.kill_apple = kill_apple;
		arc.player_stun = player_stun;
		arc.enemy_stun = enemy_stun;
		arc.apple_stun = apple_stun;
		
		if(@arc == null)
			return;
		
		if(independent_sources)
		{
			src.random_point(arc.x1, arc.y1);
		}
		else
		{
			@src = cast<ArcSource@>(sourcesList.random_point(arc.x1, arc.y1).contact);
		}
		
		ArcContactArea@ area = contactsList.random_point(arc.x2, arc.y2);
		arc.initialise(src, area, this);
	}
	
}
