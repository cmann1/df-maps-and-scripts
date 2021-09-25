class Player
{
	
	BaseScript@ script;
	int index;
	camera@ cam;
	
	float x1, y1, x2, y2;
	
	void init(BaseScript@ script, const int index)
	{
		@this.script = script;
		this.index = index;
		@this.cam = get_camera(index);
	}
	
	void step_post()
	{
		float view1_x, view1_y, view1_w, view1_h;
		float view2_x, view2_y, view2_w, view2_h;
		cam.get_layer_draw_rect(0, 19, view1_x, view1_y, view1_w, view1_h);
		cam.get_layer_draw_rect(1, 19, view2_x, view2_y, view2_w, view2_h);
		x1 = min(view1_x, view2_x) - 50;
		y1 = min(view1_y, view2_y) - 50;
		x2 = max(view1_x + view1_w, view2_x + view2_w) + 50;
		y2 = max(view1_y + view1_h, view2_y + view2_h) + 50;
	}
	
	void draw(float sub_frame)
	{
		outline_rect(script.g, 22, 22, x1 + 60, y1 + 60, x2 - 60, y2 - 60, 2, 0xffff0000);
	}
	
}
