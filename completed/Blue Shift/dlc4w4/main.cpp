#include '../lib/drawing/common.cpp';
#include '../lib/enums/ColType.cpp';
#include '../lib/tiles/closest_point_on_tile.cpp';
#include '../lib/utils/colour.cpp';

const array<int> ENTITY_TYPES = {
	Enemy,
	Projectile,
	CameraNode,
	Emitter,
	Cleansed,
	AIController,
	Trigger,
	CheckPoint,
	LevelBoundary,
	LevelStart,
	TriggerArea,
	KillZone,
};

class script : callback_base
{
	
	[persist] int bounds_y1 = -16;
	[persist] int bounds_y2 =  16;
	[persist] int level_x1 = 0;
	[persist] int level_x2 = 10;
	[entity,trigger] uint default_fog_id;
	[persist] array<LoopData> loops;
	[entity] uint start_apple_id;
	[entity] uint end_apple_id;
	[persist] int end_loop_index = 1;
	[position,mode:world,layer:20,y:end_apple_spawn_y] float end_apple_spawn_x;
	[hidden] float end_apple_spawn_y;
	[persist|tooltip:'Enable and hold Alt + Left/Right/Middle mouse to\n  add/remove/clear spikes.\nUse the mouse wheel to adjust the desired loop index.']
	bool edit_spikes;
	[persist] array<SpikeData> spikes;
	
	[hidden] bool wait_for_loop = false;
	[hidden] bool is_looping = false;
	
	int prev_load_x1, prev_load_x2;
	int prev_load_x1_cp, prev_load_x2_cp;
	int loop_index, loop_index_prev;
	int loop_index_cp, loop_index_prev_cp;
	array<LoopData@> reset_loop_data, reset_loop_data_cp;
	
	int bounds_load_width;
	int level_width;
	
	scene@ g;
	editor_api@ editor;
	input_api@ input;
	tileinfo@ empty_tile;
	
	int num_players;
	array<Player> players;
	Player@ player;
	array<float> layer_scales;
	float min_layer_scale = 1;
	
	controllable@ start_apple;
	controllable@ end_apple;
	float end_apple_start_ox, end_apple_start_oy;
	float end_apple_spawn_ox, end_apple_spawn_oy;
	
	bool spike_map_initialised;
	dictionary spike_map;
	
	bool edit_init = true;
	int edit_loop_index = 1;
	bool has_edit_cursor;
	float edit_cursor_x1, edit_cursor_y1;
	float edit_cursor_x2, edit_cursor_y2;
	float edit_cursor_nx, edit_cursor_ny;
	textfield@ loop_index_txt;
	textfield@ edge_index_txt;
	
	fog_setting@ edit_hue_fog_settings;
	fog_setting@ edit_hue_fog_settings_adjusted;
	entity@ edit_hue_fog_trigger;
	array<uint> edit_fog_colours;
	float edit_hue_sx;
	
	//bool edit_process = true;
	
	script()
	{
		@g = get_scene();
		@editor = get_editor_api();
		@input = get_input_api();
		
		num_players = num_cameras();
		players.resize(num_players);
		for(int i = 0; i < num_players; i++)
		{
			Player@ player = @players[i];
			@player.cam = get_camera(i);
		}
		
		@player = num_players == 1 ? @players[0] : null;
		
		@empty_tile = create_tileinfo();
		empty_tile.solid(false);
		
		if(@editor != null)
		{
			@loop_index_txt = create_textfield();
			loop_index_txt.align_horizontal(0);
			loop_index_txt.align_vertical(1);
			
			@edge_index_txt = create_textfield();
			edge_index_txt.align_horizontal(0);
			edge_index_txt.align_vertical(1);
			edge_index_txt.set_font('envy_bold', 20);
		}
	}
	
	void editor_init()
	{
		edit_init = false;
		
		if(@editor == null)
			return;
		
		init_layer_scales();
		
		entity@ default_fog = entity_by_id(default_fog_id);
		if(@default_fog != null && default_fog.type_name() == 'fog_trigger')
		{
			fog_setting@ fog = get_active_camera().fog;
			bool has_sub_layers;
			float fog_speed;
			int trigger_size;
			get_fog_setting(default_fog, fog, edit_fog_colours, has_sub_layers, fog_speed, trigger_size);
			get_active_camera().change_fog(fog, 0);
		}
	}
	
	void fetch_players()
	{
		for(int i = 0; i < num_players; i++)
		{
			Player@ player = @players[i];
			@player.c = controller_controllable(i);
		}
	}
	
	void on_level_start()
	{
		fetch_players();
		
		level_width = level_x2 - level_x1;
		bounds_load_width = int(ceil(level_width / 2.0));
		
		int load_x1, load_x2;
		calc_bounds(load_x1, load_x2);
		prev_load_x1 = load_x1;
		prev_load_x2 = load_x2;
		
		init_layer_scales();
		
		@start_apple = controllable_by_id(start_apple_id);
		init_start_apple();
		
		if(!spike_map_initialised)
		{
			initialise_spike_map();
			// Clear - not sure if the large amount of data could affect checkpoints
			spikes.resize(0);
		}
	}
	
	private void init_layer_scales()
	{
		min_layer_scale = 1.0;
		
		layer_scales.insertLast(1.0);
		layer_scales.insertLast(1.0);
		
		for(int i = 2; i <= 20; i++)
		{
			layer_scales.insertLast(g.layer_scale(i));
			if(i >= 7 && layer_scales[i] < min_layer_scale)
			{
				min_layer_scale = layer_scales[i];
			}
		}
	}
	
	private void initialise_spike_map()
	{
		spike_map.deleteAll();
		
		for(uint i = 0; i < spikes.length; i++)
		{
			SpikeData@ spike = @spikes[i];
			array<SpikeData@>@ spike_column = get_spike_column(spike.x, true);
			spike_column.insertLast(spike);
		}
		
		spike_map_initialised = true;
	}
	
	private void init_loop_data()
	{
		for(uint i = 0; i < loops.length; i++)
		{
			loops[i].init();
		}
	}
	
	private array<SpikeData@>@ get_spike_column(const int x, const bool create = false)
	{
		array<SpikeData@>@ spike_column = null;
		const string key = x + '';
		
		if(spike_map.exists(key))
		{
			@spike_column = cast<array<SpikeData@>@>(@spike_map[key]);
		}
		else if(create)
		{
			@spike_column = array<SpikeData@>();
			@spike_map[key] = spike_column;
		}
		
		return spike_column;
	}
	
	private void init_start_apple()
	{
		if(@start_apple != null)
		{
			start_apple.on_hurt_callback(this, 'on_start_apple_hurt', 0);
		}
	}
	
	void entity_on_add(entity@ e)
	{
		filth_ball@ fb = e.as_filth_ball();
		if(@fb != null)
		{
			fb.filth_type(1);
			return;
		}
		
		if(e.type_name() == 'hittable_apple' && e.id() == start_apple_id)
		{
			@start_apple = e.as_controllable();
			init_start_apple();
		}
	}
	
	void checkpoint_save()
	{
		prev_load_x1_cp = prev_load_x1;
		prev_load_x2_cp = prev_load_x2;
		loop_index_cp = loop_index;
		loop_index_prev_cp = loop_index_prev;
		reset_loop_data_cp = reset_loop_data;
		
		for(uint i = 0; i < loops.length; i++)
		{
			loops[i].checkpoint_save();
		}
	}
	
	void checkpoint_load()
	{
		prev_load_x1 = prev_load_x1_cp;
		prev_load_x2 = prev_load_x2_cp;
		loop_index = loop_index_cp;
		loop_index_prev = loop_index_prev_cp;
		reset_loop_data = reset_loop_data_cp;
		
		for(uint i = 0; i < loops.length; i++)
		{
			loops[i].checkpoint_load();
		}
		
		fetch_players();
		init_loop_data();
		
		if(is_looping)
		{
			@end_apple = controllable_by_id(end_apple_id);
		}
	}
	
	void step(int num_entities)
	{
		if(@player == null)
			return;
		
		if(wait_for_loop && player.c.x() > (level_x1 * 48 + level_x2 * 48) * 0.5)
		{
			wait_for_loop = false;
			start_looping();
		}
		
		if(reset_loop_data.length > 0)
		{
			for(uint i = 0; i < reset_loop_data.length; i++)
			{
				reset_loop_data[i].reset();
			}
			reset_loop_data.resize(0);
		}
		
		if(!is_looping)
			return;
		
		int player_loop_index, _a; float _b;
		get_loop_start_x(player.c.x(), player_loop_index, _a, _b);
		if(player_loop_index != loop_index_prev)
		{
			select_loop(player_loop_index);
		}
		
		int load_x1, load_x2;
		calc_bounds(load_x1, load_x2);
		bool load_right = false;
		bool load_left = false;
		
		int tx1 = prev_load_x1;
		int tx2 = prev_load_x2;
		while(tx1 < load_x1)
		{
			wrap_tiles(tx1, level_width);
			load_right = true;
			tx1++;
		}
		
		while(tx2 > load_x2)
		{
			wrap_tiles(tx2, -level_width);
			load_left = true;
			tx2--;
		}
		
		if(load_left || load_right)
		{
			// Set layer scales to 1 so prop collisions work.
			min_layer_scale = 1;
			for(int j = 2; j <= 20; j++)
			{
				layer_scales[j] = g.layer_scale(j);
				g.layer_scale(j, 1);
				
				if(layer_scales[j] < min_layer_scale)
				{
					min_layer_scale = layer_scales[j];
				}
			}
			
			const int padding_t = 1;
			const float x1 = (load_right ? prev_load_x1 - padding_t : load_x2) * 48;
			const float x2 = (load_right ? load_x1 + 1 : prev_load_x2 + padding_t) * 48;
			const int offset_tx = level_width * (load_right ? 1 : -1);
			const float offset_x = offset_tx * 48;
			const float y1 = bounds_y1 * 48;
			const float y2 = bounds_y2 * 48;
			const float bounds_x1 = load_x1 * 48;
			const float bounds_x2 = load_x2 * 48;
			
			// End apple
			if(@end_apple != null)
			{
				if(load_right && end_apple.x() < bounds_x1 || load_left && end_apple.x() >= bounds_x2)
				{
					int loop_index, loop_rel_tx; float loop_start_x;
					get_loop_start_x(end_apple.x(), loop_index, loop_rel_tx, loop_start_x);
					
					// Move the end_apple container tiles
					const int ax = int(floor((loop_start_x + end_apple_start_ox) / 48));
					const int ay = int(floor(end_apple_start_oy / 48));
					const int layer = 19;
					for(int x = ax - 1; x <= ax + 1; x++)
					{
						for(int y = ay - 1; y <= ay + 1; y++)
						{
							tileinfo@ tile = g.get_tile(x, y, layer);
							g.set_tile(x + offset_tx, y, layer, tile, false);
							g.set_tile(x, y, layer, empty_tile, false);
						}
					}
					
					get_loop_start_x(end_apple.x() + offset_x, loop_index, loop_rel_tx, loop_start_x);
					const float new_x = loop_start_x + (loop_index >= end_loop_index ? end_apple_spawn_ox : end_apple_start_ox);
					const float new_y = loop_index >= end_loop_index ? end_apple_spawn_oy : end_apple_start_oy;
					const float prev_offset_x = new_x - end_apple.x();
					const float prev_offset_y = new_y - end_apple.y();
					end_apple.set_xy(new_x, new_y);
					end_apple.prev_x(end_apple.prev_x() + prev_offset_x);
					end_apple.prev_y(end_apple.prev_y() + prev_offset_y);
				}
			}
			
			// Entities
			for(uint i = 0; i < ENTITY_TYPES.length; i++)
			{
				const float scale = layer_scales[18];
				int j = g.get_entity_collision(y1 * scale, y2 * scale, x1, x2, ENTITY_TYPES[i]);
				while(--j >= 0)
				{
					entity@ e = g.get_entity_collision_index(j);
					const float ex = e.x();
					if(load_right && ex <= bounds_x1 || load_left && ex >= bounds_x2)
					{
						wrap_entity(e, offset_x);
					}
				}
			}
			
			// Props
			int i = g.get_prop_collision(y1 / min_layer_scale, y2 / min_layer_scale, x1, x2);
			while(--i >= 0)
			{
				prop@ p = g.get_prop_collision_index(i);
				if(p.layer() < 2)
					continue;
				
				const float px = p.x();
				if(load_right && px <= bounds_x1 || load_left && px >= bounds_x2)
				{
					p.x(px + offset_x);
				}
			}
			
			// Reset layer scales
			for(int j = 2; j <= 20; j++)
			{
				g.layer_scale(j, layer_scales[j]);
			}
		}
		
		prev_load_x1 = load_x1;
		prev_load_x2 = load_x2;
	}
	
	private void get_loop_start_x(const float x, int &out loop_index, int &out loop_rel_tx, float &out loop_start_x)
	{
		const int tx = int(floor((x - level_x1 * 48) / 48));
		loop_index = int(floor((x - level_x1 * 48) / (level_width * 48)));
		loop_rel_tx = tx % level_width;
		loop_start_x = level_x1 * 48 + loop_index * level_width * 48;
	}
	
	private void wrap_tiles(const int x, const int offset_x)
	{
		// First do the filth and layer 19
		{
			const int layer = 19;
			for(int y = bounds_y1; y < bounds_y2; y++)
			{
				tilefilth@ filth = g.get_tile_filth(x, y);
				
				tileinfo@ tile = g.get_tile(x, y, layer);
				g.set_tile(x + offset_x, y, layer, tile, false);
				g.set_tile(x, y, layer, empty_tile, false);
				
				g.set_tile_filth(x + offset_x, y, filth);
				g.set_tile_filth(x, y, 0, 0, 0, 0, true, true);
			}
			
			int loop_index, loop_rel_tx; float loop_x;
			get_loop_start_x(x * 48 + 24, loop_index, loop_rel_tx, loop_x);
			const int new_loop_index = loop_index + (offset_x > 0 ? 1 : -1);
			
			array<SpikeData@>@ spike_column = get_spike_column(level_x1 + loop_rel_tx);
			if(@spike_column != null)
			{
				for(uint i = 0; i < spike_column.length; i++)
				{
					SpikeData@ spike = @spike_column[i];
					const bool add = spike.index_add == new_loop_index;
					const bool remove = spike.index_remove == new_loop_index;
					if(add || remove)
					{
						tilefilth@ filth = g.get_tile_filth(x + offset_x, spike.y);
						uint8 t = filth.top();
						uint8 b = filth.bottom();
						uint8 l = filth.left();
						uint8 r = filth.right();
						switch(spike.side)
						{
							case 0: // Top
								if(add && t == 0)
									t = 13;
								else if(remove && t == 13)
									t = 0;
							break;
							case 1: // Bottom
								if(add && b == 0)
									b = 13;
								else if(remove && b == 13)
									b = 0;
							break;
							case 2: // Left
								if(add && l == 0)
									l = 13;
								else if(remove && l == 13)
									l = 0;
							break;
							case 3: // Right
								if(add && r == 0)
									r = 13;
								else if(remove && r == 13)
									r = 0;
							break;
						}
						g.set_tile_filth(x + offset_x, spike.y, t, b, l, r, true, true);
					}
				}
				//get_loop_start_x(const float x, int &out loop_index, float &out loop_x)
			}
		}
		
		for(int layer = 7; layer <= 20; layer++)
		{
			// Skip unused layers and layers without tiles.
			if(layer == 8 || layer == 11 || layer >= 14 && layer <= 19)
				continue;
			
			for(int y = bounds_y1; y < bounds_y2; y++)
			{
				tileinfo@ tile = g.get_tile(x, y, layer);
				g.set_tile(x + offset_x, y, layer, tile, false);
				g.set_tile(x, y, layer, empty_tile, false);
			}
		}
	}
	
	private void wrap_entity(entity@ e, const float offset_x)
	{
		e.x(e.x() + offset_x);
		
		controllable@ c = e.as_controllable();
		if(@c != null)
		{
			c.prev_x(c.prev_x() + offset_x);
		}
	}
	
	private void select_loop(const int loop_index)
	{
		this.loop_index = loop_index;
		this.loop_index_prev = loop_index;
		
		for(uint i = 0; i < loops.length; i++)
		{
			LoopData@ loop = @loops[i];
			if(loop.loop_index == loop_index)
			{
				loop.select(player.cam);
				reset_loop_data.insertLast(loop);
			}
		}
	}
	
	private void on_start_apple_hurt(controllable@ attacked, controllable@ attacker, hitbox@ attack_hitbox, int arg)
	{
		init_loop_data();
		select_loop(0);
		
		wait_for_loop = true;
		g.remove_entity(start_apple.as_entity());
	}
	
	void editor_step()
	{
		if(edit_init)
		{
			editor_init();
		}
		
		adjust_trigger_hue();
		
		//if(edit_process)
		//{
		//	edit_process = false;
		//	for(uint i = 0; i < spikes.length; i++)
		//	{
		//		SpikeData@ spike = @spikes[i];
		//		if(spike.index_add == 2 && spike.index_remove == 1)
		//		{
		//			spike.index_add = 1;
		//			spike.index_remove = 0;
		//		}
		//	}
		//}
		
		if(!edit_spikes)
			return;
		
		has_edit_cursor = false;
		
		if(editor.editor_tab() != 'Scripts')
			return;
		
		if(!input.key_check_gvb(12)) // Alt
			return;
		
		const int mouse = input.mouse_state();
		if(mouse & 1 != 0 || mouse & 2 != 0)
		{
			edit_loop_index += (mouse & 1 != 0 ? 1 : -1);
		}
		
		if(!spike_map_initialised)
		{
			initialise_spike_map();
		}
		
		// Left, right, middle mouse
		const bool add = input.key_check_gvb(2);
		const bool remove = !add && input.key_check_gvb(3);
		bool clear = !add && !remove && input.key_check_gvb(4);
		
		const float mx = input.mouse_x_world(19);
		const float my = input.mouse_y_world(19);
		const int tmx = int(floor(mx / 48));
		const int tmy = int(floor(my / 48));
		
		int closest_side = -1;
		int closest_tx, closest_ty;
		float closest_x, closest_y;
		float closest_nx, closest_ny;
		float closest_dist = 9999.0;
		tileinfo@ closest_tile;
		for(int tx = tmx - 1; tx <= tmx + 1; tx++)
		{
			for(int ty = tmy - 1; ty <= tmy + 1; ty++)
			{
				tileinfo@ tile = g.get_tile(tx, ty);
				if(!tile.solid() || tile.is_dustblock())
					continue;
				
				float out_x, out_y, normal_x, normal_y;
				const int side = closest_point_on_tile(
					mx, my, tx, ty, tile.type(), tile,
					out_x, out_y, normal_x, normal_y);
				if(side == -1)
					continue;
				
				const float dx = out_x - mx;
				const float dy = out_y - my;
				const float dist = dx * dx + dy * dy;
				if(dist < closest_dist)
				{
					closest_dist = dist;
					closest_tx = tx;
					closest_ty = ty;
					closest_x = out_x;
					closest_y = out_y;
					closest_nx = normal_x;
					closest_ny = normal_y;
					closest_side = side;
					@closest_tile = tile;
				}
			}
		}
		
		if(closest_side != -1)
		{
			get_edge_points(
				closest_tile.type(), closest_side,
				edit_cursor_x1, edit_cursor_y1, edit_cursor_x2, edit_cursor_y2,
				closest_tx * 48, closest_ty * 48);
			edit_cursor_nx = closest_nx;
			edit_cursor_ny = closest_ny;
			has_edit_cursor = true;
			
			if(add || remove)
			{
				array<SpikeData@>@ spike_column = get_spike_column(closest_tx, true);
				SpikeData@ spike = null;
				for(uint i = 0; i < spike_column.length; i++)
				{
					SpikeData@ sp = @spike_column[i];
					if(sp.x == closest_tx && sp.y == closest_ty && sp.side == closest_side)
					{
						@spike = sp;
						break;
					}
				}
				
				if(@spike == null)
				{
					spikes.resize(spikes.length + 1);
					@spike = @spikes[spikes.length - 1];
					spike.x = closest_tx;
					spike.y = closest_ty;
					spike.side = closest_side;
					spike_column.insertLast(spike);
				}
				
				if(add)
				{
					spike.index_add = edit_loop_index;
				}
				else if(remove)
				{
					spike.index_remove = edit_loop_index;
				}
				
				if(spike.index_add == 0 && spike.index_remove == 0)
				{
					clear = true;
				}
			}
			
			if(clear)
			{
				array<SpikeData@>@ spike_column = get_spike_column(closest_tx);
				if(@spike_column != null)
				{
					for(uint i = 0; i < spike_column.length; i++)
					{
						SpikeData@ sp = @spike_column[i];
						if(sp.x != closest_tx || sp.y != closest_ty || sp.side != closest_side)
							continue;
						
						spike_column.removeAt(i);
						if(spike_column.length == 0)
						{
							spike_map.delete(closest_tx + '');
						}
						const int index = spikes.findByRef(sp);
						if(index != -1)
						{
							spikes.removeAt(index);
						}
						break;
					}
				}
			}
		}
	}
	
	private void adjust_trigger_hue()
	{
		// Ctrl+Shift+Alt+LeftClick/RightClick
		if(!input.key_check_gvb(11) || !input.key_check_gvb(10) || !input.key_check_gvb(12) || (
			!input.key_check_gvb(2) && !input.key_check_gvb(3) && !input.key_check_gvb(4)))
			return;
		
		if(input.key_check_pressed_gvb(2) || input.key_check_pressed_gvb(3))
		{
			if(@editor.selected_trigger != null)
			{
				@edit_hue_fog_trigger = editor.selected_trigger;
			}
			
			if(@edit_hue_fog_trigger != null)
			{
				edit_hue_sx = input.mouse_x_world(22);
				bool _a; float _b; int _c;
				@edit_hue_fog_settings = get_active_camera().fog;
				@edit_hue_fog_settings_adjusted = get_active_camera().fog;
				get_fog_setting(edit_hue_fog_trigger, edit_hue_fog_settings, edit_fog_colours, _a, _b, _c);
				get_fog_setting(edit_hue_fog_trigger, edit_hue_fog_settings_adjusted, null, _a, _b, _c);
			}
		}
		
		if(@edit_hue_fog_trigger != null)
		{
			const bool shift_hue = input.key_check_gvb(2);
			const bool shift_val = !shift_hue && input.key_check_gvb(3);
			const bool shift_sat = !shift_hue && !shift_val && input.key_check_gvb(4);
			const float amount = (input.mouse_x_world(22) - edit_hue_sx) / 360.0 * 0.75;
			varstruct@ vars = edit_hue_fog_trigger.vars();
			vararray@ colour = vars.get_var('fog_colour').get_array();
			
			for (int layer = 0; layer <= 20; layer++)
			{
				for (int sub_layer = -1; sub_layer <= 24; sub_layer++)
				{
					const uint index = (sub_layer + 1) * 21 + layer;
					const uint clr = adjust_colour(edit_fog_colours[index], amount, shift_hue, shift_sat);
					
					colour.at(index).set_int32(clr);
					if(sub_layer == -1)
					{
						edit_hue_fog_settings_adjusted.layer_colour(layer, clr);
					}
					else
					{
						edit_hue_fog_settings_adjusted.colour(layer, sub_layer, clr);
					}
				}
			}
			\
			vararray@ gradient = vars.get_var('gradient').get_array();
			
			// BG Top
			uint clr = adjust_colour(edit_hue_fog_settings.bg_top(), amount, shift_hue, shift_sat);
			edit_hue_fog_settings_adjusted.bg_top(clr);
			gradient.at(0).set_int32(clr);
			// BG Mid
			clr = adjust_colour(edit_hue_fog_settings.bg_mid(), amount, shift_hue, shift_sat);
			edit_hue_fog_settings_adjusted.bg_mid(clr);
			gradient.at(1).set_int32(clr);
			// BG Bot
			clr = adjust_colour(edit_hue_fog_settings.bg_top(), amount, shift_hue, shift_sat);
			edit_hue_fog_settings_adjusted.bg_bot(clr);
			gradient.at(2).set_int32(clr);
			
			get_active_camera().change_fog(edit_hue_fog_settings_adjusted, 0);
		}
	}
	
	private uint adjust_colour(const uint clr, const float amount, const bool shift_hue, const bool shift_sat)
	{
		int r, g, b, a;
		float h, s, v;
		int_to_rgba(clr, r, g, b, a);
		rgb_to_hsv(r, g, b, h, s, v);
		
		if(shift_hue)
		{
			h = (h + amount) % 1;
			if (h < 0) h += 1;
		}
		else if(shift_sat)
		{
			s = min(max(s + amount, 0), 1);
		}
		else
		{
			v = min(max(v + amount, 0), 1);
		}
		
		return hsv_to_rgb(h, s, v) | (a << 24);;
	}
	
	void draw(float sub_frame)
	{
		if(@player == null)
			return;
		
		if(edit_spikes)
		{
			int load_x1, load_x2;
			calc_bounds(load_x1, load_x2);
			outline_rect(g, 22, 22, load_x1 * 48, bounds_y1 * 48, (load_x2 + 1) * 48, bounds_y2 * 48, 4, 0xffff0000);
			draw_bounds();
		}
	}
	
	void editor_draw(float sub_frame)
	{
		if(edit_spikes)
		{
			draw_bounds();
		}
		
		if(edit_spikes)
		{
			if(!spike_map_initialised)
			{
				initialise_spike_map();
			}
			
			camera@ cam = get_active_camera();
			const float zoom = 1.0 / cam.editor_zoom();
			float vx, vy, vw, vh;
			cam.get_layer_draw_rect(sub_frame, 19, vx, vy, vw, vh);
			const int tx1 = int(floor(vx / 48)) - 1;
			const int ty1 = int(floor(vy / 48)) - 1;
			const int tx2 = int(ceil((vx + vw) / 48)) + 1;
			const int ty2 = int(ceil((vy + vh) / 48)) + 1;
			
			for(int x = tx1; x <= tx2; x++)
			{
				array<SpikeData@>@ spike_column = get_spike_column(x);
				if(@spike_column == null)
					continue;
				
				for(uint i = 0; i < spike_column.length; i++)
				{
					SpikeData@ sp = @spike_column[i];
					if(sp.y < ty1 || sp.y > ty2)
						continue;
					
					tileinfo@ tile = g.get_tile(sp.x, sp.y);
					float ex1, ey1, ex2, ey2;
					get_edge_points(
						tile.type(), sp.side,
						ex1, ey1, ex2, ey2,
						sp.x * 48, sp.y * 48);
					
					const uint edge_clr = sp.index_add != 0 && sp.index_remove != 0
						? 0xffffff00 : (sp.index_add != 0 ? 0xffff0000 : 0xff00ff00);
					const float dx = ex2 - ex1;
					const float dy = ey2 - ey1;
					const float length = sqrt(dx * dx + dy * dy);
					const float nx = dy / length;
					const float ny = -dx / length;
					g.draw_line_world(22, 22, ex1, ey1, ex2, ey2, 6 * zoom, edge_clr);
					const float mx = (ex1 + ex2) * 0.5;
					const float my = (ey1 + ey2) * 0.5;
					g.draw_line_world(22, 22, mx, my, mx + nx * 8 * zoom, my + ny * 8 * zoom, 3 * zoom, edge_clr);
					
					if(sp.index_add != 0)
					{
						edge_index_txt.text(sp.index_add + '+');
						edge_index_txt.colour(0xffff0000);
						edge_index_txt.draw_world(22, 23,
							mx + (sp.index_remove != 0 ? (edge_index_txt.text_width() * 0.5 + 0) * zoom : 0.0), my - 10 * zoom,
							0.75 * zoom, 0.75 * zoom, 0);
					}
					if(sp.index_remove != 0)
					{
						edge_index_txt.text(sp.index_remove + '-');
						edge_index_txt.colour(0xff00ff00);
						edge_index_txt.draw_world(22, 23,
							mx - (sp.index_add != 0 ? (edge_index_txt.text_width() * 0.5 + 0) * zoom : 0.0), my - 10 * zoom,
							0.75 * zoom, 0.75 * zoom, 0);
					}
				}
			}
			
			if(has_edit_cursor)
			{
				const uint cursor_clr = 0xffffffff;
				g.draw_line_world(22, 22, edit_cursor_x1, edit_cursor_y1, edit_cursor_x2, edit_cursor_y2, 4 * zoom, cursor_clr);
				const float mx = (edit_cursor_x1 + edit_cursor_x2) * 0.5;
				const float my = (edit_cursor_y1 + edit_cursor_y2) * 0.5;
				g.draw_line_world(22, 22, mx, my, mx + edit_cursor_nx * 12 * zoom, my + edit_cursor_ny * 12 * zoom, 4 * zoom, cursor_clr);
			}
			
			if(input.key_check_gvb(12)) // Alt
			{
				loop_index_txt.text('Loop Index: ' + edit_loop_index);
				loop_index_txt.draw_hud(2, 2, 0, 440, 0.75, 0.75, 0);
			}
		}
	}
	
	private void draw_bounds()
	{
		g.draw_line_world(22, 22, level_x1 * 48, bounds_y1 * 48, level_x1 * 48, bounds_y2 * 48, 4, 0xff0000ff);
		g.draw_line_world(22, 22, level_x2 * 48, bounds_y1 * 48, level_x2 * 48, bounds_y2 * 48, 4, 0xff0000ff);
		g.draw_line_world(22, 22, (level_x1 * 48 + level_x2 * 48) * 0.5, bounds_y1 * 48, (level_x1 * 48 + level_x2 * 48) * 0.5, bounds_y2 * 48, 5, 0xff00ff00);
	}
	
	void start_looping()
	{
		if(is_looping)
			return;
		
		is_looping = true;
		@end_apple = controllable_by_id(end_apple_id);
		if(@end_apple != null)
		{
			const float x1 = level_x1 * 48;
			end_apple_start_ox = end_apple.x() - x1;
			end_apple_start_oy = end_apple.y();
			end_apple_spawn_ox = end_apple_spawn_x - x1;
			end_apple_spawn_oy = end_apple_spawn_y;
		}
		
		// >> Clear out some of the tiles and props from before and after the level.
		for(int j = 2; j <= 20; j++)
		{
			layer_scales[j] = g.layer_scale(j);
			g.layer_scale(j, 1);
			
			if(layer_scales[j] < min_layer_scale)
			{
				min_layer_scale = layer_scales[j];
			}
		}
		
		clear_stuff(level_x1 - 13, level_x1 - 1);
		clear_stuff(level_x2, level_x2 + 6);
		
		for(int j = 2; j <= 20; j++)
		{
			g.layer_scale(j, layer_scales[j]);
		}
		
		int load_x1, load_x2;
		calc_bounds(load_x1, load_x2);
		prev_load_x1 = level_x1;
		prev_load_x2 = level_x2 - 1;
	}
	
	private void clear_stuff(const int tx1, const int tx2)
	{
		for(int x = tx1; x <= tx2; x++)
		{
			for(int y = bounds_y1; y < bounds_y2; y++)
			{
				for(int layer = 9; layer <= 20; layer++)
				{
					// Skip unused layers and layers without tiles.
					if(layer == 8 || layer == 11 || layer >= 14 && layer <= 19)
						continue;
					
					g.set_tile(x, y, layer, empty_tile, false);
				}
			}
		}
		
		int i = g.get_prop_collision((bounds_y1 * 48) / min_layer_scale, (bounds_y2 * 48) / min_layer_scale, tx1 * 48, tx2 * 48);
		while(--i >= 0)
		{
			prop@ p = g.get_prop_collision_index(i);
			if(p.layer() >= 2)
			{
				g.remove_prop(p);
			}
		}
	}
	
	void calc_bounds(int &out load_x1, int &out load_x2)
	{
		const int ctx = int(floor(player.cam.x() / 48));
		load_x1 = ctx - bounds_load_width;
		load_x2 = ctx + bounds_load_width;
		if(level_width % 2 != 0)
		{
			load_x1++;
		}
		
		load_x2--;
	}
	
}

class Player
{
	
	camera@ cam;
	controllable@ c;
	int loop = 0;
	
}

class LoopData
{
	[entity,trigger] uint fog_trigger_id;
	[entity,trigger] uint music_trigger_id;
	[entity,trigger] uint ambience_trigger_id;
	[persist] int loop_index;
	
	entity@ fog_trigger;
	entity@ music_trigger;
	entity@ ambience_trigger;
	
	bool music_reset;
	bool ambience_reset;
	float music_reset_x, music_reset_y;
	float ambience_reset_x, ambience_reset_y;
	bool music_reset_cp;
	bool ambience_reset_cp;
	float music_reset_x_cp, music_reset_y_cp;
	float ambience_reset_x_cp, ambience_reset_y_cp;
	
	void init()
	{
		@fog_trigger = entity_by_id(fog_trigger_id);
		@music_trigger = entity_by_id(music_trigger_id);
		@ambience_trigger = entity_by_id(ambience_trigger_id);
	}
	
	void checkpoint_save()
	{
		music_reset_cp = music_reset;
		ambience_reset_cp = ambience_reset;
		music_reset_x_cp = music_reset_x;
		music_reset_y_cp = music_reset_y;
		ambience_reset_x_cp = ambience_reset_x;
		ambience_reset_y_cp = ambience_reset_y;
	}
	
	void checkpoint_load()
	{
		music_reset = music_reset_cp;
		ambience_reset = ambience_reset_cp;
		music_reset_x = music_reset_x_cp;
		music_reset_y = music_reset_y_cp;
		ambience_reset_x = ambience_reset_x_cp;
		ambience_reset_y = ambience_reset_y_cp;
	}
	
	void select(camera@ cam)
	{
		if(@fog_trigger != null)
		{
			fog_setting@ fog = cam.fog;
			bool has_sub_layers; float fog_speed; int trigger_size;
			get_fog_setting(fog_trigger, fog, null, has_sub_layers, fog_speed, trigger_size);
			cam.change_fog(fog, fog_speed);
		}
		
		if(@music_trigger != null)
		{
			if(!music_reset)
			{
				music_reset_x = music_trigger.x();
				music_reset_y = music_trigger.y();
				music_reset = true;
			}
			music_trigger.set_xy(cam.x(), cam.y());
		}
		
		if(@ambience_trigger != null)
		{
			if(!ambience_reset)
			{
				ambience_reset_x = ambience_trigger.x();
				ambience_reset_y = ambience_trigger.y();
				ambience_reset = true;
			}
			ambience_trigger.set_xy(cam.x(), cam.y());
		}
	}
	
	void reset()
	{
		if(music_reset)
		{
			music_trigger.set_xy(music_reset_x, music_reset_y);
			music_reset = false;
		}
		
		if(ambience_reset)
		{
			ambience_trigger.set_xy(ambience_reset_x, ambience_reset_y);
			ambience_reset = false;
		}
	}
}

class SpikeData
{
	
	[persist] int x;
	[persist] int y;
	[persist] int side;
	[persist] int index_add;
	[persist] int index_remove;
	
}

class LoopInitTrigger : trigger_base
{
	
	script@ s;
	scripttrigger@ self;
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.s = s;
		@this.self = self;
	}
	
	void activate(controllable@ c)
	{
		if(s.is_looping)
		{
			s.g.remove_entity(self.as_entity());
			return;
		}
		if(c.player_index() != 0)
			return;
		
		s.start_looping();
		s.g.remove_entity(self.as_entity());
	}
	
}

void get_fog_setting(entity@ fog_trigger, fog_setting@ fog, array<uint>@ colours, bool &out has_sub_layers, float &out fog_speed, int &out trigger_size)
{
	varstruct@ vars = fog_trigger.vars();
	
	vararray@ colour = vars.get_var('fog_colour').get_array();
	vararray@ percent = vars.get_var('fog_per').get_array();
	
	if(@colours != null)
	{
		colours.resize((20 + 1) * (24 + 2));
	}
	
	for (int layer = 0; layer <= 20; layer++)
	{
		const uint clr = uint(colour.at(layer).get_int32());
		fog.layer_colour(layer, clr);
		fog.layer_percent(layer, percent.at(layer).get_float());
		
		if(@colours != null)
		{
			colours[layer] = clr;
		}
	}
	
	has_sub_layers = vars.get_var('has_sub_layers').get_bool();
	if (has_sub_layers)
	{
		for (int layer = 0; layer <= 20; layer++)
		{
			for (int sublayer = 0; sublayer <= 24; sublayer++)
			{
				const uint index = (sublayer + 1) * 21 + layer;
				const uint clr = uint(colour.at(index).get_int32());
				fog.colour(layer, sublayer, clr);
				fog.percent(layer, sublayer, percent.at((sublayer + 1) * 21 + layer).get_float());
				
				if(@colours != null)
				{
					colours[index] = clr;
				}
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
