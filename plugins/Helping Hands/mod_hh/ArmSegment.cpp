class ArmSegment
{
	
	sprite_group@ spr;
	
	float prev_x = 0;
	float prev_y = 0;
	float x = 0;
	float y = 0;
	float length;
	float prev_rotation;
	float rotation;
	
	ArmSegment(sprite_group@ spr, float length=100)
	{
		@this.spr = spr;
		this.length = length;
	}
	
}