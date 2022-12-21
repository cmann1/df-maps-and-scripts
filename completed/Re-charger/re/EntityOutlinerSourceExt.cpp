class EntityOutlinerSourceExt : EntityOutlinerSource
{
	
	[position,mode:world,layer:19,y:src_y] float src_x;
	[hidden] float src_y;
	
	private float real_radius;
	
	void init(script@ s, scripttrigger@ self)
	{
		EntityOutlinerSource::init(s, self);
		
		const float dx = self.x() - src_x;
		const float dy = self.y() - src_y;
		const float dist = sqrt(dx * dx + dy * dy);
		real_radius = self.radius() + dist;
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
	
}
