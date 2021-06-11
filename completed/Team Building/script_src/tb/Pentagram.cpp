class Pentagram : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	sprites@ spr;
	entity@ emitter = null;
	
	[text] string portal = "default";
	[position,layer:19,y:y] float x = 0;
	[hidden] float y = 0;
	[text] float scale = 0.6;
	[text] int layer = 14;
	[text] int sub_layer = 23;
	[text] int active_layer = 14;
	[text] int active_sub_layer = 24;
	[text] int glow_sub_layer = 22;
	[colour,alpha] uint glow_colour = 0xFFFFFFFF;
	
	int apple_count = 0;
	[hidden] bool active = false;
	[hidden] float active_time = 0;
	[text] float delay_time = 40;
	[text] float fade_time = 100;
	
	Pentagram()
	{
		@g = get_scene();
		@spr = create_sprites();
		spr.add_sprite_set("script");
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.self = @self;
		self.editor_handle_size(6);
	}
	
	void activate(controllable@ e)
	{
		if(e.type_name() == "hittable_apple")
		{
			apple_count++;
		}
	}
	
	void step()
	{
		if(apple_count > 0)
		{
			if(active_time < fade_time)
				active_time++;
			
			if(active_time >= delay_time)
			{
				if(!active)
				{
					message@ msg = create_message();
					msg.set_string("portal", portal);
					msg.set_int("count", 1);
					msg.set_float("x", self.x());
					msg.set_float("y", self.y());
					broadcast_message("portal_add_apple", msg);
					active = true;
				}
				
				if(@emitter == null)
				{
					@emitter = create_emitter(4, x, y, 96, 96, 16, 12);
					g.add_entity(emitter, false);
				}
			}
		}
		else if(active_time > 0)
		{
			if(@emitter != null)
			{
				g.remove_entity(emitter);
				@emitter = null;
			}
			
			if(active)
			{
				message@ msg = create_message();
				msg.set_string("portal", portal);
				msg.set_int("count", -1);
				broadcast_message("portal_add_apple", msg);
				active = false;
			}
			
			active_time--;
		}
		
		apple_count = 0;
	}
	
	void draw(float sub_frame)
	{
		if(active_time > 0)
		{
			const float fade_alpha = max(0, (active_time - delay_time) / (fade_time - delay_time));
			const uint alpha = uint(float((glow_colour >> 24) & 0xFF) * fade_alpha);
			spr.draw_world(active_layer, glow_sub_layer, "pent_glow", 0, 0, x, y, 0, scale, scale, (alpha << 24) + (glow_colour & 0x00FFFFFF));
			spr.draw_world(layer, sub_layer, "pentagram", 0, 0, x, y, 0, scale, scale, ((255 - alpha) << 24) + 0xFFFFFF);
			spr.draw_world(active_layer, active_sub_layer, "pentagram", 0, 0, x, y, 0, scale, scale, (alpha << 24) + 0xFFFFFF);
		}
		else
		{
			spr.draw_world(layer, sub_layer, "pentagram", 0, 0, x, y, 0, scale, scale, 0xFFFFFFFF);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}