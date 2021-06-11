class Gib : enemy_base
{
	
	scene@ g;
	scriptenemy@ self;
	sprites@ spr;
	
	float size;
	float init_vel_x;
	float init_vel_y;
	int splat_timer_max = 20;
	[text] float vel_x;
	[text] float vel_y;
	float prev_vel_x = 0;
	float prev_vel_y = 0;
	[text] string sprite;
	[text] float vel_rot = 0;
	[text] int splat_timer = splat_timer_max;
	[text] int life = 200;
	[text] int sub_layer = 11;
	
	Gib()
	{
		@g = get_scene();
		@spr = create_sprites();
		spr.add_sprite_set("script");
		int r = rand() % 3;
		if(r == 1) r = 0;
		sub_layer += r;
	}
	
	void init(script @s, scriptenemy @self)
	{
		@this.self = self;
		rectangle@ r = spr.get_sprite_rect(sprite, 0);
		size = min(r.get_width(), r.get_height()) * 0.25;
		self.base_rectangle(-size, size, -size, size);
		self.hit_rectangle(-size, size, -size, size);
		self.rotation(-20 + int(rand() % 40));
		const float max_speed = 100;
		self.set_speed_xy(prev_vel_x = init_vel_x - max_speed + frand() * max_speed * 2, prev_vel_y = init_vel_y - max_speed + frand() * max_speed * 2);
		vel_rot = -3 + int(rand() % 7);
	}
	
	void step()
	{
		prev_vel_x = vel_x;
		prev_vel_y = vel_y;
		vel_x = self.x_speed();
		vel_y = self.y_speed();
		
		if(life-- > 0)
		{
			float splat_dx = 0;
			float splat_dy = 0;
			const float v_min = 5;
			if(self.ground())
			{
				if(prev_vel_y > v_min) splat_dy = 1;
			}
			else if(self.roof())
			{
				if(prev_vel_y < -v_min) splat_dy = -1;
			}
			else if(self.wall_left())
			{
				if(prev_vel_x < -v_min) splat_dx = -1;
			}
			else if(self.wall_right())
			{
				if(prev_vel_x > v_min) splat_dx = 1;
			}
			
			if(splat_timer == splat_timer_max and (splat_dx != 0 or splat_dy != 0))
			{
				prop@ p = create_prop();
				p.layer(19);
				p.sub_layer(19);
				p.prop_set(2);
				p.prop_group(5);
				p.prop_index(16 + rand() % 2);
				const float s = 0.6 + frand() * 0.3;
				p.scale_x(s);
				p.scale_y(s);
				p.x(self.x() + splat_dx * size * 3);
				p.y(self.y() + splat_dy * size * 3);
				p.rotation(rand() % 360);
				g.add_prop(p);
				splat_timer = 0;
			}
			
			if(splat_timer < splat_timer_max)
			{
				splat_timer++;
			}
		}
		
		vel_x *= 0.99;
		vel_y *= 0.99;
		vel_rot *= 0.99;
		
		if(self.ground())
		{
			vel_x *= 0.98;
			vel_rot *= 0.95;
			if(abs(prev_vel_y * 0.5) > GRAVITY * DT) 
				vel_y = -prev_vel_y * 0.5;
		}
		else
		{
			vel_y += GRAVITY * DT;
		}
		
		life--;
		
		self.set_speed_xy(vel_x, vel_y);
		self.rotation(self.rotation() + vel_rot);
	}
	
	void draw(float sub_frame)
	{
		spr.draw_world(18, sub_layer, sprite, 0, 0, lerp(self.prev_x(), self.x(), sub_frame), lerp(self.prev_y(), self.y(), sub_frame), self.rotation(), 1, 1, 0xFFFFFFFF);
	}
	
}

void spawn_gibs(scene@ g, dustman@ dm)
{
	rectangle@ rect = dm.collision_rect();
	
	string char_prefix = "dm";
	const string char_name = dm.character();
	if(char_name == "dustkid") char_prefix = "dk";
	else if(char_name == "dustgirl") char_prefix = "dg";
	else if(char_name == "dustworth") char_prefix = "dw";
	
	const float x = dm.x() + (rect.left() + rect.right()) * 0.5;
	const float y = dm.y() + (rect.top() + rect.bottom()) * 0.5;
	array<float> pos = {
		0, 0, // torso
		0, -30, // head
		-24, 0, // arm l
		24, 0, // arm r
		-18, 36, // leg l
		18, 36, // leg r
	};
	
	for(int i = 0; i < 6; i++)
	{
		Gib@ gib = Gib();
		gib.init_vel_x = dm.x_speed();
		gib.init_vel_y = dm.y_speed();
		gib.sprite = char_prefix + "_gib_0" + (i + 1);
		scriptenemy@ e = create_scriptenemy(gib);
		e.set_xy(x + pos[i * 2] - 5 + rand() % 10, y + pos[i * 2 + 1] - 5 + rand() % 10);
		g.add_entity(e.as_entity(), true);
//		break;
	}
	
	
	BloodSplatter blood;
	blood.vel_x = dm.x_speed() * 0.5;
	blood.vel_y = dm.y_speed() * 0.5;
	scripttrigger@ tr = create_scripttrigger(blood);
	tr.set_xy(x, y);
	g.add_entity(tr.as_entity(), true);
}