class Sprite : Drawable
{
	
	sprites@ sprite;
	string sprite_name;
	
	uint32 frame = 0;
	uint32 palette = 0;
	
	Sprite(string sprite_set, string sprite_name, float x=0, float y=0, float scale_x=1, float scale_y=1, float rotation=0)
	{
		super(DrawableType::Sprite);
		
		this.sprite_name = sprite_name;
		@sprite = create_sprites();
		sprite.add_sprite_set(sprite_set);
		
		this.x = x;
		this.y = y;
		this.scale_x = scale_x;
		this.scale_y = scale_y;
		this.rotation = rotation;
	}
	
	void save(ByteArray@ &in data, StringTable@ &in string_table)
	{
		Drawable::save(data, string_table);
		data.writeEncodedInt(string_table.get_string_id(sprite_name));
		data.writeUnsignedByte(frame);
		data.writeUnsignedByte(palette);
	}
	
	void reset()
	{
		Drawable::reset();
		frame = 0;
		palette = 0;
	}
	
}