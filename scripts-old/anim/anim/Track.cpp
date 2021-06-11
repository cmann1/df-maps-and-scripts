class Track
{
	
	int node_id;
	
	array<int> frame_index;
	array<float> x;
	array<float> y;
	array<float> scale_x;
	array<float> scale_y;
	array<float> rotation;
	int num_keyframes = 0;
	
	int current = -1;
	int prev = -1;
	int next = -1;
	
	Track(int node_id)
	{
		this.node_id = node_id;
	}
	
	void addKeyFrame(int index, float x, float y, float scale_x, float scale_y, float rotation)
	{
		this.frame_index.insertLast(index);
		this.x.insertLast(x);
		this.y.insertLast(y);
		this.scale_x.insertLast(scale_x);
		this.scale_y.insertLast(scale_y);
		this.rotation.insertLast(rotation);
		num_keyframes++;
	}
	
	/////////
	
	void save(ByteArray@ &in data, StringTable@ &in string_table)
	{
		data.writeEncodedInt(node_id);
		data.writeEncodedInt(num_keyframes);
		
		for(int i = 0; i < num_keyframes; i++)
		{
			data.writeEncodedInt(frame_index[i]);
			data.writeFloat(x[i]);
			data.writeFloat(y[i]);
			data.writeFloat(scale_x[i]);
			data.writeFloat(scale_y[i]);
			data.writeFloat(rotation[i]);
		}
	}
	
}