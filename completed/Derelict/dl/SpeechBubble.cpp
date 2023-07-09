#include '../lib/drawing/Sprite.cpp';

#include 'SpeechBubbleBox.cpp';

class SpeechBubble
{
	
	DLScript@ script;
	
	float characters_per_second = 20;
	/// A delay for commas as a number of extra c characters.
	float comma_delay = 2;
	float period_delay = 4;
	float new_line_delay = 1;
	float auto_advance_delay = 25;
	
	private bool _force_auto_advance;
	
	/// Can be modified during custom attribute parsing.
	int parse_index;
	int parse_line_index;
	private int _parse_out_index;
	
	int layer = 20;
	int sub_layer = 20;
	/// Changes will only take effect after next call to `set_text`.
	float padding = 10;
	uint txt_colour = 0xff968988;
	float transition_speed = 8;
	float next_arrow_transition_speed = 6;
	
	float min_width = 100;
	
	bool draw_next_arrow = true;
	bool draw_waves = false;
	
	SpeechBubble::CustomAttributeParser@ custom_attribute_parser;
	SpeechBubble::ParseResult parse_result = None;
	SpeechBubble::ParseSign parse_sign = None;
	SpeechBubble::Listener@ listener;
	private SpeechBubble::CustomContent@ _custom_content;
	
	private bool _open;
	private float current_width, current_height;
	private float current_width_prev, current_height_prev;
	private bool in_transition;
	
	private float border_size = 4;
	
	private string _text;
	private string current_text;
	private int text_length;
	private float text_width, text_height;
	private float text_width_o, text_height_o;
	private float full_width, full_height;
	private float min_size;
	private int chr_index, chr_index_prev;
	private float chr_timer, chr_timer_max;
	private bool completed_text, completed_advance;
	private bool visible;
	private float alpha = 1;
	private uint clr = 0xffffffff;
	
	private int chr_delays_size = 16;
	private array<int> chr_delay_indices(chr_delays_size);
	private array<float> chr_delay_times(chr_delays_size);
	private int chr_delays_count;
	private int chr_delays_index;
	private bool chr_delay_pending;
	private float end_delay;
	
	private float real_characters_per_second;
	private int chr_meta_size = 16;
	private array<SpeechBubble::ChrMeta> chr_meta_list(chr_meta_size);
	private int chr_meta_count;
	private int chr_meta_index;
	private SpeechBubble::ChrMeta@ chr_meta_pending;
	
	private textfield@ text_field;
	private sprites@ spr;
	
	private SpeechBubbleBox border_box;
	private SpeechBubbleBox inner_box;
	private Sprite shadow_spr('props3', 'backdrops_3', 0.25, 0.5);
	private float shadow_spr_scale = 0.12;
	private float shadow_spr_base_rotation = 1.94799999999998;
	private Sprite arrow_spr('props3', 'facade_12', 0.96, 0.1);
	private float arrow_gap = 10;
	
	private Sprite scary_spr('props1', 'backdrops_5', 0, 0.35);
	private float scary2_arrow_gap = 30;
	
	private Sprite wave_spr('props3', 'facade_12', 0.90, 0.53);
	private float wave_size = 31;
	private float wave_spacing = 17;
	private float wave_full_size = 58;
	
	private Sprite next_arrow_spr('props5', 'symbol_1', 0.5, 0.5);
	private float next_arrow_ox = 14.5;
	private float next_arrow_oy = 11.5;
	private float next_arrow_alpha;
	private float next_arrow_alpha_t;
	private float next_arrow_flash_t;
	private float next_arrow_flash_time;
	private int next_arrow_flash_count;
	private float next_arrow_flash_scale;
	
	private float prng_y = 0;
	
	SpeechBubble()
	{
		@text_field = create_text_field();
	}
	
	bool is_visible {
		get const { return visible; }
	}
	
	bool is_open {
		get const { return _open; }
	}
	
	bool is_completed {
		get const { return completed_text; }
	}
	
	bool can_auto_advance {
		get const { return completed_advance; }
	}
	
	/// Read this value to check if an auto advance attribute was set.
	bool force_auto_advance {
		get const { return _force_auto_advance; }
		set { _force_auto_advance = value; }
	}
	
	/// Returns the current output character index during parsing.
	int parse_chr_index { get const { return _parse_out_index; } }
	
	/// Returns the current text character index.
	int progress { get const { return chr_index; } }
	
	void set_text(const string &in text)
	{
		real_characters_per_second = characters_per_second;
		chr_meta_index = 0;
		chr_meta_count = 0;
		chr_delays_index = 0;
		chr_delays_count = 0;
		end_delay = 0;
		_force_auto_advance = false;
		
		_text = text;
		text_length = int(_text.length);
		parse_text();
		text_length = int(_text.length);
		
		current_text = '';
		chr_index = 0;
		chr_index_prev = 0;
		chr_timer = 0;
		calc_chr_timer_max();
		
		init_for_content();
	}
	
	string get_text() const
	{
		return _text;
	}
	
	SpeechBubble::CustomContent@ custom_content {
		get { return _custom_content; }
		set {
			@_custom_content = value;
			init_for_content();
		}
	}
	
	void open(const bool open = true)
	{
		if(_open == open)
			return;
		
		_open = open;
		in_transition = true;
		
		if(open)
		{
			visible = true;
			next_arrow_alpha = 0;
			next_arrow_alpha_t = 0;
		}
	}
	
	bool force_complete()
	{
		if(!_open)
			return false;
		
		if(can_auto_advance)
			return true;
		
		const bool completed_text_prev = completed_text;
		const int prev_i = chr_index;
		
		chr_index = text_length;
		real_characters_per_second = characters_per_second;
		chr_timer_max = auto_advance_delay + end_delay;
		chr_timer = 0;
		completed_text = true;
		completed_advance = false;
		
		current_text = text;
		text_field.text(current_text);
		
		chr_index_prev = chr_index;
		
		trigger_listener(prev_i != chr_index, !completed_text_prev, false);
		return false;
	}
	
	void flash_next_arrow(const float scale = 1.35, const float time = 0.25, const int count = 2)
	{
		if(!_open)
			return;
		
		next_arrow_flash_scale = scale - 1.0;
		next_arrow_flash_time = time;
		next_arrow_flash_count = count > 0 ? count : 1;
	}
	
	textfield@ create_text_field()
	{
		textfield@ tf = create_textfield();
		tf.set_font('ProximaNovaReg', 26);
		tf.align_horizontal(0);
		tf.align_vertical(-1);
		return tf;
	}
	
	/// The percent of the full size the speech bubble is currenty at.
	void get_size_percent(float &out px, float &out py)
	{
		px = current_width / full_width;
		py = current_height / full_height;
	}
	
	/// Must be called after setting the text.
	void add_end_delay(const float delay)
	{
		end_delay += delay;
	}
	
	void step(const float time_scale)
	{
		if(in_transition)
		{
			if(_open)
			{
				alpha = 1;
				clr = 0xffffffff;
				
				clamp_current_size();
			}
			
			current_width_prev = current_width;
			current_height_prev = current_height;
			
			const float target_width = _open ? full_width : 0.1;
			const float target_height = _open ? full_height : 0.1;
			const float diff_x = target_width - current_width;
			const float diff_y = target_height - current_height;
			current_width += diff_x * transition_speed * DT;
			current_height += diff_y * transition_speed * DT;
			
			if(abs(current_width - target_width) < 1)
				current_width = target_width;
			if(abs(current_height - target_height) < 1)
				current_height = target_height;
			
			if(current_width == target_width && current_height == target_height)
			{
				in_transition = false;
				if(!_open)
				{
					visible = false;
					next_arrow_flash_time = 0;
					next_arrow_alpha = 0;
					next_arrow_alpha_t = 0;
				}
			}
			
			alpha = min(min(current_width, current_height) / min_size, 1.0);
			clr = 0xffffff | uint(alpha * 255) << 24;
		}
		
		if(draw_next_arrow)
		{
			if(next_arrow_alpha != next_arrow_alpha_t)
			{
				next_arrow_alpha += (next_arrow_alpha_t - next_arrow_alpha) * next_arrow_transition_speed * DT;
			}
			
			if(next_arrow_flash_time > 0)
			{
				next_arrow_flash_t += DT / next_arrow_flash_time;
				if(next_arrow_flash_t >= 1)
				{
					next_arrow_flash_t = 0;
					next_arrow_flash_time = 0;
				}
			}
		}
		
		if(!_open)
			return;
		
		if(!completed_advance)
		{
			chr_timer += real_characters_per_second * DT * time_scale;
			
			if(!completed_text)
			{
				while(chr_timer >= chr_timer_max)
				{
					chr_timer -= chr_timer_max;
					chr_index++;
					calc_chr_timer_max();
				}
				
				if(chr_index != chr_index_prev)
				{
					current_text += _text.substr(chr_index_prev, chr_index - chr_index_prev);
					text_field.text(current_text);
					chr_index_prev = chr_index;
					completed_text = chr_index >= text_length;
					
					trigger_listener(true, true, false);
				}
			}
			else if(chr_timer >= chr_timer_max)
			{
				completed_advance = true;
				trigger_listener(false, false, true);
			}
		}
		
		next_arrow_alpha_t = draw_next_arrow && !_force_auto_advance && completed_text ? 1.0 : 0.0;
	}
	
	private void trigger_listener(const bool progress, const bool trigger_complete, const bool trigger_advance)
	{
		if(@listener == null)
			return;
		
		if(progress)
		{
			listener.on_speech_bubble_progress(this, chr_index);
		}
		
		if(trigger_complete && completed_text)
		{
			listener.on_speech_bubble_complete(this);
		}
		if(trigger_advance && completed_advance)
		{
			listener.on_speech_bubble_advance(this);
		}
	}
	
	private void init_for_content()
	{
		completed_text = false;
		completed_advance = false;
		next_arrow_alpha_t = 0;
		
		calculate_size();
		min_size = padding * 2 + border_size * 2;
		
		if(@listener != null)
		{
			listener.on_speech_bubble_init(this);
		}
	}
	
	private void calculate_size(const bool test=false, const float x1=0, const float y1=0)
	{
		if(!test)
		{
			text_field.text(_text);
		}
		
		if(@_custom_content == null)
		{
			text_width = test ? abs((script.g.mouse_x_world(0, 19) - x1) * 2) : text_field.text_width();
			text_height = test ? abs(y1 - script.g.mouse_y_world(0, 19)) : text_field.text_height();
		}
		else
		{
			_custom_content.get_speech_bubble_content_final_size(text_width, text_height);
		}
		
		text_width_o = text_width + padding * 2;
		text_height_o = text_height + padding * 2;
		
		full_width = max(text_width_o + border_size * 2, min_width);
		full_height = text_height_o + border_size * 2;
		
		in_transition = true;
		
		if(@_custom_content == null)
		{
			text_field.text(current_text);
			text_field.colour(txt_colour);
		}
		
		clamp_current_size();
	}
	
	private void clamp_current_size()
	{
		float tw, th;
		
		if(@_custom_content == null)
		{
			tw = text_field.text_width();
			th = text_field.text_height();
		}
		else
		{
			_custom_content.get_speech_bubble_content_size(tw, th);
		}
		
		tw += (border_size + padding) * 2;
		th += (border_size + padding) * 2;
		
		if(current_width < tw)
		{
			const float dx = tw - current_width;
			current_width = tw;
			current_width_prev += dx;
		}
		if(current_height < th)
		{
			const float dy = th - current_height;
			current_height = th;
			current_height_prev += dy;
		}
	}
	
	private float get_chr_delay(const int index) const
	{
		const int chr = _text[index];
		// ,
		if(chr == 3 || chr == 44)
			return comma_delay;
		// - . ?
		if(chr == 45 || chr == 46 || chr == 63)
			return period_delay;
		// New line
		if(chr == 10)
			return new_line_delay;
		
		return 0;
	}
	
	private void calc_chr_timer_max()
	{
		if(chr_index >= text_length)
		{
			real_characters_per_second = characters_per_second;
			chr_timer_max = auto_advance_delay + end_delay;
			return;
		}
		
		SpeechBubble::ChrMeta@ meta = chr_meta_index < chr_meta_count ? @chr_meta_list[chr_meta_index] : null;
		if(@meta != null)
		{
			if(meta.index == chr_index)
			{
				chr_meta_index++;
			}
			else
			{
				@meta = null;
			}
		}
		
		if(@meta != null)
		{
			if(meta.cps_set)
			{
				const float old_cps = real_characters_per_second;
				if(meta.cps_is_relative)
				{
					real_characters_per_second += meta.cps_is_percent
						? real_characters_per_second * (meta.cps * 0.01)
						: meta.cps;
				}
				else if(meta.cps >= 0)
				{
					real_characters_per_second = meta.cps_is_percent
						? characters_per_second * (meta.cps * 0.01)
						: meta.cps;
				}
				// Reset
				else
				{
					real_characters_per_second = characters_per_second;
				}
			}
		}
		
		const float added_delay = @meta != null ? meta.delay : 0.0;
		chr_timer_max = max(1 + (chr_index > 0 ? get_chr_delay(chr_index - 1) : 0.0) + added_delay, 0.0);
	}
	
	// Drawing
	
	void draw(const float x, const float y, const float sub_frame)
	{
		if(!visible)
			return;
		
		const float w = lerp(current_width_prev, current_width, sub_frame);
		const float h = lerp(current_height_prev, current_height, sub_frame);
		const float ox = x - w * 0.5 + border_size;
		const float oy = y - h + border_size - arrow_gap;
		
		draw_shadow(ox - border_size, oy - border_size, w, h);
		draw_arrow(x, y, w, h, false);
		border_box.draw(ox - border_size, oy - border_size, w, h, layer, sub_layer + 1, clr);
		inner_box.draw(ox, oy, w -  border_size * 2, h - border_size * 2, layer, sub_layer + 2, clr);
		draw_text(ox, oy, w, h, sub_frame);
		
		//script.g.draw_rectangle_world(
		//	22, 22,
		//	ox - border_size, oy - border_size,
		//	ox - border_size + w, oy - border_size + h, 0, 0x33ffffff);
	}
	
	void draw_scary(const float x, const float y, const float sub_frame)
	{
		if(!visible)
			return;
		
		const float w = lerp(current_width_prev, current_width, sub_frame);
		const float h = lerp(current_height_prev, current_height, sub_frame);
		const float ox = x - w * 0.5;
		const float oy = y - h - scary2_arrow_gap;
		
		draw_arrow(x, y, w, h, true);
		draw_shadow(ox, oy, w, h, true);
		border_box.draw_scary(ox, oy, w, h, layer, sub_layer - 2, clr);
		draw_text(ox, oy, w, h, sub_frame);
		
		//script.g.draw_rectangle_world(
		//	22, 22,
		//	ox, oy,
		//	ox + w, oy + h, 0, 0x33ffffff);
	}
	
	private void draw_arrow(const float x, const float y, const float w, const float h, const bool scary)
	{
		float arrow_scale = w < 48 || h < 48
			? min(w, h) / 48.0
			: 1.0;
		arrow_spr.draw(layer, sub_layer + (scary ? -2 : 1), 0, 0, x, y, 128, arrow_scale, arrow_scale, clr);
	}
	
	private void draw_text(const float ox, const float oy, const float w, const float h, const float sub_frame)
	{
		if(_open)
		{
			if(@_custom_content == null)
			{
				text_field.draw_world(
					layer, sub_layer + 3,
					ox + (w - border_size * 2) * 0.5 - 0.5,
					oy + max((h - border_size - text_height) * 0.5, padding) - 2,
					1, 1, 0);
			}
			else
			{
				_custom_content.speech_bubble_draw(
					layer, sub_layer + 3, ox, oy, (w - border_size * 2), (h - border_size * 2), sub_frame);
			}
		}
		
		if(draw_next_arrow && next_arrow_alpha > 0)
		{
			const float scale = next_arrow_flash_time > 0
				? 1 + next_arrow_flash_scale * (1 - cos(next_arrow_flash_t * PI * 2 * next_arrow_flash_count) * 0.5 - 0.5)
				: 1.0;
			
			next_arrow_spr.draw(
				layer, sub_layer + 3, 0, 0,
				ox + w - next_arrow_ox, oy + h + next_arrow_oy,
				0, 0.75 * scale, 0.75 * scale,
				0xffffff | uint(next_arrow_alpha * alpha * 255) << 24);
		}
	}
	
	private void draw_shadow(const float x, const float y, const float w, const float h, const bool scary = false)
	{
		const float x1 = x + (!scary ? border_size * 0.5 : 0.0);
		const float y1 = y + (!scary ? border_size * 0.5 : 0.0);
		const float x2 = x + w - (!scary ? border_size * 0.5 : 0.0);
		const float y2 = y + h - (!scary ? border_size * 0.5 : 0.0);
		prng_y = 0;
		draw_shadow_side(x1, y1, x2, y1, h, scary);
		draw_shadow_side(x2, y1, x2, y2, w, scary);
		draw_shadow_side(x2, y2, x1, y2, h, scary);
		draw_shadow_side(x1, y2, x1, y1, w, scary);
	}
	
	private void draw_shadow_side(const float x1, const float y1, const float x2, const float y2, const float height, const bool scary=false)
	{
		const float dx = x2 - x1;
		const float dy = y2 - y1;
		const float length = sqrt(dx * dx + dy * dy);
		const float nx = length > 0 ? dx / length : 0.0;
		const float ny = length > 0 ? dy / length : 0.0;
		const float rotation = atan2(dy, dx) * RAD2DEG;
		
		float scale_x = shadow_spr_scale;
		float scale_y = shadow_spr_scale;
		
		float width = calculate_shadow_width(scale_x);
		int count = ceil_int(length / width);
		float spacing = calculate_shadow_spacing(length, width, count);
		
		// Fit a single shadow prop
		if(count == 1)
		{
			scale_x = shadow_spr_scale * length / width;
		}
		// Squash to prevent too much overlap making the shadow too dark
		else if(count > 1 && spacing < width * 0.65)
		{
			const float stretch = 1 - (1 - spacing / (width * 0.65)) * 0.35;
			scale_x = shadow_spr_scale * stretch;
			width = calculate_shadow_width(scale_x);
			spacing = calculate_shadow_spacing(length, width, count);
		}
		
		for(int i = 0; i < count; i++)
		{
			const float x = x1 + nx * spacing * i - (scary ? -ny * 10 : 0.0);
			const float y = y1 + ny * spacing * i - (scary ?  nx * 10 : 0.0);
			
			shadow_spr.draw(
				layer, sub_layer + (scary ? -2 : 0), 0, 0,
				x, y,
				rotation + shadow_spr_base_rotation, scale_x, scale_y, clr);
		}
		
		if(scary)
		{
			scale_x = 1;
			scale_y = 1;
			const float outset = 80;
			const float full_length = length + outset * 2;
			scale_x = full_length < scary_spr.sprite_width ? full_length / scary_spr.sprite_width : 1.0;
			width = scary_spr.sprite_width * scale_x;
			count = scale_x >= 1 ? ceil_int(full_length / max(width - scary_spr.sprite_width * 0.35, 0.0)) : 1;
			spacing = calculate_shadow_spacing(full_length, width, count);
			
			for(int i = 0; i < count; i++)
			{
				const float x = x1 + nx * spacing * i - nx * outset;
				const float y = y1 + ny * spacing * i - ny * outset;
				
				scary_spr.draw(
					layer, sub_layer - 2, 0, 0,
					x, y,
					rotation + prng2(i, prng_y, -0.75, 0.75),
					scale_x,
					scale_y * prng2(i * 10, prng_y, 1.1, 1.55),
					clr);
			}
			
			prng_y++;
		}
		
		if(draw_waves)
		{
			//script.g.draw_line_world(22, 22, x1, y1, x2, y2, 2, 0x33ffffff);
			
			const float max_height = 38 + border_size + 2;
			const float wave_scale_x = length - 6 < wave_full_size ? (length - 6) / wave_full_size : 1.0;
			const float wave_scale_y = height < max_height ? height / max_height : 1.0;
			const float wave_scale = min(wave_scale_x, wave_scale_y);
			width = wave_size + wave_spacing;
			count = ceil_int((length - wave_spacing) / width);
			spacing = calculate_shadow_spacing(length - wave_spacing, wave_size * wave_scale_y + wave_spacing, count);
			
			const float osx = nx * (wave_scale_x < 1 ? (length - wave_size * wave_scale_x) * 0.5 : wave_spacing);
			const float osy = ny * (wave_scale_x < 1 ? (length - wave_size * wave_scale_x) * 0.5 : wave_spacing);
			
			for(int i = 0; i < count; i++)
			{
				const float x = x1 + nx * spacing * i + osx;
				const float y = y1 + ny * spacing * i + osy;
				
				wave_spr.draw(
					layer, sub_layer + 1, 0, 0,
					x, y,
					rotation - 145, wave_scale, wave_scale, clr);
				wave_spr.draw(
					layer, sub_layer + 2, 0, 0,
					x - ny * (border_size + 2) * wave_scale_y,
					y + nx * (border_size + 2) * wave_scale_y,
					rotation - 145, wave_scale, wave_scale, clr);
			}
		}
	}
	
	private float calculate_shadow_width(const float scale_x)
	{
		return shadow_spr.sprite_width * scale_x * (1.0 - shadow_spr.origin_x * 2);
	}
	
	private float calculate_shadow_spacing(const float length, const float width, const int count)
	{
		return (length - width) / (count > 1 ? count - 1 : 1);
	}
	
	// -- Parsing
	
	private void parse_text()
	{
		//puts('-- parse_text -------------------');
		
		if(text_length == 0)
			return;
		
		string out_text = '';
		out_text.resize(text_length);
		
		parse_index = 0;
		_parse_out_index = 0;
		@chr_meta_pending = null;
		float line_delay_pending = 0;
		
		parse_line_index = 1;
		bool parsing_attributes = false;
		bool skip_next_bracket = false;
		
		while(parse_index < text_length)
		{
			int chr = _text[parse_index++];
			
			// DEBUG
			//const string chr_txt = _text.substr(parse_index - 1, 1);
			//puts(' ' + parse_index + ' ' + (chr_txt == '\n' ? '\\n' : chr_txt));
			
			if(!parsing_attributes)
			{
				if(chr == SpeechBubble::OpenBracket && !skip_next_bracket)
				{
					if(parse_index < text_length)
					{
						const int next_chr = _text[parse_index];
						// Escaped with double bracket
						if(next_chr == SpeechBubble::OpenBracket)
						{
							skip_next_bracket = true;
							continue;
						}
						else
						{
							parsing_attributes = true;
						}
					}
					else
					{
						log_unclosed_attributes();
					}
				}
				else
				{
					skip_next_bracket = false;
					
					if(chr == SpeechBubble::NewLine)
					{
						if(line_delay_pending != 0)
						{
							push_chr_delay(line_delay_pending, true);
							line_delay_pending = 0;
						}
						
						parse_line_index++;
					}
					
					if(@chr_meta_pending != null)
					{
						chr_meta_pending.index = _parse_out_index;
						chr_delays_count++;
						@chr_meta_pending = null;
					}
					
					out_text[_parse_out_index++] = chr;
				}
				
				continue;
			}
			
			if(chr == SpeechBubble::CloseBracket)
			{
				parsing_attributes = false;
				continue;
			}
			
			if(chr == SpeechBubble::NewLine)
			{
				log_unclosed_attributes();
				parse_line_index++;
				
				parsing_attributes = false;
				continue;
			}
			
			if(
				@custom_attribute_parser != null &&
				custom_attribute_parser.try_parse_speech_attribute(this, chr, _text, text_length))
				continue;
			
			switch(chr)
			{
				case SpeechBubble::AttribAutoAdvance: {
					_force_auto_advance = true;
				} break;
				case SpeechBubble::AttribChrDelay: {
					push_chr_delay(try_consume_float(), true);
					if(parse_result == SpeechBubble::ParseResult::Failure)
					{
						log_parse_warning('Expected float after "d"');
					}
				} break;
				case SpeechBubble::AttribLineDelay: {
					line_delay_pending = try_consume_float();
					if(parse_result == SpeechBubble::ParseResult::Failure)
					{
						log_parse_warning('Expected float after "D"');
					}
				} break;
				case SpeechBubble::AttribSpeed: {
					SpeechBubble::ChrMeta@ meta = get_chr_meta();
					meta.cps_set = true;
					meta.cps = try_consume_float();
					meta.cps_is_relative = parse_sign != None;
					meta.cps_is_percent = false;
					
					if(!meta.cps_is_relative && meta.cps < 0)
					{
						meta.cps = -meta.cps;
					}
					
					const int next_chr = parse_index < text_length ? int(_text[parse_index]) : -1;
					if(next_chr == SpeechBubble::Percent)
					{
						meta.cps_is_percent = true;
						parse_index++;
					}
					
					if(parse_result == SpeechBubble::ParseResult::Failure)
					{
						meta.cps = -1;
						meta.cps_is_relative = false;
						meta.cps_is_percent = false;
					}
				} break;
			}
		}
		
		if(line_delay_pending != 0)
		{
			end_delay = line_delay_pending;
			line_delay_pending = 0;
		}
		
		if(parsing_attributes)
		{
			log_unclosed_attributes();
		}
		
		if(int(out_text.length) > _parse_out_index)
		{
			out_text.resize(_parse_out_index);
		}
		
		//puts('== Parsed text ==================================================');
		//puts(out_text);
		//puts('==================================================');
		//puts(' Auto advance: ' + _force_auto_advance);
		//puts('==================================================');
		
		_text = out_text;
	}
	
	private void log_unclosed_attributes()
	{
		log_parse_warning('Missing attribute close bracket');
	}
	
	void log_parse_warning(const string &in msg)
	{
		puts('SpeechBubble: ' + msg + ' on line ' + parse_line_index);
	}
	
	/// Returns the parsed integer if there was one - check `parse_result` for the results.
	///   `Success` - One or more valid integer characters were found at the current index and consumed.
	///   `Failure` - No valid integer character was found.
	/// `parse_sign` will also contain whether the number was explicitly signed with a "+" or "-".
	/// Can start with a "+" or "-"
	int try_consume_int()
	{
		parse_sign = None;
		
		if(parse_index >= text_length)
		{
			parse_result = SpeechBubble::ParseResult::Failure;
			return 0.0;
		}
		
		bool neg = false;
		int chr = _text[parse_index];
		
		if(chr == SpeechBubble::Plus)
		{
			parse_sign = SpeechBubble::ParseSign::Positive;
			parse_index++;
			if(parse_index >= text_length)
				return +0.0;
			chr = _text[parse_index];
		}
		else if(chr == SpeechBubble::Minus)
		{
			parse_sign = SpeechBubble::ParseSign::Negative;
			parse_index++;
			if(parse_index >= text_length)
				return -0.0;
			neg = true;
			chr = _text[parse_index];
		}
		else if(!is_digit(chr))
		{
			parse_result = SpeechBubble::ParseResult::Failure;
			return 0.0;
		}
		
		int acc = chr - SpeechBubble::Digit0;
		parse_index++;
		
		do
		{
			chr = _text[parse_index];
			
			if(!is_digit(chr))
				break;
			
			acc *= 10;
			acc += chr - SpeechBubble::Digit0;
			
			parse_index++;
		}
		while(parse_index < text_length);
		
		parse_result = SpeechBubble::ParseResult::Success;
		return neg ? -acc : acc;
	}
	
	/// The same as `try_consume_int` but returns a float.
	float try_consume_float()
	{
		parse_sign = None;
		
		if(parse_index >= text_length)
		{
			parse_result = SpeechBubble::ParseResult::Failure;
			return 0.0;
		}
		
		bool neg = false;
		int chr = _text[parse_index];
		
		if(chr == SpeechBubble::Plus)
		{
			parse_sign = SpeechBubble::ParseSign::Positive;
			parse_index++;
			if(parse_index >= text_length)
				return +0.0;
			chr = _text[parse_index];
		}
		else if(chr == SpeechBubble::Minus)
		{
			parse_sign = SpeechBubble::ParseSign::Negative;
			parse_index++;
			if(parse_index >= text_length)
				return -0.0;
			neg = true;
			chr = _text[parse_index];
		}
		else if(!is_digit(chr) && chr != SpeechBubble::Period)
		{
			parse_result = SpeechBubble::ParseResult::Failure;
			return 0.0;
		}
		
		string acc = ' ';
		acc[0] = chr;
		int acc_index = 1;
		parse_index++;
		
		do
		{
			chr = _text[parse_index];
			
			if(!is_digit(chr) && chr != SpeechBubble::Period)
				break;
			
			acc += ' ';
			acc[acc_index++] = chr;
			
			parse_index++;
		}
		while(parse_index < text_length);
		
		parse_result = SpeechBubble::ParseResult::Success;
		return neg ? -parseFloat(acc) : parseFloat(acc);
	}
	
	/// Gets the meta data for the next character.
	/// Only use during `try_parse_speech_attribute`.
	SpeechBubble::ChrMeta@ get_chr_meta()
	{
		if(@chr_meta_pending != null)
			return chr_meta_pending;
		
		if(chr_meta_count >= chr_meta_size)
		{
			chr_meta_size *= 2;
			chr_meta_list.resize(chr_meta_size);
		}
		
		@chr_meta_pending = chr_meta_list[chr_meta_count++];
		chr_meta_pending.delay = 0;
		chr_meta_pending.cps_set = false;
		return chr_meta_pending;
	}
	
	/// Sets the display delay for the character after this attribute.
	/// If `add` is true, will add to the delay if there is any, otherwise overwrites it.
	/// By default `delay is the number of "characters" of delay, but if `is_seconds` is true then `delay`
	/// is considered to be seconds and will be converted.
	/// Only use during `try_parse_speech_attribute`.
	void push_chr_delay(const float delay, const bool add=true, const bool is_seconds=false)
	{
		if(delay == 0)
			return;
		
		get_chr_meta().update_delay(is_seconds ? delay * characters_per_second : delay, add);
	}
	
	private bool is_digit(const int chr)
	{
		return chr >= SpeechBubble::Digit0  && chr <= SpeechBubble::Digit9;
	}
	
}

namespace SpeechBubble
{
	
	const int AttribLineDelay = 68;
	const int AttribAutoAdvance = 97;
	const int AttribChrDelay = 100;
	const int AttribSpeed = 115;
	
	const int Plus = 43;
	const int Minus = 45;
	const int Period = 46;
	const int Digit0 = 48;
	const int Digit9 = 57;
	const int NewLine = 10;
	const int OpenBracket = 91;
	const int CloseBracket = 93;
	const int Percent = 37;
	
	///Allows parsing custom attributes for speech bubbles.
	 /// These take precedence over the built in ones.
	interface CustomAttributeParser
	{
		
		/// `attrib_chr` - The current attribute character code.
		/// `SpeechBubble::parse_index` - The index of the next character. Leave as is if the attribute is not parsed or no more characters are consumed.
		/// Return true if the attribute is consumed by the custom parser.
		bool try_parse_speech_attribute(SpeechBubble@ speech_bubble, int attrib_chr, const string &in text, const int text_length);
		
	}
	
	interface Listener
	{
		
		void on_speech_bubble_init(SpeechBubble@ speech_bubble);
		void on_speech_bubble_progress(SpeechBubble@ speech_bubble, const int chr_index);
		void on_speech_bubble_complete(SpeechBubble@ speech_bubble);
		void on_speech_bubble_advance(SpeechBubble@ speech_bubble);
		
	}
	
	/// Allows drawing custom content inside the speech bubble.
	interface CustomContent
	{
		
		/// Return the full content size, e.g. the full width and height of the textfield once all the text has been revealed.
		void get_speech_bubble_content_final_size(float &out width, float &out height);
		
		/// Return the current content size, e.g. if by default the size of the text as characters are revealed
		void get_speech_bubble_content_size(float &out width, float &out height);
		
		void speech_bubble_draw(const int layer, const int sub_layer, const float x, const float y, const float w, const float h, const float sub_frame);
		
	}
	
	enum ParseResult
	{
		None,
		Success,
		Failure,
	}
	
	enum ParseSign
	{
		None,
		Positive,
		Negative,
	}
	
	class ChrMeta
	{
		
		int index;
		float delay;
		/// characters_per_second
		bool cps_set;
		float cps;
		bool cps_is_relative;
		bool cps_is_percent;
		
		// Sets the display delay for this character.
		/// If `add` is true, will add to the delay if there is any, otherwise overwrites it.
		void update_delay(const float delay, const bool add=true)
		{
			this.delay = add ? this.delay + delay : delay;
		}
		
		string cps_to_string()
		{
			return (cps_is_relative ? formatFloat(cps, '+') : formatFloat(cps)) + (cps_is_percent ? '%' : '');
		}
		
	}
	
}
