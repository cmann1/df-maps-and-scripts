#include '../lib/math/math.cpp';
#include '../lib/drawing/common.cpp';

class Gear : trigger_base
{
	scene@ g;
	script@ script;
	scripttrigger@ self;
	
	[text] int layer = 12;
	[text] int sub_layer = 19;
	
	[angle] float angle = 0;
	[text] float speed = 360;
	[entity] uint connected_to_id;
	
	float rotation = 0;
	float rotation_prev = 0;
	float real_speed = 0;
	Gear@ connected_to_gear;
	
	Sprite spr('props3', 'sidewalk_10', 0.5, 0.51);
	
	Gear()
	{
		@g = get_scene();
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = @script;
		@this.self = @self;
	}
	
	void activate(controllable@ e){}
	
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
			const float circumference = 2 * PI * self.radius();
			const float other_circumference = 2 * PI * connected_to_gear.self.radius();
			const float ratio = other_circumference / circumference;
			real_speed = -connected_to_gear.real_speed * ratio;
		}
		else
		{
			real_speed = speed;
		}
		
		rotation_prev = rotation;
		rotation = (rotation + real_speed * DT) % 360.0;
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
		const float scale = self.radius() / 84.0;
		spr.draw(layer, sub_layer, 0, 0, self.x(), self.y(), lerp_angle_degrees(rotation_prev, rotation, sub_frame) + angle, scale, scale);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		
		if(@connected_to_gear != null)
		{
			draw_arrow(g, 22, 19,
				self.x(), self.y(),
				connected_to_gear.self.x(), connected_to_gear.self.y(),
				2, 10, 1, 0xFF0000FF);
		}
	}
	
}