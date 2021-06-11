#include "utils.cpp"
#include "drawing_utils.cpp"

class Debug
{
	scene@ g;
	
	uint max_lines = 100;
	array<DebugTextLine@> lines = {};
	array<DebugGraphic@> graphics = {};
	
	int align_x = -1;
	int align_y = -1;
	
	Debug(scene@ g)
	{
		@this.g = g;
	}
	
	void print(string text, int key=-1, uint colour=0xFFFFFFFF, int frames=120)
	{
		DebugTextLine@ line = null;
		
		if(key >= 0)
		{
			for(int i = lines.length() - 1; i >=0; i--)
			{
				DebugTextLine@ check_line = lines[i];
				if(check_line.key == key)
				{
					@line = check_line;
					break;
				}
			}
		}
		
		if(@line == null)
		{
			@line = DebugTextLine(text, key, colour, frames, align_x, align_y);
			lines.insertLast(line);
			
			if(lines.length() > max_lines)
			{
				lines.removeAt(0);
			}
		}
		
		line.text_field.text(text);
		line.frames = frames;
	}
	
	void line(float x1, float y1, float x2, float y2, uint layer, uint sub_layer, int frames=1, float thickness=2, uint colour=0xFFFFFFFF, bool world=true)
	{
		graphics.insertLast(DebugLineGraphic(x1, y1, x2, y2, layer, sub_layer, frames, thickness, colour, world));
	}
	
	void rect(float x1, float y1, float x2, float y2, uint layer, uint sub_layer, int frames=1, float thickness=2, uint colour=0xFFFFFFFF, bool world=true)
	{
		graphics.insertLast(DebugRectGraphic(x1, y1, x2, y2, layer, sub_layer, frames, thickness, colour, world));
	}
	
	void outline_tile(int x, int y, uint layer, uint sub_layer, int frames=1, float thickness=2, uint colour=0xFFFFFFFF, bool world=true)
	{
		x *= 48;
		y *= 48;
		graphics.insertLast(DebugRectGraphic(x, y, x + 48, y + 48, layer, sub_layer, frames, thickness, colour, world));
	}
	
	void step()
	{
		for(int i = graphics.length() - 1; i >=0; i--)
		{
			DebugGraphic@ graphic = graphics[i];
			if(graphic.frames == 0)
			{
				graphics.removeAt(i);
			}
			else if(graphic.frames > 0)
			{
				graphic.frames--;
			}
		}
		
		for(int i = lines.length() - 1; i >=0; i--)
		{
			DebugTextLine@ line = lines[i];
			if(line.frames == 0)
			{
				lines.removeAt(i);
			}
			else if(line.frames > 0)
			{
				line.frames--;
			}
		}
	}
	
	void draw() const
	{
		for(int i = graphics.length() - 1; i >=0; i--)
		{
			graphics[i].draw(g);
		}
		
		const float padding = 10;
		float x = align_x < 0 ? SCREEN_LEFT + padding : SCREEN_RIGHT - padding;
		float y = align_y < 0 ? SCREEN_TOP + padding : SCREEN_BOTTOM - padding;
		for(int i = lines.length() - 1; i >=0; i--)
		{
			DebugTextLine@ line = lines[i];
			line.text_field.draw_hud(20, 20, x, y, 0.6, 0.6, 0);
			
			y += line.text_field.text_height() + 4;
		}
	}
	
}

class DebugGraphic
{
	
	int frames;
	uint layer;
	uint sub_layer;
	bool world;
	
	DebugGraphic(uint layer, uint sub_layer, int frames, bool world)
	{
		this.frames = frames;
		this.layer = layer;
		this.sub_layer = sub_layer;
		this.world = world;
	}
	
	void draw(scene@ g) {}
	
}

class DebugLineGraphic : DebugGraphic
{
	float x1;
	float y1;
	float x2;
	float y2;
	float thickness;
	uint colour;
	
	DebugLineGraphic(float x1, float y1, float x2, float y2, uint layer, uint sub_layer, int frames=1, float thickness=2, uint colour=0xFFFFFFFF, bool world=true)
	{
		super(layer, sub_layer, frames, world);
		
		this.x1 = x1;
		this.y1 = y1;
		this.x2 = x2;
		this.y2 = y2;
		this.thickness = thickness;
		this.colour = colour;
	}
	
	void draw(scene@ g)
	{
		if(world)
		{
			g.draw_line(layer, sub_layer, x1, y1, x2, y2, thickness, colour);
		}
		else
		{
			g.draw_line_hud(layer, sub_layer, x1, y1, x2, y2, thickness, colour);
		}
	}
	
}

class DebugRectGraphic : DebugGraphic
{
	float x1;
	float y1;
	float x2;
	float y2;
	float thickness;
	uint colour;
	
	DebugRectGraphic(float x1, float y1, float x2, float y2, uint layer, uint sub_layer, int frames=1, float thickness=-1, uint colour=0xFFFFFFFF, bool world=true)
	{
		super(layer, sub_layer, frames, world);
		
		this.x1 = x1;
		this.y1 = y1;
		this.x2 = x2;
		this.y2 = y2;
		this.thickness = thickness;
		this.colour = colour;
	}
	
	void draw(scene@ g)
	{
		if(thickness <= 0)
		{
			if(world)
				g.draw_rectangle_world(layer, sub_layer, x1, y1, x2, y2, 0, colour);
			else
				g.draw_rectangle_hud(layer, sub_layer, x1, y1, x2, y2, 0, colour);
		}
		else
		{
			outline_rect(g, x1, y1, x2, y2, layer, sub_layer, thickness, colour, world);
		}
	}
	
}

class DebugTextLine
{
	textfield@ text_field;
	int key;
	int frames;
	
	DebugTextLine(string text, int key=-1, uint colour=0xFFFFFFFF, int frames=30, int align_x=-1, int align_y=-1)
	{
		this.key = key;
		this.frames = frames;
		
		@this.text_field = create_textfield();
		this.text_field.align_horizontal(align_x);
		this.text_field.align_vertical(align_y);
		this.text_field.set_font("ProximaNovaReg", 36);
		this.text_field.text(text);
		this.text_field.colour(colour);
	}
	
}