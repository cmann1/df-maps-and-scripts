class PropDef
{
	[hidden] string sprite_set = 'props1';
	[hidden] string sprite_name = 'books_8';
	[text] uint prop_set = 1;
	[text] uint prop_group = 0;
	[text] uint prop_index = 8;
	[text] float origin_x = 0.1;
	[text] float origin_y = 0.5;
	[text] float spacing = 55; 
	[text] int layer = 19;
	[text] int sub_layer = 19;
	[text] int end_layer = -1;
	[text] int end_sub_layer = -1;
	[text] int frame = 0;
	[text] int palette = 0;
	[text] float scale_x = 1;
	[text] float scale_y = 1;
	[angle] float rotation;
	[text] float rotation_rand = 0;
	[text] float scale_sx = 1;
	[text] float scale_sy = 1;
	[text] float scale_ex = 1;
	[text] float scale_ey = 1;
	
	void calculate_layer_ranges(int &out layer_start, int &out layer_end, int &out layer_count)
	{
		layer_start = (layer * 25 + sub_layer);
		layer_end   = (end_layer != -1 ? end_layer : layer) * 25 + (end_sub_layer < 0 ? sub_layer : end_sub_layer);
		layer_count = layer_end - layer_start;// + start_layer_offset + end_layer_offset;
	}
	
	void calculate_layers(int total_sub_layer, int &out current_layer, int &out current_sub_layer)
	{
		current_layer = (total_sub_layer / 25);
		current_sub_layer = (total_sub_layer % 25);
	}
	
	void calculate_layers(int layer_start, int layer_count, float t, int &out current_layer, int &out current_sub_layer)
	{
		calculate_layers(int(layer_start + layer_count * t), current_layer, current_sub_layer);
	}
}
