class BurstEmitter
{

	entity@ track;
	array<entity@> emitters;
	float timer;

	BurstEmitter() { }

	BurstEmitter(const float timer=0.25)
	{
		this.timer = timer;
	}
	
	void remove(BaseScript@ script)
	{
		for(uint i = 0; i < emitters.length; i++)
		{
			script.g.remove_entity(emitters[i]);
		}
	}
	
	bool step(BaseScript@ script)
	{
		if(@track != null)
		{
			rectangle@ r = track.base_rectangle();
			const float cx = track.x() + r.left() + r.width * 0.5;
			const float cy = track.y() + r.top() + r.height * 0.5;
			for(uint i = 0; i < emitters.length; i++)
			{
				emitters[i].set_xy(cx, cy);
			}
		}
		
		timer -= DT;
		
		if(timer <= 0)
		{
			remove(script);
			return false;
		}
		
		return true;
	}

}
