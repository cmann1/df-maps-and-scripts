class TileRegion
{
	
	float x1, y1, x2, y2;
	int tx1, ty1, tx2, ty2;
	
	void from_area(const float x1, const float y1, const float x2, const float y2)
	{
		this.x1 = x1;
		this.y1 = y1;
		this.x2 = x2;
		this.y2 = y2;
		tx1 = int(floor(x1 / 48.0));
		ty1 = int(floor(y1 / 48.0));
		tx2 = int(floor(x2 / 48.0));
		ty2 = int(floor(y2 / 48.0));
	}
	
	void expand_tiles(const int amount)
	{
		expand_tiles(amount, amount, amount, amount);
	}
	
	void expand_tiles(const int left, const int top, const int right, const int bottom)
	{
		tx1 -= left;
		ty1 -= top;
		tx2 += right;
		ty2 += bottom;
		x1 -= left * 48;
		y1 -= top * 48;
		x2 += right * 48;
		y2 += bottom * 48;
	}
	
}
