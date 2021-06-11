enum MainDoorState
{
	Idle,
	Unlock1,
	Unlock2,
	Unlock3,
	StartGears,
	Opening,
	Waiting,
	OpenDelay,
}

class MainDoor : trigger_base, callback_base
{
	
	script@ script;
	scene@ g;
	scripttrigger@ self;
	
	MainDoorState state = MainDoorState::Idle;
	float state_timer = 0;
	float state_timer_max = 0;
	
	[position,mode:world,layer:19,y:gear_top_1_y] float gear_top_1_x;
	[hidden] float gear_top_1_y;
	[position,mode:world,layer:19,y:gear_top_2_y] float gear_top_2_x;
	[hidden] float gear_top_2_y;
	[position,mode:world,layer:19,y:gear_bottom_1_y] float gear_bottom_1_x;
	[hidden] float gear_bottom_1_y;
	[position,mode:world,layer:19,y:gear_bottom_2_y] float gear_bottom_2_x;
	[hidden] float gear_bottom_2_y;
	
	[position,mode:world,layer:19,y:locks_left_y1] float locks_left_x1;
	[hidden] float locks_left_y1;
	[position,mode:world,layer:19,y:locks_left_y2] float locks_left_x2;
	[hidden] float locks_left_y2;
	[position,mode:world,layer:19,y:locks_left_y3] float locks_left_x3;
	[hidden] float locks_left_y3;
	[position,mode:world,layer:19,y:locks_right_y1] float locks_right_x1;
	[hidden] float locks_right_y1;
	[position,mode:world,layer:19,y:locks_right_y2] float locks_right_x2;
	[hidden] float locks_right_y2;
	[position,mode:world,layer:19,y:locks_right_y3] float locks_right_x3;
	[hidden] float locks_right_y3;
	
	[position,mode:world,layer:19,y:door_y1] float door_x1;
	[hidden] float door_y1;
	[position,mode:world,layer:19,y:door_y2] float door_x2;
	[hidden] float door_y2;
	
	Prop@ gear_top_1;
	Prop@ gear_top_2;
	Prop@ gear_bottom_1;
	Prop@ gear_bottom_2;
	
	array<Prop@> locks_left(3);
	array<Prop@> locks_right(3);
	
	int open_left, open_right;
	int open_top, open_bottom;
	float open_current_y;
	float open_speed = 5;
	
	float lock_speed = 1;
	float lock_state_duration = 35;
	
	MainDoor()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		if(script.in_game)
		{
			add_broadcast_receiver('main_door_unlock', this, 'on_unlock_start');
//			on_unlock_start('', null);
		}
	}
	
	void on_unlock_start(string id, message@ msg)
	{
		@locks_left[0] = get_prop_at(locks_left_x1, locks_left_y1, 2, 23, 3);
		@locks_left[1] = get_prop_at(locks_left_x2, locks_left_y2, 2, 23, 3);
		@locks_left[2] = get_prop_at(locks_left_x3, locks_left_y3, 2, 23, 3);
		@locks_right[0] = get_prop_at(locks_right_x1, locks_right_y1, 2, 23, 3);
		@locks_right[1] = get_prop_at(locks_right_x2, locks_right_y2, 2, 23, 3);
		@locks_right[2] = get_prop_at(locks_right_x3, locks_right_y3, 2, 23, 3);
		
		state = MainDoorState::Unlock1;
		state_timer = 0;
		state_timer_max = lock_state_duration;
	}
	
	Prop@ get_prop_at(float x, float y, uint set, uint group, uint index)
	{
		int count = g.get_prop_collision(y - 1, y + 1, x - 1, x + 1);
		for(int i = 0; i < count; i++)
		{
			prop@ p = g.get_prop_collision_index(i);
			if(p.prop_set() == set and p.prop_group() == group && p.prop_index() == index)
			{
				return Prop(p);
			}
		}
		
		return null;
	}
	
	void create_puff(float x , float y, int w, int h, int time=1, int layer=16, int sub_layer=12)
	{
		entity@ emitter = create_emitter(CleansedFb1, x, y, w, h, layer, sub_layer);
		g.add_entity(emitter);
		remove_timer(emitter, time);
	}
	
	void rotate_gears()
	{
		if(@gear_top_1 != null)
			gear_top_1.rotation(gear_top_1.p.rotation() - 1);
		if(@gear_top_2 != null)
			gear_top_2.rotation(gear_top_2.p.rotation() + 1);
		if(@gear_bottom_1 != null)
			gear_bottom_1.rotation(gear_bottom_1.p.rotation() - 1);
		if(@gear_bottom_2 != null)
			gear_bottom_2.rotation(gear_bottom_2.p.rotation() + 1);
	}
	
	void step()
	{
		switch(state)
		{
			case MainDoorState::Unlock1:
			case MainDoorState::Unlock2:
			case MainDoorState::Unlock3:
			{
				if(state_timer == 15)
				{
					g.play_sound('sfx_door_unlock', self.x(), self.y(), 1, false, true);
				}
				
				if(state_timer > 10)
				{
					Prop@ p_l = @locks_left[state - 1];
					Prop@ p_r = @locks_right[state - 1];
					p_l.p.x(p_l.p.x() - lock_speed);
					p_r.p.x(p_r.p.x() + lock_speed);
				}
				
				if(state_timer >= state_timer_max)
				{
					if(state == MainDoorState::Unlock3)
					{
						state = MainDoorState::StartGears;
						state_timer = -1;
						state_timer_max = 30;
					}
					else
					{
						state++;
						state_timer = -1;
						state_timer_max = lock_state_duration;
					}
				}
			} break;
			
			case MainDoorState::StartGears:
			{
				if(state_timer == 0)
				{
					@gear_top_1 = get_prop_at(gear_top_1_x, gear_top_1_y, 3, 27, 10);
					@gear_top_2 = get_prop_at(gear_top_2_x, gear_top_2_y, 3, 27, 10);
					@gear_bottom_1 = get_prop_at(gear_bottom_1_x, gear_bottom_1_y, 3, 27, 11);
					@gear_bottom_2 = get_prop_at(gear_bottom_2_x, gear_bottom_2_y, 3, 27, 11);
					create_puff(gear_top_1_x, gear_top_1_y, 48 * 3, 48 * 3);
					create_puff(gear_top_2_x, gear_top_2_y, 48 * 3, 48 * 3);
					create_puff(gear_bottom_1_x, gear_bottom_1_y, 48 * 3, 48 * 3);
					create_puff(gear_bottom_2_x, gear_bottom_2_y, 48 * 3, 48 * 3);
				}
				
				rotate_gears();
				
				if(state_timer >= state_timer_max)
				{
					state = MainDoorState::Opening;
					state_timer = -1;
					state_timer_max = 999999;
					open_top = int(floor(door_y1 / 48));
					open_bottom = int(floor(door_y2 / 48));
					open_left = int(floor(door_x1 / 48));
					open_right = int(floor(door_x2 / 48));
					open_current_y = door_y2;
				}
			} break;
			
			case MainDoorState::Opening:
			{
				bool is_solid = false;
				const int y = int(floor(open_current_y / 48));
				
				for(int x = open_left; x <= open_right; x++)
				{
					tileinfo@ tile = g.get_tile(x, y);
					if(tile.solid())
					{
						g.set_tile(x, y, 19, false, 0, 0, 0, 0);
						is_solid = true;
					}
				}
				
				if(is_solid)
				{
					create_puff((door_x1 + door_x2) * 0.5, y * 48 + 24, int(door_x2 - door_x1) + 48 * 3, 48);
				}
				
				rotate_gears();
				
				if(open_current_y <= door_y1)
				{
					state = MainDoorState::Waiting;
					create_puff((door_x1 + door_x2) * 0.5, door_y1, int(door_x2 - door_x1) + 48 * 3, 48);
					g.play_sound('sfx_door_stone_open', self.x(), self.y(), 1, false, true);
					g.play_sound('sfx_door_stone_open', self.x(), self.y(), 1, false, true);
					state_timer = 0;
					state_timer_max = 120;
				}
				open_current_y -= open_speed;
				
			} break;
			
			case MainDoorState::Waiting:
			{
				if(state_timer >= state_timer_max)
				{
					broadcast_message('main_door_open_complete', create_message());
					state = MainDoorState::OpenDelay;
					state_timer = 0;
					state_timer_max = 20;
				}
			} break;
			
			case MainDoorState::OpenDelay:
			{
				if(state_timer >= state_timer_max)
				{
					script.vars.set_bool('main_door_open', true);
					g.remove_entity(self.as_entity());
				}
			} break;
		}
		
		state_timer++;
	}
	
	void editor_draw(float sub_frame)
	{
		outline_rect(g, door_x1, door_y1, door_x2, door_y2, 22, 22, 2, 0x88FF0000);
		
		draw_dot(g, 22, 22, locks_left_x1, locks_left_y1, 5, 0xAA0000FF);
		draw_dot(g, 22, 22, locks_left_x2, locks_left_y2, 5, 0xAA0000FF);
		draw_dot(g, 22, 22, locks_left_x3, locks_left_y3, 5, 0xAA0000FF);
		draw_dot(g, 22, 22, locks_right_x1, locks_right_y1, 5, 0xAA0000FF);
		draw_dot(g, 22, 22, locks_right_x2, locks_right_y2, 5, 0xAA0000FF);
		draw_dot(g, 22, 22, locks_right_x3, locks_right_y3, 5, 0xAA0000FF);
		
		draw_dot(g, 22, 22, gear_top_1_x, gear_top_1_y, 5, 0x8800FF00);
		draw_dot(g, 22, 22, gear_top_2_x, gear_top_2_y, 5, 0x8800FF00);
		draw_dot(g, 22, 22, gear_bottom_1_x, gear_bottom_1_y, 5, 0x88FF0000);
		draw_dot(g, 22, 22, gear_bottom_2_x, gear_bottom_2_y, 5, 0x88FF0000);
	}
	
	
}