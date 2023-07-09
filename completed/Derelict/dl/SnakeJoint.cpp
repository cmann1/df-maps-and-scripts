class SnakeJoint : Particle
{
	
	float px, py;
	float dx, dy;
	float nx, ny;
	float length;
	float angle;
	
	float orientation_prev = 1;
	float orientation = 1;
	float orientation_t = 1;
	bool has_contact_prev;
	
	float scale = 1;
	
	bool in_deathzone = false;
	float in_deathzone_y;
	
	float area = 1;
	float value = 1;
	
	float light_t;
	float light_offset_x, light_offset_y;
	float light_scale;
	uint light_sub_layer;
	uint light_colour;
	
	SnakeJoint@ left;
	SnakeJoint@ right;
	DistanceConstraint@ dc1;
	DistanceConstraint@ dc2;
	
	SnakeJoint(const float x, const float y)
	{
		super(x, y);
	}
	
	void calc_values(const float p0x, const float p0y, const float sub_frame)
	{
		px = lerp(prev_x, x, sub_frame);
		py = lerp(prev_y, y, sub_frame);
		dx = px - p0x;
		dy = py - p0y;
		length = sqrt(dx * dx + dy * dy);
		nx = length != 0 ? dx / length : 0.0;
		ny = length != 0 ? dy / length : 0.0;
		angle = atan2(dy, dx) * RAD2DEG;
	}
	
	bool is_vertical()
	{
		return abs(abs(angle) - 90) <= 3;
	}
	
	bool is_mostly_vertical()
	{
		return abs(abs(angle) - 90) <= 25;
	}
	
}
