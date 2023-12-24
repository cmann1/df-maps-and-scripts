#include '../lib/drawing/Sprite.cpp';

void draw_sign(canvas@ g, Sprite@ spr, const int symbol, const float x, const float y, const float size, const float rotation, const uint clr)
{
	g.push();
	g.translate(x, y);
	g.rotate(rotation, 0, 0);
	g.scale(size, size);
	g.translate(-1.5, -1.5);
	
	switch(symbol)
	{
		case 0:
			g.draw_rectangle(1, 0, 2, 2,   0, clr);
			g.draw_rectangle(0, 2, 3, 3,   0, clr);
			break;
		case 1:
			g.draw_rectangle(0, 0, 1, 3,   0, clr);
			g.draw_rectangle(2, 0, 3, 3,   0, clr);
			break;
		case 2:
			g.draw_rectangle(0, 2, 3, 3,   0, clr);
			g.draw_rectangle(2, 0, 3, 2,   0, clr);
			break;
		case 3:
			g.draw_rectangle(0, 0, 3, 1,   0, clr);
			g.draw_rectangle(0, 1, 1, 2,   0, clr);
			g.draw_rectangle(2, 1, 3, 3,   0, clr);
			break;
		case 4:
			g.draw_rectangle(0, 0, 1, 1,   0, clr);
			g.draw_rectangle(2, 0, 3, 1,   0, clr);
			g.draw_rectangle(1, 1, 2, 2,   0, clr);
			g.draw_rectangle(0, 2, 1, 3,   0, clr);
			g.draw_rectangle(2, 2, 3, 3,   0, clr);
			break;
		case 5:
			g.draw_rectangle(1, 0, 2, 3,   0, clr);
			g.draw_rectangle(2, 0, 3, 1,   0, clr);
			g.draw_rectangle(0, 2, 1, 3,   0, clr);
			break;
		case 6:
			g.draw_rectangle(0, 2, 3, 3,   0, clr);
			g.draw_rectangle(0, 0, 1, 1,   0, clr);
			g.draw_rectangle(2, 0, 3, 1,   0, clr);
			break;
		case 7:
			g.draw_rectangle(0, 2, 3, 3,   0, clr);
			g.draw_rectangle(0, 0, 1, 1,   0, clr);
			g.draw_rectangle(2, 0, 3, 2,   0, clr);
			break;
		case 8:
			g.draw_rectangle(0, 0, 3, 1,   0, clr);
			g.draw_rectangle(0, 2, 3, 3,   0, clr);
			g.draw_rectangle(1, 1, 2, 2,   0, clr);
			break;
		case 9:
			g.draw_rectangle(0, 2, 3, 3,   0, clr);
			g.draw_rectangle(0, 1, 2, 2,   0, clr);
			g.draw_rectangle(0, 0, 1, 1,   0, clr);
			break;
		case 10:
			g.draw_rectangle(0, 0, 1, 3,   0, clr);
			g.draw_rectangle(2, 0, 3, 3,   0, clr);
			g.draw_rectangle(1, 0, 2, 1,   0, clr);
			break;
	}
	
	if(@spr != null)
	{
		const float scale_x = 0.01;
		const float scale_y = 0.02;
		float sx, sy;
		spr.real_position(1.5, 1.5, 0, sx, sy, scale_x, scale_y);
		g.draw_sprite(spr.sprite, spr.sprite_name, 0, 0, sx, sy, 0, scale_x, scale_y, clr);
	}
	
	g.pop();
}

entity@ emitter_by_id(scene@ g, const float x1, const float y1, const float x2, const float y2, const int emitter_id)
{
	int i = g.get_entity_collision(y1, y2, x1, x2, Emitter);
	while(--i >= 0)
	{
		entity@ e = g.get_entity_collision_index(i);
		if(@e == null)
			continue;
		
		varvalue@ v = e.vars().get_var('emitter_id');
		
		if(@v != null && v.get_int32() == emitter_id)
		{
			return e;
		}
	}
	
	return null;
}
