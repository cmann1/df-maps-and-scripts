class ArcContactArea
{
	
	ArcContact@ contact;
	int type;
	float x1, y1;
	float x2, y2;
	float size;
	float weight = 1;
	float life_multiplier = 1;
	bool grounded = true;
	int emitter_id = 0;
	
	ArcContactArea() {}
	
	ArcContactArea(ArcContact@ contact, const int type, const float weight=1)
	{
		@this.contact = contact;
		this.type = type;
		this.weight = weight;
	}
	
	void get_point(const float t, float &out x, float &out y)
	{
		switch(type)
		{
			case ArcContact::Point:
			{
				const float angle = rand_range(-PI, PI);
				const float dist = sqrt(frand()) * x2;
				x = x1 + cos(angle) * dist;
				y = y1 + sin(angle) * dist;
				return;
			}
			case ArcContact::Line:
			case ArcContact::Tiles:
			{
				x = x1 + (x2 - x1) * t;
				y = y1 + (y2 - y1) * t;
				return;
			}
			case ArcContact::Rect:
			{
				x = x1 + (x2 - x1) * frand();
				y = y1 + (y2 - y1) * frand();
				return;
			}
		}
	}
	
}
