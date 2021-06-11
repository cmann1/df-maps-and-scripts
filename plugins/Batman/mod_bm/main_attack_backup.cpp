#include '../common-old/utils.cpp'
#include 'embeds.cpp'
#include 'SpringSystem.cpp'
#include 'Hat.cpp'
#include '../common-old/ColType.cpp'
#include '../common-old/Debug.cpp'
#include '../common-old/Fx.cpp'

class script
{
	
	scene@ g;
	
	array<Hat@> hats;
	int num_hats = 0;
	
	dictionary apple_tracker;
	array<hitbox@> hitboxes;
	array<string> fx_names = {'bang', 'pow', 'zap', 'crash'};
	
	script()
	{
		@g = get_scene();
		
		num_hats = num_cameras();
		hats.resize(num_hats);
		
		for(int i = 0; i < num_hats; i++)
		{
			@hats[i] = Hat(i);
			@hats[i].s = this;
		}
	}
	
	void build_sprites(message@ msg)
	{
		embed_build_sprites(msg);
	}
	
	void checkpoint_load()
	{
		for(int i = 0; i < num_hats; i++)
		{
			@hats[i].player = null;
		}
		
		apple_tracker.deleteAll();
	}
	
	void entity_on_add(entity @e)
	{
		hitbox@ hb = e.as_hitbox();
		if(hb !is null)
		{
			hitboxes.insertLast(hb);
		}
	}
	
	void entity_on_remove(entity@ e)
	{
		controllable@ c = e.as_controllable();
		
		const string type_name = e.type_name();
		if(c !is null && c.stun_timer() > 0 or type_name == 'enemy_spring_ball' or type_name == 'enemy_trash_bag')
		{
			trigger_attack(c);
		}
	}
	
	void trigger_attack(entity@ e)
	{
		if(e is null) return;
		
		if(e.type_name() == 'hittable_apple')
		{
			if(apple_tracker.exists(e.id() + '') && float(apple_tracker[e.id() + '']) >= e.as_controllable().stun_timer())
				return;
		}
		
		const float rand_offset = 40;
		rectangle@ r = e.base_rectangle();
		float x = e.x() + (r.left() + r.right()) * 0.5 + rand_range(-rand_offset, rand_offset);
		float y = e.y() + (r.top() + r.bottom()) * 0.5 + rand_range(-rand_offset, rand_offset);
		
		const float scale = rand_range(0.85, 1.0);
		Fx@ fx = spawn_fx(x, y, 'script', fx_names[rand_range(0, int(fx_names.size()) - 1)], 0, 15, rand_range(-20, 20), scale, scale);
		fx.layer = 19;
		fx.sublayer = 18;
		fx.end_frame = 10;
	}
	void trigger_attack(controllable@ e)
	{
		trigger_attack(e.as_entity());
	}
	
	void step(int entities)
	{
		for(int i = 0; i < num_hats; i++)
		{
			hats[i].step();
		}
	}
	
	void step_post(int entities)
	{
		for(int i = 0; i < num_hats; i++)
		{
			hats[i].step_post();
		}
		
		for(int i = 0; i < entities; i++)
		{
			entity@ e = entity_by_index(i);
			if(e.type_name() != 'hittable_apple') continue;
			
			controllable@ c = e.as_controllable();
			if(c !is null)
			{
				const string id = e.id() + '';
				const float previous_stun = apple_tracker.exists(id) ? float(apple_tracker[id]) : 0;
				const float stun_timer = c.stun_timer();
				
//				puts(stun_timer, previous_stun);
				
				if(stun_timer > previous_stun)
				{
					trigger_attack(e);
					apple_tracker[id] = stun_timer;
				}
				else if(stun_timer > 0)
				{
					apple_tracker[id] = stun_timer;
				}
				else
				{
					apple_tracker.delete(id);
				}
			}
		}
		
		for(int i = int(hitboxes.size()) - 1; i >=0 ; i--)
		{
			hitbox@ hb = hitboxes[i];
			const int outcome = hb.hit_outcome();
			
			if(hb is null or outcome == 4)// or hb.owner() is null
			{
				hitboxes.removeAt(i);
				continue;
			}
			
			if(outcome == 1 or outcome == 3 or outcome == 5)
			{
				rectangle@ r = hb.base_rectangle();
				const float x = hb.x();
				const float y = hb.y();
				
				int count = g.get_entity_collision(
					y + r.top(), y + r.bottom(),
					x + r.left(), x + r.right(),
					COL_TYPE_HITTABLE
				);
				for(int k = 0; k < count; k++)
				{
					entity@ hit = g.get_entity_collision_index(k);
					if(hit.is_same(hb.owner())) continue;
					
					// Prevent double fx when a quill hits
					if(hb.owner() is null && hit.type_name() == 'base_projectile') continue;
					
					trigger_attack(hit);
				}
				
				hitboxes.removeAt(i);
				continue;
			}
		}
	}
	
	void draw(float sub_frame)
	{
		for(int i = 0; i < num_hats; i++)
		{
			hats[i].draw(sub_frame);
		}
	}
	
}