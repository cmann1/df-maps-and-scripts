class Clock : trigger_base
{
	
	script@ script;
	scene@ g;
	scripttrigger@ self;
	
	[position,mode:world,layer:11,y:fallback_y1] float fallback_x1;
	[hidden] float fallback_y1;
	[position,mode:world,layer:11,y:fallback_y2] float fallback_x2;
	[hidden] float fallback_y2;
	
	[position,mode:world,layer:11,y:centre_y] float centre_x;
	[hidden] float centre_y;
	
	[colour] uint border_colour = 0xFF000000;
	[colour] uint inner_colour = 0xFFFFFFFF;
	[text] float hand_scale = 0.6;
	
	int layer = 11;
	int sub_layer = 19;
	float radius = 96;
	
	int frame = 0;
	int hours = 0;
	int minutes = 0;
	int seconds = 0;
	
	bool has_triggered = false;
	int activate_hour = 12;
	int activate_minute_min = 0;
	int activate_minute_max = 3;
	
	sprites@ clock_spr;
	textfield@ number_text;
	
	Clock()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		self.editor_colour_active(0xFF75839B);
		self.editor_colour_inactive(0xFF65738B);
		self.editor_colour_circle(0xFF75839B);
		
		if(script.in_game)
		{
			removeFallback();
		}
		
		@clock_spr = create_sprites();
		clock_spr.add_sprite_set('script');
		
		@number_text = create_textfield();
		number_text.set_font('ProximaNovaReg', 26);
		number_text.align_horizontal(0);
		number_text.align_vertical(1);
		number_text.colour(0xFF000000);
		
		updateFromLocalTime();
	}
	
	void removeFallback()
	{
		const int tx1 = int(round(fallback_x1 / 48));
		const int ty1 = int(round(fallback_y1 / 48));
		const int tx2 = int(round(fallback_x2 / 48));
		const int ty2 = int(round(fallback_y2 / 48));
		
		const array<string> prop_names = {'props4','machinery_19','props1','books_10','props1','books_10','props1','books_10','props1','books_10','props1','books_10','props1','books_10','props1','books_10','props1','books_10','props1','books_10','props2','foliage_16','props2','foliage_16','props2','foliage_16','props2','foliage_16','props2','foliage_16','props2','foliage_16','props2','foliage_16','props2','foliage_16','props2','foliage_16','props2','foliage_16','props2','foliage_16','props2','foliage_16','props2','foliage_16','props2','foliage_16','props2','foliage_16','props2','foliage_16','props1','books_9','props1','books_9',};
		const array<int> prop_layers = {11,19,11,19,11,19,11,19,11,19,11,19,11,19,11,19,11,19,11,19,11,15,11,15,11,15,11,15,11,15,11,15,11,15,11,15,11,15,11,15,11,15,11,15,11,15,11,15,11,15,11,15,11,18,11,18,};
		
		dictionary sprite_map;
		for(int i = 0, count = int(prop_names.size()); i < count; i += 2)
		{
			sprite_map[prop_names[i] + '_' + prop_names[i + 1] + '_' + prop_layers[i] + '.' + prop_layers[i + 1]] = true;
		}
		
		for(int x = tx1; x <= tx2; x++)
		{
			for(int y = ty1; y <= ty2; y++)
			{
				g.set_tile(x, y, 11, false, 0, 0, 0, 0);
			}
		}
		
		int count = g.get_prop_collision(
			(ty1 - 1) * 48, (ty2 + 1) * 48,
			(tx1 - 1) * 48, (ty2 + 1) * 48);
		
		for(int i = 0; i < count; i++)
		{
			prop@ p = g.get_prop_collision_index(i);
			string sprite_set, sprite_name;
			sprite_from_prop(p, sprite_set, sprite_name);
			
			if(sprite_map.exists(sprite_set + '_' + sprite_name + '_' + p.layer() + '.' + p.sub_layer()))
				g.remove_prop(p);
		}
	}
	
	void updateFromLocalTime()
	{
		timedate@ time = localtime();
		hours = time.hour();
		minutes = time.min();
		seconds = time.sec();
	}
	
	void activate(controllable@ e)
	{
		if(e.player_index() == -1)
			return;
		
		if(!has_triggered && hours == activate_hour && minutes >= activate_minute_min && minutes <= activate_minute_max)
//		if(!has_triggered)
		{
			broadcast_message('start_crystal_music', create_message());
			has_triggered = true;
		}
	}
	
	void step()
	{
		if(++frame >= 60)
		{
			if(++seconds % 15 == 0)
			{
				updateFromLocalTime();
			}
			
			frame = 0;
			
			if(hours != activate_hour || minutes < activate_minute_min || minutes > activate_minute_max)
				has_triggered = false;
		}
	}
	
	void editor_step()
	{
		step();
	}
	
	void draw(float sub_frame)
	{
		const float inner_radius = radius - 30;
		
		const uint segments = 24;
		
		clock_spr.draw_world(layer, sub_layer, 'clock_back', 0, 0, centre_x, centre_y, 0, 1, 1, 0xFFFFFFFF);
		
//		const float seconds_t = (seconds + frame / 60.0) / 60.0;
		const float seconds_t = (seconds + 1) / 60.0;
		const float seconds_radians = seconds_t * PI * 2 - HALF_PI;
		const float minutes_t = minutes / 60.0;
		const float minutes_degrees = minutes_t * 360 - 90;
		const float minutes_radians = minutes_t * PI2 - HALF_PI;
		const float hours_degrees = ((hours + minutes_t) / 12.0) * 360 - 90;
		const float hours_radians = ((hours + minutes_t) / 12.0) * PI2 - HALF_PI;
		
		// Hours
		const float shadow_x = -2;
		const float shadow_y = 2;
		const uint shadow_colour = (border_colour & 0xFFFFFF) | 0x55000000;
		float hx = centre_x + cos(hours_radians) * radius * 0.5;
		float hy = centre_y + sin(hours_radians) * radius * 0.5;
		g.draw_line_world(layer, sub_layer,
			centre_x + shadow_x, centre_y + shadow_y,
			hx + shadow_x, hy + shadow_y,
			3, shadow_colour);
		g.draw_line_world(layer, sub_layer,
			centre_x, centre_y,
			hx, hy,
			3, border_colour);
			
		// Minutes
		hx = centre_x + cos(minutes_radians) * (radius - 10);
		hy = centre_y + sin(minutes_radians) * (radius - 10);
		g.draw_line_world(layer, sub_layer,
			centre_x + shadow_x, centre_y + shadow_y,
			hx + shadow_x, hy + shadow_y,
			3, shadow_colour);
		g.draw_line_world(layer, sub_layer,
			centre_x, centre_y,
			hx, hy,
			3, border_colour);
		
		// Seconds
		hx = centre_x + cos(seconds_radians) * (inner_radius - 5);
		hy = centre_y + sin(seconds_radians) * (inner_radius - 5);
		g.draw_line_world(layer, sub_layer,
			centre_x + shadow_x, centre_y + shadow_y,
			hx + shadow_x, hy + shadow_y,
			2, shadow_colour);
		g.draw_line_world(layer, sub_layer,
			centre_x, centre_y,
			hx, hy,
			2, border_colour);
		
		const array<string> number_strings = {'XII', 'I', 'II', 'III', 'IV', 'V', 'VI', 'VII', 'VIII', 'IX', 'X', 'XI'};
		const float text_radius = inner_radius + 5;
		for(int i = 0; i < 12; i++)
		{
			const float angle = i / 12.0 * PI * 2 - HALF_PI;
			number_text.text(number_strings[i]);
			shadowed_text_world(number_text, layer, sub_layer,
					centre_x + cos(angle) * text_radius, centre_y + sin(angle) * text_radius,
					1, 1, angle * RAD2DEG + 90, 0xDD000000, 1, 1);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
			
		const float tx1 = round(fallback_x1 / 48) * 48;
		const float ty1 = round(fallback_y1 / 48) * 48;
		const float tx2 = round(fallback_x2 / 48) * 48;
		const float ty2 = round(fallback_y2 / 48) * 48;
		
		outline_rect(g,
			tx1, ty1, tx2, ty2,
			11, 22, 2, 0x66FFFFFF
		);
		draw_arrow(g, 22, 22, self.x(), self.y(), centre_x, centre_y, 2, 10, 0.5, 0xFF75839B);
		
		draw_dot(g, 11, 22, centre_x, centre_y, 5, 0x88FFFFFF);
	}
	
}