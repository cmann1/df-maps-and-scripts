#include "../common/drawing_utils.cpp"

class PropsPulse : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	[position,layer:19,y:min_y] float min_x = 0;
	[hidden] float min_y = 0;
	[position,layer:19,y:max_y] float max_x = 0;
	[hidden] float max_y = 0;
	[text] float pulse_speed = 10;
	[text] float pulse_strength = 1;
	[text] float pulse_frequency = 5;
	
	array<prop@> props;
	array<float> props_scale_x;
	array<float> props_scale_y;
	int prop_count = 0;
	
	float t = 0;
	
	PropsPulse()
	{
		@g = get_scene();
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.self = @self;
		
		const int num_props_collision = g.get_prop_collision(min_y, max_y, min_x, max_x);
		for(int i = 0; i < num_props_collision; i++)
		{
			prop@ p = g.get_prop_collision_index(i);
			if(p.prop_set() == 4 and p.prop_group() == 28 and p.prop_index() >= 13 and p.prop_index() <= 17)
			{
				props.insertLast(p);
				props_scale_x.insertLast(p.scale_x());
				props_scale_y.insertLast(p.scale_y());
			}
		}
		prop_count = int(props.length());
	}
	
	void step()
	{
		for(int i = 0; i < prop_count; i++)
		{
			prop@ p = props[i];
			const float s = sin(p.x() + p.y() / pulse_frequency + t) * pulse_strength * DT;
			p.scale_x(props_scale_x[i] + props_scale_x[i] * s);
			p.scale_y(props_scale_y[i] + props_scale_y[i] * s);
		}
		
		t += pulse_speed * DT;
	}
	
	void editor_draw(float sub_frame)
	{
		if(self.editor_selected())
		{
			outline_rect(g, min_x, min_y, max_x, max_y, 21, 21, 2, 0xFFFFFFFF);
		}
	}
	
}