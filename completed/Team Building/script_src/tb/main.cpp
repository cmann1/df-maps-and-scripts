#include "../common/utils.cpp"
#include "../common/InstancedSprite.cpp"

#include "Teleport.cpp"
#include "PlayerMover.cpp"

#include "Blades.cpp"
#include "BloodSplatter.cpp"
#include "Gibs.cpp"
#include "BloodPit.cpp"
#include "Skull.cpp" // Can be removed - use instanced sprite instead
#include "Pentagram.cpp"
#include "HellPortal.cpp"
#include "SacrificeRumble.cpp"
#include "CrackedWall.cpp"
#include "PropsPulse.cpp"
#include "BoneWorm.cpp"

const float GRAVITY = 1500;

class script : callback_base
{
	
	scene@ g;
	
	dictionary ambience_map;
	array<AmbienceData@> ambience;
	int ambience_count = 0;
	
	[text] array<PortalAppleCount> apple_counts;
	[text] int num_apple_counts = 0;
	
	script()
	{
		@g = get_scene();
		g.override_stream_sizes(12, 8);
		
		add_broadcast_receiver("portal_add_apple", this, "portal_add_apple");
		add_broadcast_receiver("ambience", this, "on_ambience");
	}
	
	void portal_add_apple(string id, message@ msg)
	{
		PortalAppleCount@ data = null;
		const string portal = msg.get_string("portal");
		
		for(int i = 0; i < num_apple_counts; i++)
		{
			PortalAppleCount@ a = @apple_counts[i];
			if(a.portal == portal)
			{
				@data = a;
				break;
			}
		}
		
		if(data is null)
		{
			apple_counts.resize(num_apple_counts + 1);
			@data = @apple_counts[num_apple_counts++];
			data.portal = portal;
		}
		
		data.count += msg.get_int("count");
		
		message@ msg2 = create_message();
		msg2.set_string("portal", portal);
		msg2.set_int("inc", msg.get_int("count"));
		msg2.set_int("count", data.count);
		msg2.set_float("x", msg.get_float("x"));
		msg2.set_float("y", msg.get_float("y"));
		broadcast_message("portal_apple_count", msg2);
	}
	
	int ge_apple_count(const string &in portal)
	{
		int count = 0;
		
		for(int i = 0; i < num_apple_counts; i++)
		{
			PortalAppleCount@ a = @apple_counts[i];
			if(a.portal == portal)
			{
				count = a.count;
				break;
			}
		}
		
		return count;
	}
	
	void on_ambience(string id, message@ msg)
	{
		const string name = msg.get_string("name");
		const bool remove = msg.get_int("remove") == 1;
		
		AmbienceData@ data = cast<AmbienceData@>(ambience_map[name]);
		
		if(@data != null)
		{
			if(remove and !data.fade_out)
			{
				data.fade_out = true;
				data.set(0, msg.get_float("fade"));
			}
			else if(!remove and data.fade_out)
			{
				data.fade_out = false;
				data.time = 0;
				data.set(msg.get_float("volume"), msg.get_float("fade"));
			}
		}
		else
		{
			if(!remove)
			{
				@data = AmbienceData(g, name, msg.get_float("volume"), msg.get_float("fade"));
				@ambience_map[name] = data;
				ambience.insertLast(data);
				ambience_count++;
			}
		}
	}
	
	void entity_on_remove(entity @e)
	{
		dustman@ dm = e.as_dustman();
		if(@dm != null)
		{
			spawn_gibs(g, dm);
			
			for(int i = int(num_cameras()) - 1; i >= 0; i--)
			{
				if(dm.is_same(controller_controllable(i)))
				{
					g.combo_break_count(g.combo_break_count() + 1);
					break;
				}
			}
		}
	}
	
	void step(int entities)
	{
		for(int i = ambience_count - 1; i >= 0; i--)
		{
			AmbienceData@ data = @ambience[i];
			if(data.time < data.fade)
			{
				data.snd.volume(data.snd.volume() + data.vol_dx);
				if(++data.time >= data.fade)
				{
					if(data.fade_out)
					{
						data.snd.stop();
						ambience_map.delete(data.name);
						ambience.removeAt(i);
						ambience_count--;
					}
				}
			}
		}
	}
	
//	void step(int entities)
//	{
//	}
	
}

class PortalAppleCount
{
	string portal;
	int count = 0;
}

class AmbienceData
{
	
	audio@ snd;
	bool fade_out;
	string name;
	float volume = 0;
	float fade = 0;
	
	float time = 0;
	float vol_dx = 0;
	
	AmbienceData(scene@ g, string name, float volume, float fade)
	{
		this.name = name;
		@snd = g.play_script_stream(name, 2, 0, 0, true, fade < 1 ? volume : 0);
		set(volume, fade);
	}
	
	void set(float volume, float fade)
	{
		this.volume = volume;
		this.fade = fade;
		time = 0;
		vol_dx = fade != 0 ? (volume - snd.volume()) / fade : 1;
		if(fade == 0) snd.volume(volume);
	}
	
}

class AmbienceTrigger : trigger_base
{
	
	scripttrigger@ self;
	
	[text] string name;
	[text] float volume = 1;
	[text] float fade = 60;
	[text] bool remove;
	
	AmbienceTrigger()
	{
		
	}
	
	void activate(controllable@ e)
	{
		for(uint i = 0; i < num_cameras(); i++)
		{
			entity@ player = controller_entity(i);
			if(@player != null and e.is_same(player))
			{
				message@ msg = create_message();
				msg.set_string("name", name);
				msg.set_float("volume", volume);
				msg.set_float("fade", fade);
				msg.set_int("remove", remove ? 1 : 0);
				broadcast_message("ambience", msg);
				return;
			}
		}
	}
	
}