#include '../../lib/triggers/InstancedSprite.cpp';
#include '../BaseScript.cpp';
#include '../collision/CollisionManager.cpp';
#include '../MessageBroadcaster.cpp';
#include '../lab/Coil.cpp';
#include '../forest/Fairies.cpp';
#include '../factory/Bubbles.cpp';

#include 'NexusSpriteCache.cpp';
#include 'Dust.cpp';
#include 'Vines.cpp';
#include 'Lever.cpp';
#include 'Door.cpp';
#include 'Dialog.cpp';
#include 'LeverLock.cpp';
#include 'Elevator.cpp';
#include 'CoilWall.cpp';

class script : BaseScript
{
	
	[persist] bool debug_edges = true;
	[hidden] array<string> global_names;
	[hidden] array<bool> global_values;
	
	NexusSpriteCache sprites;
	CollisionManager collision_manager;
	
	script()
	{
		super();
		g.override_stream_sizes(16, 8);
	}
	
	void checkpoint_load() override
	{
		//puts('-- checkpoint_load ----------------');
		BaseScript::checkpoint_load();
		collision_manager.checkpoint_load();
	}
	
	void step(int num_entities) override
	{
		BaseScript::step(num_entities);
		
		if(is_playing)
		{
			collision_manager.update_collision_handlers(num_entities);
		}
	}
	
	void step_post(int num_entities) override
	{
		BaseScript::step_post(num_entities);
		
		if(is_playing)
		{
			collision_manager.step_post(num_entities);
		}
	}
	
	void draw(float sub_frame) override
	{
		//BaseScript::draw(sub_frame);
		
		if(debug_edges)
		{
			collision_manager.debug_draw_edges(sub_frame);
		}
	}
	
	void set_global(const string &in name, const bool value)
	{
		for(uint i = 0; i < global_names.length; i++)
		{
			if(global_names[i] == name)
			{
				global_values[i] = value;
				return;
			}
		}
		
		global_names.insertLast(name);
		global_values.insertLast(value);
	}
	
	bool get_global(const string &in name, const bool default_val=false)
	{
		for(uint i = 0; i < global_names.length; i++)
		{
			if(global_names[i] == name)
			{
				return global_values[i];
			}
		}
		
		return default_val;
	}
	
	void clear_global(const string &in name)
	{
		for(uint i = 0; i < global_names.length; i++)
		{
			if(global_names[i] == name)
			{
				global_names.removeAt(i);
				global_values.removeAt(i);
				return;
			}
		}
	}
	
	private void print_globals(const string &in v)
	{
		puts('-- globals[' + v + '] ---------');
		for(uint i = 0; i < global_names.length; i++)
		{
			puts('  [' + i + '] ' + global_names[i] + ' = ' + global_values[i]);
		}
	}
	
}
