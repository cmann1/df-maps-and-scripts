class Bone : Node
{
	
	string name;
	
	float base_length;
	float length;
	
	int depth = 0;
	
	Bone@ parent = null;
	array<Bone@> children;
	int num_children = 0;
	
	array<Drawable@> drawables;
	int num_drawables = 0;
	
	Bone(string name, float length=100)
	{
		super(NodeType::Bone);
		this.name = name;
		this.length = base_length = length;
	}
	
	void save(ByteArray@ &in data, StringTable@ &in string_table)
	{
		Node::save(data, string_table);
		data.writeEncodedInt(string_table.get_string_id(name));
		data.writeFloat(base_length);
	}
	
	void reset()
	{
		Node::reset();
		length = base_length;
	}
	
	void clear()
	{
		for(int i = 0; i < num_children; i++)
		{
			@children[i].parent = null;
			children[i].depth = 0;
		}
		children.resize(num_children = 0);
		
		drawables.resize(num_drawables = 0);
	}
	
	void addChild(Bone@ child)
	{
		if(child.parent is this) return;
		
		if(@child.parent != null)
		{
			child.parent.removeChild(child);
		}
		
		@child.parent = this;
		child.depth = depth + 1;
		children.insertLast(child);
		num_children++;
	}
	
	void removeChild(Bone@ child)
	{
		if(child.parent !is this) return;
		
		@child.parent = null;
		child.depth = 0;
		children.removeAt(children.findByRef(child));
		num_children--;
	}
	
	Drawable@ addDrawable(Drawable@ child)
	{
		drawables.insertLast(child);
		num_drawables++;
		
		return child;
	}
	
	Drawable@ removeDrawable(Drawable@ child)
	{
		const int index = drawables.findByRef(child);
		if(index >= 0)
		{
			children.removeAt(index);
			num_drawables--;
		}
		
		return child;
	}
	
}