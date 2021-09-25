class ArcEndPoint : ArcContact
{
	
	// I had to remove the annotations from fields marked with "LAB UNUSED" to try and help with
	// checkpoint lag spikes in the cmclab map
	
	[tooltip:'Non-grounded arcs will become\nthinner closer to the tip']
	bool grounded = true;
	[tooltip:'If non-zero, arcs hitting this surface will\nspawn an emitter with this id'] /* LAB UNUSED */
	int sparks_emitter_id = 0;
	[persist] /* LAB UNUSED */
	float life_multiplier = 1;
	
	protected ArcContactArea@ create_area(const int type) override
	{
		ArcContactArea@ area = ArcContact::ArcContactArea(this, type, weight);
		area.grounded = grounded;
		area.emitter_id = sparks_emitter_id;
		area.life_multiplier = life_multiplier;
		return area;
	}
	
}
