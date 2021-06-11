class TileRenderer : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	[text] int width = 1;
	[text] int height = 1;
	[text] array<string> tiles;
	
	array<string> tiles_spr;
	
	sprites@ spr;
	
	TileRenderer()
	{
		@g = get_scene();
		@spr = create_sprites();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		spr.add_sprite_set("script");
		
		const int size = tiles.length();
		tiles_spr.resize(width * height);
		int i = 0;
		for(int ix = 0; ix < width; ix++)
		{
			for(int iy = 0; iy < height; iy++)
			{
				tiles_spr[i++] = size > 0 ? tiles[rand() % size] : "";
			}
		}
	}
	
	void draw(float sub_frame)
	{
		const float x = floor(self.x() / 48);
		const float y = floor(self.y() / 48);
		const int frame = 0;
		const int palette = 0;
		
		int i = 0;
		for(int ix = 0; ix < width; ix++)
		{
			for(int iy = 0; iy < height; iy++)
			{
				spr.draw_world(20, 21, tiles_spr[i++], frame, palette, (x + ix) * 48, (y + iy) * 48, 0, 1, 1, 0xFFFFFFFF);
			}
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}

class SpriteRenderer : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	[text] string sprite = "grass";
	[text] int layer = 19;
	[text] int sub_layer = 19;
	
	sprites@ spr;
	
	SpriteRenderer()
	{
		@g = get_scene();
		@spr = create_sprites();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		spr.add_sprite_set("script");
	}
	
	void draw(float sub_frame)
	{
		const float x = floor(self.x() / 48);
		const float y = floor(self.y() / 48);
		const int frame = 0;
		const int palette = 0;
		spr.draw_world(layer, sub_layer, sprite, frame, palette, self.x(), self.y(), 0, 1, 1, 0xFFFFFFFF);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}