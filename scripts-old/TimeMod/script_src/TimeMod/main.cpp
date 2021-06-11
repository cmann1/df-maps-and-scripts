const int MAX_PLAYERS = 4;
const int MAX_RECORDED_FRAMES = 60 * 4;

class script
{
	
	array<Player> players(MAX_PLAYERS);
	
	script()
	{
		for(int i = 0; i < MAX_PLAYERS; i++)
		{
			players[i].player_index = i;
		}
	}
	
	void checkpoint_load()
	{
		for(int i = 0; i < MAX_PLAYERS; i++)
		{
			players[i].checkpoint_load();
		}
	}
	
	void step(int entities)
	{
		for(int player_index = 0; player_index < MAX_PLAYERS; player_index++)
		{
			Player@ player = players[player_index];
			
			if(@player.self == null)
			{
				entity@e = controller_entity(player_index);
				if(@e != null)
				{
					player.init(e);
				}
			}
			else
			{
				player.step(entities);
			}
		}
	}
	
}

class Player
{
	
	scene@ g;
	dustman@ self = null;
	int player_index = -1;
	
	State@ first_state = null;
	State@ last_state = null;
	int num_frames = 0;
	bool rewind = false;
	
	Player()
	{
		@g = get_scene();
	}
	
	void init(entity@ dm)
	{
		@self = dm.as_dustman();
		
	}
	
	void checkpoint_load()
	{
		@first_state = null;
		@last_state = null;
		num_frames = 0;
	}
	
	void step(int entities)
	{
		if(self.taunt_intent() != 0)
		{
			rewind = true;
		}
		
		if(rewind)
		{
			if(@last_state != null)
			{
				last_state.restore(self);
				@last_state = last_state.prev;
				if(@last_state != null) @last_state.next = null;
				else @first_state = null;
				num_frames--;
			}
			else
			{
				rewind = false;
			}
		}
		else
		{
			State@ new_state = State(self);
		
			if(@last_state != null)
			{
				@last_state.next = new_state;
				@new_state.prev = last_state;
				@last_state = new_state;
			}
			else
			{
				@first_state = new_state;
				@last_state = new_state;
			}
			
			if(++num_frames == MAX_RECORDED_FRAMES)
			{
				@first_state = first_state.next;
				@first_state.prev = null;
				num_frames--;
			}
		}
	}
	
}

class State
{
	
	State@ prev = null;
	State@ next = null;
	
	float x;
	float y;
	float rotation;
	int face;
	
	float x_speed;
	float y_speed;
	float scale;
	int state;
	string sprite_index;
	string attack_sprite_index;
	float state_timer;
	float stun_timer;
	int attack_state;
	float attack_timer;
	
	int x_intent;
	int y_intent;
//	int taunt_intent;
	int heavy_intent;
	int light_intent;
	int dash_intent;
	int jump_intent;
	int fall_intent;
	
	int skill_combo;
	int dash;
	string character;
	bool dead;
	
	State(dustman@ player)
	{
		x = player.x();
		y = player.y();
		rotation = player.rotation();
		face = player.face();
		
		x_speed = player.x_speed();
		y_speed = player.y_speed();
		scale = player.scale();
		state = player.state();
		sprite_index = player.sprite_index();
		attack_sprite_index = player.attack_sprite_index();
		state_timer = player.state_timer();
		stun_timer = player.stun_timer();
		attack_state = player.attack_state();
		attack_timer = player.attack_timer();
		
		x_intent = player.x_intent();
		y_intent = player.y_intent();
//		taunt_intent = player.taunt_intent();
		heavy_intent = player.heavy_intent();
		light_intent = player.light_intent();
		dash_intent = player.dash_intent();
		jump_intent = player.jump_intent();
		fall_intent = player.fall_intent();
		
		dash = player.dash();
		character = player.character();
		dead = player.dead();
	}

	void restore(dustman@ player)
	{
		player.x(x);
		player.y(y);
		player.rotation(rotation);
		player.face(face);
		
		player.set_speed_xy(x_speed, y_speed);
		player.scale(scale);
		player.state(state);
		player.sprite_index(sprite_index);
		player.attack_sprite_index(attack_sprite_index);
		player.state_timer(state_timer);
		player.stun_timer(stun_timer);
		player.attack_state(attack_state);
		player.attack_timer(attack_timer);
		
		player.x_intent(x_intent);
		player.y_intent(y_intent);
//		player.taunt_intent(taunt_intent);
		player.heavy_intent(heavy_intent);
		player.light_intent(light_intent);
		player.dash_intent(dash_intent);
		player.jump_intent(jump_intent);
		player.fall_intent(fall_intent);
		
		player.dash(dash);
		player.character(character);
		player.dead(dead);
	}
	
}