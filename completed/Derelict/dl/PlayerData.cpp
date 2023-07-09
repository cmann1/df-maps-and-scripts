class PlayerData
{
	
	DLScript@ script;
	
	int index = -1;
	camera@ cam;
	controllable@ c;
	dustman@ dm;
	float cam_speed_change;
	float cam_speed_change_reset_time;
	float x, y;
	
	float view_x1, view_y1, view_x2, view_y2;
	
	bool is_snake;
	Snake@ snake;
	PlayerSpawner@ player_spawner;
	
	private bool cp_set;
	private array<SnakeJointPersistData> cp_joint_data;
	private float cp_x, cp_y;
	
	private bool locked;
	private scriptenemy@ locked_se;
	private float combo_timer;
	
	private bool x_intent_locked, y_intent_locked;
	
	void checkpoint_save()
	{
		if(!is_snake)
			return;
		
		cp_set = true;
		cp_x = snake.self.x();
		cp_y = snake.self.y();
		cp_joint_data = snake.joint_persist_data;
	}
	
	void checkpoint_load()
	{
		@c = controller_controllable(index);
		@dm = @c != null ? c.as_dustman() : null;
		cam_speed_change = cam.speed_change();
		update_view_bounds();
	}
	
	void spawn_snake(float x, float y)
	{
		Snake@ prev_snake = snake;
		
		@snake = Snake(this);
		snake.is_active = true;
		
		if(@prev_snake != null)
		{
			snake.clean_filth = prev_snake.clean_filth;
			snake.auto_connect_main_cam = prev_snake.auto_connect_main_cam;
			snake.remove_tera_deathzones = prev_snake.remove_tera_deathzones;
			snake.compact = prev_snake.compact;
			
			snake.debug_draw = prev_snake.debug_draw;
			snake.ddraw_joints = prev_snake.ddraw_joints;
			snake.ddraw_indices = prev_snake.ddraw_indices;
			snake.ddraw_look = prev_snake.ddraw_look;
			snake.debug_draw = prev_snake.debug_draw;
		}
		
		if(cp_set)
		{
			x = cp_x;
			y = cp_y;
			snake.joint_persist_data = cp_joint_data;
		}
		else
		{
			y -= snake.radius;
		}
		
		scriptenemy@ se = create_scriptenemy(snake);
		se.set_xy(x, y);
		se.prev_x(x);
		se.prev_y(y);
	}
	
	void set_cam_speed(const float speed, const float reset_time = 2)
	{
		if(cam_speed_change == speed)
			return;
		
		cam_speed_change = speed;
		cam.speed_change(cam_speed_change);
		cam_speed_change_reset_time = reset_time;
	}
	
	void lock(const bool locked=true)
	{
		if(this.locked == locked)
			return;
		
		this.locked = locked;
		
		if(locked)
		{
			combo_timer = @dm != null ? dm.combo_timer() : -1;
			
			DummyPlayer player;
			@locked_se = create_scriptenemy(@player);
			locked_se.set_xy(c.x(), c.y());
			script.g.add_entity(locked_se.as_entity(), false);
			controller_entity(index, locked_se.as_controllable());
			
			c.x_intent(0);
			c.y_intent(0);
			c.taunt_intent(0);
			c.heavy_intent(0);
			c.light_intent(0);
			c.dash_intent(0);
			c.jump_intent(0);
			c.fall_intent(0);
		}
		else
		{
			controller_entity(index, c.as_controllable());
			script.g.remove_entity(locked_se.as_entity());
			@locked_se = null;
		}
	}
	
	void set_lock_pos(const float x, const float y)
	{
		if(@locked_se == null)
			return;
		
		locked_se.set_xy(x, y);
	}
	
	void control_snake(Snake@ snake)
	{
		if(is_snake || @dm == null)
			return;
		
		PlayerSpawner spawner(script.g, dm);
		
		// Pre existing snakes/script enemies will be persisted which will create copies when spawning player controlled snakes.
		// Make a non-persisted copy instead and take control of that.
		// Removing then adding the same snake on the same frame caused it to be stepped twice per frame.
		// And removing then adding on the next from causes either the snake to disappear for a frame, or
		// a checkpoint to load if controller_entity is called this from since the player is not in the scene.
		
		script.g.remove_entity(snake.self.as_entity());
		
		@snake = snake.make_copy();
		@this.snake = snake;
		is_snake = true;
		snake.take_control(this);
		script.g.add_entity(snake.self.as_entity(), false);
		controller_entity(index, snake.self.as_controllable());
		
		// Increase the break distance because the built in camera offset for dustman is -48 pixels.
		// which means that if the snake is still on screen but touching the top of the screen
		// the camera can break.
		cam.break_distance(1.25);
	}
	
	Snake@ control_dustman()
	{
		if(!is_snake || @player_spawner == null)
			return snake;
		
		snake.release_control();
		Snake@ old_snake = @snake;
		@snake = snake.make_copy();
		snake.activate();
		
		is_snake = false;
		script.g.add_entity(snake.self.as_entity(), true);
		script.g.remove_entity(old_snake.self.as_entity());
		
		player_spawner.take_control();
		@c = controller_controllable(index);
		@dm = c.as_dustman();
		
		cam.break_distance(1);
		
		return snake;
	}
	
	void kill()
	{
		if(@locked_se != null)
		{
			controller_entity(index, c);
			if(@dm != null)
			{
				dm.kill(true);
			}
		}
	}
	
	void step()
	{
		if(locked)
		{
			if(@dm != null && combo_timer > 0)
			{
				dm.combo_timer(combo_timer);
			}
		}
		
		if(cam_speed_change < 1)
		{
			cam_speed_change = min(cam_speed_change + DT / cam_speed_change_reset_time, 1.0);
			cam.speed_change(cam_speed_change);
		}
		
		update_view_bounds();
		
		if(x_intent_locked || y_intent_locked)
		{
			controllable@ c = get_controller();
			if(x_intent_locked && c.x_intent() == 0)
			{
				x_intent_locked = false;
			}
			if(y_intent_locked && c.y_intent() == 0)
			{
				y_intent_locked = false;
			}
		}
		
		c.centre(x, y);
		
		// Auto connect to the nearest camera when in the main map area.
		// Shouldn't need to but just in case the player needs to go backwards possibly for apple/secret stuff.
		if(should_reconnect())
		{
			const float cy = (MapBoundsY1 + MapBoundsY2) * 0.5;
			
			camera_node@ closest_node = null;
			float closest_dist = 10000;
			
			int i = script.g.get_entity_collision(cy - 50, cy + 50, x - 1200, x + 1200, ColType::CameraNode);
			while(--i >= 0)
			{
				camera_node@ nnode = script.g.get_entity_collision_index(i).as_camera_node();
				if(@nnode == null)
					continue;
				
				const float dist = abs(x - nnode.x());
				if(dist <= closest_dist)
				{
					@closest_node = nnode;
					closest_dist = dist;
				}
			}
			
			if(@closest_node != null)
			{
				cam.try_connect(closest_node);
			}
		}
	}
	
	private bool should_reconnect()
	{
		if(!script.in_map_map_bounds(x, y))
			return false;
		if(cam.camera_type() == 'player')
			return true;
		
		return x < view_x1 || x > view_x2 || y < view_y1 || y > view_y2;
	}
	
	bool check_action()
	{
		bool do_action = false;
		
		controllable@ c = get_controller();
		
		if(c.taunt_intent() == 1)
		{
			c.taunt_intent(0);
			do_action = true;
		}
		if(c.jump_intent() == 1)
		{
			c.jump_intent(2);
			do_action = true;
		}
		if(c.heavy_intent() > 0 && c.heavy_intent() <= 10)
		{
			c.heavy_intent(11);
			do_action = true;
		}
		if(c.light_intent() > 0 && c.light_intent() <= 10)
		{
			c.light_intent(11);
			do_action = true;
		}
		
		return do_action;
	}
	
	int check_y_intent(const bool lock_until_release=true)
	{
		controllable@ c = get_controller();
		
		const int y = lock_until_release && y_intent_locked ? 0 : c.y_intent();
		
		if(lock_until_release && y != 0)
		{
			y_intent_locked = true;
		}
		
		return y;
	}
	
	int check_x_intent(const bool lock_until_release=true)
	{
		controllable@ c = get_controller();
		
		const int x = lock_until_release && x_intent_locked ? 0 : c.x_intent();
		
		if(lock_until_release && x != 0)
		{
			x_intent_locked = true;
		}
		
		return x;
	}
	
	private controllable@ get_controller()
	{
		return @locked_se != null ? locked_se.as_controllable() : c;
	}
	
	private void update_view_bounds()
	{
		float v1_x, v1_y, v1_w, v1_h;
		float v2_x, v2_y, v2_w, v2_h;
		cam.get_layer_draw_rect(0, 19, v1_x, v1_y, v1_w, v1_h);
		cam.get_layer_draw_rect(1, 19, v2_x, v2_y, v2_w, v2_h);
		view_x1 = min(v1_x, v2_x);
		view_y1 = min(v1_y, v2_y);
		view_x2 = max(v1_x + v1_w, v2_x + v2_w);
		view_y2 = max(v1_y + v1_h, v2_y + v2_h);
	}
	
}

class DummyPlayer : enemy_base
{
	
	DLScript@ script;
	scriptenemy@ self;
	
	void init(script@ s, scriptenemy@ self)
	{
		@script = s;
		@this.self = self;
		
		// Just for testing - adding and then removing on the same frame
		// leaves the DummyPlayer in the scene.
		if(!script.is_playing)
		{
			script.g.remove_entity(self.as_entity());
		}
	}
	
	void on_add()
	{
		self.base_collision().collision_type(32);
		self.hit_collision().collision_type(32);
	}
	
}
