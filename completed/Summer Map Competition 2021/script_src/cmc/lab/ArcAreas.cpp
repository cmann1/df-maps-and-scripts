//interface IArcAreas
//{
//	
//	void initialise(script@ script);
//	array<ArcContactArea>@ get_areas_list();
//	
//}

class ArcAreas
{
	
	array<ArcContactArea@> areas;
	float total_size;
	
	void initialise()
	{
		total_size = 0;
		
		for(uint i = 0; i < areas.length; i++)
		{
			ArcContactArea@ area = @areas[i];
			total_size += area.size * area.weight;
		}
	}
	
	ArcContactArea@ random_point(float &out x, float &out y)
	{
		if(areas.length == 0)
			return null;
		
		const float rand_length = total_size * frand();
		float current_length = 0;
		
		for(uint i = 0; i < areas.length; i++)
		{
			ArcContactArea@ area = @areas[i];
			
			if(rand_length >= current_length && rand_length <= current_length + area.size * area.weight)
			{
				const float t = area.size * area.weight != 0 ?
					(rand_length - current_length) / (area.size * area.weight)
					: 0;
				area.get_point(t, x, y);
				return area;
			}
			
			current_length += area.size * area.weight;
		}
		
		return null;
	}
	
}
