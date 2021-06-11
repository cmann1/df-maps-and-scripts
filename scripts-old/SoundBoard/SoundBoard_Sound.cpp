class Sound
{
	string name;
	uint col;
	uint row;
	uint group_index;
	float x;
	float y;
	float scale;
	textfield@ text;
	bool selected = false;
	
	float width = 0;
	float height = 0;
	
	Sound(uint group_index, string name, string font_name, uint font_size, float font_scale)
	{
		this.name = name;
		this.group_index = group_index;
		this.scale = font_scale;
		
		@text = create_textfield();
		text.text(name);
		text.set_font(font_name, font_size);
		text.align_horizontal(-1);
		text.align_vertical(-1);
		
		width = text.text_width() * scale;
		height = text.text_height() * scale;
	}
	
	void draw(scene@ g, float scroll_x, float scroll_y)
	{
		text.draw_world(15, 19, x - scroll_x, y - scroll_y, scale, scale, 0);
		
		if(selected)
		{
			const float padding = 6;
			outline_rect(g, x - scroll_x - padding, y - scroll_y - padding, x - scroll_x + width + padding * 2, y - scroll_y + height + padding * 2, 15, 20, 2, 0xFFFFFFFF);
		}
	}
	
	bool opEquals(Sound@ b)
	{
		return this is b;
	}
	
}