class Location
{
	
	[position,mode:world,layer:19,y:y] float x;
	[hidden] float y;
	
	Location() {}
	
	Location(const float x, const float y)
	{
		this.x = x;
		this.y = y;
	}
	
}
