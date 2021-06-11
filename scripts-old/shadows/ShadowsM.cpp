#include '../lib/std.cpp';
#include '../lib/math/math.cpp';
#include '../lib/tiles/get_tile_edge.cpp';
#include '../lib/tiles/get_tile_edge_angle.cpp';
#include '../lib/drawing/circle.cpp';

const float ASPECT_RATIO = 1920.0 / 1080;

class script
{
	
	scene@ g;
	controllable@ player;
	
	// !! DEBUG !!
	int debug_edges_size = 400;
	int debug_edges_count;
	array<float> edges_debug(debug_edges_size);
	
	int facing_edges_size = 400;
	int facing_edges_count;
	array<float> facing_edges(facing_edges_size);
	
	dictionary edge_map;
	array<Edge@> edge_pool;
	int edge_pool_size = 0;
	
	float player_cx;
	float player_cy;
	float light_x;
	float light_y;
	
	textfield@ debug_text;
	
	[text] bool enabled = true;
	[colour, alpha] uint shadow_colour = 0xFF000000;
	[text] uint shadow_layer = 20;
	[text] uint shadow_sublayer = 24;
	[text] float light_radius = 600;
	[text] int light_radius_segments = 8;
	[text] bool dont_render_outside_light = true;
	
	script()
	{
		@g = get_scene();
		
		@debug_text = create_textfield();
		debug_text.align_vertical(-1);
		debug_text.align_horizontal(-1);
		debug_text.colour(0xFFFF0000);
		debug_text.set_font('ProximaNovaReg', 26);
	}
	
	void checkpoint_load()
	{
		@player = null;
	}
	
	void step(int entities)
	{
		if(!enabled)
			return;
		
		if(@player == null)
			@player = controller_controllable(0);
			if(@player != null)
			{
				rectangle@ rect = player.base_rectangle();
				player_cx = (rect.left() + rect.right()) * 0.5;
				player_cy = (rect.top() + rect.bottom()) * 0.5;
			}
			
		if(player is null)
			return;
		
		const bool ignore_outside_light = dont_render_outside_light and light_radius > 0;
		
		const float light_x = this.light_x = player.x() + player_cx;
		const float light_y = this.light_y = player.y() + player_cy;
		
		camera@ cam = get_active_camera();
		const float cam_x = cam.x();
		const float cam_y = cam.y();
		const float screen_width = cam.screen_height() * ASPECT_RATIO / 2;
		const float screen_height = cam.screen_height() / 2;
		float left = min(cam.x(), cam.prev_x()) - screen_width;
		float right = max(cam.x(), cam.prev_x()) + screen_width;
		float top = min(cam.y(), cam.prev_y()) - screen_height;
		float bottom = max(cam.y(), cam.prev_y()) + screen_height;
		
		if(ignore_outside_light)
		{
			if(light_x - light_radius > left)
				left = light_x - light_radius;
			if(light_x + light_radius < right)
				right = light_x + light_radius;
			if(light_y - light_radius > top)
				top = light_y - light_radius;
			if(light_y + light_radius < bottom)
				bottom = light_y + light_radius;
		}
		
		const int tile_start_x = int(floor(left / 48));
		const int tile_start_y = int(floor(top / 48));
		const int tile_count_x = int(ceil(right / 48)) - tile_start_x;
		const int tile_count_y = int(ceil(bottom / 48)) - tile_start_y;
		
		float x1, y1, x2, y2;
		
		// !! DEBUG !!
		int debug_edges_count = 0;
		int debug_edges_size = this.debug_edges_size;
		
		int facing_edges_count = 0;
		int facing_edges_size = this.facing_edges_size;
		
		dictionary@ edge_map = @this.edge_map;
		edge_map.deleteAll();
		array<Edge@> edges;
		
		for(int tile_x = 0; tile_x <= tile_count_x; tile_x++)
		{
			for(int tile_y = 0; tile_y <= tile_count_y; tile_y++)
			{
				tileinfo@ tile_info = scene.get_tile(tile_start_x + tile_x, tile_start_y + tile_y);
				if(!tile_info.solid()) continue;
				
				const float x = (tile_start_x + tile_x) * 48;
				const float y = (tile_start_y + tile_y) * 48;
				
//				if(facing_edges_count + 16 > facing_edges_size)
//				{
//					facing_edges_size += 100;
//					facing_edges.resize(facing_edges_size);
//				}
//				// !! DEBUG !!
//				if(debug_edges_count + 16 > debug_edges_size)
//				{
//					debug_edges_size += 100;
//					edges_debug.resize(debug_edges_size);
//				}
				
				const int type = tile_info.type();
//				const angle = 
				
				//
				//
				//
				
				if(tile_info.edge_left() & 0x8 != 0)
				{
					get_tile_left_edge(type, x1, y1, x2, y2);
					x1 += x; x2 += x;
					y1 += y; y2 += y;
					
					const int angle = int(atan2(x2-x1,-(y2-y1)) * RAD2DEG);
					string v1_key = angle + ',' + x1 + ',' + y1;
					string v2_key = angle + ',' + x2 + ',' + y2;
					
					Edge@ edge = null;
					
					if(edge_map.exists(v1_key))
					{
						@edge = cast<Edge@>(edge_map[v1_key]);
						edge_map.delete(edge.v1_key);
						edge_map.delete(edge.v2_key);
						if(edge.x2 == x1 and edge.y2 == y1) { x1 = edge.x1; y1 = edge.y1; }
						else { x1 = edge.x2; y1 = edge.y2; }
						v1_key = angle + ',' + x1 + ',' + y1;
					}
					
					if(edge_map.exists(v2_key))
					{
						Edge@ edge2 = cast<Edge@>(edge_map[v2_key]);
						if(edge2.x2 == x2 and edge2.y2 == y2) { x2 = edge2.x1; y2 = edge2.y1; }
						else { x2 = edge2.x2; y2 = edge2.y2; }
						edge_map.delete(edge2.v1_key);
						edge_map.delete(edge2.v2_key);
						if(@edge == null)
							@edge = @edge2;
						else
							edge2.removed = true;
						v2_key = angle + ',' + x2 + ',' + y2;
					}
					
					if(@edge == null)
					{
						@edge = Edge();
						edges.insertLast(@edge);
					}
					
					edge.x1 = x1;
					edge.y1 = y1;
					edge.x2 = x2;
					edge.y2 = y2;
					edge.v1_key = v1_key;
					edge.v2_key = v2_key;
					edge_map[v1_key] = @edge;
					edge_map[v2_key] = @edge;
				}
				
				if(tile_info.edge_right() & 0x8 != 0)
				{
					get_tile_right_edge(type, x1, y1, x2, y2);
					x1 += x; x2 += x;
					y1 += y; y2 += y;
					
					const int angle = int(atan2(x2-x1,-(y2-y1)) * RAD2DEG);
					string v1_key = angle + ',' + x1 + ',' + y1;
					string v2_key = angle + ',' + x2 + ',' + y2;
					
					Edge@ edge = null;
					
					if(edge_map.exists(v1_key))
					{
						@edge = cast<Edge@>(edge_map[v1_key]);
						edge_map.delete(edge.v1_key);
						edge_map.delete(edge.v2_key);
						if(edge.x2 == x1 and edge.y2 == y1) { x1 = edge.x1; y1 = edge.y1; }
						else { x1 = edge.x2; y1 = edge.y2; }
						v1_key = angle + ',' + x1 + ',' + y1;
					}
					
					if(edge_map.exists(v2_key))
					{
						Edge@ edge2 = cast<Edge@>(edge_map[v2_key]);
						if(edge2.x2 == x2 and edge2.y2 == y2) { x2 = edge2.x1; y2 = edge2.y1; }
						else { x2 = edge2.x2; y2 = edge2.y2; }
						edge_map.delete(edge2.v1_key);
						edge_map.delete(edge2.v2_key);
						if(@edge == null)
							@edge = @edge2;
						else
							edge2.removed = true;
						v2_key = angle + ',' + x2 + ',' + y2;
					}
					
					if(@edge == null)
					{
						@edge = Edge();
						edges.insertLast(@edge);
					}
					
					edge.x1 = x1;
					edge.y1 = y1;
					edge.x2 = x2;
					edge.y2 = y2;
					edge.v1_key = v1_key;
					edge.v2_key = v2_key;
					edge_map[v1_key] = @edge;
					edge_map[v2_key] = @edge;
				}
				
				if(tile_info.edge_top() & 0x8 != 0)
				{
					get_tile_top_edge(type, x1, y1, x2, y2);
					x1 += x; x2 += x;
					y1 += y; y2 += y;
					
					const int angle = int(atan2(x2-x1,-(y2-y1)) * RAD2DEG);
					string v1_key = angle + ',' + x1 + ',' + y1;
					string v2_key = angle + ',' + x2 + ',' + y2;
					
					Edge@ edge = null;
					
					if(edge_map.exists(v1_key))
					{
						@edge = cast<Edge@>(edge_map[v1_key]);
						edge_map.delete(edge.v1_key);
						edge_map.delete(edge.v2_key);
						if(edge.x2 == x1 and edge.y2 == y1) { x1 = edge.x1; y1 = edge.y1; }
						else { x1 = edge.x2; y1 = edge.y2; }
						v1_key = angle + ',' + x1 + ',' + y1;
					}
					
					if(edge_map.exists(v2_key))
					{
						Edge@ edge2 = cast<Edge@>(edge_map[v2_key]);
						if(edge2.x2 == x2 and edge2.y2 == y2) { x2 = edge2.x1; y2 = edge2.y1; }
						else { x2 = edge2.x2; y2 = edge2.y2; }
						edge_map.delete(edge2.v1_key);
						edge_map.delete(edge2.v2_key);
						if(@edge == null)
							@edge = @edge2;
						else
							edge2.removed = true;
						v2_key = angle + ',' + x2 + ',' + y2;
					}
					
					if(@edge == null)
					{
						@edge = Edge();
						edges.insertLast(@edge);
					}
					
					edge.x1 = x1;
					edge.y1 = y1;
					edge.x2 = x2;
					edge.y2 = y2;
					edge.v1_key = v1_key;
					edge.v2_key = v2_key;
					edge_map[v1_key] = @edge;
					edge_map[v2_key] = @edge;
				}
				
				if(tile_info.edge_bottom() & 0x8 != 0)
				{
					get_tile_bottom_edge(type, x1, y1, x2, y2);
					x1 += x; x2 += x;
					y1 += y; y2 += y;
					
					const int angle = int(atan2(x2-x1,-(y2-y1)) * RAD2DEG);
					string v1_key = angle + ',' + x1 + ',' + y1;
					string v2_key = angle + ',' + x2 + ',' + y2;
					
					Edge@ edge = null;
					
					if(edge_map.exists(v1_key))
					{
						@edge = cast<Edge@>(edge_map[v1_key]);
						edge_map.delete(edge.v1_key);
						edge_map.delete(edge.v2_key);
						if(edge.x2 == x1 and edge.y2 == y1) { x1 = edge.x1; y1 = edge.y1; }
						else { x1 = edge.x2; y1 = edge.y2; }
						v1_key = angle + ',' + x1 + ',' + y1;
					}
					
					if(edge_map.exists(v2_key))
					{
						Edge@ edge2 = cast<Edge@>(edge_map[v2_key]);
						if(edge2.x2 == x2 and edge2.y2 == y2) { x2 = edge2.x1; y2 = edge2.y1; }
						else { x2 = edge2.x2; y2 = edge2.y2; }
						edge_map.delete(edge2.v1_key);
						edge_map.delete(edge2.v2_key);
						if(@edge == null)
							@edge = @edge2;
						else
							edge2.removed = true;
						v2_key = angle + ',' + x2 + ',' + y2;
					}
					
					if(@edge == null)
					{
						@edge = Edge();
						edges.insertLast(@edge);
					}
					
					edge.x1 = x1;
					edge.y1 = y1;
					edge.x2 = x2;
					edge.y2 = y2;
					edge.v1_key = v1_key;
					edge.v2_key = v2_key;
					edge_map[v1_key] = @edge;
					edge_map[v2_key] = @edge;
				}
			}
		}
		
		array<string>@ edge_keys = edge_map.getKeys();
		int count = edges.size();
		for(int i = 0; i < count; i++)
		{
			if(facing_edges_count + 16 > facing_edges_size)
			{
				facing_edges_size += 100;
				facing_edges.resize(facing_edges_size);
			}

			// !! DEBUG !!
			if(debug_edges_count + 4 > debug_edges_size)
			{
				debug_edges_size += 100;
				edges_debug.resize(debug_edges_size);
			}
			
			Edge@ edge = edges[i];
			if(edge.removed) continue;
			
			const float dx1 = edge.	x1 - light_x, dy1 = edge.y1 - light_y;
			const float dx2 = -(edge.y2 - edge.y1), dy2 = edge.x2 - edge.x1;
			// Dot product to determine of this edge is facing the player
			if(dx1 * dx2 + dy1 * dy2 > 0)
			{
				facing_edges[facing_edges_count++] = edge.x1; facing_edges[facing_edges_count++] = edge.y1;
				facing_edges[facing_edges_count++] = edge.x2; facing_edges[facing_edges_count++] = edge.y2;
			}
			
			// !! DEBUG !!
			edges_debug[debug_edges_count++] = edge.x1; edges_debug[debug_edges_count++] = edge.y1;
			edges_debug[debug_edges_count++] = edge.x2; edges_debug[debug_edges_count++] = edge.y2;
		}
		
		this.facing_edges_count = facing_edges_count;
		
		// !! DEBUG !!
		 this.debug_edges_count = debug_edges_count;
		debug_text.text(
			'Edges tested: ' + (this.debug_edges_count / 4) + '\n' +
			'Edges drawn : ' + (this.facing_edges_count / 4) + '\n'
		);
	}
	
	void draw(float sub_frame)
	{
		if(!enabled)
			return;
		
		// !! DEBUG !!
		for(int i = 0; i < debug_edges_count; i += 4)
		{
			const float x1 = edges_debug[i];
			const float y1 = edges_debug[i + 1];
			const float x2 = edges_debug[i + 2];
			const float y2 = edges_debug[i + 3];
			scene.draw_line(21, 19, x1, y1, x2, y2, 5, 0xFFFF0000);
		}
		
		// !! DEBUG !!
		for(int i = 0; i < facing_edges_count; i += 4)
		{
			const float x1 = facing_edges[i];
			const float y1 = facing_edges[i + 1];
			const float x2 = facing_edges[i + 2];
			const float y2 = facing_edges[i + 3];
			scene.draw_line(22, 20, x1, y1, x2, y2, 7, 0xFF0000FF);
		}
		
		const uint shadow_colour = this.shadow_colour;
		const uint shadow_layer = this.shadow_layer;
		const uint shadow_sublayer = this.shadow_sublayer;
		
		for(int i = 0; i < facing_edges_count; i += 4)
		{
			const float x1 = facing_edges[i];
			const float y1 = facing_edges[i + 1];
			const float x2 = facing_edges[i + 2];
			const float y2 = facing_edges[i + 3];
			
			const float dx1 = (x1 - light_x);
			const float dy1 = (y1 - light_y);
			const float dx2 = (x2 - light_x);
			const float dy2 = (y2 - light_y);
			const float dist = 1 / sqrt(dx1 * dx1 + dy1 * dy1) * 3000;

			scene.draw_quad_world(shadow_layer, shadow_sublayer, false,
				x1, y1, x2, y2,
				x2 + dx2 * dist, y2 + dy2 * dist,
				x1 + dx1 * dist, y1 + dy1 * dist,
				shadow_colour, shadow_colour, shadow_colour, shadow_colour);
			
			// !! DEBUG !!
			scene.draw_line(22, 20, x1, y1, x1 + dx1 * dist, y1 + dy1 * dist, 1, 0xFF0000FF);
			scene.draw_line(22, 20, x2, y2, x2 + dx2 * dist, y2 + dy2 * dist, 1, 0xFF0000FF);
		}
		
		const uint shadow_transparent_colour = shadow_colour & 0xFFFFFF;
		
		if(light_radius > 0)
		{
			fill_circle_outside(shadow_layer, shadow_sublayer, light_x, light_y, light_radius, light_radius_segments, shadow_colour, shadow_colour);
			fill_circle(shadow_layer, shadow_sublayer, light_x, light_y, light_radius, light_radius_segments, shadow_transparent_colour, shadow_colour);
		}
		
		debug_text.draw_hud(22, 22, SCREEN_LEFT + 10, SCREEN_TOP + 10, 1, 1, 0);
	}
	
}

class Edge
{
	
	float x1;
	float y1;
	float x2;
	float y2;
	string v1_key;
	string v2_key;
	bool removed = false;
	
	Edge(){}
	
	Edge(float x1, float y1, float x2, float y2, string v1_key, string v2_key)
	{
		this.x1 = x1;
		this.y1 = y1;
		this.x2 = x2;
		this.y2 = y2;
		this.v1_key = v1_key;
		this.v2_key = v2_key;
	}
	
}