class ArcContactList : ArcAreas
{
	
	script@ script;
	array<ArcContact@> sources;
	
	void initialise(array<ArcContact>@ sources, const int index=-1)
	{
		this.sources.resize(sources.length);
		for(uint i = 0; i < sources.length; i++)
		{
			@this.sources[i] = @sources[i];
		}
		
		initialise(index);
	}
	
	void initialise(array<ArcSource>@ sources, const int index=-1)
	{
		this.sources.resize(sources.length);
		for(uint i = 0; i < sources.length; i++)
		{
			@this.sources[i] = @sources[i];
		}
		
		initialise(index);
	}
	
	void initialise(array<ArcEndPoint>@ sources, const int index=-1)
	{
		this.sources.resize(sources.length);
		for(uint i = 0; i < sources.length; i++)
		{
			@this.sources[i] = @sources[i];
		}
		
		initialise(index);
	}
	
	void initialise(const int index)
	{
		if(index == -1)
		{
			for(uint i = 0; i < sources.length; i++)
			{
				sources[i].initialise(script);
			}
		}
		else if(index >= 0 && index < int(sources.length))
		{
			sources[index].initialise(script);
		}
		
		initialise();
	}
	
	void initialise() override
	{
		for(uint i = 0; i < sources.length; i++)
		{
			array<ArcContactArea@>@ src_areas = @sources[i].areas;
			
			for(uint j = 0; j < src_areas.length; j++)
			{
				areas.insertLast(@src_areas[j]);
			}
		}
		
		ArcAreas::initialise();
	}
	
}
