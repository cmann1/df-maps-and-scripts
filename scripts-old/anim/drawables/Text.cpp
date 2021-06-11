class Text : Drawable
{
	
	textfield@ txt;
	
	Text(float x, float y, string text, string font='Caracteres', uint font_size=36, uint colour=0xFFFFFFFF, int align_h=-1, int align_v=-1)
	{
		super(DrawableType::Text);
		
		this.x = x;
		this.y = y;
		
		@txt = create_textfield();
		txt.text(text);
		txt.set_font(font, font_size);
		txt.colour(colour);
		txt.align_horizontal(align_h);
		txt.align_vertical(align_v);
	}
	
	void save(ByteArray@ &in data, StringTable@ &in string_table)
	{
		Drawable::save(data, string_table);
		data.writeEncodedInt(string_table.get_string_id(txt.text()));
		data.writeEncodedInt(string_table.get_string_id(txt.font()));
		data.writeUnsignedInt(txt.colour());
		data.writeByte(txt.align_horizontal());
		data.writeByte(txt.align_vertical());
	}
	
}