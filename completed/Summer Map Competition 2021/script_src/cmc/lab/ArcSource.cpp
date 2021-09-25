class ArcSource : ArcContact
{
	
	// I had to remove the annotations from fields marked with "LAB UNUSED" to try and help with
	// checkpoint lag spikes in the cmclab map
	
	[persist] /* LAB UNUSED */
	float freq_min = 0.5;
	[persist] /* LAB UNUSED */
	float freq_max = 0.75;
	[persist] int layer = 12;
	[persist] int sub_layer_min = 17;
	[persist] int sub_layer_max = -1;
	[persist] /* LAB UNUSED */
	float life_min = 0.5;
	[persist] /* LAB UNUSED */
	float life_max = 1;
	[persist] /* LAB UNUSED */
	float resolution = 24;
	[persist] /* LAB UNUSED */
	float thickness_min = 3;
	[persist] /* LAB UNUSED */
	float thickness_max = 4;
	[persist] /* LAB UNUSED */
	int branches = 3;
	[persist] /* LAB UNUSED */
	float spark_volume = 0;
	[colour,alpha] /* LAB UNUSED */
	uint colour1 = 0xffffffff;
	[colour,alpha] /* LAB UNUSED */
	uint colour2 = 0xffffffff;
	
	float charge;
	
	void initialise(script@ script) override
	{
		ArcContact::initialise(script);
		
		charge = rand_range(freq_min, freq_max);
	}
	
	bool try_spawn_arc()
	{
		charge -= DT;
		
		if(charge <= 0)
		{
			charge = rand_range(freq_min, freq_max);
			return true;
		}
		
		return false;
	}
	
	void prime()
	{
		charge = 0;
	}
	
}
