class Tentacles : trigger_base, callback_base
{
	
	script@ script;
	scene@ g;
	scripttrigger@ self;
	
	[text] int layer = 14;
	[text] int sublayer = 19;
	[text] int glow_layer = 14;
	[text] int glow_sublayer = 24;
	[text] float width_start_min = 18;
	[text] float width_start_max = 28;
	[text] float width_end_min = 18;
	[text] float width_end_max = 28;
	[colour,alpha] uint colour = 0xFFFFFFFF;
	
	[text] array<Tendril> tendrils;
	
	Sprite glow_spr('props3', 'backdrops_3');
	
	bool active = false;
	
	Tentacles()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		self.editor_colour_active(0xFFDDDDDD);
		self.editor_colour_inactive(0xFFDDDDDD);
		
		script.vars.listen('main_door_open', this, 'on_main_door_unlock');
		active = script.vars.get_bool('main_door_open');
		
		for(int i = int(tendrils.size()) - 1; i >= 0; i--)
		{
			Tendril@ tendril = @tendrils[i];
			@tendril.glow_spr = @glow_spr;
			tendril.width_start = rand_range(width_start_min, width_start_max);
			tendril.width_end = rand_range(width_end_min, width_end_max);
			tendril.colour = colour;
			tendril.layer = layer;
			tendril.sublayer = sublayer;
			tendril.glow_layer = glow_layer;
			tendril.glow_sublayer = glow_sublayer;
			
//			for(int j = int(tendril.joints.size()) - 1; j >= 0; j--)
//			{
//				tendril.joints[j].x += 48 * 1;
//			}
		}
	}
	
	void on_main_door_unlock(string id, message@ msg)
	{
		active = script.vars.get_bool('main_door_open');
	}
	
	void step()
	{
		if(!active) return;
		
		for(int i = int(tendrils.size()) - 1; i >= 0; i--)
			tendrils[i].step();
	}
	
	void draw(float sub_frame)
	{
		if(!active) return;
		
		for(int i = int(tendrils.size()) - 1; i >= 0; i--)
			tendrils[i].draw(g, sub_frame);
	}
	
	void editor_draw(float sub_frame)
	{
		if(!self.editor_selected()) return;
		
		for(int i = int(tendrils.size()) - 1; i >= 0; i--)
			tendrils[i].editor_draw(g, sub_frame);
	}
	
}

class Tendril
{
	
	float t;
	uint colour;
	int layer;
	int sublayer;
	int glow_layer;
	int glow_sublayer;
	float width_start;
	float width_end;
	Sprite@ glow_spr;
	[text] float width_multiplier = 1;
	[text] array<TendrilJoint> joints;
	
	Tendril()
	{
		t = rand() % 500;
	}
	
	void step()
	{
		const int num_joints = int(joints.size());
		const float width_diff = abs(width_end - width_start);
		const float width_min = min(width_end, width_start);
		
		if(num_joints < 2) return;
		
		for(int i = num_joints - 1; i >= 0; i--)
		{
			TendrilJoint@ joint = joints[i];
			TendrilJoint@ joint_left = i > 0 ? @joints[i - 1] : null;
			TendrilJoint@ joint_right = i < num_joints - 1 ? @joints[i + 1] : null;
			
			const float width_t = (cos((t + i * 20) * 0.025) + 1) / 2 * 0.4 + 1;
			const float w = width_min + width_diff * width_multiplier * (1 - float(i) / (num_joints - 1)) * width_t;
			
			const float bob_t = (joint.x + joint.y + t)  / 25;
			const float jx = joint.x + sin(bob_t) * 3;
			const float jy = joint.y + cos(bob_t) * 6;
			
			if(joint_left is null and joint_right is null)
			{
				joint.x1 = jx + w;
				joint.y1 = jy + w;
				joint.x2 = jx - w;
				joint.y2 = jy - w;
				break;
			}
			
			joint.x1 = 0; joint.y1 = 0;
			joint.x2 = 0; joint.y2 = 0;
			
			if(joint_left !is null)
			{
				joint.calculate(@joint_left, @joint, w);
			}
			if(joint_right !is null)
			{
				joint.calculate(@joint, @joint_right, w);
				if(joint_left !is null)
				{
					joint.x1 /= 2;
					joint.y1 /= 2;
					joint.x2 /= 2;
					joint.y2 /= 2;
				}
			}
			
			joint.x1 += jx;
			joint.y1 += jy;
			joint.x2 += jx;
			joint.y2 += jy;
		}
		
		t++;
	}
	
	void draw(scene@ g, float sub_frame)
	{
		
		const int num_joints = int(joints.size());

		if(num_joints < 2 or glow_spr is null) return;
		
		for(int i = num_joints - 2, j = i + 1; i >= 0; i--, j--)
		{
			TendrilJoint@ joint1 = joints[i];
			TendrilJoint@ joint2 = joints[j];
			g.draw_quad_world(layer, sublayer, false,
				joint1.x1, joint1.y1,
				joint1.x2, joint1.y2,
				joint2.x2, joint2.y2,
				joint2.x1, joint2.y1,
				colour, colour, colour, colour);
			
			const float j1x = (joint1.x1 + joint1.x2) * 0.5;
			const float j1y = (joint1.y1 + joint1.y2) * 0.5;
			const float j2x = (joint2.x1 + joint2.x2) * 0.5;
			const float j2y = (joint2.y1 + joint2.y2) * 0.5;
			const float dx = j1x - j2x;
			const float dy = j1y - j2y;
			glow_spr.draw_world(glow_layer, glow_sublayer, 0, 0,
				(j1x + j2x) * 0.5, (j1y + j2y) * 0.5,
				atan2(dy, dx) * RAD2DEG,
				joint1.length / glow_spr.sprite_width * 1.5, joint1.width / glow_spr.sprite_height * 4,
				0xFFFFFFFF);
			glow_spr.draw_world(glow_layer + 3, glow_sublayer, 0, 0,
				(j1x + j2x) * 0.5, (j1y + j2y) * 0.5,
				atan2(dy, dx) * RAD2DEG,
				joint1.length / glow_spr.sprite_width * 1.5, joint1.width / glow_spr.sprite_height * 1,
				0xFFFFFFFF);
		}
	}
	
	void editor_draw(scene@ g, float sub_frame)
	{
//		step();
//		draw(g, sub_frame);
		
		for(int i = int(joints.size()) - 2, j = i + 1; i >= 0; i--, j--)
		{
			TendrilJoint@ joint1 = joints[i];
			TendrilJoint@ joint2 = joints[j];
			draw_dot(g, 22, 22, joint1.x, joint1.y, 5, 0x88FF0000, 45);
			g.draw_line_world(22, 22, joint1.x, joint1.y, joint2.x, joint2.y, 3, 0x88FFFFFF);
		}
	}
	
}

class TendrilJoint
{
	
	float x1, y1;
	float x2, y2;
	float mx, my;
	float width;
	float length;
	
	[position,mode:world,layer:19,y:y] float x;
	[hidden] float y;
	[text] float width_multiplier = 1;
	
	TendrilJoint()
	{
	}
	
	void calculate(TendrilJoint@ from, TendrilJoint@ to, float width)
	{
		this.width = width;
		float dx = to.x - from.x;
		float dy = to.y - from.y;
		length = magnitude(dx, dy);
		if(length == 0) length = 0.001;
		mx = from.x + dx * 0.5;
		my = from.y + dy * 0.5;
		dx = dx / length;
		dy = dy / length;
		x1 += -dy * width;
		y1 +=  dx * width;
		x2 -= -dy * width;
		y2 -=  dx * width;
	}
	
}