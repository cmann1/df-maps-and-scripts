#include '../lib/std.cpp';
#include '../lib/math/math.cpp';
#include '../lib/emitters/EmitterBurstManager.cpp';
#include '../lib/enums/FilthType.cpp';
#include '../lib/enums/GVB.cpp';
#include '../lib/tiles/DustblockCleaner.cpp';
#include '../lib/tiles/TileEdge.cpp';
#include '../lib/triggers/InstancedSprite.cpp';
#include '../lib/triggers/MessageSystem.cpp';
//#include '../lib/utils/print_vars.cpp';

#include '../shared/entity_outliner/EntityOutliner.cpp';

#include 'Alarm.cpp';
#include 'Bubbles.cpp';
#include 'CameraDisconnectTrigger.cpp';
#include 'Checkpoint.cpp';
#include 'Light.cpp';
#include 'PlayerSpawner.cpp';
#include 'PlayerData.cpp';
#include 'PrismPropFade.cpp';
#include 'SecurityGate.cpp';
#include 'SecurityGateTrigger.cpp';
#include 'SecuritySwitch.cpp';
#include 'SecurityTrigger.cpp';
#include 'Snake0.cpp';
#include 'Snake.cpp';
#include 'SnakeIntro.cpp';
#include 'SnakeTestTrigger.cpp';
#include 'SpriteData.cpp';
#include 'SwitchRelay.cpp';
#include 'Terminal.cpp';
#include 'TileRemoveTrigger.cpp';

const float MapBoundsX1 = -190;
const float MapBoundsY1 = -1250;
const float MapBoundsX2 = 47700;
const float MapBoundsY2 = 70;

class script : DLScript {}

class DLScript
{
	
	[persist] bool show_triggers = true;
	[persist] bool draw_map_bounds;
	[persist] bool draw_camera;
	[position,mode:world,layer:19,y:map_y1] float map_x1;
	[hidden] float map_y1;
	[position,mode:world,layer:19,y:map_y2] float map_x2;
	[hidden] float map_y2;
	[persist] bool blur_bg;
	[persist|label:Outliner] EntityOutlinerSettings settings;
	
	scene@ g;
	
	bool is_playing;
	int num_players;
	camera@ cam;
	PlayerData@ active_player;
	float time_scale = 1;
	
	editor_api@ editor;
	input_api@ input;
	bool debug_triggers = true;
	
	array<PlayerData> players;
	array<PlayerData> players_cp;
	
	int security_level = 0;
	int security_level_cp = 0;
	
	MessageSystem messages;
	
	EmitterBurstManager@ emitter_bursts;
	DustblockCleaner@ dustblocks;
	
	SpriteData sprites;
	EntityOutliner entity_outliner;
	
	int prism_props_fade_size = 2;
	int prism_props_fade_count = 0;
	array<PrismPropFade> prism_props_fade(prism_props_fade_size);
	
	float prism_prop_fade_dt = DT * 1.75;
	
	int frame;
	
	//private textfield@ cb_txt = create_textfield();
	
	private float max_cam_width = 0;
	
	DLScript()
	{
		@editor = get_editor_api();
		@input = get_input_api();
		
		@g = get_scene();
		// Setting a load size of 8 might cause issues with entities not loading/unloading correctly?
		// Causing the map to eventually crash.
		g.override_stream_sizes(32, 8);
		
		num_players = num_cameras();
		@cam = get_active_camera();
		
		players.resize(num_players);
		players_cp.resize(num_players);
		
		for(int i = 0; i < num_players; i++)
		{
			PlayerData@ player = @players[i];
			player.index = i;
			@player.script = this;
			@player.cam = get_camera(i);
			player.checkpoint_load();
			
			players_cp[i].index = -1;
		}
		
		@emitter_bursts = EmitterBurstManager(g);
		@dustblocks = DustblockCleaner(g);
		@dustblocks.rng = RandomStream();
		@dustblocks.emitter_bursts = emitter_bursts;
		
		entity_outliner.init_settings(@settings);
	}
	
	void editor_var_changed(var_info@ info)
	{
		entity_outliner.editor_var_changed(info, @settings);
	}
	
	void on_level_start()
	{
		// Setting a load size of 8 might cause issues with entities not loading/unloading correctly?
		// Causing the map to eventually crash and/or losing chunks.
		g.override_stream_sizes(8, 8);
		
		g.special_enabled(!g.special_enabled());
		@cam = get_active_camera();
		@active_player = players[cam.player()];
		
		is_playing = true;
		
		for(int i = 0; i < num_players; i++)
		{
			players[i].checkpoint_load();
		}
		
		time_scale = g.time_warp();
	}
	
	void spawn_player(message@ msg)
	{
		const int player_index = msg.get_int('player');
		const float x = msg.get_float('x');
		const float y = msg.get_float('y');
		
		// Spawn_player is called before `checkpoint_load` so in single player
		// load the checkpoint data here.
		if(players_cp[player_index].index != -1)
		{
			if(num_players == 1)
			{
				players = players_cp;
			}
			else
			{
				players[player_index] = players_cp[player_index];
			}
		}
		
		PlayerData@ player = @players[player_index];
		
		if(player.is_snake)
		{
			player.spawn_snake(x, y);
			
			g.add_entity(player.snake.self.as_entity(), false);
			
			msg.set_entity('player', player.snake.self.as_entity());
		}
		else
		{
			const float ry = y - 90;
			
			for(int i = -1; i <= 1; i++)
			{
				raycast@ ray = g.ray_cast_tiles(x + 48 * i, ry, x + 48 * i, ry + 98, TileEdge::TopBit);
				if(ray.hit())
				{
					const int tx = ray.tile_x();
					const int ty = ray.tile_y();
					tilefilth@ t = g.get_tile_filth(tx, ty);
					// Remove spikes
					if(t.top() >= 9 || t.top() <= 13)
					{
						t.top(0);
						g.set_tile_filth(tx, ty, t);
					}
				}
			}
		}
	}
	
	void checkpoint_save()
	{
		for(int i = 0; i < num_players; i++)
		{
			players[i].checkpoint_save();
		}
		
		players_cp = players;
		security_level_cp = security_level;
		
		entity_outliner.checkpoint_save();
	}
	
	void checkpoint_save(int player_index)
	{
		players[player_index].checkpoint_save();
		players_cp[player_index] = players[player_index];
	}
	
	void checkpoint_load()
	{
		@cam = get_active_camera();
		@active_player = players[cam.player()];
		
		for(int i = 0; i < num_players; i++)
		{
			players[i].checkpoint_load();
		}
		
		security_level = security_level_cp;
		
		messages.clear();
		
		emitter_bursts.reset();
		dustblocks.clear();
		
		prism_props_fade_count = 0;
	}
	
	void checkpoint_load(int player_index)
	{
		players[player_index].checkpoint_load();
	}
	
	void entity_on_add(entity@ e)
	{
		const string name = e.type_name();
		
		if(name == 'entity_cleansed_walk' || name == 'entity_cleansed_full')
		{
			g.remove_entity(e);
			return;
		}
		
		filth_ball@ fb = e.as_filth_ball();
		if(@fb != null)
		{
			const int type = fb.filth_type();
			if(type >= FilthType::Dust && type <= FilthType::Poly)
			{
				fb.filth_type(FilthType::Slime);
			}
			return;
		}
	}
	
	void entity_on_remove(entity@ e)
	{
		const string name = e.type_name();
		
		if(name == 'enemy_tutorial_square')
		{
			if(prism_props_fade_count == prism_props_fade_size)
			{
				prism_props_fade.resize(prism_props_fade_size * 2);
			}
			
			PrismPropFade@ pp = @prism_props_fade[prism_props_fade_count++];
			pp.x = e.x();
			pp.y = e.y();
			pp.alpha = 1;
			prism_props_fade.insertLast(pp);
			
			int i = g.get_prop_collision(pp.y, pp.y, pp.x, pp.x);
			while(--i >= 0)
			{
				prop@ p = g.get_prop_collision_index(i);
				if(p.prop_set() != 3 || p.prop_group() != 22 || p.prop_index() != 3)
					continue;
				if(p.layer() != 18 || p.sub_layer() != 1)
					continue;
				
				g.remove_prop(p);
			}
		}
	}
	
	void step(int)
	{
		@cam = get_active_camera();
		@active_player = players[cam.player()];
		
		time_scale = g.time_warp();
		
		for(int i = 0; i < num_players; i++)
		{
			players[i].step();
		}
		
		emitter_bursts.update(time_scale);
		dustblocks.update(time_scale);
		
		for(int i = prism_props_fade_count - 1; i >= 0; i--)
		{
			PrismPropFade@ pp = @prism_props_fade[i];
			pp.alpha -= prism_prop_fade_dt;
			if(pp.alpha <= 0)
			{
				prism_props_fade[i] = prism_props_fade[--prism_props_fade_count];
			}
		}
		
		entity_outliner.step(cam);
		
		frame++;
	}
	
	void draw(float sub_frame)
	{
		if(blur_bg)
		{
			g.draw_glass_world(8, 23, map_x1, map_y1, map_x2, map_y2, 0, 0x00ffffff);
		}
		
		for(int i = 0; i < prism_props_fade_count; i++)
		{
			PrismPropFade@ pp = @prism_props_fade[i];
			sprites.PrismLarge.draw(pp.x, pp.y, 0, 1, 0, 0, lerp(min(pp.alpha + prism_prop_fade_dt, 1.0), pp.alpha, sub_frame));
		}
		
		entity_outliner.draw(sub_frame);
		
		//cb_txt.align_vertical(-1);
		//cb_txt.align_horizontal(-1);
		//cb_txt.text(g.combo_break_count() + '');
		//cb_txt.draw_hud(1, 1, -790, -440, 1, 1, 0);
	}
	
	void editor_entity_on_create(entity@ e)
	{
		if(e.type_name() == 'enemy_tutorial_square')
		{
			e.layer(17);
		}
	}
	
	void editor_step()
	{
		// debug_triggers = editor.triggers_visible();
		debug_triggers = show_triggers || editor.editor_tab() == 'Triggers';
		
		entity_outliner.editor_step(cam);
		
		// Alt+C
		if(input.key_check_pressed_vk(0x43) && input.key_check_vk(0x12))
		{
			draw_camera = !draw_camera;
		}
	}
	
	void editor_draw(float sub_frame)
	{
		if(blur_bg)
		{
			g.draw_glass_world(8, 23, map_x1, map_y1, map_x2, map_y2, 0, 0x00ffffff);
		}
		
		entity_outliner.draw(sub_frame);
		
		if(draw_map_bounds)
		{
			outline_rect(g, 22, 22, MapBoundsX1, MapBoundsY1, MapBoundsX2, MapBoundsY2, 3, 0x44ff0000);
		}
		
		if(draw_camera)
		{
			float view1_x, view1_y, view1_w, view1_h;
			float view2_x, view2_y, view2_w, view2_h;
			cam.get_layer_draw_rect(0, 21, view1_x, view1_y, view1_w, view1_h);
			cam.get_layer_draw_rect(1, 21, view2_x, view2_y, view2_w, view2_h);
			const float padding = 96;
			view1_x -= padding; view1_y -= padding;
			view2_x -= padding; view2_y -= padding;
			view1_w += padding * 2; view1_h += padding * 2;
			view2_w += padding * 2; view2_h += padding * 2;
			
			const float view_x1 = min(view1_x, view2_x);
			const float view_y1 = min(view1_y, view2_y);
			const float view_x2 = max(view1_x + view1_w, view2_x + view2_w);
			const float view_y2 = max(view1_y + view1_h, view2_y + view2_h);
			
			dictionary visited_nodes;
			int i = g.get_entity_collision(view_y1 - max_cam_width, view_y2 + max_cam_width, view_x1 - max_cam_width, view_x2 + max_cam_width, ColType::CameraNode);
			while(--i >= 0)
			{
				camera_node@ node1 = g.get_entity_collision_index(i).as_camera_node();
				if(@node1 == null)
					continue;
				const string key1 = node1.id() + '';
				
				varstruct@ vars1 = node1.vars();
				const float h1 = vars1.get_var('zoom_h').get_int32() * 0.5;
				const float w1 = h1 * 1.77777777777778;
				const float x1 = node1.x();
				const float y1 = node1.y();
				max_cam_width = max(w1, max_cam_width);
				
				const float c1x1 = x1 - w1;
				const float c1y1 = y1 - h1;
				const float c1x2 = x1 + w1;
				const float c1y2 = y1 + h1;
				
				const uint clr = 0x33ffffff;
				int connection_count = 0;
				vararray@ c_node_ids = vars1.get_var('c_node_ids').get_array();
				for(int j = int(c_node_ids.size()) - 1; j >= 0; j--)
				{
					entity@ e = entity_by_id(c_node_ids.at(j).get_int32());
					if(@e == null)
						continue;
					camera_node@ node2 = e.as_camera_node();
					if(@node2 == null)
						continue;
					const string key2 = node1.id() + '.' + node2.id();
					if(visited_nodes.exists(key2))
						continue;
					
					visited_nodes[key2] = true;
					
					varstruct@ vars2 = node2.vars();
					const float h2 = vars2.get_var('zoom_h').get_int32() * 0.5;
					const float w2 = h2 * 1.77777777777778;
					const float x2 = node2.x();
					const float y2 = node2.y();
					max_cam_width = max(w2, max_cam_width);
					
					const float c2x1 = x2 - w2;
					const float c2y1 = y2 - h2;
					const float c2x2 = x2 + w2;
					const float c2y2 = y2 + h2;
					
					if(c1y1 < c2y1 && c1x1 > c2x1)
						g.draw_line_world(22, 22, c1x1, c1y1, c2x1, c2y1, 4, clr);
					if(c1y1 < c2y1 && c1x1 < c2x1)
						g.draw_line_world(22, 22, c1x1, c1y2, c2x1, c2y2, 4, clr);
					if(c1y1 < c2y1 && c1x2 < c2x2)
						g.draw_line_world(22, 22, c1x2, c1y1, c2x2, c2y1, 4, clr);
					if(c1y1 > c2y1 && c1x2 < c2x2)
						g.draw_line_world(22, 22, c1x2, c1y2, c2x2, c2y2, 4, clr);
					
					if(c1y1 < c2y1)
						g.draw_line_world(22, 22, c1x1, c1y1, c1x2, c1y1, 4, clr);
					if(c1y2 > c2y2)
						g.draw_line_world(22, 22, c1x1, c1y2, c1x2, c1y2, 4, clr);
					
					if(c1x1 < c2x1)
						g.draw_line_world(22, 22, c1x1, c1y1, c1x1, c1y2, 4, clr);
					if(c1x2 > c2x2)
						g.draw_line_world(22, 22, c1x2, c1y1, c1x2, c1y2, 4, clr);
					
					connection_count++;
				}
				
				if(connection_count == 0)
				{
					outline_rect(g, 21, 21, x1 - w1, y1 - h1, x1 + w1, y1 + h1, 2, clr);
				}
			}
		}
	}
	
	void add_screen_shake(const float x, const float y, const float dir, const float force)
	{
		for(int i = 0; i < num_players; i++)
		{
			players[i].cam.add_screen_shake(x, y, dir, force);
		}
	}
	
	void update_security_level(const int new_level)
	{
		if(security_level == new_level)
			return;
		
		security_level = new_level;
		messages.broadcast('security_updated', null);
		messages.broadcast('post_security_updated', null);
	}
	
	bool in_map_map_bounds(const float x, const float y) const
	{
		return x >= MapBoundsX1 && x <= MapBoundsX2 && y >= MapBoundsY1 && y <= MapBoundsY2;
	}
	
}
