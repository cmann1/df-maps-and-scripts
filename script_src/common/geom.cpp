class Box
{
	
	float left;
	float top;
	float bottom;
	float right;
	
	Box(float left=0, float top=0, float right=0, float bottom=0)
	{
		this.left = left;
		this.top = top;
		this.bottom = bottom;
		this.right = right;
	}
	
	void set(float left, float top, float right, float bottom)
	{
		this.left = left;
		this.top = top;
		this.bottom = bottom;
		this.right = right;
	}
	
	bool is_inside(float x, float y, float position_x, float position_y)
	{
		return x >= position_x + left and x <= position_x + right and y >= position_y + top and y <= position_y + bottom;
	}
	
}