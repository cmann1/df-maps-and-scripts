void draw_dust_arrow(scene@ g, sprites@ spr, const string sprite_name,
	const float x1, const float y1,
	const float x2, const float y2,
	const float view_x, const float view_y,
	const float view_x1, const float view_y1,
	const float view_x2, const float view_y2)
{
	if(x1 <= view_x2 && x2 >= view_x1 && y1 <= view_y2 && y2 >= view_y1)
		return;
	
	float x, y, t;
	const float mid_x = (x1 + x2) * 0.5;
	const float mid_y = (y1 + y2) * 0.5;

	if(line_rectangle_intersection(view_x, view_y, mid_x, mid_y, view_x1, view_y1, view_x2, view_y2, x, y, t))
	{
		const float dx = mid_x - view_x;
		const float dy = mid_y - view_y;
		const float arrow_size = map_clamped(magnitude(dx, dy), ARROW_DISTANCE_MIN, ARROW_DISTANCE_MAX, ARROW_SCALE_MIN, ARROW_SCALE_MAX);
		spr.draw_world(22, 24, sprite_name, 0, 0, x, y, atan2(dy, dx) * RAD2DEG, arrow_size, arrow_size, 0xffffffff);
	}
}