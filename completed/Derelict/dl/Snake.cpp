#include '../lib/debug/Debug.cpp';
#include '../lib/drawing/common.cpp';
#include '../lib/drawing/circle.cpp';
#include '../lib/easing/cubic.cpp';
#include '../lib/easing/quad.cpp';
#include '../lib/enums/ColType.cpp';
#include '../lib/enums/GVB.cpp'; // REMOVE
#include '../lib/enums/Team.cpp';
#include '../lib/math/Bezier.cpp';
#include '../lib/phys/springs/CachedTileProvider.cpp';
#include '../lib/phys/springs/SpringSystem.cpp';
#include '../lib/phys/springs/ITileProvider.cpp';
#include '../lib/tiles/TileEdge.cpp';

#include 'SnakeSettings.cpp';
#include 'SnakeDialog.cpp';
#include 'SnakeConstraint.cpp';
#include 'SnakeJoint.cpp';
#include 'SnakeJointPersistData.cpp';
#include 'SpeechBubble.cpp';
#include 'TileRegion.cpp';
#include 'SnakeListener.cpp';

class Snake : enemy_base, callback_base, SpeechBubble::CustomAttributeParser
{
	
	// Make sure to copy all persist vars in `PlayerData::spawn_snake` when adding.
	// -->
	
	/// Don't render or step when inactive
	[persist] bool is_active = false;
	[persist] bool clean_filth;
	[persist|tooltip:"If true will auto connec to to the nearest camera node\nwhen inside the main map area."]
	bool auto_connect_main_cam = true;
	[persist|tooltip:"If true will remove deathzones inside tera drops.\n(Deathzones with height < 24)",scale:0.5]
	bool remove_tera_deathzones;
	[persist] bool compact;
	
	[persist] bool ddraw_joints;
	[persist] bool ddraw_indices;
	[persist] bool ddraw_contacts;
	[persist] bool ddraw_look;
	bool debug_draw;
	
	[hidden] array<SnakeJointPersistData> joint_persist_data;
	
	DLScript@ script;
	scriptenemy@ self;
	
	SpringSystem spring_system;
	CachedTileProvider@ tile_provider;
	
	Bezier@ curve;
	SnakeListener@ listener;
	
	private array<SnakeJoint@> joints;
	private array<DistanceConstraint@> constraints;
	private array<TileConstraint@> collision_constraints;
	private SnakeConstraint joint_constraint;
	private int num_joints = 24;
	private int ground_check_joints = int(min(3, num_joints));
	private int max_ground_joints = int(num_joints * 0.85);
	private int taunt_joints = int(num_joints * 0.65);
	private SnakeJoint@ head;
	private SnakeJoint@ neck;
	private float x1, y1, x2, y2;
	
	private float base_radius = 22;
	private float joint_spacing_p = 0.75;
	private textfield@ tf;
	private raycast@ ray;
	
	private sprites@ spr;
	
	private float look_target_x, look_target_y;
	private controllable@ look_target;
	private bool look_target_is_locked;
	/// Interval in frames  to check for new targets.
	private float look_target_timer_interval = 10;
	/// The cooldown after losing sight of a target before checking for a new one.
	private float look_target_timer_cooldown = 60;
	/// A target will be lost if is not visible for this number of frames in a row.
	private float look_target_timer_sight = 60;
	private float look_target_timer = look_target_timer_interval;
	private float look_target_hitbox_x1, look_target_hitbox_y1;
	private float look_target_hitbox_x2, look_target_hitbox_y2;
	private float look_target_hitbox_size = 200;
	private float head_angle, head_angle_prev;
	private float head_angle_t;
	private float head_angle_inc = 460;
	private float head_angle_range = 45;
	
	private float target_dialog_timer;
	private float target_dialog_cooldown = 240;
	private array<Snake::DialogOptions> target_dialogs;
	private bool has_target_dialog;
	private dictionary seen_targets;
	
	/// If true this is just a dummy snake spawned on death so the snake doesn't disappear while the
	/// checkpoint loads.
	private bool is_dummy = false;
	private PlayerData@ player = null;
	private bool dead = false;
	private bool is_created;
	private bool is_added;
	private bool is_initialised;
	
	private float move_x, move_y;
	
	private float ground_contact_per = 0;
	private int ground_contact_i = -1;
	
	/// Is any part of the snake in contact with a surface.
	private float has_contact;
	/// Is any part of the snake in contact with a surface considered ground.
	private bool has_ground_contact;
	private audio@ slide_snd;
	private float slide_volume, slide_volume_t = 0;
	private float slide_pitch = 1, slide_pitch_t = 1;
	private float slide_volume_mult = 1;
	
	private float attack_cooldown_timer = 0;
	private hitbox@ attack_hitbox;
	private float attack_force_x, attack_force_y;
	private float attack_dir_x, attack_dir_y;
	private float attack_x, attack_y;
	private float attack_radius;
	private float attack_dir;
	private float attack_timer;
	private controllable@ locked_attack_target;
	
	private int attack_hiss_sfx_i = 0;
	private int hiss_subtle_sfx_i = 0;
	
	private bool is_taunting;
	private int taunt_dir;
	private array<AngularConstraint@> taunt_constraints;
	private float taunt_ground_timer;
	private float taunt_transition_timer;
	
	private float combo_break_timer;
	
	private int t;
	private float time_scale = 1;
	private float frame_time = 1;
	
	private bool physics_enabled = true;
	private array<Bezier>@ _curves;
	private float _curve_t = -1;
	
	private array<TileRegion> pending_cache_clear;
	
	private SpeechBubble speech_bubble;
	private int speech_meta_size = 16;
	private array<Snake::SpeechMeta> speech_meta_list(speech_meta_size);
	private int speech_meta_count;
	private int speech_meta_index;
	private int speech_bubble_progress_prev;
	
	private float jaw_ox = 4;
	private float jaw_oy = 6;
	private float jaw_angle, jaw_angle_prev;
	private float jaw_angle_t;
	private float jaw_speed = 360;
	
	private float tongue_cooldown;
	private int tongue_flicks = 2;
	private float tongue_t = -1;
	
	private float head_orientation_lock = 0;
	
	private int lights_list_size = 2;
	private int lights_count;
	private array<Light@> lights(lights_list_size);
	
	float get_radius() const { return base_radius; }
	float x { get const { return head.x; } }
	float y { get const { return head.y; } }
	bool is_ready{ get const { return is_created; } }
	bool is_collapsed { get const { return joints[num_joints - 1].is_static; } }
	float speed_x { get const { return head.x - head.prev_x; } }
	float speed_y { get const { return head.y - head.prev_y; } }
	
	Snake() { }
	
	Snake(PlayerData@ player)
	{
		@this.player = player;
	}
	
	void init(script@ script, scriptenemy@ self)
	{
		@this.script = script;
		@this.self = self;
		
		self.auto_physics(false);
		self.team(Team::Cleaner);
		
		if(is_active && script.is_playing)
		{
			do_init();
		}
		
		debug_draw = ddraw_joints || ddraw_indices || ddraw_contacts || ddraw_look;
	}
	
	private void do_init()
	{
		if(is_initialised)
			return;
		
		is_initialised = true;
		
		if(@tile_provider == null)
		{
			@tile_provider = CachedTileProvider();
			@tile_provider.g = script.g;
			tile_provider.frame = -1;
			// Increase cache duration cause we can just clear it ourselves when something changes.
			tile_provider.max_age = 120;
			tile_provider.max_age_dustblock = tile_provider.max_age;
			tile_provider.max_age_solid = tile_provider.max_age;
		}
		
		spring_system.gravity.y = 96 * 9.81;
		spring_system.constraint_iterations = 3;
		
		if(script.is_playing)
		{
			target_dialogs.resize(Snake::DlgAppleHit - Snake::DlgSlimeBallSee + 1);
			for(int i = Snake::DlgSlimeBallSee; i <= Snake::DlgAppleHit; i++)
			{
				Snake::DialogOptions@ dlg = @target_dialogs[i - Snake::DlgSlimeBallSee];
				dlg.init(@Snake::Dialog[i]);
			}
		}
		
		@speech_bubble.script = script;
		
		@spr = create_sprites();
		spr.add_sprite_set('script');
		
		tongue_cooldown = lerp(Snake::TongueCooldownMin, Snake::TongueCooldownMax, 0.15);
	}
	
	void on_add()
	{
		if(!script.is_playing)
			return;
		
		if(is_dummy)
		{
			// Set something custom so we don't get killed by deathzones.
			self.hit_collision().collision_type(ColType::Null);
			self.base_collision().collision_type(ColType::Null);
		}
		
		is_added = true;
		
		if(is_active)
		{
			do_add();
		}
		
		@speech_bubble.custom_attribute_parser = this;
	}
	
	void on_remove()
	{
		if(@slide_snd != null)
		{
			slide_snd.stop();
			@slide_snd = null;
		}
	}
	
	private void do_add()
	{
		self.on_hit_callback(@this, 'on_hit', 0);
		self.on_hurt_callback(@this, 'on_hurt', 0);
		
		if(is_dummy)
		{
			// Set something custom so we don't get killed by deathzones.
			self.hit_collision().collision_type(ColType::Null);
			self.base_collision().collision_type(ColType::Null);
		}
		else
		{
			// ColType::Player allows the snake to trigger checkpoints and triggers.
			self.base_collision().collision_type(@player != null ? ColType::Player : ColType::Enemy);
			create();
		}
	}
	
	private void on_hit(controllable@ attacker, controllable@ attacked, hitbox@ hb, int)
	{
		do_hit_stop();
		attack_cooldown_timer *= 0.5;
		
		const string name = attacked.type_name();
		const EmitterBurstSettings@ settings = name.substr(6, 3) == 'tut' ? @Snake::AttackPolyEmitterBurst : @Snake::AttackBloodEmitterBurst;
		script.emitter_bursts.add(settings, attack_x, attack_y, 0);
		
		try_set_target_kill_dialog(attacked.type_name(), attacked.life() - hb.damage() <= 0);
		
		if(@listener != null)
		{
			listener.on_snake_attack_hit(attacked, hb);
		}
	}
	
	private void on_hurt(controllable@ attacked, controllable@ attacker, hitbox@ hb, int)
	{
		if(@locked_attack_target != null)
		{
			const float force = hb.attack_strength() * 50;
			float dir = hb.aoe()
				? atan2(hb.x() - head.x, hb.y() - head.y)
				: hb.attack_dir() * DEG2RAD;
			head.impulse_x += sin(dir) * force;
			head.impulse_y += cos(dir) * -force;
		}
		
		if(@player == null)
			return;
		
		try_set_target_hurt_dialog(attacker.type_name());
		
		if(combo_break_timer == 0)
		{
			script.g.combo_break_count(script.g.combo_break_count() + 1);
			combo_break_timer = 30;
		}
	}
	
	private void create()
	{
		if(is_created)
			return;
		
		const int facing = self.face();
		float x = self.x();
		float y = self.y();
		SnakeJoint@ p0_prev;
		SnakeJoint@ p0;
		SnakeJoint@ pb;
		float radius_prev = base_radius;
		
		joints.resize(num_joints);
		collision_constraints.resize(num_joints);
		
		float total_joint_weight = 0;
		int dir = 1;
		
		const bool load_persist = script.is_playing && int(joint_persist_data.length) == num_joints;
		
		if(!load_persist)
		{
			joint_persist_data.resize(num_joints);
		}
		
		for(int i = 0; i < num_joints; i++)
		{
			SnakeJointPersistData@ persist_data = null;
			
			if(load_persist)
			{
				@persist_data = @joint_persist_data[i];
				x = persist_data.x;
				y = persist_data.y;
			}
			
			const float t = float(i) / (num_joints - 1);
			SnakeJoint@ p = SnakeJoint(x, y);
			@joints[i] = p;
			
			if(load_persist)
			{
				p.prev_x = persist_data.prev_x;
				p.prev_y = persist_data.prev_y;
			}
			
			p.orientation = head_orientation_lock != 0 ? head_orientation_lock : facing;
			p.orientation_t = p.orientation;
			p.orientation_prev = p.orientation;
			p.radius = lerp(5, base_radius, ease_in_cubic(1 - max(t - 0.65, 0.0)));
			spring_system.add_particle(p);
			
			p.scale = p.radius / base_radius;
			
			x -= (p.radius + radius_prev) * joint_spacing_p * facing * dir;
			radius_prev = p.radius;
			// Make joints closer to the head have more value.
			float joint_value_mult = lerp(0.35, 1, float(num_joints - 1 - i) / (num_joints - 1));
			
			p.area = PI * (p.radius * p.radius);
			p.value = joint_value_mult;
			total_joint_weight += p.area * p.value;
			
			TileConstraint@ tc = TileConstraint(@tile_provider, p, 10);
			@collision_constraints[i] = tc;
			spring_system.add_constraint(tc);
			
			if(@p0 != null)
			{
				DistanceConstraint@ dc = DistanceConstraint(p0, p, 0.98, 0.25);
				spring_system.add_constraint(dc);
				constraints.insertLast(dc);
				
				if(@pb != null)
				{
					AngularConstraint@ ac = AngularConstraint(p0, p, pb, -65, 65, 0.01, 0.01);
					spring_system.add_constraint(ac);
				}
			}
			
			if(@p0_prev != null)
			{
				joint_constraint.particle_pairs.insertLast(p);
				joint_constraint.particle_pairs.insertLast(p0_prev);
				@p0.left = p0_prev;
				@p0.right = p;
				@p0.dc1 = constraints[i - 1];
				@p0.dc2 = constraints[i - 2];
			}
			
			@pb = p0;
			@p0_prev = p0;
			@p0 = p;
			
			if(compact && (i - 2) % 4 == 0)
			{
				dir = -dir;
			}
		}
		
		for(int i = 0; i < num_joints; i++)
		{
			SnakeJoint@ p = @joints[i];
			p.value = (p.area * p.value) / total_joint_weight;
		}
		
		@head = joints[0];
		@neck = joints[1];
		
		spring_system.add_constraint(joint_constraint);
		
		is_created = true;
		
		head_angle = head_angle_prev = atan2(head.y- neck.y, head.x - neck.x);
		head_angle_t = head_angle;
	}
	
	void activate()
	{
		if(is_active)
			return;
		
		is_active = true;
		
		do_init();
		
		if(is_added)
		{
			do_add();
		}
	}
	
	void take_control(PlayerData@ player)
	{
		activate();
		
		@this.player = player;
		
		@player.c = self.as_controllable();
	}
	
	void release_control()
	{
		@player = null;
	}
	
	Snake@ make_copy()
	{
		Snake copy = this;
		copy.is_active = false;
		copy.is_added = false;
		@copy.slide_snd = null;
		scriptenemy@ se = create_scriptenemy(copy);
		se.face(self.face());
		se.set_xy(self.x(), self.y());
		se.prev_x(self.prev_x());
		se.prev_y(self.prev_y());
		se.set_speed_xy(self.x_speed(), self.y_speed());
		return @copy;
	}
	
	array<Bezier>@ curves
	{
		get { return _curves; }
		set {
			@_curves = value;
			physics_enabled = @_curves == null;
		}
	}
	
	float curve_t
	{
		get const { return _curve_t; }
		set {
			if(_curve_t == value)
				return;
			
			_curve_t = value;
		}
	}
	
	void position_along_curve(const bool extend = false)
	{
		if(@_curves == null)
			return;
		
		float total_length = 0;
		for(uint i = 0; i < _curves.length; i++)
		{
			total_length += _curves[i].length;
		}
		
		float dist = clamp(total_length * _curve_t, 0, total_length);
		
		uint curve_index = 0;
		Bezier@ curve = _curves[curve_index++];
		float curve_start_length = 0;
		float curve_end_length = curve.length;
		bool on_curve = true;
		float prev_x = 0, prev_y = 0;
		float prev_dist = 0;
		float last_nx, last_ny;
		for(int i = 0; i < num_joints; i++)
		{
			SnakeJoint@ p = joints[i];
			
			if(on_curve)
			{
				while(dist >= curve_end_length)
				{
					if(curve_index < _curves.length)
					{
						curve_start_length = curve_end_length;
						@curve = @_curves[curve_index++];
						curve_end_length += curve.length;
					}
					else
					{
						last_nx = curve.x4 - curve.x3;
						last_ny = curve.y4 - curve.y3;
						normalize(last_nx, last_ny, last_nx, last_ny);
						on_curve = false;
						break;
					}
				}
			}
			
			if(on_curve || !extend)
			{
				const float d = clamp(dist - curve_start_length, 0, curve.length);
				p.reset(curve.mx(d), curve.my(d));
			}
			// Just extend any points back along the curve
			else
			{
				p.reset(prev_x + last_nx * prev_dist, prev_y + last_ny * prev_dist);
			}
			
			if(i < num_joints - 1)
			{
				prev_dist = constraints[i].rest_length;
				dist += prev_dist;
			}
			
			prev_x = p.x;
			prev_y = p.y;
		}
	}
	
	void lock_head_orientation(const float lock)
	{
		head_orientation_lock = lock == 0 ? 0.0 : lock > 0 ? 1.0 : -1.0;
	}
	
	void unlock_head_orientation()
	{
		head_orientation_lock = 0;
	}
	
	void set_dialog(const string &in text, const bool open=true)
	{
		speech_meta_index = 0;
		speech_meta_count = 0;
		
		speech_bubble.text = text;
		speech_bubble_progress_prev = -1;
		speech_bubble.draw_next_arrow = true;
		
		if(open)
		{
			speech_bubble.open();
		}
	}
	
	void close_dialog()
	{
		speech_bubble.open(false);
	}
	
	void force_taunt()
	{
		try_taunt(true);
		taunt_ground_timer = 120;
	}
	
	void lock_target(controllable@ target)
	{
		look_target_is_locked = @target != null;
		
		if(@target == null)
		{
			lose_target(true);
		}
	}
	
	void lock_attack_target(controllable@ target)
	{
		@locked_attack_target = target;
	}
	
	/// "Collapse" joints that are beyond `x` to fake the snake crawling into a hole in the wall.
	void collapse(const float x, const int dir)
	{
		int max_collapsed = 0;
		
		for(int i = 0; i < num_joints; i++)
		{
			SnakeJoint@ p = @joints[i];
			if(p.is_static)
			{
				max_collapsed = i;
				continue;
			}
			if(!(dir >= 0 && p.x >= x) && !(dir < 0 && p.x <= x))
				continue;
			
			if(i < num_joints - 1)
			{
				DistanceConstraint@ d = constraints[i];
				d.min_rest_length = 0;
			}
			
			p.is_static = true;
			max_collapsed = i;
			p.x = dir > 0 ? max(x + 24, p.x) : min(x - 24, p.x);
			p.prev_x = p.x;
			spring_system.remove_constraint(collision_constraints[i]);
		}
		
		slide_volume_mult = 1 - max_collapsed / float(num_joints - 1);
	}
	
	void update_slide_volume_mult(const float mult)
	{
		slide_volume_mult = mult;
	}
	
	void step()
	{
		if(!is_created || !is_active)
			return;
		
		frame_time = script.time_scale * time_scale;
		
		if(@player != null && !dead && self.destroyed())
		{
			dead = true;
			
			script.g.combo_break_count(script.g.combo_break_count() + 1);
			
			if(script.num_players == 1)
			{
				// Add a dummy snake cause this one gets removed as soon as the checkpoint is loaded
				// and trying to add this one back in has some issues.
				Snake@ dummy_snake = make_copy();
				dummy_snake.is_dummy = true;
				dummy_snake.is_active = true;
				dummy_snake.dead = false;
				@dummy_snake.player = null;
				script.g.add_entity(dummy_snake.self.as_entity(), false);
				
				script.g.load_checkpoint();
			}
			else
			{
				message@ msg = create_message();
				msg.set_int('player', player.index);
				msg.set_float('x', script.g.get_checkpoint_x(player.index));
				msg.set_float('y', script.g.get_checkpoint_y(player.index));
				script.spawn_player(msg);
				controller_entity(player.index, player.snake.self.as_controllable());
				reset_camera(player.index);
				script.g.remove_entity(self.as_entity());
				
				script.g.add_effect('editor', 'respawnteam1', player.snake.self.x(), player.snake.self.y(), 0, 1, 1, 16);
				script.g.play_sound('sfx_damage_spikes', player.snake.self.x(), player.snake.self.y(), 1, false, true);
			}
		}
		
		jaw_angle_prev = jaw_angle;
		if(jaw_angle < jaw_angle_t)
		{
			jaw_angle = min(jaw_angle + jaw_speed * DEG2RAD * DT, jaw_angle_t);
		}
		else if(jaw_angle > jaw_angle_t)
		{
			jaw_angle = max(jaw_angle - jaw_speed * DEG2RAD * DT, jaw_angle_t);
		}
		
		check_ground();
		do_movement();
		try_attack();
		try_taunt();
		update_tongue();
		
		tile_provider.frame++;
		
		if(physics_enabled)
		{
			spring_system.step(frame_time);
		}
		
		update_joints();
		update_target();
		update_lights();
		
		if(self.freeze_frame_timer() > 0)
		{
			self.freeze_frame_timer(self.freeze_frame_timer() - 24 * DT * frame_time);
		}
		
		self.set_xy(head.x, head.y);
		self.set_speed_xy((head.x - head.prev_x) / DT, (head.y - head.prev_y) / DT);
		
		if(@player != null && !is_dummy)
		{
			if(auto_connect_main_cam && t % 10 == 0 && script.in_map_map_bounds(head.x, head.y))
			{
				try_connect_camera();
			}
			if((t + 2) % 5 == 0)
			{
				check_deathzones();
			}
		}
		
		for(int i = 0; i < num_joints; i++)
		{
			SnakeJointPersistData@ persist_data = @joint_persist_data[i];
			SnakeJoint@ p = @joints[i];
			persist_data.x = p.x;
			persist_data.y = p.y;
			persist_data.prev_x = p.prev_x;
			persist_data.prev_y = p.prev_y;
		}
		
		if(time_scale < 1)
		{
			time_scale = min(time_scale + 1.5 * DT * script.time_scale, 1.0);
			self.time_warp(time_scale);
		}
		
		if(pending_cache_clear.length > 0)
		{
			for(uint i = 0; i < pending_cache_clear.length; i++)
			{
				TileRegion@ r = @pending_cache_clear[i];
				tile_provider.clear(r.tx1, r.ty1, r.tx2, r.ty2);
			}
			pending_cache_clear.resize(0);
		}
		
		if(combo_break_timer > 0)
		{
			combo_break_timer = max(combo_break_timer - frame_time, 0.0);
		}
		
		update_speech_bubble();
		speech_bubble_progress_prev = speech_bubble.progress;
		
		t++;
	}
	
	private void update_joints()
	{
		// Some padding for the head sprite
		const float head_p = 48;
		x1 = head.x - head.radius - head_p;
		y1 = head.y - head.radius - head_p;
		x2 = head.x + head.radius + head_p;
		y2 = head.y + head.radius + head_p;
		
		head.calc_values(neck.x, neck.y, 1);
		
		if(head_orientation_lock != 0)
		{
			head.orientation_t = head_orientation_lock;
		}
		else if(!head.is_vertical())
		{
			head.orientation_t = head.angle < -90 || head.angle > 90 ? -1: 1;
		}
		// Prefer to orient towards walls.
		else if(head.has_contact && dot(1, 0, abs(head.contact_nx), head.contact_ny) > 0.9)
		{
			head.orientation_t = -int(sign(head.contact_nx) * sign(-head.dy));
		}
		
		float orientation = head.orientation_t;
		
		SnakeJoint@ p0 = @joints[0];
		for(int i = 0; i < num_joints - 1; i++)
		{
			SnakeJoint@ p = p0;
			@p0 = @joints[i + 1];
			
			if(p.x - p.radius < x1) x1 = p.x - p.radius;
			if(p.y - p.radius < y1) y1 = p.y - p.radius;
			if(p.x + p.radius > x2) x2 = p.x + p.radius;
			if(p.y + p.radius > y2) y2 = p.y + p.radius;
			
			p.in_deathzone = false;
			p.light_t = 0;
			
			p.calc_values(p0.x, p0.y, 1);
			p.orientation_prev = p.orientation;
			
			// When the joint is mostly horizontal instantly set the orientation so it's facing down.
			if(abs(p.angle) < 65 || abs(p.angle) > 155)
			{
				p.orientation = p.orientation_t = p.angle < -90 || p.angle > 90 ? -1: 1;
			}
			// If it's vertical propagate the orientation down over a few frames.
			// This sort of helps with quick flip-flopping when climbing tunnels.
			else
			{
				if(orientation >= 1 || orientation <= -1)
				{
					p.orientation_t = orientation;
				}
				
				if(p.orientation < p.orientation_t)
				{
					p.orientation = min(p.orientation + 20 * DT, 1.0);
				}
				else if(p.orientation > p.orientation_t)
				{
					p.orientation = max(p.orientation - 20 * DT, -1.0);
				}
			}
			
			orientation = p.orientation;
		}
		
		if(p0.x - p0.radius < x1) x1 = p0.x - p0.radius;
		if(p0.y - p0.radius < y1) y1 = p0.y - p0.radius;
		if(p0.x + p0.radius > x2) x2 = p0.x + p0.radius;
		if(p0.y + p0.radius > y2) y2 = p0.y + p0.radius;
		
		p0.in_deathzone = false;
		p0.light_t = 0;
	}
	
	private void update_target()
	{
		head_angle_prev = head_angle;
		float head_angle_diff = shortest_angle(head_angle, head_angle_t);
		if(head_angle_diff > 0)
		{
			head_angle = min(head_angle + head_angle_inc * DT * frame_time * DEG2RAD, head_angle_t);
		}
		else if(head_angle_diff < 0)
		{
			head_angle = max(head_angle - head_angle_inc * DT * frame_time * DEG2RAD, head_angle_t);
		}
		
		if(target_dialog_timer > 0)
		{
			target_dialog_timer = max(target_dialog_timer - frame_time, 0.0);
		}
		
		if(has_target_dialog)
		{
			target_dialog_timer =  target_dialog_cooldown;
			
			if(speech_bubble.can_auto_advance)
			{
				has_target_dialog = false;
				speech_bubble.open(false);
			}
		}
		
		// Clamp the head angle
		head_angle_diff = shortest_angle(head.angle * DEG2RAD, head_angle);
		if(abs(head_angle_diff) * RAD2DEG > head_angle_range)
		{
			head_angle = head.angle * DEG2RAD + head_angle_range * DEG2RAD * sign(head_angle_diff);
		}
		
		// -- Update targetted entity.
		
		bool ground_check = false;
		
		if(!look_target_is_locked)
		{
			if(@look_target != null && look_target.destroyed())
			{
				lose_target(true);
			}
			
			if(@look_target != null)
			{
				look_target.centre(look_target_x, look_target_y);
				
				bool lost = false;
				
				// Facing the other way
				// And distance check
				if(
					sign(look_target_x - head.x) != sign(head.dx) ||
					distance(head.x, head.y, look_target_x, look_target_y) >= look_target_hitbox_size * 3)
				{
					lose_target();
					lost = true;
					
					if(sign(look_target_x - head.x) != sign(head.dx))
					{
						//look_target_x = head.x + head.nx * 100;
						//look_target_y = head.y + head.ny * 100;
						ground_check = true;
					}
				}
				// Collision check
				else
				{
					@ray = script.g.ray_cast_tiles(head.x, head.y, look_target_x, look_target_y, ray);
					if(ray.hit())
					{
						lose_target();
						lost = true;
					}
				}
				
				if(!lost)
				{
					look_target_timer = look_target_timer_sight;
				}
			}
		}
		
		// -- Find an entity to target
		
		bool find_target = false;
		
		// Cooldown
		if(@look_target == null)
		{
			if(look_target_timer < 0)
			{
				look_target_timer += frame_time;
				if(look_target_timer >= 0)
				{
					look_target_timer = 0;
					find_target = true;
				}
			}
			else
			{
				look_target_timer -= frame_time;
				if(look_target_timer <= 0)
				{
					look_target_timer = look_target_timer_interval;
					find_target = true;
				}
			}
		}
		
		if(find_target || (ddraw_look && @look_target == null))
		{
			const float look_target_hitbox_angle = lerp_angle(head.orientation == 1 ? 0.0 : PI, head.angle * DEG2RAD, 0.5);
			const float look_target_hb_x = head.px + cos(look_target_hitbox_angle) * look_target_hitbox_size;
			const float look_target_hb_y = head.py + sin(look_target_hitbox_angle) * look_target_hitbox_size;
			look_target_hitbox_x1 = look_target_hb_x - look_target_hitbox_size;
			look_target_hitbox_y1 = look_target_hb_y - look_target_hitbox_size;
			look_target_hitbox_x2 = look_target_hb_x + look_target_hitbox_size;
			look_target_hitbox_y2 = look_target_hb_y + look_target_hitbox_size;
			
			if(head.orientation == 1)
			{
				look_target_hitbox_x2 += look_target_hitbox_size;
			}
			else 
			{
				look_target_hitbox_x1 -= look_target_hitbox_size;
			}
		}
		
		if(find_target)
		{
			controllable@ closest_c = null;
			float closest_dist = -1;
			int i = script.g.get_entity_collision(look_target_hitbox_y1, look_target_hitbox_y2, look_target_hitbox_x1, look_target_hitbox_x2, ColType::Hittable);
			while(--i >= 0)
			{
				controllable@ c = script.g.get_controllable_collision_index(i);
				if(@c == null)
					return;
				if(c.is_same(self.as_controllable()))
					continue;
				
				float cx, cy;
				c.centre(cx, cy);
				const float dist = distance(head.x, head.y, cx, cy);
				
				if(@closest_c == null || dist < closest_dist)
				{
					@ray = script.g.ray_cast_tiles(head.x, head.y, cx, cy, ray);
					if(!ray.hit())
					{
						@closest_c = c;
						closest_dist = dist;
					}
				}
			}
			
			if(@closest_c != null)
			{
				@look_target = closest_c;
				look_target.centre(look_target_x, look_target_y);
				
				try_set_target_see_dialog(closest_c);
			}
		}
		
		// -- Ray cast to ground for manual look target.
		
		if(@look_target == null || ground_check)
		{
			float angle;
			
			if(!head.is_mostly_vertical())
			{
				const float facing = !head.is_vertical()
					? head.x - neck.x >= 0 ? 1.0 : -1.0
					: head.orientation;
				angle = facing == -1 ? 170 : 10;
			}
			else
			{
				angle = head.y - neck.y >= 0 ? 90 : -90;
			}
			
			look_target_x = head.x + cos(angle * DEG2RAD) * 290;
			look_target_y = head.y + sin(angle * DEG2RAD) * 290;
			
			@ray = script.g.ray_cast_tiles(head.x, head.y, look_target_x, look_target_y, ray);
			if(ray.hit())
			{
				look_target_x = ray.hit_x();
				look_target_y = ray.hit_y() - base_radius;
			}
		}
		
		head_angle_t = atan2(look_target_y - head.y, look_target_x - head.x);
	}
	
	private void lose_target(const bool instant=false)
	{
		look_target_timer -= frame_time;
		
		if(instant || look_target_timer <= 0)
		{
			@look_target = null;
			look_target_timer = -look_target_timer_cooldown;
		}
	}
	
	private void try_set_target_see_dialog(controllable@ c)
	{
		const string key = c.id() + '';
		if(seen_targets.exists(key))
			return;
		
		const string type = c.type_name();
		if(target_dialog_timer > 0)
			return;
		if(speech_bubble.is_open)
			return;
		
		seen_targets[key] = true;
		int dlg_index = -1;
		
		if(type == 'enemy_slime_ball')
		{
			dlg_index = Snake::DlgSlimeBallSee;
		}
		else if(type == 'enemy_slime_beast')
		{
			dlg_index = Snake::DlgSlimeBearSee;
		}
		
		set_target_dialog(dlg_index);
	}
	
	private void try_set_target_kill_dialog(const string &in type, const bool kill)
	{
		int dlg_index = -1;
		
		if(kill)
		{
			if(type == 'enemy_slime_ball')
			{
				dlg_index = Snake::DlgSlimeBallKill;
			}
			else if(type == 'enemy_slime_beast')
			{
				dlg_index = Snake::DlgSlimeBearKill;
			}
			else if(type == 'enemy_tutorial_square')
			{
				dlg_index = Snake::DlgPrismKill;
			}
		}
		else if(type == 'hittable_apple')
		{
			dlg_index = Snake::DlgAppleHit;
		}
		
		set_target_dialog(dlg_index);
	}
	
	private void try_set_target_hurt_dialog(const string &in type)
	{
		if(speech_bubble.is_open)
			return;
		
		int dlg_index = -1;
		
		if(type == 'slime_shock_wave')
		{
			dlg_index = Snake::DlgSlimeBearHurt;
		}
		
		set_target_dialog(dlg_index);
	}
	
	private void set_target_dialog(const int dlg_index)
	{
		if(dlg_index == -1)
			return;
		
		Snake::DialogOptions@ dlg = @target_dialogs[dlg_index - Snake::DlgSlimeBallSee];
		const string txt = dlg.next();
		
		if(txt !=  '')
		{
			set_dialog(txt);
			speech_bubble.draw_next_arrow = false;
			speech_bubble.add_end_delay(speech_bubble.characters_per_second);
			has_target_dialog = true;
		}
		
		target_dialog_timer = target_dialog_cooldown;
	}
	
	private void update_tongue()
	{
		if(tongue_cooldown > 0)
		{
			tongue_cooldown = max(tongue_cooldown - frame_time, 0.0);
			
			if(tongue_cooldown == 0.0)
			{
				tongue_t = 0;
				tongue_flicks = rand_range(Snake::TongueFlicksMin, Snake::TongueFlicksMax);
			}
		}
		
		if(tongue_t >= 0)
		{
			tongue_t = min(tongue_t + DT * (
				tongue_t < 1 || tongue_t >= tongue_flicks + 1
					? Snake::TongueSpeedStart : Snake::TongueSpeed),
				float(tongue_flicks + 2));
			
			if(tongue_t >= tongue_flicks + 2)
			{
				tongue_t = -1;
				tongue_cooldown = rand_range(Snake::TongueCooldownMin, Snake::TongueCooldownMax);
			}
		}
	}
	
	private void update_lights()
	{
		int i = script.g.get_entity_collision(y1, y2, x1, x2, LightColType);
		
		while(lights_list_size <= i)
		{
			lights.resize(lights_list_size *= 2);
		}
		
		lights_count = 0;
		
		while(--i >= 0)
		{
			scripttrigger@ st = script.g.get_scripttrigger_collision_index(i);
			if(@st == null)
				continue;
			Light@ light = cast<Light@>(st.get_object());
			if(@light == null)
				continue;
			if(!light.is_on)
				continue;
			
			@lights[lights_count++] = light;
			
			for(int j = 0; j < num_joints; j++)
			{
				SnakeJoint@ p = joints[j];
				if(p.x - p.radius > light.x2 || p.y - p.radius > light.y2 || p.x + p.radius < light.x1 || p.y + p.radius < light.y1)
					continue;
				
				const float t = 1 - clamp01((distance(p.x, p.y, light.src_x, light.src_y) - light.min_radius) / (light.radius - light.min_radius));
				if(t <= p.light_t)
					continue;
				
				const float offset = lerp(light.offset_min, light.offset_max, t);
				const float angle = atan2(light.src_y - p.y, light.src_x - p.x);
				p.light_t = t;
				p.light_offset_x = cos(angle) * offset;
				p.light_offset_y = sin(angle) * offset;
				p.light_sub_layer = light.sub_layer;
				p.light_colour = multiply_alpha(light.colour, t);
				p.light_scale = light.scale;
			}
		}
	}
	
	/// Checks for contact between the first X joints and thr ground and sets `controllable::ground`.
	/// This allows camera disconnects to work correctly when offscreen.
	private void check_ground()
	{
		float land_vel = 0;
		float land_x, land_y;
		
		bool main_ground_contact = false;
		has_ground_contact = false;
		
		bool found_contact;
		float max_contact_slide = 0;
		
		for(int i = 0; i < num_joints; i++)
		{
			SnakeJoint@ p = joints[i];
			const bool p_ground = p.has_contact ? dot(p.contact_nx, p.contact_ny, 0, -1) > 0.7 : false;
			
			if(p.has_contact)
			{
				found_contact = true;
				
				float dx = p.x - p.prev_x;
				float dy = p.y - p.prev_y;
				project(dx, dy, -p.contact_ny, p.contact_nx, dx, dy);
				const float p_t = p.radius / base_radius;
				const float dist = sqrt(dx * dx + dy * dy) * p_t;
				
				if(dist > max_contact_slide)
				{
					max_contact_slide = dist;
				}
				
				if(clean_filth)
				{
					script.g.project_tile_filth(p.x, p.y, 1, 1, FilthType::Clean, 0, 28, 360, true, true, true, true, false, true);
				}
			}
			
			if(!main_ground_contact && i < ground_check_joints)
			{
				if(p.has_contact && p_ground)
				{
					main_ground_contact = true;
				}
			}
			
			if(p_ground)
			{
				has_ground_contact = true;
				
				if(!p.has_contact_prev)
				{
					const float p_t = p.radius / base_radius;
					if(p.contact_vy * p_t > land_vel)
					{
						land_vel = p.contact_vy * p_t;
						land_x = p.x;
						land_y = p.y;
					}
				}
			}
			
			p.has_contact_prev = p.has_contact;
		}
		
		// Land SFX
		if(land_vel >= Snake::LandMin && land_vel <= Snake::LandMax)
		{
			float land_per = map_clamped(land_vel, Snake::LandMin, Snake::LandMax, 0, 1);
			const bool is_heavy = land_per >= 0.5;
			const string sfx = is_heavy ? 'sfx_land_generic_heavy' : 'sfx_land_carpet';
			land_per = is_heavy ? (land_per - 0.5) * 2 : land_per * 2;
			audio@ snd = script.g.play_sound(sfx, land_x, land_y, land_per, false, true);
			
			if(is_heavy)
			{
				script.add_screen_shake(land_x, land_y, 180, lerp(20, 35, land_per));
			}
		}
		
		// Slide SFX
		if(found_contact)
		{
			has_contact = 3;
		}
		else if(has_contact > 0)
		{
			has_contact = max(has_contact - frame_time, 0.0);
		}
		
		if(!is_dummy && !dead)
		{
			if(found_contact)
			{
				slide_volume_t = map_clamped(max_contact_slide, 0, Snake::SlideMaxForce, 0, Snake::SlideMaxVolume);
				
				if(slide_volume_t < 0.01)
				{
					slide_volume_t = 0;
				}
				
				slide_pitch_t = max_contact_slide <= Snake::SlideMaxForce
					? map_clamped(max_contact_slide, 0, Snake::SlideMaxForce, Snake::SlidePitchUnderMin, 1)
					: map_clamped(max_contact_slide, Snake::SlideMaxForce, Snake::SlidePitchOverForce, 1, Snake::SlidePitchOver);
			}
			else
			{
				slide_volume_t = 0;
			}
			
			const float final_target_volume = slide_volume_t * slide_volume_mult;
			if(slide_volume != final_target_volume)
			{
				const int dir = final_target_volume < slide_volume ? 1 : -1;
				slide_volume += DT / (found_contact ? Snake::SlideVolumeSpeed : Snake::SlideFadeSpeed);
				if(dir == -1 && slide_volume < final_target_volume || dir == 1 && slide_volume > final_target_volume)
				{
					slide_volume = final_target_volume;
				}
				
				if(slide_volume * slide_volume_mult > 0)
				{
					if(@slide_snd == null)
					{
						@slide_snd = script.g.play_sound('sfx_slide_poly_loop', head.x, head.y, 0, true, true);
					}
					
					slide_snd.volume(slide_volume * slide_volume_mult * lerp(0.75, 1.25, cos(t * DT) * 0.5 + 0.5));
				}
				else if(@slide_snd != null)
				{
					slide_snd.stop();
					@slide_snd = null;
				}
			}
			
			if(slide_pitch != slide_pitch_t)
			{
				const int dir = slide_pitch_t < slide_pitch ? 1 : -1;
				slide_pitch += DT / Snake::SlidePitchSpeed;
				if(dir == -1 && slide_pitch < slide_pitch_t || dir == 1 && slide_pitch > slide_pitch_t)
				{
					slide_pitch = slide_pitch_t;
				}
				
				if(@slide_snd != null)
				{
					slide_snd.time_scale(slide_pitch * lerp(0.75, 1.25, sin(t * DT) * 0.5 + 0.5));
				}
			}
			
			if(@slide_snd != null)
			{
				slide_snd.volume(slide_volume * lerp(0.75, 1.25, cos(t * DT * Snake::SlideVolumeWobbleSpeed) * 0.5 + 0.5));
				slide_snd.set_position(head.x, head.y);
			}
		}
		
		self.ground(main_ground_contact);
	}
	
	private void do_movement()
	{
		// Reset any relaxed distance constraints.
		for(int i = num_joints - 2; i >= 0; i--)
		{
			DistanceConstraint@ dc = constraints[i];
			if(dc.min_rest_length > -1)
			{
				dc.min_rest_length += 0.25;
				if(dc.min_rest_length >= dc.rest_length)
				{
					dc.min_rest_length = -1;
				}
			}
		}
		
		move_x = 0;
		move_y = 0;
		float raw_move_x, raw_move_y;
		get_move_dir(raw_move_x, raw_move_y);
		
		if(raw_move_x != 0 || raw_move_y != 0)
		{
			normalize(raw_move_x, raw_move_y, move_x, move_y);
		}
		
		//-- First apply a force to the head. But multiply/reduce the force based on the closest joint
		//-- to the head that's in contact with the ground.
		
		ground_contact_per = 0;
		ground_contact_i = -1;
		
		for(int i = 0; i < max_ground_joints; i++)
		{
			SnakeJoint@ p = joints[i];
			
			ground_contact_per = 1 - float(i) / (max_ground_joints - 1);
			ground_contact_i = i;
			
			if(p.has_contact)
				break;
		}
		
		if(dead)
			return;
		if(raw_move_x == 0 && raw_move_y == 0)
			return;
		
		// Don't normalise - this give a little extra power when climbing
		if(!head.has_contact)
		{
			raw_move_x = move_x;
			raw_move_y = move_y;
		}
		
		const float ground_contact_mult = ease_out_cubic(ground_contact_per);
		head.impulse_x += raw_move_x * Snake::HeadStrength * ground_contact_mult;
		head.impulse_y += raw_move_y * Snake::HeadStrength * ground_contact_mult;
		
		if(is_taunting)
			return;
		
		// -- Handle climbing up a single wide tunnel. If the head is in contact with a wall and the snake is moving up
		// -- Apply a small sticking force to the body parts.
		
		float head_cling_force = 0.0;
		
		// Is the snake touching a surface, and moving up, and is the surface a wall.
		if(head.has_contact && dot(move_x, move_y, 0, -1) >= 0.5 && abs(dot(head.contact_nx, head.contact_ny, 1, 0)) >= 0.75)
		{
			const float ray_tolerance = 4;
			// A full tile width
			const float ray_radius = 48 - base_radius + ray_tolerance;
			
			@ray = script.g.ray_cast_tiles(head.x, head.y, head.x + head.contact_nx * ray_radius, head.y + head.contact_ny * ray_radius, ray);
			head_cling_force = ray.hit() ? Snake::HeadClingForceTunnel : Snake::HeadClingForceWall;
		}
		
		// -- Next starting from the tail apply a force moving the previous joint towards the next one,
		// -- also reducing the force based on how far this joint is from the previous joint that's in contact with the ground.
		
		// If the input direction is mostly in the opposite direction of the head, try reverse.
		float dir = 1;
		float head_dx = head.x - neck.x;
		float head_dy = head.y - neck.y;
		normalize(head_dx, head_dy, head_dx, head_dy);
		if(dot(move_x, move_y, head_dx, head_dy) <= -0.5)
		{
			dir = -0.75;
			
			// Lift the head slighyly when reversing horizontally to avoid awkwardly moving backwards in a straight line.
			if(dot(abs(head_dx), head_dy, 1, 0) > 0.75)
			{
				head.impulse_y -= spring_system.gravity.y + 100;
			}
		}
		
		int prev_contact = num_joints + 100;
		float prev_nx = 0, prev_ny = 0;
		
		SnakeJoint@ p0 = joints[num_joints - 1];
		
		for(int i = num_joints - 1; i >= 1; i--)
		{
			SnakeJoint@ p = p0;
			@p0 = joints[i - 1];
			
			const float pdx = p0.x - p.x;
			const float pdy = p0.y - p.y;
			const float length = sqrt(pdx * pdx + pdy * pdy);
			const float nx = length > 0 ? pdx / length : 0.0;
			const float ny = length > 0 ? pdy / length : 0.0;
			const float dp = dot(prev_nx, prev_ny, nx, ny);
			
			// Check for folds - when this joint is touching/overlapping the next next joint.
			// If it is temporaraly relax the distance constraint. Prevents folds gettings stuck in narrow tunnels.
			if(@p.left != null)
			{
				const float fold_dx = p.left.x - p.right.x;
				const float fold_dy = p.left.y - p.right.y;
				const float fold_dist = sqrt(fold_dx * fold_dx + fold_dy * fold_dy);
				if(fold_dist < p.radius * 0.3 && dp <= -0.9)
				{
					p.dc1.min_rest_length = 0;
					p.dc2.min_rest_length = 0;
				}
			}
			
			if(p.has_contact)
			{
				prev_contact = i;
			}
			
			// Multiply the strength force based on how close this joint is to a ground contact.
			const int joint_count = 3;
			const float t = 1.0 - (prev_contact - i) / joint_count;
			if(t > 0)
			{
				// Don't apply force if two consecutive joints are pointing towards each other - meant to help prevent
				// tangling/getting stuck in tight spaces but didn't help.
				if(dp > -0.5)
				{
					const float joint_strength = p.radius / base_radius * t;
					// Lerp between BodyStrength and BodyStrengthFull based on the joint desired direction, and the head movement direction.
					// The closer they are to each other the stronger the force applied to the joint.
					const float incedent_strength = map_clamped(
						dot(nx, ny, move_x, move_y), 0, 1,
						Snake::BodyStrength, Snake::BodyStrengthFull);
					p.impulse_x += nx * dir * incedent_strength * joint_strength;
					p.impulse_y += ny * dir * incedent_strength * joint_strength;
				}
			}
			
			if(head_cling_force > 0 && !p.has_contact)
			{
				p.impulse_x -= head.contact_nx * head_cling_force;
				p.impulse_y -= head.contact_ny * head_cling_force;
			}
			
			prev_nx = nx;
			prev_ny = ny;
		}
	}
	
	private void get_move_dir(float &out fx, float &out fy)
	{
		const int intent_x = self.x_intent();
		const int intent_y = self.y_intent();
		
		fx = 0;
		fy = 0;
		
		if(intent_y == -1)
			fy -= 1;
		else if(intent_y == 1)
			fy += 1;
		
		if(intent_x == -1)
			fx -= 1;
		else if(intent_x == 1)
			fx += 1;
	}
	
	private void try_attack()
	{
		if(attack_cooldown_timer > 0)
		{
			attack_cooldown_timer = max(attack_cooldown_timer - frame_time, 0.0);
		}
		
		if(@attack_hitbox != null)
		{
			head.impulse_x += attack_force_x;
			head.impulse_y += attack_force_y;
			
			if(attack_hitbox.triggered())
			{
				const float p = 10;
				TileRegion r;
				r.from_area(
					attack_x - attack_radius - p, attack_y - attack_radius - p,
					attack_x + attack_radius + p, attack_y + attack_radius + p);
				const int clean_count = script.dustblocks.clean_rect(r.x1, r.y1, r.x2, r.y2, 0);
				
				if(clean_count > 0)
				{
					// Make sure to expand on all sides by a tile otherwise the now solid inside edges of exposed
					// tiles won't be updated immedialtely.
					r.expand_tiles(1);
					pending_cache_clear.insertLast(r);
					attack_cooldown_timer *= 0.5;
				}
				
				attack_hitbox.activate_time(-9);
				@attack_hitbox = null;
				audio@ snd = script.g.play_sound('sfx_book_attack', attack_x, attack_y, 1, false, true);
				snd.time_scale(rand_range(0.9, 1.1));
				
				jaw_angle_t = 0;
				jaw_speed = 1080;
			}
			else
			{
				bool found_hit = false;
				
				// Check for enemy collision and immediatelly trigger hitbox so we don't pass through enemies while attacking.
				int i = script.g.get_entity_collision(
					head.y - attack_radius, head.y + attack_radius, head.x - attack_radius, head.x + attack_radius, ColType::Hittable);
				while(--i >= 0)
				{
					hittable@ h = script.g.get_entity_collision_index(i).as_hittable();
					if(@h == null)
						continue;
					if(h.team() == self.team())
						continue;
					if(h.destroyed())
						continue;
					if(@locked_attack_target != null && !h.is_same(locked_attack_target))
						continue;
					
					attack_hitbox.state_timer(attack_hitbox.activate_time());
					found_hit = true;
					break;
				}
				
				// Hitting a wall
				if(
					@locked_attack_target == null && !found_hit &&
					head.has_contact && dot(attack_dir_x, attack_dir_y, -head.contact_nx, -head.contact_ny) > 0)
				{
					do_hit_stop();
					
					attack_hitbox.state_timer(attack_hitbox.activate_time());
					found_hit = true;
				}
				
				update_attack_hitbox_pos();
				return;
			}
		}
		
		if(dead)
			return;
		if(attack_cooldown_timer > 0)
			return;
		
		const int heavy_intent = self.heavy_intent();
		if(heavy_intent <= 0 || heavy_intent > 10)
			return;
		
		float fx, fy;
		get_move_dir(fx, fy);
		
		if(fx == 0 && fy == 0)
		{
			fx = head.x - neck.x;
			fy = head.y - neck.y;
		}
		
		attack_radius = base_radius * 1.25;
		attack_dir = normalize_degress(atan2(fx, -fy) * RAD2DEG);
		@attack_hitbox = create_hitbox(
			self.as_controllable(), 0.2,
			head.x, head.y,
			-attack_radius, attack_radius, -attack_radius, attack_radius);
		update_attack_hitbox_pos();
		attack_hitbox.attack_dir(int(attack_dir));
		attack_hitbox.damage(Snake::AttackDamage);
		attack_hitbox.aoe(false);
		attack_hitbox.attack_strength(Snake::AttackStrength);
		attack_hitbox.parry_ff_strength(10);
		attack_hitbox.attack_ff_strength(6);
		attack_hitbox.stun_time(3);
		script.g.add_entity(@attack_hitbox.as_entity(), false);
		jaw_angle_t = 85 * DEG2RAD;
		jaw_speed = 520;
		
		normalize(fx, fy, attack_dir_x, attack_dir_y);
		attack_force_x = attack_dir_x * Snake::AttackHeadForce / DT;
		attack_force_y = attack_dir_y * Snake::AttackHeadForce / DT;
		head.impulse_x += attack_dir_x * Snake::AttackHeadImpulse / DT;
		head.impulse_y += attack_dir_y * Snake::AttackHeadImpulse / DT;
		
		self.heavy_intent(11);
		attack_cooldown_timer = Snake::AttackCooldown;
		tongue_t = max(float(tongue_flicks + 1), tongue_t);
		
		attack_hiss_sfx_i = rand_range_unique(attack_hiss_sfx_i, 4);
		audio@ snd = script.g.play_script_stream(
			'attack_hiss_' + (attack_hiss_sfx_i + 1), 0, head.x, head.y, false,
			rand_range(0.45, 0.55));
		snd.positional(true);
		snd.time_scale(rand_range(1.1, 1.2));
	}
	
	private void update_attack_hitbox_pos()
	{
		const float angle = atan2(head.y - neck.y, head.x - neck.x);
		attack_x = head.x + cos(angle) * base_radius * 0.75;
		attack_y = head.y + sin(angle) * base_radius * 0.75;
		attack_hitbox.set_xy(attack_x, attack_y);
		// Make sure to call this each frame or dustspred won't work correctly.
		attack_hitbox.base_rectangle(-attack_radius, attack_radius, -attack_radius, attack_radius);
	}
	
	private void try_taunt(const bool force=false)
	{
		if(@attack_hitbox != null || dead)
		{
			stop_taunting();
			return;
		}
		
		if(is_taunting)
		{
			// Allow some time off ground before cancelling the taunt.
			taunt_ground_timer = !has_ground_contact
				? taunt_ground_timer - frame_time
				: 12;
			
			if(taunt_ground_timer <= 0 || self.taunt_intent() == 1)
			{
				self.taunt_intent(0);
				stop_taunting();
			}
			
			if(taunt_transition_timer < Snake::TauntTransitionTime)
			{
				taunt_transition_timer = min(taunt_transition_timer + frame_time, Snake::TauntTransitionTime);
				const float stiffness = taunt_transition_timer / Snake::TauntTransitionTime * Snake::TauntStiffness;
				for(int i = 0; i < taunt_joints; i++)
				{
					taunt_constraints[i].stiffness = stiffness;
				}
				
				if(taunt_transition_timer >= Snake::TauntTransitionTime)
				{
					jaw_angle_t = 0 * DEG2RAD;
					jaw_speed = 220;
				}
			}
			
			return;
		}
		
		if(!force)
		{
			if(!has_ground_contact)
				return;
			if(move_x != 0 || move_y != 0)
				return;
			if(self.taunt_intent() == 0)
				return;
		}
		
		start_taunt();
		self.taunt_intent(0);
	}
	
	audio@ play_hiss(int level, const float volume = 0.75)
	{
		level = clamp(level, Snake::HissSubtle, Snake::HissHard);
		
		float max_volume = 1.0;
		float pitch = 1.0;
		string sfx;
		
		switch(level)
		{
			case Snake::HissSubtle: {
				hiss_subtle_sfx_i = rand_range_unique(hiss_subtle_sfx_i, 1, 3);
				max_volume = rand_range(0.9, 1.0);
				pitch = rand_range(0.85, 1.15);
				sfx = 'subtle_' + hiss_subtle_sfx_i;
			} break;
			case Snake::HissMed: max_volume = 0.8; sfx = 'med'; break;
			case Snake::HissHard: max_volume = 0.8; sfx = 'hard'; break;
			case Snake::HissSoft: default:
			max_volume = 1.0; sfx = 'soft'; break;
		}
		
		audio@ snd = script.g.play_script_stream(
			'hiss_' + sfx,
			0, head.x, head.y, false, rand_range(max_volume * 0.8, max_volume) * volume);
		snd.positional(true);
		snd.time_scale(pitch * rand_range(0.9, 1.1));
		return @snd;
	}
	
	SpeechBubble@ set_speech_listener(SpeechBubble::Listener@ listener)
	{
		@speech_bubble.listener = listener;
		return speech_bubble;
	}
	
	bool force_complete_speech()
	{
		return speech_bubble.force_complete();
	}
	
	bool is_touching_wall()
	{
		if(!head.has_contact)
			return false;
		
		return dot(abs(head.contact_nx), head.contact_ny, 1, 0) >= 0.5;
	}
	
	void set_friction(const float fric)
	{
		for(int i = 0 ; i < num_joints; i++)
		{
			joints[i].air_friction = fric;
		}
	}
	
	void lock(const float x, const float y)
	{
		head.reset(x, y);
	}
	
	bool is_offscreen()
	{
		for(int i = 0; i < script.num_players; i++)
		{
			PlayerData@ player = script.players[i];
			
			if(x2 >= player.view_x1 && y2 >= player.view_y1 && x1 <= player.view_x2 && y1 <= player.view_y2)
				return false;
		}
		
		return true;
	}
	
	float on_screen_percent()
	{
		float p = 0;
		
		for(int i = 0; i < script.num_players; i++)
		{
			PlayerData@ player = script.players[i];
			
			if(x2 < player.view_x1 || y2 < player.view_y1 || x1 > player.view_x2 || y1 > player.view_y2)
				continue;
			
			if(x1 >= player.view_x1 && y1 >= player.view_y1 && x2 <= player.view_x2 && y2 <= player.view_y2)
				return 1;
			
			// Off left side of screen
			if(x1 < player.view_x1 && x2 > player.view_x1)
			{
				const float pp = clamp((x2 - player.view_x1) / (x2 - x1), 0.0, 1.0);
				if(pp > p) p = pp;
			}
			// Off right side of screen
			if(x1 < player.view_x2 && x2 > player.view_x2)
			{
				const float pp = clamp((player.view_x2 - x1) / (x2 - x1), 0.0, 1.0);
				if(pp > p) p = pp;
			}
			// Off top side of screen
			if(y1 < player.view_y1 && y2 > player.view_y1)
			{
				const float pp = clamp((y2 - player.view_y1) / (y2 - y1), 0.0, 1.0);
				if(pp > p) p = pp;
			}
			// Off bottom side of screen
			if(y1 < player.view_y2 && y2 > player.view_y2)
			{
				const float pp = clamp((player.view_y2 - y1) / (y2 - y1), 0.0, 1.0);
				if(pp > p) p = pp;
			}
		}
		
		return p;
	}
	
	private void start_taunt()
	{
		audio@ snd = play_hiss(1);
		snd.time_scale(snd.time_scale() * rand_range(0.9, 1.1));
		
		is_taunting = true;
		taunt_transition_timer = 0;
		
		jaw_angle_t = 25 * DEG2RAD;
		jaw_speed = 520;
		
		if(taunt_dir == 0)
		{
			taunt_dir = head.x >= neck.x ? 1 : -1;
		}
		
		SnakeJoint@ p0 = joints[taunt_joints];
		DistanceConstraint@ dc = constraints[taunt_joints - 1];
		
		if(taunt_constraints.length == 0)
		{
			taunt_constraints.resize(taunt_joints);
			for(int i = 0; i < taunt_joints; i++)
			{
				@taunt_constraints[i] = AngularConstraint(null, null, null, 0, 0, Snake::TauntStiffness, 0.0075);
			}
		}
		
		for(int i = taunt_joints - 1; i >= 0; i--)
		{
			SnakeJoint@ p = joints[i];
			const float t = (taunt_dir == -1 ? 1.0 : 0.0) - abs(ease_out_quad(float(i) / (taunt_joints - 1)) * 2 - 1) * -taunt_dir;
			const float angle = t * 180;
			
			AngularConstraint@ ac = @taunt_constraints[i];
			@ac.particle1 = p;
			@ac.particle2 = p0;
			ac.set_range(angle, angle);
			ac.stiffness = 0;
			spring_system.add_constraint(ac);
			
			@dc = i > 0 ? @constraints[i - 1] : null;
			@p0 = p;
		}
	}
	
	private void stop_taunting()
	{
		if(!is_taunting)
			return;
		
		is_taunting = false;
		taunt_dir = 0;
		
		if(@attack_hitbox == null && !dead)
		{
			jaw_angle_t = 0 * DEG2RAD;
			jaw_speed = 520;
		}
		
		for(int i = 0; i < taunt_joints; i++)
		{
			spring_system.remove_constraint(taunt_constraints[i]);
		}
	}
	
	/// Scrapped
	private void try_jump_X()
	{
		if(ground_contact_i < 0 || ground_contact_i > Snake::JumpMinGroundContactJoints)
			return;
		if(self.jump_intent() != 1)
			return;
		
		SnakeJoint@ p = joints[ground_contact_i];
		SnakeJoint@ p0 = joints[ground_contact_i + 1];
		
		float dx = p.x - p0.x;
		float dy = p.y - p0.y;
		
		project(dx, dy, -p.contact_ny, p.contact_nx, dx, dy);
		normalize(dx, dy, dx, dy);
		script.g.draw_line_world(22, 22, head.x,head.y,head.x+dx*48,head.y+dy*48,2,0xffff0000);
		script.g.draw_line_world(22, 22, head.x,head.y,head.x+p.contact_nx*48,head.y+p.contact_ny*48,2,0xffffff00);
		
		float angle = atan2(p.contact_ny, p.contact_nx);
		
		if(dot(dx, dy, move_x, move_y) >= 0.3)
		{
			angle = lerp_angle(angle, atan2(dy, dx), 0.8);
		}
		
		dx = cos(angle);
		dy = sin(angle);
		
		
		const float jump_strength = map_clamped(ground_contact_i, Snake::JumpMinGroundContactJoints, 0, 0.5, 1.0);
		const float JumpForce = 800;
		const float WallJumpForce = 2500;
		const float force = dot(abs(p.contact_nx), p.contact_ny, 1, 0) >= 0.8 ? WallJumpForce : JumpForce;
		
		for(int i = 0; i <= ground_contact_i; i++)
		{
			@p = joints[i];
			p.impulse_x += dx * force / DT;
			p.impulse_y += dy * force / DT;
		}
		
		self.jump_intent(2);
	}
	
	private void do_hit_stop()
	{
		normalize(attack_force_x, attack_force_y, attack_force_x, attack_force_y);
		head.impulse_x -= attack_force_x * Snake::AttackRecoilForce / DT;
		head.impulse_y -= attack_force_y * Snake::AttackRecoilForce / DT;
		head.prev_x = head.x;
		head.prev_y = head.y;
		
		time_scale = 0.6;
		self.time_warp(time_scale);
		
		audio@ snd = script.g.play_sound('sfx_trashbeast_footstep', attack_x, attack_y, 1, false, true);
		snd.time_scale(rand_range(1.3, 1.5));
		
		script.add_screen_shake(attack_x, attack_y, attack_dir, 20);
	}
	
	/// When the player/snake is in the main map area, always try connect to the closest horizontal node.
	/// This needs to be done due to the specific use of one way nodes in this map taht don't work correctly
	/// when skipping sections.
	private void try_connect_camera()
	{
		if(player.cam.camera_type() != 'test')
			return;
		
		camera_node@ node = player.cam.current_node();
		const float base_y = @node != null ? node.y() : 0;
		
		if(base_y >= -500 || base_y <= -700)
			return;
		
		camera_node@ closest_node = null;
		float closest_dist = 10000;
		
		int i = script.g.get_entity_collision(base_y - 50, base_y + 50, head.x - 200, head.x + 200, ColType::CameraNode);
		while(--i >= 0)
		{
			camera_node@ nnode = script.g.get_entity_collision_index(i).as_camera_node();
			if(@nnode == null)
				continue;
			if(!script.in_map_map_bounds(nnode.x(), nnode.y()))
				continue;
			
			const float dist = abs(head.x - nnode.x());
			if(dist <= closest_dist)
			{
				@closest_node = nnode;
				closest_dist = dist;
			}
		}
		
		if(@closest_node == null)
			return;
		
		entity@ enode = closest_node.as_entity();
		camera_node@ next_node = player.cam.next_node();
		if(@next_node != null && next_node.is_same(enode))
			return;
		
		if(node.is_same(enode))
			return;
		
		if(player.cam.try_connect(closest_node))
		{
			player.set_cam_speed(0.1, 2);
		}
	}
	
	private void check_deathzones()
	{
		const float radius = 200;
		const float dx = (head.x - head.prev_x) * 15;
		const float dy = (head.y - head.prev_y) * 15;
		const float x1 = this.x1 - radius + min(dx, 0.0);
		const float y1 = this.y1 - radius + min(dy, 0.0);
		const float x2 = this.x2 + radius + max(dx, 0.0);
		const float y2 = this.y2 + radius + max(dy, 0.0);
		
		float deathzone_per = 0;
		
		int i = script.g.get_entity_collision(y1, y2, x1, x2, ColType::KillZone);
		while(--i >= 0)
		{
			entity@ e = script.g.get_entity_collision_index(i);
			// Not sure why this could return null, but it has.
			if(@e == null)
				return;

			const float x = e.x();
			const float y = e.y();
			const float w = e.vars().get_var('width').get_int32();
			const float h = e.vars().get_var('height').get_int32();

			if(remove_tera_deathzones && h < 24)
			{
				script.g.remove_entity(e);
				continue;
			}

			for(int j = 0; j < num_joints; j++)
			{
				SnakeJoint@ p = joints[j];
				if(p.in_deathzone)
					continue;
				
				const float top = y - h * 0.5;
				if(p.x < x - w * 0.5 || p.y < top || p.x > x + w * 0.5 || p.y > y + h * 0.5)
				{
					if(p.x >= x - w * 0.5 && p.x <= x + w * 0.5)
					{
						p.in_deathzone_y = p.y;
					}
					continue;
				}
				
				p.in_deathzone = true;
				deathzone_per += p.value;
				
				if(p.y > p.in_deathzone_y && p.in_deathzone_y < top)
				{
					script.emitter_bursts.add(Snake::AcidSplashEmitterBurst, p.x, top, 0, p.radius * 2, p.radius * 2);
				}
				
				p.in_deathzone_y = p.y;
			}
		}
		
		if(deathzone_per > 0.35)
		{
			script.g.remove_entity(self.as_entity());
		}
		
		//outline_rect(script.g, 22, 22, x1, y1, x2, y2, 2, 0xffff0000);
	}
	
	private void update_speech_bubble()
	{
		if(!speech_bubble.is_visible)
			return;
		
		speech_bubble.step(frame_time);
		
		if(speech_meta_count == 0)
			return;
		
		const int chr_i = speech_bubble.progress;
		if(speech_bubble_progress_prev == chr_i)
			return;
		
		int hiss_level = -1;
		
		while(speech_meta_index < speech_meta_count)
		{
			Snake::SpeechMeta@ meta = @speech_meta_list[speech_meta_index];
			
			if(meta.index >= chr_i)
				break;
			
			if(meta.hiss_level > hiss_level)
			{
				hiss_level = meta.hiss_level;
			}
			
			speech_meta_index++;
		}
		
		if(hiss_level >= 0)
		{
			play_hiss(hiss_level);
		}
	}
	
	void draw(float sub_frame)
	{
		if(!is_created || !is_active)
			return;
		
		bool is_last = true;
		SnakeJoint@ p0 = joints[num_joints - 1];
		p0.px = lerp(p0.prev_x, p0.x, sub_frame);
		p0.py = lerp(p0.prev_y, p0.y, sub_frame);
		
		for(int i = num_joints - 2; i >= 0; i--)
		{
			SnakeJoint@ p = joints[i];
			p.calc_values(p0.px, p0.py, sub_frame);
			
			const float scale_x = p.length / ((p.radius + p0.radius) * joint_spacing_p) * p0.scale;
			const int orientation = p.orientation >= 0 ? 1 : -1;
			const int orientation_t = p.orientation_t >= 0 ? 1 : -1;
			const float scale_y = orientation * p0.scale;
			
			spr.draw_world(18, 11,
				'snake_segment_shadow', 0, 0,
				p.px, p.py + 1, p.angle, scale_x * 0.7, scale_y * 0.95, 0x40ffffff);
			
			string spr_name;
			if(orientation != orientation_t)
			{
				spr_name = p.radius == p0.radius ? 'snake_segment_flat' : (!is_last ? 'snake_segment' : 'snake_segment_last');
				spr.draw_world(18, 11,
					spr_name, 0, 0,
					p.px, p.py, p.angle, scale_x, scale_y, 0xffffffff);
				spr.draw_world(18, 11,
					spr_name, 0, 0,
					p.px, p.py, p.angle, scale_x, -scale_y, uint((1 - abs(lerp(p.orientation_prev, p.orientation, sub_frame))) * 255) << 24 | 0xffffff);
			}
			else
			{
				spr_name =p.radius ==  p0.radius ? 'snake_segment_flat' : (!is_last ? 'snake_segment' : 'snake_segment_last');
				spr.draw_world(18, 11,
					spr_name, 0, 0,
					p.px, p.py, p.angle, scale_x, scale_y, 0xffffffff);
			}
			
			if(p.light_t > 0)
			{
				spr.draw_world(18, p.light_sub_layer,
					spr_name, 0, 0,
					p.px + p.light_offset_x, p.py + p.light_offset_y, p.angle,
					scale_x * p.light_scale, scale_y * p.light_scale, p.light_colour);
			}
			
			if(i == 0)
			{
				const float base_angle = lerp_angle(head_angle_prev, head_angle, sub_frame);
				const float jaw_angle_f = base_angle + lerp_angle(jaw_angle_prev, jaw_angle, sub_frame) * orientation_t;
				float jaw_x, jaw_y;
				rotate(jaw_ox, jaw_oy * orientation_t, base_angle, jaw_x, jaw_y);
				jaw_x = p.px + jaw_x;
				jaw_y = p.py + jaw_y;
				
				if(tongue_t > 0)
				{
					spr.draw_world(18, 11,
						tongue_t < 1 ? 'snake_tongue_1' : 'snake_tongue_' + int((tongue_t - 1) % 2 + 1),
						0, 0,
						p.px, p.py, base_angle * RAD2DEG,
						tongue_t < 1 ? tongue_t
							: tongue_t > tongue_flicks + 1 ? (1 - (tongue_t - tongue_flicks - 1))
							: 1.0,
						orientation_t, 0xffffffff);
				}
				
				spr.draw_world(18, 11,
					'snake_gums', 0, 0,
					jaw_x, jaw_y, jaw_angle_f * RAD2DEG,
					1, orientation_t * p0.scale, 0xffffffff);
				spr.draw_world(18, 11,
					'snake_teeth', 0, 0,
					p.px, p.py, base_angle * RAD2DEG,
					1, orientation_t * p0.scale, 0xffffffff);
				spr.draw_world(18, 11,
					'snake_jaw', 0, 0,
					jaw_x, jaw_y, jaw_angle_f * RAD2DEG,
					1, orientation_t * p0.scale, 0xffffffff);
				spr.draw_world(18, 11,
					'snake_head', 0, 0,
					p.px, p.py, base_angle * RAD2DEG,
					1, orientation_t * p0.scale, 0xffffffff);
				
				if(p.light_t > 0)
				{
					spr.draw_world(18, p.light_sub_layer,
						'snake_jaw', 0, 0,
						jaw_x + p.light_offset_x, jaw_y + p.light_offset_y, jaw_angle_f * RAD2DEG,
						1 * p.light_scale, orientation_t * p0.scale * p.light_scale, p.light_colour);
					spr.draw_world(18, p.light_sub_layer,
						'snake_head', 0, 0,
						p.px + p.light_offset_x, p.py + p.light_offset_y, base_angle * RAD2DEG,
						1 * p.light_scale, orientation_t * p0.scale * p.light_scale, p.light_colour);
				}
				
				if(attack_cooldown_timer <= 0)
				{
					spr.draw_world(18, 11,
						'snake_eye', 0, 0,
						p.px, p.py, base_angle * RAD2DEG,
						1, orientation_t * p0.scale, 0xffffffff);
				}
			}
			
			@p0 = @p;
			is_last = false;
		}
		
		if(speech_bubble.is_visible)
		{
			speech_bubble.draw_scary(
				lerp(head.prev_x, head.x, sub_frame),
				lerp(head.prev_y, head.y, sub_frame) - base_radius * 2,
				sub_frame);
		}
		
		if(debug_draw)
		{
			if(ddraw_indices && @tf == null)
			{
				@tf = create_textfield();
				tf.colour(0x66ffffff);
			}
			
			float px = 0, py = 0;
			for(int i = 0; i < num_joints; i++)
			{
				SnakeJoint@ p = joints[i];
				const float x = lerp(p.prev_x, p.x, sub_frame);
				const float y = lerp(p.prev_y, p.y, sub_frame);
				
				if(ddraw_joints)
				{
					if(i > 0)
					{
						script.g.draw_line_world(22, 22, px, py, x, y, 2, 0x77ffffff);
					}
					
					drawing::circle(script.g, 22, 22, x, y, p.radius, 32, 1, 0x44ffffff);
				}
				
				if(ddraw_contacts)
				{
					if(p.has_contact)
					{
						const float cx = x - p.contact_nx * p.radius;
						const float cy = y - p.contact_ny * p.radius;
						script.g.draw_line_world(22, 22,
							cx, cy, cx + p.contact_nx * p.radius * 0.75, cy + p.contact_ny * p.radius * 0.75,
							2, 0x88ff0000);
					}
				}
				
				if(ddraw_indices)
				{
					tf.text(i + '');
					tf.draw_world(22, 22, x, y, 0.5, 0.5, 0);
				}
				
				px = x;
				py = y;
			}
			
			if(ddraw_look)
			{
				if(@look_target == null)
				{
					outline_rect(script.g, 22, 22,
						look_target_hitbox_x1, look_target_hitbox_y1, look_target_hitbox_x2, look_target_hitbox_y2,
						2, 0x44ff0000);
				}
				
				script.g.draw_line_world(22, 22, head.px, head.py, look_target_x, look_target_y, 2, 0x44ffaa00);
				script.g.draw_rectangle_world(22, 22, look_target_x - 4, look_target_y - 4, look_target_x + 4, look_target_y + 4, 45, 0xaaffaa00);
				
				script.g.draw_line_world(22, 22,
					head.px, head.py,
					head.px + cos((head.angle - head_angle_range) * DEG2RAD) * 96,
					head.py + sin((head.angle - head_angle_range) * DEG2RAD) * 96,
					2, 0x55ff00ff);
				script.g.draw_line_world(22, 22,
					head.px, head.py,
					head.px + cos((head.angle + head_angle_range) * DEG2RAD) * 96,
					head.py + sin((head.angle + head_angle_range) * DEG2RAD) * 96,
					2, 0x55ff00ff);
			}
		}
		
		//outline_rect(script.g, 22, 22, x1, y1, x2, y2, 2, 0xffff0000);
	}
	
	void editor_step()
	{
		if(script.input.key_check_pressed_gvb(GVB::MiddleClick))
		{
			const float mx = script.input.mouse_x_world(19);
			const float my = script.input.mouse_y_world(19);
			int i = script.g.get_entity_collision(my, my, mx, mx, ColType::Hittable);
			while(--i >= 0)
			{
				entity@ e = script.g.get_entity_collision_index(i);
				if(e.is_same(self.as_entity()))
				{
					self.face(-self.face());
					break;
				}
			}
		}
	}
	
	void editor_draw(float sub_frame)
	{
		const float x = self.x();
		const float y = self.y();
		const int f = self.face();
		script.g.draw_rectangle_world(22, 22, x - 24, y - 24, x + 24, y + 24, 0, 0x33ffff55);
		script.g.draw_rectangle_world(22, 22, x + 24 * f, y - 5, x + 42 * f, y + 5, 0, 0x33ffff55);
	}
	
	//
	
	bool try_parse_speech_attribute(SpeechBubble@ speech_bubble, int attrib_chr, const string &in text, const int text_length)
	{
		if(attrib_chr != Snake::ChrHissAttrib)
			return false;
		
		const int next_chr = text[speech_bubble.parse_index];
		int level = -1;
		
		switch(next_chr)
		{
			case Snake::ChrHissSubtle: {
				level = Snake::HissSubtle;
			} break;
			case Snake::ChrHissSoft: {
				level = Snake::HissSoft;
			} break;
			case Snake::ChrHissMed: {
				level = Snake::HissMed;
			} break;
			case Snake::ChrHissHard: {
				level = Snake::HissHard;
			} break;
			default: {
				speech_bubble.log_parse_warning('Expected one of hiss level "s", S", "M", or "H" after "h"');
			} break;
		}
		
		if(level == -1)
			return true;
		
		if(speech_meta_count >= speech_meta_size)
		{
			speech_meta_size *= 2;
			speech_meta_list.resize(speech_meta_size);
		}
		
		Snake::SpeechMeta@ meta = @speech_meta_list[speech_meta_count++];
		meta.index = speech_bubble.parse_chr_index;
		meta.hiss_level = level;
		
		speech_bubble.parse_index++;
		return true;
	}

}
