#include '../lib/drawing/common.cpp';
#include '../lib/enums/GVB.cpp';
#include '../lib/utils/colour.cpp';
#include '../lib/string.cpp';

class script
{
	[persist] bool enabled = true;
	[persist|tooltip:'Click to update after making any changes to the text trigger.']
	bool update;
	[entity,trigger|tooltip:
		'Each line represents a layer and should be in the format:\n'
		'  LAYER.SUB_LAYER - DESCRIPTION\n'
		'SUB_LAYER is optional e.g\n'
		'  12 - BG tiles\n'
		'  12.19 - Lamps']
	uint layers_ztext_trigger_id;
	[persist] float line_width = 1;
	[persist|tooltip:
		'If true scale the line width by the camera size to\n'
		'keep a consistent size regardless of the camera zoom.']
	bool dynamic_line_width = true;
	[persist] array<Layer> layers;
	[slider,min:0,max:1] float hover_fade_alpha = 0.5;
	[slider,min:0,max:1] float press_text_alpha = 0.25;
	[slider,min:0,max:1] float press_line_alpha = 0.0;
	[option,'None','Hover','Press'] LayerRevealType reveal_layers_on = Press;
	[persist|tooltip:
		'If true hides all layers except the revealed one.\n'
		'If false hides all layers above.']
	bool reveal_single = false;
	[persist] float hover_line_width_mult = 1.5;
	[persist] float press_line_width_mult = 1.5;
	[persist|tooltip:
		'If set, adjusts the camera fog to highlight the hovered layer.\n'
		'sample_fog_id must be set for layer highlighting to work.']
	bool hover_highlight = true;
	[persist|tooltip:'Highlights while the mouse is pressed.']
	bool press_highlight = false;
	[colour,alpha|tooltip:'Alpha value represents the fog blend percent']
	uint hover_highlight_colour = 0x445c5e26;
	[entity,trigger]
	uint sample_fog_id;
	[position,mode:world,layer:20,y:sample_fog_y1|tooltip:
		'If this and sample_fog_id are set, will automatically change\n'
		'to the sample fog when the camera enters this region.']
	float sample_fog_x1;
	[hidden] float sample_fog_y1;
	[position,mode:world,layer:20,y:sample_fog_y2]
	float sample_fog_x2;
	[hidden] float sample_fog_y2;
	
	[colour,alpha|tooltip:'Draws a box on the sub layer below the layer text.']
	uint text_bg = 0x66000000;
	[persist] bool txt_bg_glass = true;
	[persist] float text_bg_padding = 48;
	[persist] array<BgBox> bg_boxes;
	
	[hidden] bool has_stored_txt_colour;
	[hidden] uint stored_txt_colour;
	
	scene @ g;
	editor_api@ editor;
	input_api@ input;
	camera@ cam;
	float view_x, view_y;
	float ed_zoom;
	
	bool pending_init = true;
	
	entity@ trigger;
	varstruct@ vars;
	varvalue@ colour_var;
	
	float txt_x, txt_y;
	float txt_w, txt_h;
	int txt_layer, txt_sub_layer;
	string txt_font;
	int txt_font_size;
	float txt_scale;
	float txt_line_height;
	uint txt_colour;
	float txt_alpha;
	textfield@ txt_field;
	
	array<LayerLine@> lines;
	float lines_x1, lines_y1;
	float lines_x2, lines_y2;
	int line_hover_index = -1;
	LayerLine@ hovered_line;
	bool layers_revealed;
	float text_fade_alpha = 1;
	float line_fade_alpha = 1;
	float width_mult = 1;
	
	entity@ sample_fog_trigger;
	fog_setting@ sample_fog;
	bool sample_fog_has_sub_layers;
	float sample_fog_time;
	float sample_fog_rect_timer;
	bool inside_sample_fog_region;
	bool is_layer_highlighted;
	int highlight_layer = -1;
	int highlight_sub_layer = -1;
	array<uint> highlight_base_colour(25);
	array<float> highlight_base_percent(25);
	
	script()
	{
		@g = get_scene();
		@editor = get_editor_api();
		@input = get_input_api();
		
		@txt_field = create_textfield();
		txt_field.align_horizontal(-1);
		txt_field.align_vertical(-1);
	}
	
	void editor_var_changed(var_info@ info)
	{
		if (info.name == 'layers_ztext_trigger_id' || info.name == 'update')
		{
			const bool force_read_txt_colour = info.name == 'update';
			parse_text(force_read_txt_colour);
			update_layers();
		}
		else if(info.get_name(0) == 'layers')
		{
			if (info.name == 'layers' || info.name == 'layer' || info.name == 'sub_layer')
			{
				update_layers();
			}
			else if(info.name == 'x')
			{
				Layer@ layer = @layers[info.get_index(0)];
				layer.x = input.mouse_x_world(clamped_layer(layer.layer));
			}
			else if(info.name == 'y')
			{
				Layer@ layer = @layers[info.get_index(0)];
				layer.y = input.mouse_y_world(clamped_layer(layer.layer));
			}
		}
		else if (info.name == 'sample_fog_id')
		{
			init_fog();
		}
		else if (info.name == 'sample_fog_x1' || info.name == 'sample_fog_y1' || info.name == 'sample_fog_x2' || info.name == 'sample_fog_y2')
		{
			sample_fog_rect_timer = 120;
		}
	}
	
	void editor_step()
	{
		if (pending_init)
		{
			init_fog();
			parse_text();
			update_layers();
			pending_init = false;
		}
		
		if (!enabled)
			return;
		if (@trigger == null)
			return;
		
		update_cam();
		update_sample_fog();
		
		txt_x = trigger.x();
		txt_y = trigger.y();
		
		update_hovered_line();
		
		if (sample_fog_rect_timer > 0)
		{
			sample_fog_rect_timer = max(sample_fog_rect_timer - 1.0, 0.0);
		}
	}
	
	void editor_draw(float)
	{
		if (!enabled)
			return;
		if(@trigger == null)
			return;
		
		update_cam();
		
		txt_x = trigger.x();
		txt_y = trigger.y();
		
		// Main text trigger background.
		if (text_bg & 0xff000000 != 0 || txt_bg_glass)
		{
			draw_bg_rect(
				txt_layer, txt_sub_layer - 1,
				txt_x - text_bg_padding,
				txt_y - text_bg_padding,
				txt_x + txt_w + text_bg_padding,
				txt_y + txt_h + text_bg_padding,
				txt_bg_glass, text_bg);
		}
		
		// Custom background boxes.
		for (uint i = 0; i < bg_boxes.length; i++)
		{
			BgBox@ box = @bg_boxes[i];
			draw_bg_rect(
				box.layer >= 0 ? box.layer : txt_layer,
				box.sub_layer >= 0 ? box.sub_layer : txt_sub_layer - 1,
				box.x1, box.y1, box.x2, box.y2,
				txt_bg_glass, box.override_colour ? box.colour : text_bg);
		}
		
		// Layer arrows.
		for (uint i = 0; i < layers.length; i++)
		{
			Layer@ l = @layers[i];
			if(@l.line == null)
				continue;
			
			const bool is_hovered = @hovered_line != null && hovered_line.has_layer(l);
			const uint clr = @hovered_line == null || is_hovered
					? l.colour : multiply_alpha(l.colour, line_fade_alpha);
			
			const float width = (l.line_width <= 0 ? line_width : l.line_width) * (is_hovered ? width_mult : 1.0) * (dynamic_line_width ? ed_zoom : 1.0);
			const float dot_size = width / 0.6;
			const float x1 = txt_x + l.line.x - dot_size - 6;
			const float y1 = txt_y + l.line.y + txt_line_height * 0.5 + LINE_SPACING * txt_scale;
			float x2, y2;
			transform_layer_position(l.x, l.y, l.layer, txt_layer, x2, y2);
			
			draw_arrow_fill(
				g, txt_layer, txt_sub_layer, x1, y1, x2, y2, width,
				width * 2.5, width * 4, 1, clr);
			
			if (@l.line.layer_refs[0] == @l)
			{
				g.draw_rectangle_world(
					txt_layer, txt_sub_layer, x1 - dot_size, y1 - dot_size, x1 + dot_size, y1 + dot_size, 45, clr);
			}
		}
		
		// Highlight hovered line text.
		if (@hovered_line != null && line_fade_alpha < 1)
		{
			txt_field.text(hovered_line.text);
			txt_field.colour(txt_colour & 0xffffff | (255 << 24));
			txt_field.draw_world(txt_layer, txt_sub_layer, txt_x + hovered_line.x, txt_y + hovered_line.y, txt_scale, txt_scale, 0);
		}
		
		// Show the debug fog region while editing it.
		if (sample_fog_rect_timer > 0)
		{
			outline_rect_inside(g, txt_layer, txt_sub_layer, sample_fog_x1, sample_fog_y1, sample_fog_x2, sample_fog_y2, 2 * ed_zoom, 0xffee3333);
		}
	}
	
	//
	// Initialisation
	//
	
	void init_fog()
	{
		@sample_fog_trigger = entity_by_id(sample_fog_id);
		if (@sample_fog_trigger == null)
			return;
		if (sample_fog_trigger.type_name() != 'fog_trigger')
		{
			puts('Warning: sample_fog_id must be a fog trigger.');
			@sample_fog_trigger = null;
		}
		
		if (@sample_fog == null)
		{
			@sample_fog = get_active_camera().fog;
		}
		
		float size;
		get_fog_setting(sample_fog_trigger, sample_fog, sample_fog_has_sub_layers, sample_fog_time, size);
	}
	
	void parse_text(const bool force_read_txt_colour = false)
	{
		@trigger = entity_by_id(layers_ztext_trigger_id);
		if (@trigger == null)
			return;
		if (trigger.type_name() != 'z_text_prop_trigger')
		{
			@trigger = null;
			puts('Warning: layers_ztext_trigger_id must be a z text trigger.');
			return;
		}
		
		@vars = trigger.vars();
		txt_layer = vars.get_var('layer').get_int32();
		txt_sub_layer = vars.get_var('sublayer').get_int32();
		txt_font = vars.get_var('font').get_string();
		txt_font_size = vars.get_var('font_size').get_int32();
		txt_scale = vars.get_var('text_scale').get_float();
		txt_field.set_font(txt_font, txt_font_size);
		txt_field.text(' ');
		txt_line_height = txt_field.text_height() * txt_scale;
		@colour_var = vars.get_var('colour');
		txt_colour = has_stored_txt_colour && !force_read_txt_colour ? stored_txt_colour : colour_var.get_int32();
		txt_alpha = ((txt_colour >> 24) & 0xff) / 255.0;
		colour_var.set_int32(txt_colour);
		
		if (!has_stored_txt_colour || force_read_txt_colour)
		{
			stored_txt_colour = txt_colour;
			has_stored_txt_colour = true;
		}
		
		const string txt = vars.get_var('text').get_string();
		txt_field.text(txt);
		txt_w = txt_field.text_width() * txt_scale;
		txt_h = txt_field.text_height() * txt_scale;
		
		int index = 0;
		int line_index = 0;
		const int txt_length = int(txt.length);
		
		lines.resize(0);
		lines_x1 = 99999;
		lines_y1 = 99999;
		lines_x2 = -99999;
		lines_y2 = -99999;
		
		// Break the text into lines.
		while (index < txt_length)
		{
			const int start_index = index;
			int line_end = txt.findFirst('\n', index);
			if (line_end == -1)
			{
				line_end = txt_length;
			}
			
			// Parse LAYER.SUB_LAYER, skipping leading whitespace.
			LineParseState state = LeadingWhitespace;
			string buffer = '';
			int layer = -1;
			int sub_layer = -1;
			
			while (index <= line_end)
			{
				// Dummy character at end of line to ensure layer and sublayer buffers get saved
				// if there are no trailing characters.
				const int chr = index < line_end ? int(txt[index]) : -1;
				index++;
				
				if (state == LeadingWhitespace)
				{
					if (string::is_whitespace(chr))
						continue;
					
					if (string::is_digit(chr))
					{
						buffer = ' ';
						buffer[0] = chr;
						state = Layer;
					}
					// Some non-digit non-whitespace character found - stop parsing line.
					else
					{
						break;
					}
				}
				else if(state == Layer || state == SubLayer)
				{
					if(string::is_digit(chr))
					{
						buffer.resize(buffer.length + 1);
						buffer[buffer.length - 1] = chr;
					}
					else
					{
						if (state == Layer)
						{
							layer = parseInt(buffer);
							if (chr == CHR_PERIOD)
							{
								buffer = '';
								state = SubLayer;
							}
							else
							{
								state = Done;
							}
						}
						else
						{
							if(buffer.length > 0)
							{
								sub_layer = parseInt(buffer);
							}
							
							state = Done;
						}
					}
				}
			}
			
			if (layer != -1)
			{
				const string line_txt = txt.substr(start_index, line_end - start_index);
				
				txt_field.text(line_txt);
				
				LayerLine@ line_data = LayerLine();
				line_data.layer = layer;
				line_data.sub_layer = sub_layer;
				line_data.text = line_txt;
				line_data.x = 0;
				line_data.y = line_index * (txt_line_height + LINE_SPACING * txt_scale);
				line_data.x1 = line_data.x - LINE_PADDING_X;
				line_data.y1 = line_data.y;
				line_data.x2 = line_data.x1 + txt_field.text_width() * txt_scale + LINE_PADDING_X * 2;
				line_data.y2 = line_data.y1 + (txt_field.text_height() + LINE_SPACING) * txt_scale;
				
				lines_x1 = min(lines_x1, line_data.x1);
				lines_y1 = min(lines_y1, line_data.y1);
				lines_x2 = max(lines_x2, line_data.x2);
				lines_y2 = max(lines_y2, line_data.y2);
				
				lines.insertLast(line_data);
			}
			
			// Finish loop
			index = line_end != -1 ? line_end + 1 : txt_length;
			line_index++;
		}
	}
	
	/// Matches layers with lines of text.
	void update_layers()
	{
		for (uint i = 0; i < lines.length; i++)
		{
			lines[i].layer_refs.resize(0);
		}
		
		for (uint i = 0; i < layers.length; i++)
		{
			Layer@ layer = @layers[i];
			
			for (uint j = 0; j < lines.length; j++)
			{
				LayerLine@ line = lines[j];
				if(line.layer == layer.layer && line.sub_layer == layer.sub_layer)
				{
					@layer.line = line;
					line.layer_refs.insertLast(layer);
					break;
				}
			}
		}
	}
	
	//
	// Update
	//
	
	/// Read and initialise camera values for this frame
	void update_cam()
	{
		@cam = get_active_camera();
		view_x = cam.x();
		view_y = cam.y();
		ed_zoom = 1 / cam.editor_zoom();
	}
	
	/// Switch to sample fog trigger when entering the sample region.
	void update_sample_fog()
	{
		if (@sample_fog_trigger == null)
			return;
		if (sample_fog_x1 == 0 && sample_fog_y1 == 0 && sample_fog_x2 == 0 && sample_fog_y2 == 0)
			return;
		if (is_layer_highlighted)
			return;
		
		if (view_x >= sample_fog_x1 && view_y >= sample_fog_y1 && view_x <= sample_fog_x2 && view_y <= sample_fog_y2)
		{
			if (!inside_sample_fog_region)
			{
				cam.change_fog(sample_fog, sample_fog_time);
				inside_sample_fog_region = true;
			}
		}
		else
		{
			inside_sample_fog_region = false;
		}
	}
	
	/// Checks which layer/line the mouse is over.
	void update_hovered_line()
	{
		if (
			reveal_layers_on == Never &&
			hover_fade_alpha >= 1 && press_text_alpha >= 1 && press_line_alpha >= 1 &&
			hover_line_width_mult == 1 && press_line_width_mult == 1
		)
			return;
		
		const float mouse_x = input.mouse_x_world(txt_layer);
		const float mouse_y = input.mouse_y_world(txt_layer);
		const bool mouse_down = input.mouse_state() & 0x4 != 0 && !input.key_check_gvb(GVB::Space);
		
		int new_hover_line_index = -1;
		bool new_layers_revealed = false;
		
		// Check each layer/line bounding box to find the line the mouse is over.
		// Check the text bounding box first.
		// I doubt there'll ever be so many lines that performandce will be an issue so this is probably pointless?
		if (mouse_x >= txt_x + lines_x1 && mouse_y >= txt_y + lines_y1 && mouse_x <= txt_x + lines_x2 && mouse_y <= txt_y + lines_y2)
		{
			for (uint i = 0; i < lines.length; i++)
			{
				LayerLine@ line = lines[i];
				
				if (mouse_x >= txt_x + line.x1 && mouse_y >= txt_y + line.y1 && mouse_x <= txt_x + line.x2 && mouse_y <= txt_y + line.y2)
				{
					new_hover_line_index = i;
					new_layers_revealed = reveal_layers_on == Hover || mouse_down;
					break;
				}
			}
		}
		
		// No change, do nothing.
		if (new_hover_line_index == line_hover_index && new_layers_revealed == layers_revealed)
			return;
		
		layers_revealed = new_layers_revealed;
		line_hover_index = new_hover_line_index;
		@hovered_line = line_hover_index != -1 ? @lines[line_hover_index] : null;
		
		int new_highlight_layer = -1;
		int new_highlight_sub_layer = -1;
		
		if (@hovered_line != null)
		{
			text_fade_alpha = reveal_layers_on == Press && layers_revealed ? press_text_alpha : hover_fade_alpha;
			line_fade_alpha = reveal_layers_on == Press && layers_revealed ? press_line_alpha : hover_fade_alpha;
			width_mult = mouse_down && press_line_width_mult > 0 ? press_line_width_mult : hover_line_width_mult;
			
			colour_var.set_int32(multiply_alpha(txt_colour | 0xff000000, text_fade_alpha));
			
			if (reveal_layers_on == Hover || reveal_layers_on == Press)
			{
				for (int i = 1; i <= 20; i++)
				{
					editor.set_layer_visible(i, (reveal_single ? i == hovered_line.layer : i <= hovered_line.layer) || !new_layers_revealed);
				}
			}
			
			if (@sample_fog_trigger != null && (
				hover_highlight && (reveal_layers_on != Press || !mouse_down) ||
				press_highlight && mouse_down))
			{
				is_layer_highlighted = true;
				new_highlight_layer = hovered_line.layer;
				new_highlight_sub_layer = hovered_line.sub_layer;
			}
			else
			{
				is_layer_highlighted = false;
			}
		}
		else
		{
			text_fade_alpha = 1;
			line_fade_alpha = 1;
			width_mult = 1;
			
			colour_var.set_int32(txt_colour);
			
			if (reveal_layers_on == Hover || reveal_layers_on == Press)
			{
				for (int i = 1; i <= 20; i++)
				{
					editor.set_layer_visible(i, true);
				}
			}
			
			if (is_layer_highlighted)
			{
				update_sample_fog();
				is_layer_highlighted = false;
			}
		}
		
		update_highlighted_layer_fog(new_highlight_layer, new_highlight_sub_layer);
	}
	
	/// Updates fog to highlight the hovered layer.
	void update_highlighted_layer_fog(const int new_highlight_layer, const int new_highlight_sub_layer)
	{
		// No change
		if (new_highlight_layer == highlight_layer && new_highlight_sub_layer == highlight_sub_layer)
			return;
		
		// Reset previously highlighted layer.
		if (highlight_layer != -1)
		{
			for (uint i = 0; i <= 24; i++)
			{
				sample_fog.colour(highlight_layer, i, highlight_base_colour[i]);
				sample_fog.percent(highlight_layer, i, highlight_base_percent[i]);
			}
		}
		
		if (new_highlight_layer != -1)
		{
			// Store the base/real layer fog colour so it can be restored when unhighlighted.
			for (uint i = 0; i <= 24; i++)
			{
				highlight_base_colour[i] = sample_fog.colour(new_highlight_layer, i);
				highlight_base_percent[i] = sample_fog.percent(new_highlight_layer, i);
			}
			
			const float percent = (hover_highlight_colour >> 24 & 0xff) / 255.0;
			// Highlight the entire layer.
			if (new_highlight_sub_layer == -1)
			{
				if (hovered_line.highlight_tiles_only())
				{
					for (int i = 10; i <= 13; i++)
					{
						sample_fog.colour(new_highlight_layer, i, hover_highlight_colour);
						sample_fog.percent(new_highlight_layer, i, percent);
					}
				}
				else
				{
					sample_fog.layer_colour(new_highlight_layer, hover_highlight_colour);
					sample_fog.layer_percent(new_highlight_layer, percent);
				}
			}
			// Highlight a single sub layer.
			else
			{
				sample_fog.colour(new_highlight_layer, new_highlight_sub_layer, hover_highlight_colour);
				sample_fog.percent(new_highlight_layer, new_highlight_sub_layer, percent);
			}
		}
		
		highlight_layer = new_highlight_layer;
		highlight_sub_layer = new_highlight_sub_layer;
		cam.change_fog(sample_fog, sample_fog_time * 0.25);
	}
	
	//
	// Utils Methods
	//
	
	int clamped_layer(const int layer)
	{
		if (layer < 0)
			return 0;
		if (layer > 20)
			return 20;
		return layer;
	}
	
	void draw_bg_rect(
		const int layer, const int sub_layer,
		const float x1, const float y1, const float x2, const float y2, const bool glass, const uint clr)
	{
		if (glass)
		{
			g.draw_glass_world(layer, sub_layer, x1, y1, x2, y2, 0, 0x00000000);
		}
		
		g.draw_rectangle_world(layer, sub_layer, x1, y1, x2, y2, 0, clr);
	}
	
	void get_fog_setting(entity@ fog_trigger, fog_setting@ fog, bool &out has_sub_layers, float &out fog_speed, int &out trigger_size)
	{
		varstruct@ vars = fog_trigger.vars();
		
		vararray@ colour = vars.get_var('fog_colour').get_array();
		vararray@ percent = vars.get_var('fog_per').get_array();
		
		for (int layer = 0; layer <= 20; layer++)
		{
			fog.layer_colour(layer, colour.at(layer).get_int32());
			fog.layer_percent(layer, percent.at(layer).get_float());
		}
		
		has_sub_layers = vars.get_var('has_sub_layers').get_bool();
		if (has_sub_layers)
		{
			for (int layer = 0; layer <= 20; layer++)
			{
				for (int sublayer = 0; sublayer <= 24; sublayer++)
				{
					fog.colour(layer, sublayer, colour.at((sublayer + 1) * 21 + layer).get_int32());
					fog.percent(layer, sublayer, percent.at((sublayer + 1) * 21 + layer).get_float());
				}
			}
		}
		
		vararray@ gradient = vars.get_var('gradient').get_array();
		fog.bg_top(gradient.at(0).get_int32());
		fog.bg_mid(gradient.at(1).get_int32());
		fog.bg_bot(gradient.at(2).get_int32());
		
		fog.bg_mid_point(vars.get_var('gradient_middle').get_float());
		
		fog.stars_top(vars.get_var('star_top').get_float());
		fog.stars_mid(vars.get_var('star_middle').get_float());
		fog.stars_bot(vars.get_var('star_bottom').get_float());
		
		fog_speed = vars.get_var('fog_speed').get_float();
		trigger_size = vars.get_var('width').get_int32();
	}
	
	void transform_layer_position(
		const float x, const float y,
		const int from_layer, const int to_layer,
		float &out out_x, float &out out_y)
	{
		const float scale = g.layer_scale(clamped_layer(from_layer)) / g.layer_scale(clamped_layer(to_layer));
		const float dx = (x - view_x) * scale;
		const float dy = (y - view_y) * scale;
		out_x = view_x + dx;
		out_y = view_y + dy;
	}
	
}

class Layer
{
	[persist|tooltip:'This and sub_layer should match a line in the text trigger.']
	int layer = 19;
	[persist] int sub_layer = -1;
	[position,mode:world,layer:20,y:y] float x;
	[hidden] float y;
	[persist|tooltip:'If -1 inherit the global LineWidth']
	float line_width = -1;
	[colour,alpha] uint colour = 0xffaaaaaa;
	[persist|tooltip:'If set only sublayers 10-13 will be highlighted']
	bool highlight_tiles_only;
	
	LayerLine@ line;
}

class LayerLine
{
	int layer, sub_layer;
	float x, y;
	float x1, y1, x2, y2;
	string text;
	array<Layer@> layer_refs;
	
	bool has_layer(Layer@ layer) const
	{
		for (uint i = 0; i < layer_refs.length; i++)
		{
			if (@layer_refs[i] == @layer)
				return true;
		}
		
		return false;
	}
	
	bool highlight_tiles_only()
	{
		for (uint i = 0; i < layer_refs.length; i++)
		{
			if (layer_refs[i].highlight_tiles_only)
				return true;
		}
		
		return false;
	}
	
}

class BgBox
{
	[position,mode:world,layer:20,y:y1] float x1;
	[hidden] float y1;
	[position,mode:world,layer:20,y:y2] float x2;
	[hidden] float y2;
	[persist] int layer = -1;
	[persist] int sub_layer = -1;
	[persist|tooltip:'If false will use the text_bg colour.']
	bool override_colour = false;
	[colour,alpha]
	uint colour = 0x55000000;
}

enum LineParseState
{
	LeadingWhitespace,
	Layer,
	SubLayer,
	Done,
}

enum LayerRevealType
{
	Never,
	Hover,
	Press,
}

const int CHR_PERIOD = 46;

/// Text fields add this much space between each line/
const float LINE_SPACING = 3;
const float LINE_PADDING_X = 4;
