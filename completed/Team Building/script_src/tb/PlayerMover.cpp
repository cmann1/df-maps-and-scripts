class PlayerMover : trigger_base
{
	
	scripttrigger@ self;
	
	controllable@ player = null;
	camera@ cam;
	int player_index = 0;
	float move_x = 0;
	float move_y = 0;
	float x = 0;
	float y = 0;
	float cam_x = 0;
	float cam_y = 0;
	float max_move = 16 * 48;
	
	PlayerMover()
	{
		
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.self = @self;
	}
	
	void set(controllable@ player, int player_index, float x, float y, float cam_x, float cam_y)
	{
		@this.player = player;
		this.player_index = player_index;
		this.x = x;
		this.y = y;
		this.cam_x = cam_x;
		this.cam_y = cam_y;
		move_x = x - player.x();
		move_y = y - player.y();
		const float dist = magnitude(move_x, move_y);
		move_x = move_x / dist * max_move;
		move_y = move_y / dist * max_move;
		
		@cam = get_camera(player_index);
	}
	
	void step()
	{
		const float dx = self.x() - x;
		const float dy = self.y() - y;
		const float dist = dx * dx + dy * dy;
		
		player.set_xy(self.x() + move_x, self.y() + move_y);
		self.set_xy(self.x() + move_x, self.y() + move_y);
		cam.prev_x(player.x());
		cam.prev_y(player.y());
		cam.x(player.x());
		cam.y(player.y());
		
		player.set_speed_xy(0, 0);
		player.light_intent(0);
		player.heavy_intent(0);
		player.jump_intent(0);
		player.dash_intent(0);
		player.x_intent(0);
		player.y_intent(0);
		player.taunt_intent(0);
		player.fall_intent(0);
		
		if(dist <= max_move * max_move)
		{
			player.set_xy(x, y);
			player.time_warp(1);
			cam.prev_x(cam_x);
			cam.prev_y(cam_y);
			cam.x(cam_x);
			cam.y(cam_y);
			get_scene().remove_entity(self.as_entity());
		}
	}
	
}