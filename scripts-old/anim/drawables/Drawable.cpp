#include 'Sprite.cpp'
#include 'Text.cpp'
#include 'Rect.cpp'
#include 'Line.cpp'

enum DrawableType
{
	None,
	Sprite,
	Text,
	Rect,
	Line
}

class Drawable : Node
{
	
	DrawableType type;
	
	int layer = 17;
	int sub_layer = 19;
	
	Drawable(DrawableType type=DrawableType::None)
	{
		super(NodeType::Drawable);
		this.type = type;
	}
	
	void save(ByteArray@ &in data, StringTable@ &in string_table)
	{
		Node::save(data, string_table);
		data.writeUnsignedByte(type);
		data.writeUnsignedByte(layer);
		data.writeUnsignedByte(sub_layer);
	}
	
}