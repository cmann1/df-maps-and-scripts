//#include '../lib/math/math.cpp';
//#include '../lib/drawing/common.cpp';

class Arm : trigger_base
{
	scene@ g;
	script@ script;
	scripttrigger@ self;
	
	[text] int layer = 12;
	[text] int sub_layer = 19;
	
	[entity] uint connected_to_id;
	[text] float connection_point = 0.6;
	[angle,radian] float connection_angle = 0;
	
	[text] string prop_group = 'props4';
	[text] string prop_name = 'machinery_6';
	[text] float prop_origin_x = 0.02;
	[text] float prop_origin_y = 0.5;
	[angle] float prop_angle = 0;
	
	Gear@ connected_to_gear;
	
	float connection_x, connection_y;
	float base_x, base_y;
	float prev_connection_x, prev_connection_y;
	float prev_base_x, prev_base_y;
	float connection_radius;
	float connection_distance;
	float slide_angle;
	
	Sprite spr('props4', 'machinery_6', 0.02, 0.5);
	
	Arm()
	{
		@g = get_scene();
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = @script;
		@this.self = @self;
		
		self.editor_handle_size(6);
		spr.set(prop_group, prop_name, prop_origin_x, prop_origin_y);
	}
	
	void step()
	{
		if(connected_to_id != 0 and connected_to_gear is null)
		{
			entity@ e = entity_by_id(connected_to_id);
			if(@e != null)
			{
				scripttrigger@ st = e.as_scripttrigger();
				if(@st != null)
				{
					@connected_to_gear = cast<Gear>(st.get_object());
				}
			}
		}
		
		if(@connected_to_gear != null)
		{
			prev_base_x = base_x;
			prev_base_y = base_y;
			prev_connection_x = connection_x;
			prev_connection_y = connection_y;
			
			const float x1 = self.x();
			const float y1 = self.y();
			const float x2 = connected_to_gear.self.x();
			const float y2 = connected_to_gear.self.y();
			connection_distance = magnitude(x2 - x1, y2 - y1);
			
			connection_radius = connected_to_gear.self.radius() * connection_point;
			connection_x = x2 + cos(connected_to_gear.rotation * DEG2RAD + connection_angle) * connection_radius;
			connection_y = y2 + sin(connected_to_gear.rotation * DEG2RAD + connection_angle) * connection_radius;
			
			slide_angle = atan2(y2 - y1, x2 - x1);
			
			const float base_offset = (cos(shortest_angle(connected_to_gear.rotation * DEG2RAD + connection_angle, slide_angle)) + 1) * connection_radius;
			const float connection_angle = atan2(connection_y - y1, connection_x - x1);
			
			base_x = self.x() + cos(slide_angle) * base_offset;
			base_y = self.y() + sin(slide_angle) * base_offset;
		}
	}
	
	void editor_step()
	{
		if(@connected_to_gear != null)
		{
			if(connected_to_gear.self.id() != connected_to_id)
				@connected_to_gear = null;
		}
		
		step();
	}
	
	void draw(float sub_frame)
	{
		if(@connected_to_gear == null)
			return;
		
		const float base_x = lerp(prev_base_x, this.base_x, sub_frame);
		const float base_y = lerp(prev_base_y, this.base_y, sub_frame);
		const float connection_x = lerp(prev_connection_x, this.connection_x, sub_frame);
		const float connection_y = lerp(prev_connection_y, this.connection_y, sub_frame);
		
		spr.draw(layer, sub_layer, 0, 0,
			base_x,
			base_y,
			atan2(connection_y - base_y, connection_x - base_x) * RAD2DEG + prop_angle,
			(connection_distance - connected_to_gear.self.radius() * connection_point) / spr.sprite_width, 1);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		
		if(@connected_to_gear != null)
		{
			draw_dot(g, 22, 22, connection_x, connection_y, 5, 0xFF0000FF);
			g.draw_line_world(22, 22, self.x(), self.y(), self.x() + cos(slide_angle) * connection_radius * 2, self.y() + sin(slide_angle) * connection_radius * 2, 3, 0xFF0000FF);
//			draw_arrow(g, 22, 19,
//				self.x(), self.y(),
//				connected_to_gear.self.x(), connected_to_gear.self.y(),
//				2, 10, 1, 0xFF0000FF);
		}
	}
	
}















