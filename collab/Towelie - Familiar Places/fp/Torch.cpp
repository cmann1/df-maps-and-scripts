#include '../common/SpriteBatch.cpp';
#include '../common/AnimatedSpriteBatch.cpp';

class Torch : trigger_base
{
	
	script@ script;
	scene@ g;
	scripttrigger @self;
	
	[hidden] bool active = false;
	[text] int key_id = 0;
	
	[hidden] uint burst_emitter_id;
	[hidden] int burst_emitter_timer = 10;
	
	SpriteBatch spr(
		array<string>={'props4','machinery_19','props1','books_10',},
		array<int>={16,24,16,24,},
		array<float>={-90,-213,42,-56,},
		array<float>={0.613238,0.613238,0.442637,0.442637,},
		array<float>={0,90,},
		array<uint>={0,0,});
	AnimatedSpriteBatch torch_spr(
		'props1','lighting_7',
		array<int>={13,19,},
		array<float>={24,-101,},
		array<float>={1,1,},
		array<float>={0,},
		array<uint>={0,});
	
	Torch()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@script = s;
		@this.self = self;
		self.radius(38);
		torch_spr.t = rand() % 50;
	}
	
	void activate(controllable @e)
	{
		if(!active and script.has_key(key_id))
		{
			active = true;
			
			g.play_sound('sfx_door_unlock', self.x(), self.y(), 1, false, true);
			
			message@ msg = create_message();
			broadcast_message('unlock_bridge', msg);
			
			entity@ emitter = create_emitter(125, self.x(), self.y() - 145, 48, 48, 17, 0);
			g.add_entity(emitter, true);
			burst_emitter_id = emitter.id();
			burst_emitter_timer = 15;
		}
	}
	
	void step()
	{
		if(active)
		{
			torch_spr.step();
			
			if(burst_emitter_timer > 0)
			{
				burst_emitter_timer--;
				if(burst_emitter_timer <= 0)
				{
					entity@ emitter = entity_by_id(burst_emitter_id);
					if(@emitter != null)
						g.remove_entity(emitter);
				}
			}
		}
	}
 
	void draw(float sub_frame)
	{
		const float x = floor(self.x() / 48) * 48;
		const float y = floor(self.y() / 48) * 48;
		spr.draw(x, y);
		
		if(active)
			torch_spr.draw(x, y);
	}
 
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
	
}