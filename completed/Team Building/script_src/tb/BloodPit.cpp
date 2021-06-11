class BloodPit : trigger_base
{
	scene@ g;
	scripttrigger@ self;

//	array<BloodPitBlood@> entities;

	[colour,alpha] uint colour = 0xFFFFFFFF;
	[colour,alpha] uint colour2 = 0xFFFFFFFF;
	[text] float tile_length = 10;
	[text] float resolution = 1;
	[position,layer:19,y:pool_y] float pool_x = 0;
	[hidden] float pool_y = 0;
	[text] float height = 48;
	[text] int layer = 20;
	[text] int sub_layer = 10;
	float px = 0;
	float py = 0;

	[text] float tension = 0.0075;
	[text] float dampening = 0.05;
	[text] float spread = 1;
	[text] float splash_damping = 275;

	[text] float splash_min_speed = 1;
	[text] float splash_min_dist = 10;
	[text] int splash_emitter_id = 126;
	[text] int splash_layer = 18;
	[text] int splash_sub_layer = 24;

	array<WaterColumn> columns;
	int columm_count = 0;
	float column_spacing = 0;

	array<SplashEmitter@> splashes;
	int num_splashes = 0;

	BloodPit()
	{
		@g = get_scene();
	}

	void init(script @s, scripttrigger @self)
	{
		@this.self = self;
		px = floor(pool_x / 48) * 48;
		py = ceil(pool_y / 48) * 48;
		columm_count = int(max(2, tile_length * resolution));
		columns.resize(columm_count);
		column_spacing = 48 / resolution;

		for(int i = 0; i < columm_count; i++)
		{
			WaterColumn@ c = @columns[i];
			c.target_height = c.height = height;
		}
	}

	void activate(controllable@ e)
	{
//		BloodPitBlood@ blood = null;
//
//		for(int i = int(entities.length) - 1; i >= 0 ; i--)
//		{
//			@blood = entities[i];
//			if(blood.e.is_same(e))
//			{
//				break;
//			}
//
//			@blood = null;
//		}
//
//		if(blood is null)
//		{
//			@blood = BloodPitBlood(e);
//			entities.insertLast(blood);
//		}
//
//		blood.update(self.y() - self.radius());
		splash(e.x() - px, max(abs(e.x_speed()), abs(e.y_speed())) / splash_damping);
	}

	void step()
	{
		for(int i = 0; i < columm_count; i++)
		{
			columns[i].update(dampening, tension);
		}

		array<float> left_deltas(columm_count);
		array<float> right_deltas(columm_count);

		// do some passes where springs pull on their neighbours
		for(int j = 0; j < 1; j++)
		{
			for(int i = 0; i < columm_count; i++)
			{
				if(i > 0)
				{
					left_deltas[i] = spread * (columns[i].height - columns [i - 1].height) * DT;
					columns[i - 1].speed += left_deltas[i];
				}
				if (i < columm_count - 1)
				{
					right_deltas[i] = spread * (columns[i].height - columns [i + 1].height) * DT;
					columns[i + 1].speed += right_deltas[i];
				}
			}

			for (int i = 0; i < columm_count; i++)
			{
				if(i > 0)
					columns[i - 1].height += left_deltas[i];
				if(i < columm_count - 1)
					columns[i + 1].height += right_deltas[i];
			}
		}

//		for(int i = int(entities.length) - 1; i >= 0 ; i--)
//		{
//			BloodPitBlood@ blood = entities[i];
//			if(blood.time-- <= 0)
//			{
//				entities.removeAt(i);
//			}
//		}
	}

	void draw(float sub_frame)
	{
		float x = px;
		float y = py;
		for(int i = 1; i < columm_count; i++)
		{
			float height_1 = columns[i - 1].height;
			float height_2 = columns[i].height;
			g.draw_quad_world(layer, sub_layer, false,
				x, y, x + column_spacing, y,
				x + column_spacing, y - height_2, x, y - height_1,
				colour2, colour2, colour, colour);

			x += column_spacing;
		}

		for(int i = num_splashes - 1; i >= 0; i--)
		{
			if(splashes[i].time-- <= 0)
			{
				splashes[i].remove_emitter(g);
				splashes.removeAt(i);
				num_splashes--;
			}
		}

//		for(int i = int(entities.length) - 1; i >= 0 ; i--)
//		{
//			BloodPitBlood@ blood = entities[i];
//
//		}
	}

	void editor_draw(float sub_frame)
	{
		px = floor(pool_x / 48) * 48;
		py = ceil(pool_y / 48) * 48;
		draw(sub_frame);
	}

	void splash(float x, float speed)
	{
		int index = int(max(0, min(columm_count - 1, x / column_spacing)));
		if(index < 0) index = 0;
		else if(index >= columm_count) index = columm_count - 1;
		for(int i = int(max(0, index - 1)), e = int(min(columm_count - 1, index + 2)); i < e; i++)
		{
			if(abs(speed) > abs(columns[i].speed))
				columns[i].speed = speed;
		}

		if(speed > splash_min_speed)
		{
			if(num_splashes == 0 or abs(splashes[num_splashes - 1].x - (px + x)) > splash_min_dist)
			{
				splashes.insertLast(SplashEmitter(g, px + x, py - columns[index].height, splash_emitter_id, splash_layer, splash_sub_layer));
				num_splashes++;
			}
		}
	}

}

class SplashEmitter
{

	int max_time = 5;
	int time = max_time;
	float x;
	float y;
	entity@ emitter;

	SplashEmitter(scene@ g, float x, float y, int emitter_id, int layer, int sub_layer)
	{
		this.x = x;
		this.y = y;
		@emitter = create_emitter(emitter_id, x, y, 30, 30, layer, sub_layer);
		g.add_entity(emitter, false);
	}

	void remove_emitter(scene@ g)
	{
		g.remove_entity(emitter);
		@emitter = null;
	}

}

class WaterColumn
{
	float target_height = 0;
	float height = 0;
	float speed = 0;

	void update(float dampening, float tension)
	{
		float x = target_height - height;
		speed += tension * x - speed * dampening;
		height += speed;
	}
}

//class BloodPitBlood
//{
//
//	controllable@ e;
//	float blood_height = 0;
//	float e_bottom = 0;
//	int time_max = 60;
//	int time = time_max;
//
//	BloodPitBlood(controllable@ e)
//	{
//		@this.e = e;
//		rectangle@ r = e.collision_rect();
//		e_bottom = r.bottom();
//	}
//
//	void update(float blood_top)
//	{
//		blood_height = max(blood_height, e.y() + e_bottom - blood_top);
//		time = time_max;
//	}
//
//}

//class WetBlood : trigger_base
//{
//	scene@ g;
//	scripttrigger@ self;
//
//	WetBlood()
//	{
//		@g = get_scene();
//	}
//
//	void init(script @s, scripttrigger @self)
//	{
//		@this.self = self;
//	}
//
//}
