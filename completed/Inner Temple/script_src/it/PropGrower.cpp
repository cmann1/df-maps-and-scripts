class PropGrower : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	array<PropGrowStruct> props = {
		PropGrowStruct(0, -1164.92, 20.718, 349.997, 1, 1, 1, 5, 13, 0, 12, 8),
		PropGrowStruct(0, -1103.92, 99.718, 349.997, 1, 1, 1, 5, 13, 0, 13, 19),
		PropGrowStruct(0, -1131.92, 113.718, 349.997, 1, 1, 1, 5, 13, 0, 17, 19),
		PropGrowStruct(0, -1176.39, 109.13, 339.999, 1, 1, 2, 5, 3, 0, 17, 19),
		PropGrowStruct(0, -1190.45, 117.042, 19.9951, 1, 1, 2, 5, 10, 0, 16, 20),
		PropGrowStruct(0, -1110.89, 65.2589, 29.9982, -1, 1, 1, 5, 13, 0, 12, 8),
		PropGrowStruct(0, -1232.07, 115.719, 9.99756, -1, 1, 1, 5, 13, 0, 17, 19),
		PropGrowStruct(0, -1183.66, 120.482, 349.997, 1, 1, 2, 5, 10, 0, 16, 20),
		PropGrowStruct(0, -1258, 101, 0, 1, 1, 2, 5, 4, 0, 13, 19),
		PropGrowStruct(0, -800.204, 261.216, 9.99756, 1, 1, 2, 20, 8, 0, 16, 19),
		PropGrowStruct(0, -733, 15, 0, 1, 1, 2, 19, 2, 0, 17, 19),
		PropGrowStruct(0, -766.726, 29.2321, 9.99756, 1, 1, 2, 18, 3, 0, 13, 19),
		PropGrowStruct(0, -167, -383, 0, 1, 1, 2, 19, 2, 0, 19, 5),
		PropGrowStruct(0, 22.6401, -141.82, 349.997, 1, 1, 2, 20, 5, 0, 16, 18),
		PropGrowStruct(0, -337, -373, 0, 1, 1, 2, 18, 6, 0, 13, 19),
		PropGrowStruct(0, -343, -339, 0, 1, 1, 2, 19, 2, 0, 19, 5),
		PropGrowStruct(0, -41.5, -320, 0, 1, 1, 2, 18, 3, 0, 13, 19),
		PropGrowStruct(0, -334, 13, 0, 1, 1, 2, 5, 3, 0, 16, 21),
		PropGrowStruct(0, -363.108, -5.20673, 329.996, 1, 1, 2, 5, 4, 0, 16, 21),
		PropGrowStruct(0, -309.7, 8.65348, 39.9957, 1, 1, 2, 5, 3, 0, 16, 21),
		PropGrowStruct(0, -268.924, 21.718, 349.997, 1, 1, 1, 5, 13, 0, 17, 19),
		PropGrowStruct(0, -287.764, 8.68137, 329.996, 1, 1, 2, 5, 10, 0, 16, 20),
		PropGrowStruct(0, -328.766, 9.68513, 29.9982, 1, 1, 2, 5, 10, 0, 16, 20),
		PropGrowStruct(0, -238.619, 4.77878, 349.997, 1, 1, 2, 5, 4, 0, 16, 21),
		PropGrowStruct(0, -15.2193, -155.849, 309.996, 1, 1, 2, 20, 1, 0, 16, 18),
		PropGrowStruct(0, -26.476, -381.09, 289.995, 1, 1, 2, 20, 2, 0, 16, 17),
		PropGrowStruct(0, -198.111, -229.388, 339.999, 1, 1, 2, 19, 6, 0, 19, 5),
		PropGrowStruct(0, -505.892, -35.7411, 29.9982, -1, 1, 1, 5, 13, 0, 12, 8),
		PropGrowStruct(0, -763.699, 77.028, 49.9988, 1, 1, 2, 5, 10, 0, 16, 20),
		PropGrowStruct(0, -758, 168, 0, 1, 1, 2, 5, 10, 0, 16, 20),
		PropGrowStruct(0, -258, -125, 0, 1, 1, 2, 18, 1, 0, 13, 19),
		PropGrowStruct(0, -221.108, -15.2067, 329.996, 1, 1, 2, 5, 4, 0, 13, 19),
		PropGrowStruct(0, -237.892, 9.25887, 29.9982, -1, 1, 1, 5, 13, 0, 17, 19),
		PropGrowStruct(0, 105.691, 6.12482, 339.999, 1, 1, 2, 20, 4, 0, 16, 19),
		PropGrowStruct(0, -172.742, -101.893, 299.998, 1, 1, 1, 5, 13, 0, 12, 8),
		PropGrowStruct(0, -40.2595, 7.26706, 9.99756, 1, 1, 2, 20, 3, 0, 16, 19),
		PropGrowStruct(0, 64, -238, 0, 1, 1, 2, 19, 5, 0, 19, 5),
		PropGrowStruct(0, 99.0323, -316.863, 39.9957, 1, 1, 2, 20, 2, 0, 16, 14),
		PropGrowStruct(0, 18.2593, -130.252, 39.9957, 1, 1, 2, 20, 8, 0, 16, 18),
		PropGrowStruct(0, 289, -266, 0, 1, 1, 2, 18, 1, 0, 13, 19),
		PropGrowStruct(0, 356.954, -432.94, 329.996, 1, 1, 2, 18, 1, 0, 13, 19),
		PropGrowStruct(0, 256, -338, 0, 1, 1, 2, 19, 2, 0, 19, 5),
		PropGrowStruct(0, 65.0895, -204.476, 19.9951, 1, 1, 2, 20, 2, 0, 16, 17),
		PropGrowStruct(0, 456.549, -37.7552, 319.999, 1, 1, 1, 5, 13, 0, 12, 8),
		PropGrowStruct(0, 410.893, 14.2582, 29.9982, 1, 1, 1, 5, 13, 0, 13, 19),
		PropGrowStruct(0, 411, 6, 0, 1, 1, 1, 5, 13, 0, 17, 19),
		PropGrowStruct(0, 366.3, 11.6535, 39.9957, 1, 1, 2, 5, 3, 0, 16, 21),
		PropGrowStruct(0, 345.55, 23.0425, 19.9951, 1, 1, 2, 5, 10, 0, 16, 20),
		PropGrowStruct(0, 773.917, -60.8031, 329.996, 1, 1, 2, 18, 2, 0, 13, 19),
		PropGrowStruct(0, 324.524, 11.3082, 9.99756, 1, 1, 2, 5, 3, 0, 16, 21),
		PropGrowStruct(0, 345.345, 17.4821, 349.997, 1, 1, 2, 5, 10, 0, 16, 20),
		PropGrowStruct(0, 315.893, 10.2572, 329.996, 1, 1, 1, 5, 13, 0, 17, 19),
		PropGrowStruct(0, 11.4246, -74.6256, 149.996, 1, 1, 2, 19, 6, 0, 17, 19),
		PropGrowStruct(0, 242, -19, 0, 1, 1, 1, 5, 13, 0, 13, 19),
		PropGrowStruct(0, -19.4418, -95.9642, 189.998, 1, 1, 2, 19, 6, 0, 17, 19),
		PropGrowStruct(0, 88.5501, -157.958, 19.9951, 1, 1, 2, 5, 10, 0, 16, 20),
		PropGrowStruct(0, 116.216, -62.328, 339.999, 1, 1, 2, 5, 10, 0, 16, 20),
		PropGrowStruct(0, -68.4821, -125.655, 79.9969, 1, 1, 2, 5, 10, 0, 16, 20),
		PropGrowStruct(0, -101.766, -65.3149, 29.9982, 1, 1, 2, 5, 10, 0, 16, 20),
		PropGrowStruct(0, -39.1076, -0.206725, 329.996, 1, 1, 2, 5, 4, 0, 17, 22),
		PropGrowStruct(0, -52.4499, -5.95754, 19.9951, 1, 1, 2, 5, 10, 0, 16, 20),
		PropGrowStruct(0, -3, 12, 0, 1, 1, 1, 5, 13, 0, 17, 19),
		PropGrowStruct(0, 80.6026, -163.71, 9.99756, 1, 1, 2, 18, 6, 0, 13, 19),
		PropGrowStruct(0, -53.972, -143.301, 319.999, 1, 1, 2, 5, 10, 0, 16, 20),
		PropGrowStruct(0, 543.076, -24.282, 349.997, 1, 1, 1, 5, 13, 0, 12, 8),
		PropGrowStruct(0, 744, -56, 0, 1, 1, 2, 19, 4, 0, 17, 19),
		PropGrowStruct(0, 146, -159, 0, 1, 1, 2, 19, 2, 0, 19, 5),
		PropGrowStruct(0, 867.224, 148.431, 339.999, -1, 1, 2, 20, 8, 0, 16, 19),
		PropGrowStruct(0, 852.893, -28.7428, 329.996, 1, 1, 1, 5, 13, 0, 13, 19),
		PropGrowStruct(0, 863.355, -50.6472, 339.999, 1, 1, 2, 5, 4, 0, 16, 21),
		PropGrowStruct(0, 846.978, -17.8194, 319.999, 1, 1, 2, 5, 4, 0, 16, 21),
		PropGrowStruct(0, 793, 60, 0, 1, 1, 2, 5, 10, 0, 16, 20),
		PropGrowStruct(0, 800.275, -27.0419, 309.996, 1, 1, 2, 5, 10, 0, 16, 20),
	};
	uint prop_count;
	
	[hidden] array<int> props_t;
	[hidden] array<int> props_id;
	
	[hidden] int apple_id;
	[hidden] float apple_x;
	[hidden] float apple_y;
	
	[text] int grow_time = 120;
	
	dictionary sprite_sets;
	
	PropGrower()
	{
		@g = get_scene();
		
		prop_count = props.length();
		props_t.resize(prop_count);
		props_id.resize(prop_count);
		
		for(uint i = 0; i < prop_count; i++)
		{
			PropGrowStruct@ p = @props[i];
			props_id[i] = 0;
		}
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.self = @self;
		
		if(script.in_game)
		{
			const float x = self.x();
			const float y = self.y();
			
			for(uint i = 0; i < prop_count; i++)
			{
				PropGrowStruct@ p = @props[i];
				
				if(props_id[i] != 0)
				{
					@p.prop = prop_by_id(props_id[i]);
				}
				else
				{
					props_t[i] = int(min(0, p.y / 2));
					
					if(p.prop_set == 2 and (p.prop_group == 18 or p.prop_group == 19) and  p.prop_index >= 1 and  p.prop_index <= 8)
					{
						props_t[i] -= int(grow_time * 0.75);
					}
					
					string sprite_set, sprite_name;
					sprite_from_prop(p.prop_set, p.prop_group, p.prop_index, sprite_set, sprite_name);
					
					float scale_x = p.scale_x;
					float scale_y = p.scale_y;
					float rot = p.rotation;
					
					rectangle@ rect = get_sprites(sprite_set).get_sprite_rect(sprite_name, 0);
					float ox = (rect.left() + rect.get_width() * 0.5) * scale_x;
					float oy = (rect.top() + rect.get_height() * 1) * scale_y;
					rotate(ox, oy, rot * DEG2RAD, ox, oy);
					
					float off_x = x + p.x - ox;
					float off_y = y + p.y - oy;
					
					prop@ pr = create_prop(p.prop_set, p.prop_group, p.prop_index, p.layer, p.sub_layer, off_x, off_y, p.rotation);
					pr.scale_x(0);
					pr.scale_y(0);
					g.add_prop(pr);
					@p.prop = pr;
					props_id[i] = pr.id();
				}
			}
		}
	}
	
	sprites@ get_sprites(string sprite_set)
	{
		sprites@ spr = cast<sprites>(sprite_sets[sprite_set]);
		if(spr is null)
		{
			@spr = create_sprites();
			spr.add_sprite_set(sprite_set);
			@sprite_sets[sprite_set] = spr;
		}
		
		return spr;
	}
	
	void step()
	{
		const float x = self.x();
		const float y = self.y();
		
		int prop_busy_count = 0;
		
		for(uint i = 0; i < prop_count; i++)
		{
			PropGrowStruct@ p = @props[i];
			
			string sprite_set, sprite_name;
			sprite_from_prop(p.prop_set, p.prop_group, p.prop_index, sprite_set, sprite_name);
			
			const float t = min(1.0, max(0.0, props_t[i]) / grow_time);
			float scale_x = p.scale_x * t;
			float scale_y = p.scale_y * t;
			float rot = p.rotation;
			
			rectangle@ rect = get_sprites(sprite_set).get_sprite_rect(sprite_name, 0);
			float ox = (rect.left() + rect.get_width() * 0.5) * scale_x;
			float oy = (rect.top() + rect.get_height() * 1) * scale_y;
			rotate(ox, oy, rot * DEG2RAD, ox, oy);
			
			float off_x = p.x - ox;
			float off_y = p.y - oy;
			
			prop@ pr = p.prop;
			pr.scale_x(scale_x);
			pr.scale_y(scale_y);
			pr.x(x + off_x);
			pr.y(y + off_y);
			
			if(t < 1) prop_busy_count++;
			
			props_t[i]++;
		}
		
		if(prop_busy_count == 0)
		{
			entity@ apple = entity_by_id(apple_id);
			if(apple !is null)
			{
				apple.set_xy(apple_x, apple_y);
			}
			
			g.remove_entity(self.as_entity());
		}
	}
	
}

class PropGrowStruct
{
	
	int t;
	float x, y;
	float rotation;
	float scale_x, scale_y;
	uint prop_set, prop_group, prop_index;
	uint palette;
	uint layer, sub_layer;
	
	prop@ prop = null;
	
	PropGrowStruct(){}
	
	PropGrowStruct(int t, float x, float y, float rotation, float scale_x, float scale_y, uint prop_set, uint prop_group, uint prop_index, uint palette, uint layer, uint sub_layer)
	{
		this.t = t;
		this.x = x;
		this.y = y;
		this.rotation = rotation;
		this.scale_x = scale_x;
		this.scale_y = scale_y;
		this.prop_set = prop_set;
		this.prop_group = prop_group;
		this.prop_index = prop_index;
		this.palette = palette;
		this.layer = layer;
		this.sub_layer = sub_layer;
	}
	
}