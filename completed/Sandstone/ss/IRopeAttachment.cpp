namespace IRopeAttachment
{
	
	IRopeAttachment@ get(entity@ e)
	{
		if(@e == null)
			return null;
		
		scripttrigger@ st = e.as_scripttrigger();
		if(@st != null)
			return cast<IRopeAttachment@>(st.get_object());
		
		scriptenemy@ se = e.as_scriptenemy();
		if(@se != null)
			return cast<IRopeAttachment@>(se.get_object());
		
		return null;
	}
	
}

interface IRopeAttachment
{
	
	void get_attachment_point(float &out x, float &out y) const;
	float get_attachment_rotation() const;
	void get_position(float &out x, float &out y) const;
	
	void attach(Rope@ rope, Particle@ p);
	
}
