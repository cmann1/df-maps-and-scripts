#include '../../lib/easing/cubic.cpp';
#include '../../lib/easing/quad.cpp';
#include '../../lib/enums/ColType.cpp';
#include '../../lib/enums/EmitterId.cpp';

namespace Elevator
{
	
	const float OriginX = -3 * 48 - 1;
	const float OriginY = 0;
	
	const float L = -(8 * 48 + 10);
	const float R = 2 * 48 + 10;
	const float T = -4 * 48;
	const float B = 3 * 48;
	const float T0 = T - 30;
	const float B0 = B + 40;
	const float T1 = T + 48;
	const float L0 = L - 40;
	const float R0 = R + 40;
	const float L1 = L + 48;
	const float R1 = R - 48;
	
	const int DoorSegments = 9;
	const float DoorSpeed = 0.42;
	
	const float JerkAcc = 600;
	const float JerkDist = 6;
	const float Acc = 150;
	const float MaxSpeed = 400;
	const float BrakeDist = 200;
	const int PlayerWaitFrames = 120;
	
	const int GearSmlLayer = 13;
	const int GearSmlSubLayer = 17;
	const int GearLrgLayer = 17;
	const int GearLrgSubLayer = 15;
	const float GearSmlX = 3 * 48 + 15;
	const float GearSmlY = -3 * 48 - 22;
	const float GearLrgX = 5 * 48 - 5;
	const float GearLrgY = -6 * 48;
	const float GearMidX = 0;
	const float GearMidY = -5 * 48 - 15;
	
	const float ScreenShakeForce = 7;
	const float ScreenShakeInterval = 3;
	
}

enum ElevatorState
{
	Closed,
	Open,
	Closing,
	MovingUp,
	MovingDown,
	Stopping,
}

class Elevator : enemy_base, callback_base, ICollisionEntity, IPreStepHandler, IInitHandler
{
	
	//[position,mode:world,layer:19,y:test_y1] float test_x1;
	//[hidden] float test_y1;
	
	[persist] array<GearSet> gears;
	[persist] int top = 20;
	[persist] int count = 2;
	
	[hidden] ElevatorState state = Closed;
	[hidden] float speed;
	[hidden] float y;
	[hidden] float x1;
	[hidden] float y1;
	[hidden] float door_open_t = 0;
	[hidden] int has_player;
	
	script@ script;
	scriptenemy@ self;
	
	array<Line> base_edges(14);
	Line@ door_left = @base_edges[0];
	Line@ door_right = @base_edges[1];
	Line@ door_outer_left = @base_edges[2];
	Line@ door_outer_right = @base_edges[3];
	array<CollisionEdge> edges(14);
	
	float door_t_delta;
	float door_real_t;
	float y_delta;
	audio@ elevator_snd;
	float gear_small_rot;
	float gear_big_rot;
	float gear_mid_rot;
	
	Sprite door_spr('props4', 'machinery_9', 0.5, 0.05);
	Sprite gear_big_spr('props3', 'sidewalk_10', 0.5, 0.5);
	Sprite gear_small_spr('props3', 'sidewalk_11', 0.505, 0.52);
	
	SpriteRectangle bounds;
	
	private bool enabled { get { return count == 0; } }
	
	private float centre_x { get { return self.x() + Elevator::OriginX; } }
	private float centre_y { get { return self.y() + Elevator::OriginY; } }
	
	void init(script@ script, scriptenemy@ self)
	{
		@this.self = self;
		@this.script = script;
		
		init_edges();
		
		self.auto_physics(false);
		const float t = -35;
		const float b = 20;
		const float l = 0;
		const float r = 30;
		self.base_rectangle(t, b, l, r);
		self.hit_rectangle(t, b, l, r);
		
		script.init_subscribe(this);
		
		if(!script.is_playing)
			return;
		
		for(uint i = 0; i < edges.length; i++)
		{
			CollisionEdge@ edge = @edges[i];
			@edge.entity = this;
			edge.surface_type = 'metal';
		}
		
		self.on_hurt_callback(this, 'on_hurt', 0);
		
		update_door_real_t();
		update_collision_edges();
		
		if(x1 == 0 && y1 == 0)
		{
			x1 = self.x();
			y1 = self.y();
		}
	}
	
	void init() override
	{
		bounds.left = -530;
		bounds.top = -450;
		bounds.right = 230;
		bounds.bottom= 250;
		
		if(!script.is_playing)
			return;
		
		script.pre_step_subscribe(this);
		script.messages.add_listener('elevator', MessageHandler(on_message));
		
		for(uint i = 0; i < edges.length; i++)
		{
			script.collision_manager.add_collision_edge(@edges[i]);
		}
	}
	
	private void init_edges()
	{
		door_left.x1 = door_left.x2 = Elevator::L;
		door_left.y1 = Elevator::T1;
		door_left.y2 = Elevator::B;
		door_right.x1 = door_right.x2 = Elevator::R;
		door_right.y1 = Elevator::B;
		door_right.y2 = Elevator::T1;
		
		door_outer_left.x1 = door_outer_left.x2 = Elevator::L0;
		door_outer_left.y1 = Elevator::B;
		door_outer_left.y2 = Elevator::T0;
		door_outer_right.x1 = door_outer_right.x2 = Elevator::R0;
		door_outer_right.y1 = Elevator::T0;
		door_outer_right.y2 = Elevator::B;
		
		Line@ top = @base_edges[4];
		top.x1 = Elevator::R1;
		top.x2 = Elevator::L1;
		top.y1 = top.y2 = Elevator::T;
		
		Line@ bottom = @base_edges[5];
		bottom.x1 = Elevator::L0;
		bottom.x2 = Elevator::R0;
		bottom.y1 = bottom.y2 = Elevator::B;
		
		Line@ tl = @base_edges[6];
		tl.x1 = Elevator::L1;
		tl.x2 = Elevator::L;
		tl.y1 = Elevator::T;
		tl.y2 = Elevator::T1;
		
		Line@ tr = @base_edges[7];
		tr.x1 = Elevator::R;
		tr.x2 = Elevator::R1;
		tr.y1 = Elevator::T1;
		tr.y2 = Elevator::T;
		
		Line@ door_tl = @base_edges[8];
		door_tl.x1 = Elevator::L;
		door_tl.x2 = Elevator::L0;
		door_tl.y1 = door_tl.y2 = Elevator::T1 + 48;
		
		Line@ door_tr = @base_edges[9];
		door_tr.x1 = Elevator::R0;
		door_tr.x2 = Elevator::R;
		door_tr.y1 = door_tr.y2 = Elevator::T1 + 48;
		
		Line@ roof = @base_edges[10];
		roof.x1 = Elevator::L0;
		roof.x2 = Elevator::R0;
		roof.y1 = roof.y2 = Elevator::T0;
		
		Line@ floor = @base_edges[11];
		floor.x1 = Elevator::R0;
		floor.x2 = Elevator::L0;
		floor.y1 = floor.y2 = Elevator::B0;
		
		Line@ floor_l = @base_edges[12];
		floor_l.x1 = floor_l.x2 = Elevator::L0;
		floor_l.y1 = Elevator::B0;
		floor_l.y2 = Elevator::B;
		
		Line@ floor_r = @base_edges[13];
		floor_r.x1 = floor_r.x2 = Elevator::R0;
		floor_r.y1 = Elevator::B;
		floor_r.y2 = Elevator::B0;
	}
	
	private void update_door_real_t()
	{
		const float prev = door_real_t;
		door_real_t = ease_in_out_cubic(door_open_t);
		door_t_delta = abs(door_real_t - prev);
	}
	
	void on_remove()
	{
		if(@elevator_snd != null)
		{
			elevator_snd.stop();
			@elevator_snd = null;
		}
		
		if(!script.is_playing)
			return;
		
		for(uint i = 0; i < edges.length; i++)
		{
			CollisionEdge@ edge = @edges[i];
			script.collision_manager.remove_collision_edge(edge);
			edge.clear();
		}
		
		script.pre_step_unsubscribe(this);
	}
	
	void pre_step() override
	{
		update_collision_edges();
	}
	
	void step()
	{
		switch(state)
		{
			case Closed: state_closed(); break;
			case Open: state_open(); break;
			case Closing: state_closing(); break;
			case MovingUp:
			case MovingDown: state_moving(); break;
		}
		
		if(has_player > 0)
		{
			has_player--;
		}
		
		if(state == Closed || state == Open)
		{
			has_player = max(script.g.get_entity_collision(
				self.y() + Elevator::T,
				self.y() + Elevator::B,
				self.x() + Elevator::L - 40,
				self.x() + Elevator::R + 40,
				ColType::Player) > 0 ? Elevator::PlayerWaitFrames : 0, has_player);
		}
	}
	
	private void state_closed()
	{
		if(door_open_t > 0)
		{
			door_open_t = max(0.0, door_open_t - DT * Elevator::DoorSpeed);
			update_door_real_t();
		}
	}
	
	private void state_open()
	{
		if(door_open_t < 1)
		{
			has_player = Elevator::PlayerWaitFrames;
			door_open_t = min(1.0, door_open_t + DT * Elevator::DoorSpeed);
			update_door_real_t();
		}
		
		if(door_open_t >= 1 && has_player == 0)
		{
			enter_state_closed(Closed);
		}
	}
	
	private void state_closing()
	{
		if(door_open_t > 0)
		{
			door_open_t = max(0.0, door_open_t - DT * Elevator::DoorSpeed);
			update_door_real_t();
		}
		else
		{
			if(!enabled)
			{
				rectangle@ r = self.base_rectangle();
				const float size_x = r.get_width();
				const float size_y = r.get_height();
				const float cx = self.x() + r.left() + size_x * 0.5;
				const float cy = self.y() + r.top() + size_y * 0.5;
				script.add_emitter_burst(
					EmitterId::SlimeBall, 17, 23,
					cx, cy, size_x, size_y, 0, 0.25, 1);
				script.g.play_sound('sfx_sparks_' + rand_range(1, 12), cx, cy,
					rand_range(0.9, 1.0), false, true);
			}
			
			state = y == 0 ? MovingUp : MovingDown;
			
			if(enabled)
			{
				message@ msg = create_message();
				script.messages.broadcast('elevator_' + (state == MovingUp ? 'up' : 'down'), msg);
			}
		}
	}
	
	private void enter_state_closed(const ElevatorState closedState)
	{
		state = closedState;
		play_gate_sound();
		
		message@ msg = create_message();
		msg.set_string('type', 'unlock');
		script.messages.broadcast('elevator', msg);
	}
	
	private void state_moving()
	{
		const float dir = state == MovingUp ? -1 : 1;
		const float dist = abs(y - (state == MovingUp ? -top : 0) * 48);
		const bool slow_down = dist < Elevator::BrakeDist;
		
		if(!enabled)
		{
			const float jerk_dist = top * 48 - dist;
			if(jerk_dist > Elevator::JerkDist && jerk_dist < Elevator::JerkDist * 3)
			{
				state = state == MovingUp ? MovingDown : MovingUp;
			}
		}
		
		if(slow_down)
		{
			speed = Elevator::MaxSpeed * dir * ease_out_quad(dist / Elevator::BrakeDist);
		}
		else
		{
			const float acc = enabled ? Elevator::Acc : Elevator::JerkAcc;
			speed = clamp(speed + dir * acc * DT, -Elevator::MaxSpeed, Elevator::MaxSpeed);
		}
		
		float prev_y = y;
		y = clamp(y + speed * DT, -top * 48.0, 0);
		
		if(abs(dist) <= 1)
		{
			speed = 0;
			prev_y = y = state == MovingUp ? -top * 48 : 0;
			state = Open;
			play_gate_sound();
		}
		
		self.set_xy(self.x(), y1 + y);
		y_delta = y - prev_y;
		
		if(script.frame % Elevator::ScreenShakeInterval == 0)
		{
			const float speed_t = abs(y_delta) / (Elevator::MaxSpeed * DT);
			script.add_screen_shake(
				centre_x, centre_y,
				rand_range(0.0, 360.0),
				Elevator::ScreenShakeForce * map(speed_t, 0, 1, 0.85, 1));
		}
		
		update_sound();
		
		gear_small_rot += y_delta;
		gear_big_rot -= y_delta * 0.5;
		gear_mid_rot += y_delta * 0.75;
	}
	
	private void update_sound()
	{
		if(y_delta != 0)
		{
			if(@elevator_snd == null)
			{
				@elevator_snd = script.g.play_script_stream('elevator_move', 0,
					centre_x, centre_y,
					true, 0);
				elevator_snd.positional(true);
			}
			
			const float t = abs(y_delta) / (Elevator::MaxSpeed * DT);
			elevator_snd.volume(t);
			elevator_snd.time_scale(map(t, 0, 1, 0.5, 1));
		}
		
		if(@elevator_snd != null)
		{
			if(abs(y_delta) > 0.01)
			{
				elevator_snd.set_position(self.x() + Elevator::OriginX, self.y() + Elevator::OriginY);
			}
			else
			{
				elevator_snd.stop();
				@elevator_snd = null;
			}
		}
	}
	
	void draw(float sub_frame)
	{
		const float x = lerp(self.prev_x(), self.x(), sub_frame);
		const float y = lerp(self.prev_y(), self.y(), sub_frame);
		
		for(uint i = 0; i < gears.length; i++)
		{
			GearSet@ gear = @gears[i];
			gear_small_spr.draw(
				17, 18, 0, 0,
				gear.x, gear.y - 50, gear_small_rot * gear.speed);
			gear_small_spr.draw(
				17, 18, 0, 0,
				gear.x, gear.y + 50, -gear_small_rot * gear.speed);
		}
		
		if(!script.should_draw(x + bounds.left, y + bounds.top, x + bounds.right, y + bounds.bottom))
			return;
		
		float lever_dir = 0;
		
		if(enabled)
		{
			if(state == Closing)
			{
				lever_dir = this.y == 0 ? -1 : 1;
			}
			else
			{
				lever_dir = state == MovingUp ? -1 : state == MovingDown ? 1 : 0;
			}
		}
		
		script.sprites.elevator.draw(x, y);
		script.sprites.elevator_lever.draw(x, y + lever_dir * 20);
		
		const float ox = x + Elevator::OriginX;
		const float oy = y + Elevator::OriginY;
		gear_small_spr.draw(
			Elevator::GearLrgLayer, Elevator::GearSmlSubLayer, 0, 0,
			ox + Elevator::GearMidX, oy + Elevator::GearMidY, gear_mid_rot);
		gear_small_spr.draw(
			Elevator::GearSmlLayer, Elevator::GearSmlSubLayer, 0, 0,
			ox + Elevator::GearSmlX, oy + Elevator::GearSmlY, gear_small_rot);
		gear_small_spr.draw(
			Elevator::GearSmlLayer, Elevator::GearSmlSubLayer, 0, 0,
			ox - Elevator::GearSmlX, oy + Elevator::GearSmlY, -gear_small_rot);
		gear_big_spr.draw(
			Elevator::GearLrgLayer, Elevator::GearLrgSubLayer, 0, 0,
			ox + Elevator::GearLrgX, oy + Elevator::GearLrgY, gear_big_rot);
		gear_big_spr.draw(
			Elevator::GearLrgLayer, Elevator::GearLrgSubLayer, 0, 0,
			ox - Elevator::GearLrgX, oy + Elevator::GearLrgY, -gear_big_rot);
		
		if(enabled)
		{
			script.sprites.elevator_light.draw(x, y);
		}
		
		for(int i = Elevator::DoorSegments - 1; i >= 0; i--)
		{
			float door_x = x + Elevator::L - 12;
			const float door_y = y + Elevator::T1 + 48 + i * 26 * (1 - door_real_t) - 30 * door_real_t;
			
			for(int j = 0; j < 2; j++)
			{
				door_spr.draw(15, 24, 0, 1, door_x, door_y, 0, 1, 1 - door_real_t * 0.75);
				door_x = x + Elevator::R + 13;
			}
		}
		
		//outline_rect(script.g, 22, 24,
		//	x + bounds.left, y + bounds.top,
		//	x + bounds.right, y + bounds.bottom, 2, 0x22ffffff);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		
		const float x = lerp(self.prev_x(), self.x(), sub_frame);
		const float y = lerp(self.prev_y(), self.y(), sub_frame);
		
		script.sprites.elevator.draw(x, y - top * 48, 0x88ffffff);
		draw_line(script.g, 21, 21,
			x, y, x, y - top * 48,
			2 * script.ed_zoom, 0x33ffffff);
		
		for(uint i = 0; i < base_edges.length; i++)
		{
			Line@ edge = @base_edges[i];
			draw_line(script.g, 21, 21,
				x + edge.x1, y + edge.y1, x + edge.x2, y + edge.y2,
				1 * script.ed_zoom, 0x55ffffff);
		}
		
		rectangle@ r = self.base_rectangle();
		script.g.draw_rectangle_world(21, 21,
			x + r.left(), y + r.top(),
			x + r.right(), y + r.bottom(),
			0, 0x44ffff00);
		
		//draw_dot(script.g, 22, 22, test_x1, test_y1, 3, 0xffff4400, 45);
	}
	
	private void update_collision_edges()
	{
		door_left.y2 = door_left.y1 + 48 + (Elevator::B - (door_left.y1 + 48)) * (1 - door_real_t);
		door_right.y1 = door_left.y2;
		door_outer_left.y1 = door_left.y2;
		door_outer_right.y2 = door_left.y2;
		
		const float x = self.x();
		const float y = self.y();
		
		for(uint i = 0; i < edges.length; i++)
		{
			CollisionEdge@ edge = @edges[i];
			Line@ line = @base_edges[i];
			edge.x1 = x + line.x1;
			edge.y1 = y + line.y1;
			edge.x2 = x + line.x2;
			edge.y2 = y + line.y2;
			edge.update();
		}
	}
	
	private void play_gate_sound()
	{
		audio@ door_snd = script.g.play_script_stream(
			'gate_rattle', 0,
			centre_x, centre_y, false, 0);
		door_snd.positional(true);
		door_snd.volume(1);
	}
	
	private void on_hurt(controllable@ attacked, controllable@ attacker, hitbox@ attack_hitbox, int arg)
	{
		if(state == Open && door_open_t >= 0.5 || state == Closed && has_player >= Elevator::PlayerWaitFrames - 4)
		{
			enter_state_closed(Closing);
		}
	}
	
	private void on_message(const string &in, message@ msg)
	{
		const string type = msg.get_string('type');
		
		if(type == 'lever')
		{
			const int pos = y == 0 ? -1 : 1;
			const int lever_pos = msg.get_int('id') == 1 ? -1 : 1;
			
			if(pos != lever_pos)
			{
				if(state == Closed || state == Open)
				{
					if(state == Open)
					{
						play_gate_sound();
					}
					
					state = Closing;
				}
			}
			else if((state == Closed || state == Open) && msg.get_int('count') >= 0)
			{
				if(state == Closed)
				{
					play_gate_sound();
				}
				
				has_player = Elevator::PlayerWaitFrames;
				state = Open;
			}
		}
		else if(type == 'dust')
		{
			if(!enabled)
			{
				count -= msg.get_int('count');
				
				if(count <= 0)
				{
					message@ new_msg = create_message();
					new_msg.set_int('enabled', 1);
					script.messages.broadcast('elevator_unlock', new_msg);
				}
			}
		}
	}
	
	//void editor_var_changed(var_info@ var)
	//{
	//	const string name = var.name;
	//	
	//	if(name == 'test_x1' || name == 'test_y1' || name == 'test_x2' || name == 'test_y2')
	//	{
	//		puts(test_x1 - centre_x, test_y1 - centre_y);
	//	}
	//}
	
}

class GearSet
{
	
	[position,mode:world,layer:19,y:y] float x;
	[hidden] float y;
	[persist] float speed = 1;
	
}
