#include '../lib/std.cpp';
#include '../lib/debug/Debug.cpp';
#include '../lib/emitters/common.cpp';
#include '../lib/enums/ColType.cpp';
#include '../lib/enums/EmitterId.cpp';
#include '../lib/enums/EntityState.cpp';
#include '../lib/input/Mouse.cpp';
#include '../lib/math/math.cpp';
#include '../lib/math/line_tests.cpp';
#include '../lib/phys/springs/SpringSystem.cpp';
#include '../lib/phys/springs/ITileProvider.cpp';
#include '../lib/phys/springs/constraints/BoxesCollisionConstraint.cpp';

#include '../module/camera/CameraDisconnectTrigger.cpp';
#include '../module/entity_outliner/EntityOutliner.cpp';
#include '../module/tiles/BreakableWall.cpp';
#include '../module/tiles/BreakableWallActivator.cpp';

#include 'settings/Collision.cpp';
#include 'BreakableAnchor.cpp';
#include 'BreakableFloor.cpp';
#include 'CollisionData.cpp';
#include 'CollisionEdge.cpp';
#include 'FinalApple.cpp';
#include 'funcs.cpp';
#include 'ICollisionEntity.cpp';
#include 'ILifecycleEntity.cpp';
#include 'JumpPad.cpp';
#include 'Mover.cpp';
#include 'Player.cpp';
#include 'PlayerWind.cpp';
#include 'Ribbon.cpp';
#include 'Rope.cpp';
#include 'ScarfGet.cpp';
#include 'SpriteData.cpp';
#include 'StoneBlock.cpp';
#include 'TempEmitter.cpp';

class script : callback_base, ITileProvider
{
	
	[persist] bool debug_draw_edges;
	[persist] bool debug_draw_springs;
	[persist] bool debug_wind;
	[persist] bool debug_view;
	[persist] float attack_force = 2;
	[persit] float wind_strength = 6000;
	[persist] int chirp_med_scarf_length = 4;
	[persist] int chirp_big_scarf_length = 6;
	[persist] float chirp_big_time = 45;
	
	[text|label:Outliner] EntityOutlinerSettings settings;

	scene@ g;
	camera@ cam;
	int num_players;
	array<Player> players;
	Debug debug;
	private EntityOutliner entity_outliner;
	Mouse mouse;
	editor_api@ editor;
	input_api@ input;
	float ed_zoom = 1;
	
	SpriteData spr_cache;
	
	private dictionary registered_collision_handlers;
	private int size_collision_handlers = 32;
	private int num_collision_handlers;
	private array<CollisionData@> collision_handlers(size_collision_handlers);
	private int size_custom_edges = 32;
	private int num_custom_edges;
	private array<CollisionEdge@> custom_edges(size_custom_edges);
	
	private int size_lifecycle_entities = 32;
	private int num_lifecycle_entities;
	private array<ILifecycleEntity@> lifecycle_entities(size_lifecycle_entities);
	private int size_queued_activate = 32;
	private int num_queued_activate;
	private array<ILifecycleEntity@> queued_activate(size_queued_activate);
	/*// TODO: Remove if not used
	private int size_pre_step_listeners = 32;
	private int num_pre_step_listeners;
	private array<IPreStepHandler@> pre_step_listeners(size_pre_step_listeners);
	private int size_post_step_listeners = 32;
	private int num_post_step_listeners;
	private array<IPostStepHandler@> post_step_listeners(size_post_step_listeners);*/
	
	array<TempEmitter> remove_emitters;
	int collision_layer = 19;
	float frame = 0;
	bool is_playing;
	
	SpringSystem spring_system;
	BoxesCollisionConstraint boxes_constraint;
	array<Ribbon@> ribbons;
	
	/// Global reusable raycast result
	raycast@ ray;
	sprites@ spr;

	script()
	{
		@g = get_scene();
		@cam = get_active_camera();
		num_players = num_cameras();
		players.resize(num_players);
		
		@editor = get_editor_api();
		@input = get_input_api();
		mouse.hud = false;
		
		@ray = g.ray_cast_tiles_ex(0, 0, 1, 0, 19, ray);
		@spr = create_sprites();
		spr.add_sprite_set('script');

		for(int i = 0; i < num_players; i++)
		{
			players[i].init(this, i);
		}
		
		boxes_constraint.friction = 50;
		spring_system.gravity.y = 96 * 9.81;
		spring_system.constraint_iterations = 3;
		spring_system.add_constraint(boxes_constraint);
		
		entity_outliner.init_settings(@settings);
		
		g.reset_layer_order();
		for(uint i = 16; i < 18; i++)
		{
			g.swap_layer_order(i, i + 1);
		}
		
		g.override_stream_sizes(16, 8);
	}
	
	void on_level_start()
	{
		//puts('-- on_level_start --------------');
		is_playing = true;
		spr.add_sprite_set('script');
		
		//g.override_stream_sizes(16, 8);
	}

	void checkpoint_load()
	{
		//puts('-- checkpoint_load --------------');
		
		registered_collision_handlers.deleteAll();
		num_collision_handlers = 0;
		
		num_custom_edges = 0;
		num_lifecycle_entities = 0;
		
		remove_emitters.resize(0);
		spring_system.clear();
		boxes_constraint.clear();
		spring_system.add_constraint(boxes_constraint);
		
		ribbons.resize(0);
		
		@cam = get_active_camera();
		entity_outliner.checkpoint_load(cam);
		
		for(int i = 0; i < num_players; i++)
		{
			players[i].checkpoint_load();
		}
	}
	
	void checkpoint_save()
	{
		entity_outliner.checkpoint_save();
		
		for(int i = 0; i < num_players; i++)
		{
			players[i].checkpoint_save();
		}
		
		for(int i = num_lifecycle_entities - 1; i >= 0; i--)
		{
			lifecycle_entities[i].on_checkpoint_save();
		}
	}
	
	void step(int num_entities)
	{
		@cam = get_active_camera();
		
		collision_layer = g.default_collision_layer();
		
		update_collision_handlers(num_entities);
		
		for(int i = int(remove_emitters.length - 1); i >= 0; i--)
		{
			TempEmitter@ t = @remove_emitters[i];
			t.timer -= DT;
			if(t.timer <= 0)
			{
				g.remove_entity(t.e);
				remove_emitters.removeAt(i);
			}
		}
		
		for(int i = 0; i < num_players; i++)
		{
			players[i].step(num_entities);
		}
		
		spring_system.step(g.time_warp());
		
		entity_outliner.step(cam);
		
		debug.step();
		frame++;
	}
	
	void step_post(int num_entities)
	{
		if(num_queued_activate > 0)
		{
			for(int i = 0; i < num_queued_activate; i++)
			{
				ILifecycleEntity@ listener = queued_activate[i];
				listener.active = true;
				listener.on_activate();
				
				if(listener.in_view)
				{
					listener.on_enter_view();
				}
			}
			
			num_queued_activate = 0;
		}
		
		for(int i = num_lifecycle_entities - 1; i >= 0; i--)
		{
			ILifecycleEntity@ listener = lifecycle_entities[i];
			
			if(!listener.active)
				continue;
			
			if(!listener.was_run)
			{
				listener.active = false;
				listener.trigger_on_leave_view();
				listener.on_deactivate();
			}
			else
			{
				listener.was_run = false;
			}
		}
		
		for(int i = 0; i < num_players; i++)
		{
			players[i].step_post(num_entities);
		}
	}
	
	void editor_step()
	{
		ed_zoom = 1 / cam.editor_zoom();
		
		mouse.step();
		frame++;
		
		entity_outliner.editor_step(cam);
	}
	
	void draw(float sub_frame)
	{
		for(int i = 0; i < num_players; i++)
		{
			players[i].draw(sub_frame);
		}
		
		entity_outliner.draw(sub_frame);
		
		if(debug_draw_springs)
			spring_system.debug_draw(g);
		if(debug_draw_edges)
			debug_draw_custom_collision();
		
		debug.draw(sub_frame);
	}
	
	private void debug_draw_custom_collision()
	{
		for(int i = num_custom_edges - 1; i >= 0; i--)
		{
			CollisionEdge@ edge = custom_edges[i];
			uint clr;
			
			switch(edge.side)
			{
				case SideType::Left:   clr = 0xff55ffff; break;
				case SideType::Right:  clr = 0xffff55ff; break;
				case SideType::Roof:   clr = 0xffffff55; break;
				case SideType::Ground: clr = 0xffff5555; break;
			}
			
			g.draw_line_world(21, 22, edge.x1, edge.y1, edge.x2, edge.y2, 2, clr);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		//array<uint> ids = {3488, 3489};
		//for(uint i = 0; i < ids.length; i++)
		//{
		//	entity@ e = entity_by_id(ids[i]);
		//	if(@e != null)
		//	{
		//		draw_dot(g, 22, 22, e.x(), e.y(), 48, 0x3300ff00, 0);
		//		if(ids[i] == 3574)
		//			g.remove_entity(e);
		//		puts(ids[i]);
		//	}
		//}
		
		entity_outliner.draw(sub_frame);
	}
	
	private bool check_collision(
		const float w1, const float w2, const float r1, const float r2,
		const SideType side, tilecollision@ tc, CollisionData@ data, CollisionEdge@ &out out_edge)
	{
		const bool vertical = side == SideType::Ground || side == SideType::Roof;
		const float mx1 = vertical ? min(w1, w2) : min(r1, r2);
		const float my1 = vertical ? min(r1, r2) : min(w1, w2);
		const float mx2 = vertical ? max(w1, w2) : max(r1, r2);
		const float my2 = vertical ? max(r1, r2) : max(w1, w2);
		
		const bool is_pos = my1 >= 0;
		const int num_rays = vertical ? Collision::Rays : Collision::HorRays + (is_pos ? 0 : 1);
		const int mid_ray = (num_rays - 1) / 2;
		const int index_factor = num_rays % 2 == 0 ? -1 : 1;
		const int side_factor = side == SideType::Left ? 1 : -1;
		
		for(int i = num_custom_edges - 1; i >= 0; i--)
		{
			CollisionEdge@ edge = custom_edges[i];
			
			if((edge.mask & data.type) == 0)
				continue;
			if(edge.side != side)
				continue;
			if(mx1 > edge.bounds_x2 || mx2 < edge.bounds_x1 || my1 > edge.bounds_y2 || my2 < edge.bounds_y1)
				continue;
			
			for(int j = 0; j < num_rays; j++)
			{
				// Start from the middle and move out
				const int ji = mid_ray +
					(j % 2 == 0 ? j / 2 : -(j / 2 + 1)) * index_factor;
				
				const float side_offset = !vertical ? (0.9 * (abs(ji  - mid_ray) + (ji > mid_ray && !is_pos ? -1 : 0))) * side_factor : 0;
				const float wt = w1 + ji / (num_rays - 1.0) * (w2 - w1);
				const float x1 = vertical ? wt : r1;
				const float y1 = vertical ? r1 : wt + (ji > mid_ray && !is_pos ? -0.5 : 0);
				const float x2 = vertical ? wt : r2 + side_offset;
				const float y2 = vertical ? r2 : wt + (ji > mid_ray && !is_pos ? -0.5 : 0);
				
				float x, y, t;
				if(!line_line_intersection(
					edge.x1, edge.y1, edge.x2, edge.y2,
					x1, y1, x2, y2,
					x, y, t))
						continue;
				
				// Extrapolate if this is not the middle ray to find where the intersection
				// point would be along the centre ray
				if(vertical && (j != mid_ray || j % num_rays == 0))
				{
					ray_ray_intersection(
						edge.x1, edge.y1, edge.x2, edge.y2,
						vertical ? (w1 + w2) * 0.5 : r1,
						vertical ? r1 : (w1 + w2) * 0.5,
						vertical ? (w1 + w2) * 0.5 : r2,
						vertical ? r2 : (w1 + w2) * 0.5,
						x, y, t);
				}
				
				tc.hit(true);
				tc.type(edge.angle);
				tc.hit_x(x - side_offset);
				tc.hit_y(y);
				@out_edge = edge;
				edge.add_contact(data, x - side_offset, y, t);
				
				return true;
			}
		}
		
		@out_edge = null;
		return false;
	}
	
	private void register_collision_handler(controllable@ c, const int index)
	{
		// Slope min/max, slant min/max angles.
		// Required for non-45deg slope sliding
		c.set_ground_angles(
			Collision::GroundSlopeMin, Collision::GroundSlopeMax,
			Collision::GroundSlantMin, Collision::GroundSlantMax);
		// Slant down facing min/max, slant up facing min/max angles
		// Required or walls > 116deg won't collide
		c.set_wall_angles(
			Collision::WallSlantDownMin, Collision::WallSlantDownMax,
			Collision::WallSlantUpMin, Collision::WallSlantUpMax);
		// Roof slope min/max, roof slant min/max angles
		// Required to make the player rotate on ceilings > 154
		c.set_roof_angles(
			Collision::RoofSlopeMin, Collision::RoofSlopeMax,
			Collision::RoofSlantMin, Collision::RoofSlantMax);
		
		c.set_collision_handler(this, 'collision_handler', index);
		c.set_texture_type_handler(this, 'texture_type_handler', index);
	}
	
	private void update_collision_handlers(const int num_entities)
	{
		for(int i = num_collision_handlers - 1; i >= 0; i--)
		{
			CollisionData@ data = collision_handlers[i];
			
			if(!data.stepped)
			{
				const string key = data.c.player_index() != -1
					? -(data.c.player_index() + 1) + '' : data.c.id() + '';
				registered_collision_handlers.delete(key);
				
				//puts('Removing ' + key + ' ' + data.c.type_name());
				
				@data = collision_handlers[--num_collision_handlers];
				register_collision_handler(data.c, i);
				@collision_handlers[i] = data;
				continue;
			}
			
			data.stepped = false;
		}
		
		for(int i = 0; i < num_entities; i++)
		{
			controllable@ c = entity_by_index(i).as_controllable();
			
			if(@c == null)
				continue;
			
			const string key = c.player_index() != -1
				? -(c.player_index() + 1) + '' : c.id() + '';
			
			if(registered_collision_handlers.exists(key))
			{
				cast<CollisionData@>(registered_collision_handlers[key]).stepped = true;
				continue;
			}
			
			if(num_collision_handlers + 1 >= size_collision_handlers)
				collision_handlers.resize(size_collision_handlers *= 2);
			
			//puts('Adding ' + key + ' ' + c.type_name());
			
			register_collision_handler(c, num_collision_handlers);
			CollisionData@ data = CollisionData(c);
			@registered_collision_handlers[key] = data;
			@collision_handlers[num_collision_handlers++] = data;
		}
	}
	
	void texture_type_handler(controllable@ c, texture_type_query@ query, int entity_index)
	{
		CollisionData@ data = collision_handlers[entity_index];
		
		if(@data.collision_ground != null)
			query.result(data.collision_ground.surface_type);
		else if(@data.collision_roof != null)
			query.result(data.collision_roof.surface_type);
		else if(@data.collision_left != null)
			query.result(data.collision_left.surface_type);
		else if(@data.collision_right != null)
			query.result(data.collision_right.surface_type);
	}
	
	void collision_handler(controllable@ c, tilecollision@ tc,
		int side, bool moving, float snap_offset, int entity_index)
	{
		CollisionData@ data = collision_handlers[entity_index];
		
		if(moving)
		{
			snap_offset += 10;
		}
		
		const float x = c.x();
		const float y = c.y();
		const float x_speed = c.x_speed();
		const float y_speed = c.y_speed();
		rectangle@ rect = c.collision_rect();
		const float l = rect.left();
		const float r = rect.right();
		const float t = rect.top();
		const float b = rect.bottom();
		const float mh_x = int((l + r) * 0.5);
		const float mh_y = int((t + b) * 0.5);
		const float mh_w = int((r - l) / 4);
		const float mh_h = int((b - t) / 8);
		
		bool hit = false;
		
		switch(side)
		{
			case SideType::Left:
				hit = check_collision(
					y + mh_y - mh_h,
					y + mh_y + mh_h,
					x + mh_x - (moving ? min(0.0, x_speed * DT) : 0),
					x + l - snap_offset,
					SideType::Left, tc, data, data.collision_left);
				break;
			case SideType::Right:
				hit = check_collision(
					y + mh_y - mh_h,
					y + mh_y + mh_h,
					x + mh_x - (moving ? max(0.0, x_speed * DT) : 0),
					x + r + snap_offset,
					SideType::Right, tc, data, data.collision_right);
				break;
			case SideType::Roof:
				hit = check_collision(
					x + mh_x - mh_w,
					x + mh_x + mh_w,
					y + mh_y - (moving ? min(0.0, y_speed * DT) : 0),
					y + t - snap_offset,
					SideType::Roof, tc, data, data.collision_roof);
				break;
			case SideType::Ground:
				hit = check_collision(
					x + mh_x - mh_w,
					x + mh_x + mh_w,
					y + mh_y - (moving ? max(0.0, y_speed * DT) : 0),
					y + b + snap_offset,
					SideType::Ground, tc, data, data.collision_ground);
				break;
		}
		
		if(!hit)
		{
			if(moving)
			{
				snap_offset -= 10;
			}
			
			c.check_collision(tc, side, moving, snap_offset);
		}
	}
	
	void add_collision_edge(CollisionEdge@ edge)
	{
		if(num_custom_edges + 1 >= size_custom_edges)
			custom_edges.resize(size_custom_edges *= 2);
		
		@custom_edges[num_custom_edges++] = edge;
	}
	
	void remove_collision_edge(CollisionEdge@ edge)
	{
		if(num_custom_edges == 0)
			return;
		
		const int index = custom_edges.findByRef(edge);
		
		if(index != -1)
		{
			@custom_edges[index] = custom_edges[--num_custom_edges];
		}
	}
	
	void register_lifecycle_entity(ILifecycleEntity@ e)
	{
		if(num_lifecycle_entities + 1 >= size_lifecycle_entities)
			lifecycle_entities.resize(size_lifecycle_entities *= 2);
		
		@lifecycle_entities[num_lifecycle_entities++] = e;
	}
	
	void unregister_lifecycle_entity(ILifecycleEntity@ e)
	{
		const int index = lifecycle_entities.findByRef(e);
				
		if(index >= 0)
		{
			@lifecycle_entities[index] = lifecycle_entities[--num_lifecycle_entities];
		}
	}
	
	void queue_lifecycle_activate(ILifecycleEntity@ e)
	{
		if(num_queued_activate + 1 >= size_queued_activate)
			queued_activate.resize(size_queued_activate *= 2);
		
		@queued_activate[num_queued_activate++] = e;
	}
	
	/*// TODO: Remove if not used
	void pre_step_subscribe(IPreStepHandler@ listener)
	{
		if(num_pre_step_listeners + 1 >= size_pre_step_listeners)
		{
			pre_step_listeners.resize(size_pre_step_listeners *= 2);
		}
		
		@pre_step_listeners[num_pre_step_listeners++] = listener;
	}
	
	void pre_step_unsubscribe(IPreStepHandler@ listener)
	{
		const int index = pre_step_listeners.findByRef(listener);
				
		if(index >= 0)
		{
			@pre_step_listeners[index] = @pre_step_listeners[--num_pre_step_listeners];
		}
	}
	
	void post_step_subscribe(IPostStepHandler@ listener)
	{
		if(num_post_step_listeners + 1 >= size_pre_step_listeners)
		{
			post_step_listeners.resize(size_post_step_listeners *= 2);
		}
		
		@post_step_listeners[num_post_step_listeners++] = listener;
	}
	
	void post_step_unsubscribe(IPostStepHandler@ listener)
	{
		const int index = post_step_listeners.findByRef(listener);
				
		if(index >= 0)
		{
			@post_step_listeners[index] = @post_step_listeners[--num_post_step_listeners];
		}
	}*/
	
	TileData@ get_tile(const int tx, const int ty) override
	{
		return TileData(g.get_tile(tx, ty, collision_layer));
	}
	
	void add_emitter_burst(
		const int emitter_id,
		const int layer, const int sub_layer,
		const float x, const float y,
		const float size_x, const float size_y, float rotation, const float time=0.25, const int count=1)
	{
		for(int i = 0; i < count; i++)
		{
			entity@ e = create_emitter(emitter_id,
				x, y,
				int(size_x), int(size_y),
				layer, sub_layer, int(rotation)
			);
			g.add_entity(e, false);
			remove_emitters.insertLast(TempEmitter(e, time));
		}
	}
	
	void editor_var_changed(var_info@ info)
	{
		entity_outliner.editor_var_changed(info, @settings);
	}
	
}
