const int ATTACK_TYPE_IDLE = 0;
const int ENTITY_STATE_FALL = 5;
const int ENTITY_STATE_HOVER = 7;
const int ENTITY_STATE_WALLRUN = 11;
const int ENTITY_STATE_RAISE = 19;

class Mirror : trigger_base
{
	
	[text] float offset_x = -10;
	[text] float offset_y = -4;
	[text] int layer = 13;
	[text] int sublayer = 8;
	[text] float bg_parallax = 0.6;
	[text] float player_parallax = 0.8;
	[colour,alpha] uint colour = 0xffffffff;
	
	[hidden] int activate_count;
	
	scripttrigger@ self;
	array<controllable@> players;
	array<int> player_timers;
	int players_count;
	sprites@ spr;
	sprites@ player_spr;
	
	bool monster;
	bool invis;
	
	Mirror()
	{
		@player_spr = create_sprites();
		@spr = create_sprites();
		spr.add_sprite_set('script');
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		rand();
	}
	
	void activate(controllable@ c)
	{
		if(c.player_index() == -1)
			return;
		
		const int index = c.player_index();
		
		if(int(players.length()) <= index)
		{
			players.resize(index + 1);
			player_timers.resize(players.length());
		}
		
		if(player_timers[index] == 0)
		{
			if(players_count == 0)
			{
				activate_count++;
				
				if(activate_count > 3)
				{
					if(frand() > 0.9)
					{
						monster = true;
						activate_count = 0;
					}
//					else if(frand() > 0.9)
//					{
//						invis = true;
//						activate_count = 0;
//					}
				}
			}
			
			players_count++;
		}
		
		@players[index] = c;
		player_timers[index] = 5;
	}
	
	void step()
	{
		for(uint i = 0; i < players.length(); i++)
		{
			if(@players[i] == null)
				continue;
			
			if(--player_timers[i] == 0)
			{
				@players[i] = null;
				players_count--;
				
				if(players_count == 0)
				{
					monster = false;
					invis = false;
				}
			}
		}
	}
	
	void draw(float sub_frame)
	{
		const float sx = self.x();
		const float sy = self.y();
		
		camera@ cam = get_active_camera();
		const float cx = cam.x();
		const float cy = cam.y();
		
		for(int x = -1; x <= 1; x++)
		{
			for(int y = -1; y <= 1; y++)
			{
				spr.draw_world(layer, sublayer - 1, 'wallpaper', 0, 0,
					cx + (sx - cx) * bg_parallax + 479 * x,
					cy + (sy - cy) * bg_parallax + 286 * y,
					0, 1, y == 0 ? 1 : - 1, 0xffffffff);
			}
		}
		
		if(!invis)
		{
			for(uint i = 0; i < players.length(); i++)
			{
				controllable@ c = @players[i];
				
				if(@c == null)
					continue;
				
				sprites@ spr;
				
				dustman@ dm = c.as_dustman();
				
				if(@dm != null)
				{
					player_spr.add_sprite_set(dm.character());
					
					if(monster)
					{
						player_spr.add_sprite_set('dustwraith');
					}
					
					@spr = player_spr;
				}
				else
				{
					@spr = c.get_sprites();
				}
				
				string sprite_name;
				uint frame;
				float face;
				float draw_offset_x = c.draw_offset_x();
				float draw_offset_y = c.draw_offset_y();
				
				const string type_name = c.type_name();
				
				if(c.attack_state() != ATTACK_TYPE_IDLE)
				{
					sprite_name = c.attack_sprite_index();
					frame = uint(max(c.attack_timer(), 0.0));
					face = c.attack_face();
					
					if(c.state() == ENTITY_STATE_WALLRUN)
					{
						draw_offset_x = draw_offset_y = 0;
					}
				}
				else
				{
					sprite_name = c.sprite_index();
					frame = uint(max(c.state_timer(), 0.0));
					face = c.face();
				}	
				frame = frame % spr.get_animation_length(sprite_name);
				
				const float x = lerp(c.prev_x(), c.x(), sub_frame) + draw_offset_x;
				const float y = lerp(c.prev_y(), c.y(), sub_frame) + draw_offset_y;
				
				int layer1, sublayer1;
				int layer2, sublayer2;
				
				spr.draw_world(
					layer, sublayer,
					sprite_name, frame, 0,
					cx + (x - cx) * bg_parallax + offset_x,
					cy + (y - cy) * bg_parallax + offset_y,
					c.rotation(), c.scale() * face, c.scale(), colour);
			}
		}
		
		spr.draw_world(layer, sublayer, 'mirror_surface', 0, 0,
			sx, sy,
			0, 0.9, 0.9, 0xaaffffff);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}