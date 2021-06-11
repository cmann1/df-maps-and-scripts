class Rect : Drawable
{
	
	float x1;
	float y1;
	float x2;
	float y2;
	uint colour;
	
	Rect(float x1, float y1, float x2, float y2, float rotation, uint colour)
	{
		super(DrawableType::Rect);
		
		this.x1 = x1;
		this.y1 = y1;
		this.x2 = x2;
		this.y2 = y2;
		this.rotation = rotation;
		this.colour = colour;
	}
	
	void save(ByteArray@ &in data, StringTable@ &in string_table)
	{
		Drawable::save(data, string_table);
		data.writeFloat(x1);
		data.writeFloat(y1);
		data.writeFloat(x2);
		data.writeFloat(y2);
		data.writeUnsignedInt(colour);
	}
	
}