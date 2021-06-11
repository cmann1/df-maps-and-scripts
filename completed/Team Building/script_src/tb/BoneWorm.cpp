#include "../common/Bezier.cpp"
#include "../common/Sprite.cpp"

class BoneWorm : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	[position,layer:19,y:y1] float x1 = 0;
	[hidden] float y1 = 0;
	[position,layer:19,y:y2] float x2 = 0;
	[hidden] float y2 = 0;
	[position,layer:19,y:y3] float x3 = 0;
	[hidden] float y3 = 0;
	[position,layer:19,y:y4] float x4 = 0;
	[hidden] float y4 = 0;
	[text] int layer = 17;
	[text] int rib_sub_layer = 18;
	[text] int spine_sub_layer = 19;
	[text] float scale = 1.5;
	[text] float spacing = 15;
	[text] float speed = 10;
	[text] float scale_rnd = 0.35;
	[text] float rot_rnd = 5;
	[text] float wobble = 5;
	
	float t = 0;
	
	Bezier path(0, 0, 0, 0, 0, 0, 0, 0);
	Sprite spine_spr("props2", "foliage_23", 0.5, 0.5);
	Sprite rib_spr("props2", "foliage_22", 0.45, 0.2);
	
	BoneWorm()
	{
		@g = get_scene();
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.self = @self;
		
		if(x1 == 0 and y1 == 0 and x2 == 0 and y2 == 0 and x3 == 0 and y3 == 0 and x4 == 0 and y4 == 0)
		{
			x1 = self.x() - 300;
			y1 = self.y() - 100;
			x2 = self.x() - 100;
			y2 = self.y() - 200;
			x3 = self.x() + 100;
			y3 = self.y() - 200;
			x4 = self.x() + 300;
			y4 = self.y() - 100;
		}
		
		path.x1 = x1;
		path.y1 = y1;
		path.x2 = x2;
		path.y2 = y2;
		path.x3 = x3;
		path.y3 = y3;
		path.x4 = x4;
		path.y4 = y4;
		path.update();
	}
	
	void step()
	{
		t += speed * DT;
	}
	
	void editor_step()
	{
		t += speed * DT;
	}
	
	void draw(float sub_frame)
	{
		const float spacing = this.spacing * scale;
		float d = 0 + t % spacing;
		float ox = path.mx(d);
		float oy = path.my(d);
		d += spacing;
		float i = 900 + floor(t / spacing);
		while(d <= path.length)
		{
			const float rnd = float(i * (i + 3) % 100) / 99;
			const float rnd2 = float(i * 2 * (i + 3) % 100) / 99;
			const float rnd3 = float(i * 4 * (i + 3) % 100) / 99;
			const float s = scale * (1 + rnd * scale_rnd);
			const float s2 = scale * (1 + rnd2 * scale_rnd * 1.2);
			const float s3 = scale * (1 + rnd3 * scale_rnd * 1.2);
			const float px = path.mx(d);
			const float py = path.my(d);
			const float rot = atan2(py - oy, px - ox) * RAD2DEG + sin((t + d) * 0.05) * wobble + rnd * rot_rnd;
			if(rnd2 > 0.3)
				rib_spr.draw_world(layer, rib_sub_layer, 0, 0, px, py, rot + rnd2 * rot_rnd, s2, s2, 0xFFFFFFFF);
			if(rnd3 > 0.3)
				rib_spr.draw_world(layer, rib_sub_layer, 0, 0, px, py, rot + 180 + rnd3 * rot_rnd, -s3, s3, 0xFFFFFFFF);
			spine_spr.draw_world(layer, spine_sub_layer, 0, 0, px, py, rot + 103, s, s, 0xFFFFFFFF);
			d += spacing;
			ox = px;
			oy = py;
			i--;
		}
	}
	
	void editor_draw(float sub_frame)
	{
		if(x1 != path.x1 or y1 != path.y1 or x2 != path.x2 or y2 != path.y2 or x3 != path.x3 or y3 != path.y3 or x4 != path.x4 or y4 != path.y4)
		{
			path.x1 = x1;
			path.y1 = y1;
			path.x2 = x2;
			path.y2 = y2;
			path.x3 = x3;
			path.y3 = y3;
			path.x4 = x4;
			path.y4 = y4;
			path.update();
		}
		
		draw(sub_frame);
		
		const float x = self.x();
		const float y = self.y();
		const float size = 4;
		g.draw_rectangle_world(layer, 24, x1 - size, y1 - size, x1 + size, y1 + size, 0, 0xFFFFFFFF);
		g.draw_rectangle_world(layer, 24, x2 - size, y2 - size, x2 + size, y2 + size, 0, 0xFF0000FF);
		g.draw_rectangle_world(layer, 24, x3 - size, y3 - size, x3 + size, y3 + size, 0, 0xFF0000FF);
		g.draw_rectangle_world(layer, 24, x4 - size, y4 - size, x4 + size, y4 + size, 0, 0xFFFFFFFF);
		
		if(self.editor_selected())
		{
			const float s = 6;
			g.draw_rectangle_world(21, 21, x1 - s, y1 - s, x1 + s, y1 + s, 0, 0x66FF0000);
			g.draw_rectangle_world(21, 21, x2 - s, y2 - s, x2 + s, y2 + s, 0, 0x66FF0000);
			g.draw_rectangle_world(21, 21, x3 - s, y3 - s, x3 + s, y3 + s, 0, 0x66FF0000);
			g.draw_rectangle_world(21, 21, x4 - s, y4 - s, x4 + s, y4 + s, 0, 0x66FF0000);
		}
	}
	
}


















