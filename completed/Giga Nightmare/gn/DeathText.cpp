class DeathText
{
	
	string text;
	float x;
	float y;
	float scale_x;
	float scale_y;
	float rotation;
	
	DeathText()
	{
		
	}
	
	DeathText(string text, float x, float y, float scale_x, float scale_y, float rotation)
	{
		this.text = text;
		this.x = x;
		this.y = y;
		this.scale_x = scale_x;
		this.scale_y = scale_y;
		this.rotation = rotation;
	}
	
	void draw(textfield@ text_field, uint clr)
	{
		float x = this.x + rand_range(-5, 5);
		float y = this.y + rand_range(-5, 5);
		
		text_field.text(text);
//		text_field.colour(0xFF000000);
//		text_field.draw_hud(22, 24, x - 1, y - 1, scale_x, scale_y, rotation);
//		text_field.draw_hud(22, 24, x + 1, y - 1, scale_x, scale_y, rotation);
//		text_field.draw_hud(22, 24, x - 1, y + 1, scale_x, scale_y, rotation);
//		text_field.draw_hud(22, 24, x + 1, y + 1, scale_x, scale_y, rotation);
		text_field.colour(clr);
		text_field.draw_hud(22, 24, x, y, scale_x, scale_y, rotation);
	}
	
}