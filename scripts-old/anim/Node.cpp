enum NodeType
{
	Bone,
	Drawable
}

class Node
{
	
	int id = 0;
	NodeType node_type;
	
	float x = 0;
	float y = 0;
	float scale_x = 1;
	float scale_y = 1;
	float rotation = 0;
	
	Node(NodeType node_type)
	{
		this.node_type = node_type;
	}
	
	void reset()
	{
		x = 0;
		y = 0;
		scale_x = 1;
		scale_y = 1;
		rotation = 0;
	}
	
	void save(ByteArray@ &in data, StringTable@ &in string_table)
	{
		data.writeUnsignedByte(node_type);
		data.writeEncodedInt(id);
	}
	
}