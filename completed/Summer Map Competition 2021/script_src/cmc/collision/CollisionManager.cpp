#include '../../lib/math/math.cpp';
#include '../../lib/drawing/common.cpp';

#include 'Collision.cpp';
#include 'SideType.cpp';
#include 'CollisionData.cpp';
#include 'CollisionEdge.cpp';

class CollisionManager : callback_base
{
	
	private scene@ g;
	
	private dictionary registered_collision_handlers;
	private int size_collision_handlers = 32;
	private int num_collision_handlers;
	private array<CollisionData@> collision_handlers(size_collision_handlers);
	private int size_custom_edges = 32;
	private int num_custom_edges;
	private array<CollisionEdge@> custom_edges(size_custom_edges);
	
	CollisionManager()
	{
		@g = get_scene();
	}
	
	void checkpoint_load()
	{
		registered_collision_handlers.deleteAll();
		num_collision_handlers = 0;
		num_custom_edges = 0;
	}
	
	void update_collision_handlers(const int num_entities)
	{
		for(int i = num_collision_handlers - 1; i >= 0; i--)
		{
			CollisionData@ data = collision_handlers[i];
			
			if(data.contact_count != 0)
			{
				data.c.set_xy(
					data.c.x() + data.contact_dx / data.contact_count,
					data.c.y() + data.contact_dy / data.contact_count);
			}
			
			if(!data.stepped)
			{
				const string key = data.c.player_index() != -1
					? -(data.c.player_index() + 1) + '' : data.c.id() + '';
				registered_collision_handlers.delete(key);
				
				@data = collision_handlers[--num_collision_handlers];
				register_collision_handler(data.c, i);
				@collision_handlers[i] = data;
				continue;
			}
			
			data.stepped = false;
			data.contact_dx = 0;
			data.contact_dy = 0;
			data.contact_count = 0;
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
			
			register_collision_handler(c, num_collision_handlers);
			CollisionData@ data = CollisionData(c);
			@registered_collision_handlers[key] = data;
			@collision_handlers[num_collision_handlers++] = data;
		}
	}
	
	void step_post(int entities)
	{
		//for(int i = num_collision_handlers - 1; i >= 0; i--)
		//{
		//	CollisionData@ data = collision_handlers[i];
		//	
		//	if(data.contact_count != 0)
		//	{
		//		data.c.set_xy(
		//			data.c.x() + data.contact_dx / data.contact_count,
		//			data.c.y() + data.contact_dy / data.contact_count);
		//	}
		//}
	}
	
	void add_collision_edge(CollisionEdge@ edge)
	{
		if(num_custom_edges + 1 >= size_custom_edges)
			custom_edges.resize(size_custom_edges *= 2);
		
		@custom_edges[num_custom_edges++] = edge;
	}
	
	void remove_collision_edge(CollisionEdge@ edge)
	{
		const int index = custom_edges.findByRef(edge);
		
		if(index != -1 && index < num_custom_edges)
		{
			@custom_edges[index] = custom_edges[--num_custom_edges];
		}
	}
	
	void debug_draw_edges(const float sub_frame)
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
			
			draw_line(g, 21, 22, edge.x1, edge.y1, edge.x2, edge.y2, 1, clr);
		}
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
	
	private void texture_type_handler(controllable@ c, texture_type_query@ query, int entity_index)
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
	
	private void collision_handler(controllable@ c, tilecollision@ tc,
		int side, bool moving, float snap_offset, int entity_index)
	{
		const bool t_hit = c.check_collision(tc, side, moving, snap_offset);
		const float t_hit_x = tc.hit_x();
		const float t_hit_y = tc.hit_y();
		const int t_type = tc.type();
		tc.reset();
		
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
		float hit_base = 0;
		float t_hit_dist = 0;
		float hit_dist = 0;
		
		switch(side)
		{
			case SideType::Left:
				hit = check_collision(
					y + mh_y - mh_h,
					y + mh_y + mh_h,
					x + mh_x - (moving ? min(0.0, x_speed * DT) : 0),
					x + l - snap_offset,
					SideType::Left, tc, data, data.collision_left);
				t_hit_dist = t_hit_x;
				hit_base = x + mh_x;
				if(hit)
					hit_dist = tc.hit_x();
				break;
			case SideType::Right:
				hit = check_collision(
					y + mh_y - mh_h,
					y + mh_y + mh_h,
					x + mh_x - (moving ? max(0.0, x_speed * DT) : 0),
					x + r + snap_offset,
					SideType::Right, tc, data, data.collision_right);
				t_hit_dist = t_hit_x;
				hit_base = x + mh_x;
				if(hit)
					hit_dist = tc.hit_x();
				break;
			case SideType::Roof:
				hit = check_collision(
					x + mh_x - mh_w,
					x + mh_x + mh_w,
					y + mh_y - (moving ? min(0.0, y_speed * DT) : 0),
					y + t - snap_offset,
					SideType::Roof, tc, data, data.collision_roof);
				hit_base = t + mh_y;
				t_hit_dist = t_hit_y;
				if(hit)
					hit_dist = tc.hit_y();
				break;
			case SideType::Ground:
				hit = check_collision(
					x + mh_x - mh_w,
					x + mh_x + mh_w,
					y + mh_y - (moving ? max(0.0, y_speed * DT) : 0),
					y + b + snap_offset,
					SideType::Ground, tc, data, data.collision_ground);
				hit_base = t + mh_y;
				t_hit_dist = t_hit_y;
				if(hit)
					hit_dist = tc.hit_y();
				break;
		}
		
		if(
			!hit && t_hit ||
			t_hit &&
			abs(hit_base - t_hit_dist) <
			abs(hit_base - hit_dist))
		{
			tc.hit(true);
			tc.hit_x(t_hit_x);
			tc.hit_y(t_hit_y);
			tc.type(t_type);
		}
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
	
}
