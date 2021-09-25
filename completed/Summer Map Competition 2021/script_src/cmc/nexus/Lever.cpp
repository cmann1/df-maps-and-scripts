#include '../../lib/enums/ColType.cpp';
#include '../../lib/easing/bounce.cpp';

namespace Lever
{
	
	const float BaseAngle = 25;
	const float OpenTransitionTime = 0.75;
	
	void trigger(BaseScript@ script, const string &in name, const int count, const float time, const int id, const string &in type='lever')
	{
		message@ msg = create_message();
		msg.set_string('type', type);
		msg.set_int('count', count);
		msg.set_float('time', time);
		msg.set_int('id', id);
		script.messages.broadcast(name, msg);
	}
	
}

class Lever : trigger_base, callback_base, IInitHandler
{
	
	[option,0:Dungeon,1:Lab] LeverType type = Dungeon;
	[angle] float rotation = 90;
	[option,0:Left,1:Side,2:Right,3:Any] AxisDir x_dir = Any;
	[option,0:Up,1:Side,2:Down,3:Any,4:UpOrSide] AxisDir y_dir = Any;
	[option,0:Both,1:Light,2:Heavy] AttackType attack = Both;
	[persist] string name = 'door1';
	[persist] int id = 0;
	[persist] int count = 1;
	[persist] float time = 2;
	[persist] bool enabled = true;
	[persist] string enable_event = '';
	[persist] bool global = false;
	
	[hidden] float open;
	
	script@ script;
	scripttrigger@ self;
	
	SpriteGroup@ base_spr;
	SpriteGroup@ handle_spr;
	SpriteRectangle bounds;
	bool in_view = true;
	
	float x, y;
	float scale = 1;
	
	float prev_open_t;
	float open_t;
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = script;
		@this.self = self;
		
		self.radius(0);
		self.editor_handle_size(5);
		update_sprite();
		update_size();
		
		script.init_subscribe(this);
		
		if(!script.is_playing)
			return;
		
		prev_open_t = open_t = open > 0 || open == -1 ? 1 : 0;
	}
	
	void init() override
	{
		if(!script.is_playing)
			return;
		
		script.messages.add_listener(name, MessageHandler(on_lever));
		
		if(enable_event != '')
		{
			script.messages.add_listener(enable_event, MessageHandler(on_enable));
		}
	}
	
	private void update_sprite()
	{
		switch(type)
		{
			case Lab:
				@base_spr = script.sprites.lever_base_lab;
				@handle_spr = script.sprites.lever_handle_lab;
				break;
			case Dungeon:
			default:
				@base_spr = script.sprites.lever_base_dungeon;
				@handle_spr = script.sprites.lever_handle_dungeon;
		}
		
		update_bounds();
	}
	
	private void update_size()
	{
		switch(attack)
		{
			case AttackType::Heavy:
				scale = 1.5;
				break;
			case AttackType::Both:
			case AttackType::Light:
			default:
				scale = 1;
		}
		
		update_bounds();
	}
	
	private void update_bounds()
	{
		const float x = self.x();
		const float y = self.y();
		bounds.left = bounds.right = x;
		bounds.top = bounds.bottom = y;
		SpriteRectangle r;
		for(uint i = 0; i < 4; i++)
		{
			r = handle_spr.get_rectangle(i * 90, scale);
			r.shift(x, y);
			bounds.union_with(@r);
		}
	}
	
	private void get_hitbox(float &out x1, float &out y1, float &out x2, float &out y2)
	{
		const float x = self.x();
		const float y = self.y();
		const float x_size = abs(rotation) >= 45 && abs(rotation) < 135 ? 34 : 24;
		const float y_size = x_size == 24 ? 34 : 24;
		const float cx = x + cos((rotation - 90) * DEG2RAD) * x_size * scale;
		const float cy = y + sin((rotation - 90) * DEG2RAD) * y_size * scale;
		x1 = cx - x_size * scale;
		y1 = cy - y_size * scale;
		x2 = cx + x_size * scale;
		y2 = cy + y_size * scale;
	}
	
	void unlock()
	{
		if(open < 0)
		{
			open = 0.01;
		}
	}
	
	void step()
	{
		prev_open_t = open_t;
		open_t = clamp(open_t + DT * (open > 0 || open < 0 ? 1 : -1), 0, Lever::OpenTransitionTime);
		
		if(!enabled && open_t >= Lever::OpenTransitionTime * 0.2)
		{
			open = 0;
		}
		
		if(open > 0)
		{
			open = max(open - DT, 0.0);
			
			if(open == 0)
			{
				audio@ sound = script.g.play_sound(
					'sfx_knight_block', self.x(), self.y(),
					rand_range(0.9, 1.0), false, true);
				sound.time_scale(rand_range(1.8, 2.0));
				
				Lever::trigger(script, name, -count, time, id);
				if(global)
				{
					script.set_global(name, false);
				}
			}
		}
		
		in_view = script.in_view(bounds.left, bounds.top, bounds.right, bounds.bottom);
		if(!in_view)
			return;
		
		float x1, y1, x2, y2;
		get_hitbox(x1, y1, x2, y2);
		int i = script.g.get_entity_collision(
			y1, y2, x1, x2,
			ColType::Hitbox);
		
		while(--i >= 0)
		{
			hitbox@ hb = script.g.get_hitbox_collision_index(i);
			if(@hb == null)
				continue;
			if(!hb.triggered() || hb .state_timer() != hb.activate_time())
				continue;
			controllable@ c = hb.owner();
			if(@c == null || c.player_index() == -1)
				continue;
			if(attack != Both && (hb.damage() == 1 && attack != Light || hb.damage() > 1 && attack != Heavy))
				continue;
			
			const int attack_dir = hb.attack_dir() - 90;
			const int x_axis = (attack_dir == -5 || attack_dir == -60 || attack_dir == 60) ? 1 : -1;
			const int y_axis = attack_dir == 60 || attack_dir == 61 || attack_dir == -240 || attack_dir == -241
				? 1
				: attack_dir == -60 || attack_dir == -120 ? -1 : 0;
			
			if(
				(x_dir == Any || x_axis == (x_dir - 1)) &&
				(y_dir == Any || y_axis == (y_dir - 1) || y_axis <= 0 && y_dir == UpOrSide)
			)
			{
				if(open == 0)
				{
					audio@ sound = script.g.play_sound(
						enabled ? 'sfx_knight_block' : 'sfx_springball_land_light',
						self.x(), self.y(),
						rand_range(0.9, 1.0), false, true);
					sound.time_scale(enabled
						? rand_range(1.3, 1.4)
						: rand_range(1.5, 1.6));
				}
				else
				{
					audio@ sound = script.g.play_sound(
						'sfx_springball_land_light', self.x(), self.y(),
						rand_range(0.9, 1.0), false, true);
					sound.time_scale(rand_range(0.9, 1.1));
					
					if(open_t == Lever::OpenTransitionTime)
					{
						open_t -= DT * 15;
					}
				}
				
				if(enabled)
				{
					Lever::trigger(script, name, open == 0 ? count : 0, time, id);
					if(global)
					{
						script.set_global(name, true);
					}
				}
				
				open = time >= 0 ? max(open, time) : -1;
			}
		}
	}
	
	void editor_var_changed(var_info@ var)
	{
		const string name = var.name;
		if(name == 'type')
		{
			update_sprite();
		}
		else if(name == 'attack')
		{
			update_size();
		}
	}
	
	void draw(float sub_frame)
	{
		if(!in_view || !script.should_draw(bounds.left, bounds.top, bounds.right, bounds.bottom))
			return;
		
		const float rotation = -shortest_angle_degrees(this.rotation - 90, 0);
		const int x_axis = abs(rotation) <= 45 ? 1 : abs(rotation) >= 135 ? -1 : 0;
		const int y_axis = rotation > 45 && rotation < 135 ? 1 : rotation < -45 && rotation > -135 ? -1 : 0;
		float base_rotation = 0;
		float open_dir = 1;
		
		if(x_axis != 0 && y_dir != Any)
		{
			base_rotation = (y_dir - 1) * -Lever::BaseAngle * x_axis;
			open_dir = (y_dir - 1) * x_axis;
		}
		else if(y_axis != 0 && x_dir != Any)
		{
			base_rotation = (x_dir - 1) * Lever::BaseAngle * y_axis;
			open_dir = (x_dir - 1) * -y_axis;
		}
		
		const float t = lerp(prev_open_t, open_t, sub_frame) / Lever::OpenTransitionTime;
		const float open_rotation = Lever::BaseAngle * open_dir * 2 * (
			open > 0 || open < 0 ? ease_out_bounce(t) : ease_in_bounce(t));
		
		handle_spr.draw(
			self.x(), self.y(),
			rotation + base_rotation + open_rotation, scale,
			0, 0, 1);
		base_spr.draw(
			self.x(), self.y(),
			rotation, scale,
			0, 0, 1);
		
		//outline_rect(script.g, 22, 24, bounds.left, bounds.top, bounds.right, bounds.bottom, 2, 0x22ffffff);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		
		float x1, y1, x2, y2;
		get_hitbox(x1, y1, x2, y2);
		outline_rect(script.g, 22, 22, x1, y1, x2, y2, 1 * script.ed_zoom, 0xaaff5533);
	}
	
	private void on_lever(const string &in id, message@ msg)
	{
		const string type = msg.get_string('type');
		
		if(type == 'door')
		{
			const float time = msg.get_float('time');
			open = time >= 0 ? max(open, time) : -1;
		}
		else if(type == 'unlock')
		{
			unlock();
		}
	}
	
	private void on_enable(const string &in, message@ msg)
	{
		enabled = msg.get_int('enabled') == 1;
	}
	
}

enum LeverType
{
	Dungeon,
	Lab,
}

enum AxisDir
{
	Left = 0,
	Side = 1,
	Right = 2,
	Any = 3,
	Up = 0,
	Down = 2,
	UpOrSide = 4,
}

enum AttackType
{
	Both,
	Light,
	Heavy,
}
