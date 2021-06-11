class EntityOutliner
{
	
	array<controllable@> entity_list;
	
	void step(int entities)
	{
		entity_list.resize(0);
		
		for(int i = 0; i < entities; i++)
		{
			controllable@ c = entity_by_index(i).as_controllable();
			if(@c != null and c.life() > 0)
			{
				entity_list.insertLast(c);
			}
		}
	}
	
	void draw(float sub_frame)
	{
		for(uint count = entity_list.length(), i = 0; i < count; i++)
		{
			controllable@ c = entity_list[i];
			if(c.life() <= 0) continue;
			sprites@ spr = c.get_sprites();
			
			string sprite_name;
			uint frame;
			float face;
			if(c.attack_state() == ATTACK_TYPE_IDLE)
			{
				sprite_name = c.sprite_index();
				frame = uint(c.state_timer());
				face = c.face();
			}
			else
			{
				sprite_name = c.attack_sprite_index();
				frame = uint(c.attack_timer());
				face = c.attack_face();
			}
			frame = frame % spr.get_animation_length(sprite_name);
			
			const float x = c.x();
			const float y = c.y();
			const float prev_x = c.prev_x();
			const float prev_y = c.prev_y();
			
			spr.draw_world(
				c.layer(), c.type_name() == "hittable_apple" ? 6 : (@c.as_dustman() != null ? 9 : 7),
				sprite_name, frame, 0,
				lerp(prev_x, x, sub_frame) + c.draw_offset_x() + 1.5,
				lerp(prev_y, y, sub_frame) + c.draw_offset_y() - 1.5,
				c.rotation(), c.scale() * face, c.scale(), 0xFFFFFFFF);
		}
	}
	
}