const uint LightColType = 64;

class Light : EntityOutlinerSource
{
	
	DLScript@ script;
	
	[position,mode:world,layer:19,y:src_y] float src_x;
	[hidden] float src_y;
	
	float x1, y1, x2, y2;
	float x, y;
	float radius, real_radius;
	bool is_on = true;
	
	private collision@ light_hitbox;
	
	void init(script@ s, scripttrigger@ self)
	{
		EntityOutlinerSource::init(s, self);
		
		const float dx = self.x() - src_x;
		const float dy = self.y() - src_y;
		const float dist = sqrt(dx * dx + dy * dy);
		real_radius = self.radius() + dist;
		
		@this.script = s;
	}
	
	void on_add()
	{
		if(!script.is_playing)
			return;
		
		x = self.x();
		y = self.y();
		radius = self.radius();
		x1 = x - radius;
		y1 = y - radius;
		x2 = x + radius;
		y2 = y + radius;
		@light_hitbox = script.g.add_collision(self.as_entity(), -radius, radius, -radius, radius, LightColType);
		rectangle@ rec = light_hitbox.rectangle();
		light_hitbox.rectangle(rec, x, y);
	}
	
	void on_remove()
	{
		if(@light_hitbox != null)
		{
			light_hitbox.remove();
		}
	}
	
	protected float get_source_radius() override
	{
		return real_radius;
	}
	
	protected float get_source_centre_x() override
	{
		return src_x;
	}
	
	protected float get_source_centre_y() override
	{
		return src_y;
	}
	
	void editor_draw(float)
	{
		if(!script.debug_triggers)
			return;
		
		script.g.draw_line_world(22, 1, self.x(), self.y(), src_x, src_y, 3, 0x88ff5522);
		script.g.draw_rectangle_world(22, 22, src_x - 5, src_y - 5, src_x + 5, src_y + 5, 45, 0xffff5522);
	}
	
}
