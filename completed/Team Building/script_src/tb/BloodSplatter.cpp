class BloodSplatter : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	entity@ emitter = null;
	sprites@ spr;
	
	[hidden] int emitter_life = 30;
	float vel_x = 0;
	float vel_y = 0;
	
	[hidden] int drip_timer = 4;
	[hidden] int drip_count;
	[hidden] array<int> prop_ids;
	[hidden] array<float> prop_positions;
	[hidden] array<float> prop_scales;
	[hidden] array<float> prop_speed;
	array<prop@> props;
	
	BloodSplatter()
	{
		@g = get_scene();
		drip_count = 4 + rand() % 7;
	}
	
	void init(script @s, scripttrigger @self)
	{
		@this.self = self;
		
		props.resize(prop_ids.length());
		for(int i = int(prop_ids.length()) - 1; i >= 0; i--)
		{
			@props[i] = prop_by_id(prop_ids[i]);
		}
	}
	
	void step()
	{
		if(@emitter != null)
		{
			self.set_xy(self.x() + vel_x * DT, self.y() + vel_y * DT);
			emitter.set_xy(self.x(), self.y());
			if(emitter_life-- <= 0)
			{
				g.remove_entity(emitter);
				@emitter = null;
			}
		}
		else if(emitter_life == 30)
		{
			@emitter = create_emitter(114, self.x(), self.y(), 72, 72, 18, 12);
			g.add_entity(emitter, false);
		}
		
		
		if(drip_count > 0)
		{
			if(drip_timer-- <= 0)
			{
				prop@ p = create_prop(1, 22, 5, 13, 19, 0, 0, 90);
				p.scale_x(0.05 + frand() * 0.05);
				p.scale_y(0.2 + frand() * 0.25);
				const float px = self.x() - 40 + frand() * 80;
				const float py = self.y() - 40 + frand() * 80;
				prop_positions.insertLast(px);
				prop_positions.insertLast(py);
				position_prop(p, px, py);
				g.add_prop(p);
				prop_ids.insertLast(p.id());
				props.insertLast(p);
				prop_scales.insertLast(0.2 + frand() * 0.35);
				prop_speed.insertLast(0.02 + frand() * 0.1);
				
				if(rand() % 2 > 0)
				{
					@p = create_prop(2, 5, 16 + (rand() % 3), 13, 19, px - 15 + frand() * 30, py - 15 + frand() * 30, rand() % 360);
					g.add_prop(p);
				}
				
				drip_count--;
				drip_timer = 4 + rand() % 3;
			}
		}
		
		for(int i = int(prop_ids.length()) - 1; i >= 0; i--)
		{
			prop@ p = props[i];
			if(@p == null) @p = @props[i] = prop_by_id(prop_ids[i]);
			
			p.scale_x(p.scale_x() + prop_speed[i] * DT);
			position_prop(p, prop_positions[i * 2], prop_positions[i * 2 + 1]);
			
			if(p.scale_x() >= prop_scales[i] - 0.07)
			{
				prop_scales.removeAt(i);
				prop_speed.removeAt(i);
				props.removeAt(i);
				prop_ids.removeAt(i);
				prop_positions.removeAt(i * 2);
				prop_positions.removeAt(i * 2);
			}
		}
		
		if(drip_count <= 0 and prop_ids.length() == 0 and @emitter == null)
		{
			g.remove_entity(self.as_entity());
		}
	}
	
	void position_prop(prop@ p, float x, float y)
	{
		float ox = 188 - 60;
		float oy = 101 - 20;
		rotate(ox * p.scale_x(), oy * p.scale_y(), p.rotation() * DEG2RAD, ox, oy);
		p.x(x + ox);
		p.y(y + oy);
	}
	
}