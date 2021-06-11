class Key: trigger_base
{
	
	script@ script;
	scene@ g;
	scripttrigger @self;
	
	sprites@ spr;
	float t = 0;
	
	float fade_speed = 1;
	
	[hidden] bool active = true;
	[hidden] float fade = 1;
	[hidden] bool requires_init = true;
	[hidden] uint emitter_id;
	[hidden] uint burst_emitter_id;
	[hidden] int burst_emitter_timer = 15;
	
	SpriteBatch glow_spr(
		array<string>={'props3','backdrops_4','props3','backdrops_4','props3','backdrops_4','props2','backdrops_4',},
		array<int>={17,0,17,0,17,0,17,0,},
		array<float>={-368.577,-3.81201,-334.577,-63.812,-289.577,-117.812,12.4229,202.188,},
		array<float>={1,1,1,1,1,1,1,1,},
		array<float>={343.279,353.282,3.27942,0,},
		array<uint>={0,0,0,0,});
	
	[text] int key_id = 0;
	[text] float bob_speed = 2;
	[text] float bob_height = 10;
	
	Key()
	{
//		@g = get_scene();
//		@spr = create_sprites();
//		spr.add_sprite_set('editor');
	}
	
//	void init(script@ s, scripttrigger@ self)
//	{
//		@script = s;
//		@this.self = self;
//		self.radius(38);
//		
//		t = rand() % 50;
//	}
	
//	void activate(controllable @e)
//	{
//		if(!active) return;
//		
//		if(e.player_index() != -1)
//		{
//			active = false;
//			script.set_key(key_id);
//			
//			entity@ emitter = create_emitter(125, self.x(), self.y(), 48, 48, 17, 0);
//			g.add_entity(emitter, true);
//			burst_emitter_id = emitter.id();
//			
//			@emitter = entity_by_id(emitter_id);
//			if(@emitter != null)
//				g.remove_entity(emitter);
//		}
//	}
	
//	void step()
//	{
//		t += DT;
//		
//		if(requires_init)
//		{
//			const float x = self.x();
//			const float y = self.y();
//			
//			raycast@ ray = g.ray_cast_tiles(x, y, x, y + 150);
//			float rx = x;
//			float ry = y + 48 * 1.5;
//			if(ray.hit())
//				ry = ray.hit_y();
//			
//			entity@ emitter = create_emitter(122, rx, ry, 48, 48, 17, 0);
//			g.add_entity(emitter, true);
//			emitter_id = emitter.id();
//			
//			requires_init = false;
//		}
//		
//		if(!active)
//		{
//			burst_emitter_timer--;
//			
//			if(burst_emitter_timer <= 0)
//			{
//				entity@ emitter = entity_by_id(burst_emitter_id);
//				if(@emitter != null)
//					g.remove_entity(emitter);
//			}
//			
//			fade -= fade_speed * DT;
//			
//			if(fade < 0)
//			{
//				g.remove_entity(self.as_entity());
//				fade = 0;
//			}
//		}
//	}
 
//	void editor_step()
//	{
//		t += DT;
//	}
	
//	void draw(float sub_frame)
//	{
//		const float x = self.x();
//		const float y = self.y();
//		
//		if(active)
//			spr.draw_world(18, 11, 'rubykey', 0, 0, x, y + sin(t * bob_speed) * bob_height, 0, 1, 1, 0xFFFFFFFF);
//		
//		glow_spr.draw_colour(x, y, (uint(fade * 0xFF) << 24) + 0xFFFFFF);
//	}
 
//	void editor_draw(float sub_frame)
//	{
//		draw(sub_frame);
//	}
	
}