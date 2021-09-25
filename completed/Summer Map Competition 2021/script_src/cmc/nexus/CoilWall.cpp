#include '../../lib/enums/EmitterId.cpp';
#include '../../lib/math/Line.cpp';

#include '../lab/DamageType.cpp';

class CoilWall : trigger_base, callback_base, IInitHandler
{
	
	[entity] int coil_id;
	[position,mode:world,layer:19,y:y1] float x1;
	[hidden] float y1;
	[position,mode:world,layer:19,y:y2] float x2;
	[hidden] float y2;
	[option,0:Off,1:Stun,2:Kill,3:KillAndStun] DamageType kill_player = KillAndStun;
	[option,0:Off,1:Stun,2:Kill,3:KillAndStun] DamageType kill_enemy = KillAndStun;
	[option,0:Off,1:Stun,2:Kill,3:KillAndStun] DamageType kill_apple = KillAndStun;
	[persist] int player_stun = 500;
	[persist] int enemy_stun = 500;
	[persist] int apple_stun = 500;
	[persist] string event;
	[persist] int count = 1;
	[persist] float time = 0;
	[persist] bool play_sound;
	[hidden] int max_count = -1;
	
	script@ script;
	scripttrigger@ self;
	
	Coil@ coil;
	Line line;
	float length;
	float dx, dy;
	float mx, my;
	float nx, ny;
	
	int size_entities = 8;
	int num_entities;
	array<EntityData> entities(size_entities);
	
	textfield@ tf;
	
	void init(script@ script, scripttrigger@ self)
	{
		@tf = create_textfield();
		@this.script = script;
		@this.self= self;
		
		self.radius(0);
		self.editor_colour_inactive(0xffff5555);
		self.editor_colour_active(0xffff8888);
		
		if(count > max_count)
		{
			max_count = count;
		}
		
		script.init_subscribe(this);
		
		if(!script.is_playing)
			return;
	}
	
	void init() override
	{
		init_coil();
		init_line();
		
		if(event != '')
		{
			script.messages.add_listener(event, MessageHandler(on_event));
		}
	}
	
	private void init_coil()
	{
		scripttrigger@ st = scripttrigger_by_id(coil_id);
		@coil = @st != null ? cast<Coil@>(st.get_object()) : null;
	}
	
	private void init_line()
	{
		line.x1 = x1;
		line.y1 = y1;
		line.x2 = x2;
		line.y2 = y2;
		
		dx = x2 - x1;
		dy = y2 - y1;
		mx = x1 + dx * 0.5;
		my = y1 + dy * 0.5;
		length = magnitude(dx, dy);
		dx = length != 0 ? dx / length : 0;
		dy = length != 0 ? dy / length : 0;
		nx = -dy;
		ny =  dx;
	}
	
	void step()
	{
		if(@coil == null)
			return;
		
		if(!coil.on)
		{
			num_entities = 0;
			return;
		}
		
		const bool check_entities = kill_player != Off || kill_enemy != Off || kill_apple != Off;
		int i = check_entities ? script.g.get_entity_collision(
			min(y1, y2),
			max(y1, y2),
			min(x1, x2),
			max(x1, x2),
			ColType::Hittable)
				: 0;
		
		while(--i >= 0)
		{
			controllable@ c = script.g.get_controllable_collision_index(i);
			if(@c == null)
				continue;
			
			const int id = c.id();
			EntityData@ data = null;
			for(int j = num_entities - 1; j >= 0; j--)
			{
				EntityData@ e = @entities[j];
				if(e.id == id)
				{
					@data = e;
					break;
				}
			}
			
			if(@data != null)
			{
				data.frames = 4;
				data.x = c.prev_x();
				data.y = c.prev_y();
				continue;
			}
			
			float stun_force = 0;
			DamageType damage = Off;
			
			if(c.player_index() != -1)
			{
				if(kill_player == Off)
					continue;
				
				damage = kill_player;
				stun_force = player_stun;
			}
			const string type = c.type_name();
			if(type == 'hittable_apple')
			{
				if(kill_apple == Off)
					continue;
				
				damage = kill_apple;
				stun_force = apple_stun;
			}
			if(type.substr(0, 6) == 'enemy_')
			{
				if(kill_enemy == Off)
					continue;
				
				damage = kill_enemy;
				stun_force = enemy_stun;
			}
			
			if(num_entities + 1 >= size_entities)
			{
				entities.resize(size_entities *= 2);
			}
			
			@data = @entities[num_entities++];
			data.id = id;
			@data.c = c;
			data.frames = 4;
			data.x = c.prev_x();
			data.y = c.prev_y();
			rectangle@ r = c.base_rectangle();
			data.ox = r.left() + r.width * 0.5;
			data.oy = r.top() + r.height * 0.5;
			data.w = r.width;
			data.h = r.height;
			data.stun_force = stun_force;
			data.damage = damage;
		}
		
		for(int j = num_entities - 1; j >= 0; j--)
		{
			EntityData@ data = @entities[j];
			
			if(data.frames-- <= 0)
			{
				entities[j] = entities[--num_entities];
				continue;
			}
			
			controllable@ c = data.c;
			const float cx1 = data.x + data.ox;
			const float cy1 = data.y + data.oy;
			const float cx2 = c.x() + data.ox;
			const float cy2 = c.y() + data.oy;
			
			float ix, iy, t;
			if(!line_line_intersection(
				x1, y1, x2, y2,
				cx1, cy1, cx2, cy2,
				ix, iy, t)
			)
				continue;
			
			c.set_xy(data.x, data.y);
			
			const float dist = distance(cx1, cy1, ix, iy);
			int side = line_side(x1, y1, x2, y2, cx1, cy1);
			if(side == 0)
			{
				side = -line_side(x1, y1, x2, y2, cx2, cy2);
			}
			const float stun_force = data.stun_force * side;
			c.set_speed_xy(
				c.x_speed() + nx * side * stun_force * (dist / 24),
				c.y_speed() + ny * side * stun_force * (dist / 24));
			
			const bool is_stunned = c.state() >= EntityState::Stun && c.state() <= EntityState::StunGround;
			if(
				(data.damage == Stun || data.damage == KillAndStun) &&
				!is_stunned)
			{
				c.stun(nx * stun_force, ny * stun_force);
			}
			
			if(data.damage == Kill || data.damage == KillAndStun)
			{
				dustman@ dm = c.as_dustman();
				if(@dm != null)
				{
					dm.kill(false);
					continue;
				}
				
				script.g.remove_entity(c.as_entity());
			}
			
			if(!is_stunned)
			{
				BurstEmitter@ burst = script.add_emitter_burst(
					EmitterId::SlimeBall, 15, 7,
					cx1, cy1, data.w, data.h, 0, 0.25, 1);
				@burst.track = c.as_entity();
				for(int k = 0; k < 2; k++)
				{
					script.g.play_sound('sfx_sparks_' + rand_range(1, 12), cx1, cy1,
						rand_range(0.9, 1.0), false, true);
				}
			}
		}
	}
	
	void draw(float sub_frame)
	{
		tf.draw_world(22, 22, self.x(), self.y() - 50, 1, 1, 0);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		
		if(@coil != null)
		{
			draw_line(script.g, 21, 21,
				self.x(), self.y(), coil.self.x(), coil.self.y(),
				2 * script.ed_zoom, 0xaaff5555);
		}
		
		const float arr_size = 24;
		const float thickness = 2 * script.ed_zoom;
		const uint clr = 0xaaff2222;
		draw_line(script.g, 21, 21,
			x1, y1, x2, y2,
			thickness, clr);
		draw_dot(script.g, 21, 21, x1, y1, thickness * 2, clr, 45);
	}
	
	void editor_var_changed(var_info@ var)
	{
		const string name = var.name;
		
		if(name == 'coil_id')
		{
			init_coil();
		}
		else if(name == 'x1' || name == 'y1' || name == 'x2' || name == 'y2')
		{
			init_line();
		}
	}
	
	private void on_event(const string &in id, message@ msg)
	{
		if(@coil == null)
			return;
		
		const bool on = count == 0;
		count = clamp(count - msg.get_int('count'), 0, max_count);
		const bool new_on = count == 0;
		
		if(on != new_on)
		{
			if(!coil.on)
			{
				coil.on = true;
				coil.prime();
			}
			else
			{
				coil.power_off();
				
				if(time > 0)
				{
					Lever::trigger(script, event, 0, time, 0, 'door');
				}
				
				if(time < 0)
				{
					script.g.remove_entity(coil.self.as_entity());
					script.g.remove_entity(self.as_entity());
				}
			}
			
			toggle_glow_props();
			
			if(play_sound)
			{
				audio@ snd = script.g.play_script_stream(
					'power_down', 0,
					self.x(), self.y(), false, 0);
				snd.positional(true);
				snd.volume(0.75);
			}
		}
	}
	
	private void toggle_glow_props()
	{
		const bool remove = !coil.on && time < 0;
		
		int i = script.g.get_prop_collision(
			min(y1, y2), max(y1, y2),
			min(x1, x2), max(x1, x2));
		
		while(--i >= 0)
		{
			prop@ p = script.g.get_prop_collision_index(i);
			
			if(p.layer() != uint(coil.on ? 12 : 19))
				continue;
			if(p.sub_layer() != 8 && p.sub_layer() != 9)
				continue;
			
			// Sun prop
			if(p.prop_set() != 2 || p.prop_group() != 22 || p.prop_index() != 4)
				continue;
			
			if(remove)
			{
				script.g.remove_prop(p);
			}
			else
			{
				p.layer(p.layer() == 19 ? 12 : 19);
			}
		}
	}
	
}

class EntityData
{
	
	int id;
	float x, y;
	float w, h;
	float ox, oy;
	float stun_force;
	DamageType damage;
	controllable@ c;
	int frames;
	
}
