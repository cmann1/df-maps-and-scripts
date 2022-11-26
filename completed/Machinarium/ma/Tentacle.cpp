#include '../lib/std.cpp';
#include '../lib/props/common.cpp';
#include '../lib/props/Prop.cpp';
#include '../lib/drawing/common.cpp';
#include '../lib/math/Bezier.cpp';
#include '../lib/drawing/Sprite.cpp';

#include 'PropDef.cpp';
#include 'PropData.cpp';

class Tentacle : trigger_base
{
	
	scene@ g;
	script@ script;
	scripttrigger@ self;
	
	[text] bool relative = true;
	[text] bool hide_overlays = true;
	[text] bool smart_handles = true;
	[text] bool accurate = true;
	[text] array<Bezier> curves;
	[text] PropDef prop_def;
	
	Sprite spr;
	
	int vertex_count = 0;
	array<float> previous_curve_values;
	float total_length;
	float bounds_x1, bounds_y1, bounds_x2, bounds_y2;
	
	array<PropData> props;
	int prop_count;
	
	int dragged_vertex_index = -1;
	float dragged_vertex_dx;
	float dragged_vertex_dy;
	float vertex_radius = pow(7, 2);
	int mouse_dragged_vertex_index = -1;
	int mouse_over_vertex_index = -1;
	
	float prev_x, prev_y;
	float mx, my;
	float cx0, cy0, cx1, cy1;
	
	float t = 0;
	float rand_x, rand_y;
	
	void init(script@ script, scripttrigger@ self)
	{
		@g = get_scene();
		@this.script = script;
		@this.self = self;
		
		const int curve_count = curves.length();
		previous_curve_values.resize(vertex_count = curve_count * 8);
		int j = 0;
		for(int i = 0; i < curve_count; i++)
		{
			Bezier@ curve = @curves[i];
			curve.update();
			previous_curve_values[j++] = curve.x1;
			previous_curve_values[j++] = curve.y1;
			previous_curve_values[j++] = curve.y2;
			previous_curve_values[j++] = curve.y2;
			previous_curve_values[j++] = curve.y3;
			previous_curve_values[j++] = curve.y3;
			previous_curve_values[j++] = curve.y4;
			previous_curve_values[j++] = curve.y4;
			
			curve.update();
			total_length += curve.length;
		}
		
		prev_x = self.x();
		prev_y = self.y();
		
		init_prop();
		calculate_props();
		
		if(curves.length > 1)
		{
			Bezier@ c1 = @curves[0];
			Bezier@ c2 = @curves[1];
			mx = c1.x4;
			my = c1.y4;
			cx0 = c1.x3;
			cy0 = c1.y3;
			cx1 = c2.x2;
			cy1 = c2.y2;
		}
		
		t = self.x();
		rand_x = rand_range(-0.002, 0.002);
		rand_y = rand_range(-0.002, 0.002);
	}
	
	void init_prop()
	{
		if(spr.sprite_set == '' || prop_def.sprite_set != spr.sprite_set || prop_def.sprite_name != spr.sprite_set)
		{
			sprite_from_prop(
				prop_def.prop_set, prop_def.prop_group, prop_def.prop_index, prop_def.sprite_set, prop_def.sprite_name);
			spr.set(prop_def.sprite_set, prop_def.sprite_name, prop_def.origin_x, prop_def.origin_y);
		}
	}
	
	void calculate_props()
	{
		const int curve_count = int(curves.length());
		float x = relative ? self.x() : 0;
		float y = relative ? self.y() : 0;
		
		prop_count = 0;
		props.resize(0);
		
		if(curve_count <= 0)
			return;
		
		bounds_x1 = INFINITY;
		bounds_y1 = INFINITY;
		bounds_x2 = -INFINITY;
		bounds_y2 = -INFINITY;
		
		const float spacing = prop_def.spacing;
		float d = spacing;
		float total_d = d;
		int curve_index = 0;
		Bezier@ curve = @curves[curve_index++];
		float x1 = curve.x1;
		float y1 = curve.y1;
		
		const float scale_sx = prop_def.scale_sx;
		const float scale_sy = prop_def.scale_sy;
		float d_scale_x = prop_def.scale_ex - scale_sx;
		float d_scale_y = prop_def.scale_ey - scale_sy;
		
		int layer_start, layer_end, layer_count;
		prop_def.calculate_layer_ranges(layer_start, layer_end, layer_count);
		float layer_current = layer_start;
		
		while(true)
		{
			float t = total_length != 0 ? total_d / total_length : 0;
			float x2, y2;
			float dx, dy;
			float dist, diff;
			float prev_diff = 0;
			int flip_count = 0;
			
			do
			{
				x2 = curve.mx(d);
				y2 = curve.my(d);
				dx = x2 - x1;
				dy = y2 - y1;
				dist = sqrt(dx * dx + dy * dy);
				diff = spacing - dist;
				
				if(!accurate)
				{
					break;
				}
				
				if(prev_diff == 0)
				{
					prev_diff = diff * 0.5;
				}
				
				d += diff;
				
				if(d >= curve.length)
				{
					x2 = curve.x4;
					y2 = curve.y4;
					dist = curve.length - d;
					break;
				}
				
				if(diff < 0 && prev_diff > 0 || diff > 0 && prev_diff < 0)
				{
					if(++flip_count > 4)
						break;
				}
				
				if(flip_count > 0 && abs(diff) >= abs(prev_diff))
					break;
				
				prev_diff = diff;
			}
			while(abs(diff) > 1);
			
			float angle =
				(atan2(dy, dx) * RAD2DEG) + prop_def.rotation +
				rand_range(-prop_def.rotation_rand, prop_def.rotation_rand);
			
			float base_t = (total_d - spacing) / (total_length - spacing);
			int current_layer, current_sub_layer;
			prop_def.calculate_layers(layer_start, layer_count, base_t, current_layer, current_sub_layer);
			
			props.resize(++prop_count);
			PropData@ p = @props[prop_count - 1];
			
			p.layer = current_layer;
			p.sub_layer = current_sub_layer;
			p.frame = prop_def.frame;
			p.palette = prop_def.palette;
			p.x = x + x1;
			p.y = y + y1;
			p.rotation = angle;
			p.scale_x = prop_def.scale_x * (scale_sx + d_scale_x * t);
			p.scale_y = prop_def.scale_y * (scale_sy + d_scale_y * t);
			
			float px1, py1, px2, py2;
			spr.get_bounds(
				p.x, p.y,
				p.rotation,
				p.scale_x, p.scale_y,
				px1, py1, px2, py2);
			
			if(px1 < bounds_x1) bounds_x1 = px1;
			if(py1 < bounds_y1) bounds_y1 = py1;
			if(px2 > bounds_x2) bounds_x2 = px2;
			if(py2 > bounds_y2) bounds_y2 = py2;
			
			d += dist;
			total_d += dist;
			x1 = x2;
			y1 = y2;
			
			if(d >= curve.length || dist <= 0)
			{
				if(curve_index >= curve_count)
					break;
					
				d -= curve.length;
				@curve = @curves[curve_index++];
			}
		}
		
		bounds_x1 -= 30;
		bounds_y1 -= 30;
		bounds_x2 -= 30;
		bounds_y2 -= 30;
	}
	
	void editor_var_changed(var_info@ info)
	{
		if(info.get_name(0) == 'prop_def')
		{
			init_prop();
			calculate_props();
		}
	}
	
	void step()
	{
		if(
			bounds_x1 > script.view_x2 || bounds_x2 < script.view_x1 ||
			bounds_y1 > script.view_y2 || bounds_y2 < script.view_y1)
			return;
		if(curves.length <= 1)
			return;
		
		Bezier@ c1 = @curves[0];
		Bezier@ c2 = @curves[1];
		
		const float mdx = sin(t * (0.003 + rand_x)) * 20;
		const float mdy = cos(t * (0.01 + rand_y)) * 20;
		const float cdx = sin((t + 23.23) * (0.004 + rand_x)) * 15;
		const float cdy = cos((t + 13.3) * (0.01 + rand_y)) * 25;
		c1.x4 = c2.x1 = mx + mdx;
		c1.y4 = c2.y1 = my + mdy;
		c1.x3 = cx0 + mdx + cdx;
		c1.y3 = cy0 + mdy + cdy;
		c2.x2 = cx1 + mdx - cdx;
		c2.y2 = cy1 + mdy - cdy;
		calculate_props();
		
		t += script.time_scale;
	}
	
	void editor_step()
	{
		const int curve_count = curves.length();
		bool requires_update = false;

		if(vertex_count != curve_count * 8)
		{
			const int count_prev = vertex_count / 8;
			previous_curve_values.resize(vertex_count = curve_count * 8);
			
			// Initialise new curves to something reasonable
			if(curve_count > count_prev)
				for(int i = count_prev; i < curve_count; i++)
				{
					const int vi = i * 8;
					const bool p = i > 0;
					Bezier@ cp = p ? @curves[i - 1] : null;
					Bezier@ curve = @curves[i];
					previous_curve_values[vi    ] = curve.x1 = p ? cp.x4 : (relative ? -100.0 : self.x() - 100.0);
					previous_curve_values[vi + 1] = curve.y1 = p ? cp.y4 : (relative ? 0 : self.y());
					previous_curve_values[vi + 2] = curve.x2 = curve.x1 + (p ? (cp.x4 - cp.x3) :  50);
					previous_curve_values[vi + 3] = curve.y2 = curve.y1 + (p ? (cp.y4 - cp.y3) : -50);
					previous_curve_values[vi + 4] = curve.x3 = curve.x1 + 150;
					previous_curve_values[vi + 5] = curve.y3 = curve.y1 - 50;
					previous_curve_values[vi + 6] = curve.x4 = curve.x1 + 200;
					previous_curve_values[vi + 7] = curve.y4 = curve.y1;
				}
		}
		
		const float self_x = self.x();
		const float self_y = self.y();
		float tx = relative ? self_x : 0;
		float ty = relative ? self_y : 0;
		
		const bool left_mouse_down = g.mouse_state(0) & 4 != 0;
		const bool middle_mouse_down = g.mouse_state(0) & 16 != 0;
		const float mouse_x = g.mouse_x_world(0, 22) - tx;
		const float mouse_y = g.mouse_y_world(0, 22) - ty;
		
		bool smart_handles = this.smart_handles;

		if(self.editor_selected())
		{
			if(middle_mouse_down)
				smart_handles = !smart_handles;
				
			mouse_over_vertex_index = -1;
			
			if(mouse_dragged_vertex_index == -1)
			{
				float dx, dy;
				
				for(int i = 0; i < curve_count; i++)
				{
					Bezier@ curve = @curves[i];
					
					dx = curve.x1 - mouse_x; dy = curve.y1 - mouse_y;
					if(dx * dx + dy * dy <= vertex_radius)
					{
						mouse_over_vertex_index = i * 8;
						break;
					}
					
					dx = curve.x2 - mouse_x; dy = curve.y2 - mouse_y;
					if(dx * dx + dy * dy <= vertex_radius)
					{
						mouse_over_vertex_index = i * 8 + 2;
						break;
					}
					
					dx = curve.x3 - mouse_x; dy = curve.y3 - mouse_y;
					if(dx * dx + dy * dy <= vertex_radius)
					{
						mouse_over_vertex_index = i * 8 + 4;
						break;
					}
					
					dx = curve.x4 - mouse_x; dy = curve.y4 - mouse_y;
					if(dx * dx + dy * dy <= vertex_radius)
					{
						mouse_over_vertex_index = i * 8 + 6;
						break;
					}
				}
			}
			
			if(left_mouse_down || middle_mouse_down)
			{
				if(mouse_dragged_vertex_index == -1)
					mouse_dragged_vertex_index = mouse_over_vertex_index;
			}
			else
			{
				mouse_dragged_vertex_index = -1;
			}
			
			if(mouse_dragged_vertex_index != -1)
			{
				const int dragged_handle = mouse_dragged_vertex_index % 8;
				Bezier@ curve = @curves[mouse_dragged_vertex_index / 8];
				
				if(dragged_handle == 0)
				{ curve.x1 = mouse_x; curve.y1 = mouse_y; }
				else if(dragged_handle == 2)
				{ curve.x2 = mouse_x; curve.y2 = mouse_y; }
				else if(dragged_handle == 4)
				{ curve.x3 = mouse_x; curve.y3 = mouse_y; }
				else if(dragged_handle == 6)
				{ curve.x4 = mouse_x; curve.y4 = mouse_y; }
			}
		}
		else if(mouse_dragged_vertex_index != -1)
		{
			mouse_dragged_vertex_index = -1;
		}
		
		dragged_vertex_index = -1;
		dragged_vertex_dx = 0;
		dragged_vertex_dy = 0;
		
		int pc_index = 0; // previous_curve_values_index
		for(int i = 0; i < curve_count; i++)
		{
			Bezier@ curve = @curves[i];
			
			if(previous_curve_values[pc_index] != curve.x1 || previous_curve_values[pc_index + 1] != curve.y1)
			{
				if(dragged_vertex_index == -1)
				{
					dragged_vertex_index = pc_index;
					dragged_vertex_dx = curve.x1 - previous_curve_values[pc_index];
					dragged_vertex_dy = curve.y1 - previous_curve_values[pc_index + 1];
				}
				previous_curve_values[pc_index] = curve.x1;
				previous_curve_values[pc_index + 1] = curve.y1;
				curve.requires_update = true;
				requires_update = true;
			}
			pc_index += 2;
			if(previous_curve_values[pc_index] != curve.x2 || previous_curve_values[pc_index + 1] != curve.y2)
			{
				if(dragged_vertex_index == -1)
				{
					dragged_vertex_index = pc_index;
					dragged_vertex_dx = curve.x2 - previous_curve_values[pc_index];
					dragged_vertex_dy = curve.y2 - previous_curve_values[pc_index + 1];
				}
				previous_curve_values[pc_index] = curve.x2;
				previous_curve_values[pc_index + 1] = curve.y2;
				curve.requires_update = true;
				requires_update = true;
			}
			pc_index += 2;
			if(previous_curve_values[pc_index] != curve.x3 || previous_curve_values[pc_index + 1] != curve.y3)
			{
				if(dragged_vertex_index == -1)
				{
					dragged_vertex_index = pc_index;
					dragged_vertex_dx = curve.x3 - previous_curve_values[pc_index];
					dragged_vertex_dy = curve.y3 - previous_curve_values[pc_index + 1];
				}
				previous_curve_values[pc_index] = curve.x3;
				previous_curve_values[pc_index + 1] = curve.y3;
				curve.requires_update = true;
				requires_update = true;
			}
			pc_index += 2;
			if(previous_curve_values[pc_index] != curve.x4 || previous_curve_values[pc_index + 1] != curve.y4)
			{
				if(dragged_vertex_index == -1)
				{
					dragged_vertex_index = pc_index;
					dragged_vertex_dx = curve.x4 - previous_curve_values[pc_index];
					dragged_vertex_dy = curve.y4 - previous_curve_values[pc_index + 1];
				}
				previous_curve_values[pc_index] = curve.x4;
				previous_curve_values[pc_index + 1] = curve.y4;
				curve.requires_update = true;
				requires_update = true;
			}
			pc_index += 2;
		}
		
		if(dragged_vertex_index != -1)
		{
			const int curve_index = dragged_vertex_index / 8;
			Bezier@ curve = @curves[curve_index]; 
			bool move_x2 = false;
			bool move_x3 = false;
			bool move_next_x2 = false;
			bool move_prev_x3 = false;
			
			// Keep vertices on adjacent curves together, and (if smart handles is on) handles in a straight line to form a smooth join.
			
			const int dragged_handle = dragged_vertex_index % 8;
			
			// x1, y1
			if(dragged_handle == 0)
			{
				// Update x4 on prev curve
				if(curve_index > 0)
				{
					previous_curve_values[dragged_vertex_index - 2] = curves[curve_index - 1].x4 = curve.x1;
					previous_curve_values[dragged_vertex_index - 1] = curves[curve_index - 1].y4 = curve.y1;
					curves[curve_index - 1].requires_update = true;
					requires_update = true;
					move_prev_x3 = true;
				}
				
				move_x2 = true;
			}
			
			// x2, y2
			if(dragged_handle == 2)
			{
				// Update x3 on prev curve
				if(curve_index > 0 && smart_handles)
				{
					float dx1 = curve.x2 - curve.x1;
					float dy1 = curve.y2 - curve.y1;
					const float dx2 = curves[curve_index - 1].x3 - curve.x1;
					const float dy2 = curves[curve_index - 1].y3 - curve.y1;
					const float length1 = sqrt(dx1 * dx1 + dy1 * dy1);
					const float length2 = sqrt(dx2 * dx2 + dy2 * dy2);
					dx1 /= length1;
					dy1 /= length1;
					previous_curve_values[dragged_vertex_index - 6] = curves[curve_index - 1].x3 = curve.x1 - dx1 * length2;
					previous_curve_values[dragged_vertex_index - 5] = curves[curve_index - 1].y3 = curve.y1 - dy1 * length2;
					curves[curve_index - 1].requires_update = true;
					requires_update = true;
				}
			}
			// x3, y3
			if(dragged_handle == 4)
			{
				// Update x2 on next curve
				if(curve_count > curve_index + 1 && smart_handles)
				{
					float dx1 = curve.x3 - curve.x4;
					float dy1 = curve.y3 - curve.y4;
					const float dx2 = curves[curve_index + 1].x2 - curve.x4;
					const float dy2 = curves[curve_index + 1].y2 - curve.y4;
					const float length1 = sqrt(dx1 * dx1 + dy1 * dy1);
					const float length2 = sqrt(dx2 * dx2 + dy2 * dy2);
					dx1 /= length1;
					dy1 /= length1;
					previous_curve_values[dragged_vertex_index + 6] = curves[curve_index + 1].x2 = curve.x4 - dx1 * length2;
					previous_curve_values[dragged_vertex_index + 7] = curves[curve_index + 1].y2 = curve.y4 - dy1 * length2;
					curves[curve_index + 1].requires_update = true;
					requires_update = true;
				}
			}
			// x4, y4
			if(dragged_handle == 6)
			{
				// Update x1 on next curve
				if(curve_count > curve_index + 1)
				{
					previous_curve_values[dragged_vertex_index + 2] = curves[curve_index + 1].x1 = curve.x4;
					previous_curve_values[dragged_vertex_index + 3] = curves[curve_index + 1].y1 = curve.y4;
					curves[curve_index + 1].requires_update = true;
					requires_update = true;
					move_next_x2 = true;
				}
				
				move_x3 = true;
			}
			
			// Keep handles relative to vertices
			if(smart_handles)
			{
				if(move_x2)
				{
					previous_curve_values[dragged_vertex_index + 2] = (curve.x2 += dragged_vertex_dx);
					previous_curve_values[dragged_vertex_index + 3] = (curve.y2 += dragged_vertex_dy);
				}
				if(move_x3)
				{
					previous_curve_values[dragged_vertex_index - 2] = (curve.x3 += dragged_vertex_dx);
					previous_curve_values[dragged_vertex_index - 1] = (curve.y3 += dragged_vertex_dy);
				}
				if(move_prev_x3)
				{
					previous_curve_values[dragged_vertex_index - 4] = (curves[curve_index - 1].x3 += dragged_vertex_dx);
					previous_curve_values[dragged_vertex_index - 3] = (curves[curve_index - 1].y3 += dragged_vertex_dy);
				}
				if(move_next_x2)
				{
					previous_curve_values[dragged_vertex_index + 4] = (curves[curve_index + 1].x2 += dragged_vertex_dx);
					previous_curve_values[dragged_vertex_index + 5] = (curves[curve_index + 1].y2 += dragged_vertex_dy);
				}
			}
		}
		
		if(self_x != prev_x || self_y != prev_y)
		{
			prev_x = self_x;
			prev_y = self_y;
			requires_update = true;
		}
		
		if(requires_update)
		{
			total_length = 0;
			for(int i = 0; i < curve_count; i++)
			{
				Bezier@ curve = @curves[i];
				if(curve.requires_update)
				{
					curve.update();
				}
				
				total_length += curve.length;
			}
			
			calculate_props();
		}
	}
	
	void draw_handles()
	{
		const bool is_selected = self.editor_selected();
		const bool draw = is_selected || ! hide_overlays;
		if(!draw)
			return;
		
		const float segment_length = 5;
		const int curve_count = int(curves.length());
		float x = relative ? self.x() : 0;
		float y = relative ? self.y() : 0;
		
		float total_length = 0;
		const int layer = 22;
		
		if(mouse_over_vertex_index != -1 || mouse_dragged_vertex_index != -1)
		{
			int index = mouse_dragged_vertex_index != -1 ? mouse_dragged_vertex_index : mouse_over_vertex_index;
			const int dragged_handle = index % 8;
			Bezier@ curve = @curves[index / 8];
			
			if(dragged_handle == 0)
				draw_dot(g, layer, 23, x + curve.x1, y + curve.y1, 8, 0xFFFFFFFF, 0);
			else if(dragged_handle == 2)
				draw_dot(g, layer, 243, x + curve.x2, y + curve.y2, 7, 0xFFFFFFFF, 45);
			else if(dragged_handle == 4)
				draw_dot(g, layer, 23, x + curve.x3, y + curve.y3, 7, 0xFFFFFFFF, 45);
			else if(dragged_handle == 6)
				draw_dot(g, layer, 23, x + curve.x4, y + curve.y4, 8, 0xFFFFFFFF, 0);
		}
		
		for(int i = 0; i < curve_count; i++)
		{
			Bezier@ curve = @curves[i];
			total_length += curve.length;
			
			float x1 = curve.x1;
			float y1 = curve.y1;
			float x2 = 0, y2 = 0;
			float d = segment_length;
			
			while(d <= curve.length)
			{
				x2 = curve.mx(d);
				y2 = curve.my(d);
				
				g.draw_line_world(layer, 24, x + x1, y + y1, x + x2, y + y2, 3, 0xFF3333FF);
				
				x1 = x2;
				y1 = y2;
				d += segment_length;
			}
			
			g.draw_line_world(layer, 24, x + x1, y + y1, x + x2, y + y2, 3, 0xFF3333FF);
			
			g.draw_line_world(layer, 24, x + curve.x1, y + curve.y1, x + curve.x2, y + curve.y2, 2, 0xFFFF44FF);
			g.draw_line_world(layer, 24, x + curve.x3, y + curve.y3, x + curve.x4, y + curve.y4, 2, 0xFFFF44FF);
			
			draw_dot(g, layer, 24, x + curve.x1, y + curve.y1, 5, 0xFFEE3333, 0);
			draw_dot(g, layer, 24, x + curve.x2, y + curve.y2, 4, 0xFFEE4488, 45);
			draw_dot(g, layer, 24, x + curve.x3, y + curve.y3, 4, 0xFFFF4444, 45);
			draw_dot(g, layer, 24, x + curve.x4, y + curve.y4, 5, 0xFFFF0000, 0);
		}
	}
	
	void draw(float sub_frame)
	{
		if(
			bounds_x1 > script.view_x2 || bounds_x2 < script.view_x1 ||
			bounds_y1 > script.view_y2 || bounds_y2 < script.view_y1)
			return;
		
		for(int i = prop_count - 1; i >= 0; i--)
		{
			PropData@ p = @props[i];
			spr.draw(p.layer, p.sub_layer, p.frame, p.palette,
				p.x, p.y,
				p.rotation,
				p.scale_x, p.scale_y,
				0xffffffff
			);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw_handles();
		draw(sub_frame);
	}
	
}
