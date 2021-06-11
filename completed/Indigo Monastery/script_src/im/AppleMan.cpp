class AppleMan : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	dustman@ man = null;
	entity@ target = null;
	[position,mode:world,layer:19,y:room_y1] float room_x1;
	[hidden] float room_y1;
	[position,mode:world,layer:19,y:room_y2] float room_x2;
	[hidden] float room_y2;
	[position,mode:world,layer:19,y:man_y] float man_x;
	[hidden] float man_y;
	
	[position,mode:world,layer:19,y:hawk_y] float hawk_x;
	[hidden] float hawk_y;
	[text] int hawk_layer = 19;
	[text] int hawk_sublayer = 16;
	[text] string hawk_sprite = 'airidle';
	[text] uint hawk_frame = 5;
	[text] float hawk_scale = 0.75;
	
	int t = 0;
	int t_step = 4;
	int attack_timer = 0;
	int attack_timer_max1 = 30;
	int attack_timer_max2 = 70;
	int jump_timer = 0;
	int jump_timer_max = 200;
	
	bool active = false;
	
	sprites@ hawk_spr;
	
	AppleMan()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		@hawk_spr = create_sprites();
		hawk_spr.add_sprite_set('hawk');
		
//		if(s.in_game)
//		{
//		}
	}
	
	void activate(controllable@ e)
	{
		if(e.player_index() != -1)
			active = true;
	}
	
	void step()
	{
		if(!active)
			return;
		
		if(man is null)
		{
			entity@ e = create_entity('dust_man').as_entity();
			e.set_xy(man_x, man_y);
			
			@man = e.as_dustman();
			man.ai_disabled(true);
			
			g.add_entity(e, false);
		}
		
		const float dm_x = man.x();
		const float dm_y = man.y();
		
		if(t++ % t_step == 0)
		{
			if(man is null) return;
				
			int count = g.get_entity_collision(room_y1, room_y2, room_x1, room_x2, COL_TYPE_HITTABLE);
			float apple_distance = 999999;
			@target = null;
			
			for(int i = 0; i < count; i++)
			{
				entity@ e = g.get_entity_collision_index(i);
				
				if(e.type_name() != 'hittable_apple')
					continue;
					
				const float dist = dist_sqr(dm_x, dm_y, e.x(), e.y());
				if(dist < apple_distance)
				{
					apple_distance = dist;
					@target = e;
				}
			}
			
			if(target is null)
			{
				man.x_intent(0);
				man.y_intent(0);
				man.jump_intent(0);
			}
		}
		
		if(attack_timer > 0)
			attack_timer--;
		if(jump_timer > 0)
			jump_timer--;
		
		if(@target != null)
		{
			const float tx = target.x();
			const float ty = target.y();
			const float dx = dm_x - tx;
			const float dy = dm_y - ty;
			
			if(dm_x < tx)
				man.x_intent(abs(dx) > 48 ? 1 : 0);
			else
				man.x_intent(abs(dx) > 48 ? -1 : 0);
			
			if(dy > 96)
				man.y_intent(-1);
			else if(dy <= 10)
				man.y_intent(1);
			else
				man.y_intent(frand() > 0.75 ? 1 : 0);
				
			if(dy > 50 && jump_timer == 0)
			{
				man.jump_intent(1);
				jump_timer = jump_timer_max;
			}
			
			if(attack_timer == 0)
			{
				if(abs(dx) < 96)
				{
					if(dx < 0 && man.face() < 0)
						man.face(1);
					else if(dx > 0 && man.face() > 0)
						man.face(-1);
					
					if(frand() > 0.8)
						man.heavy_intent(1);
					else
						man.light_intent(1);
					attack_timer = rand_range(attack_timer_max1, attack_timer_max2);
				}
			}
		}
	}
	
	void draw(float sub_frame)
	{
		hawk_spr.draw_world(hawk_layer, hawk_sublayer, hawk_sprite, hawk_frame, 0, hawk_x, hawk_y, 0, hawk_scale, hawk_scale, 0xFFFFFFFF);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		draw_dot(g, 22, 22, man_x, man_y, 5, 0xFF0000FF, 0);
		draw_dot(g, 22, 22, hawk_x, hawk_y, 5, 0xFFFFFFFF, 0);
		outline_rect(g, room_x1, room_y1, room_x2, room_y2, 22, 22, 2, 0x88FF0000);
	}
	
}