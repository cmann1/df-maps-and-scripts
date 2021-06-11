#include '../common/drawing_utils.cpp'
#include 'ArmSegment.cpp'

class Arm
{
	
	scene@ g;
	
	controllable@ player = null;
	dustman@ dm;
	
	array<ArmSegment@> segments;
	int num_segments = 0;
	
	float max_reach = 0;
	bool locked = false;
	
	float x = 0;
	float y = 0;
	float target_x = 0;
	float target_y = 0;
	float target_tx = 0;
	float target_ty = 0;
	
	int dir_x;
	int dir_y;
	bool has_target = false;
	bool found_target = false;
	float found_target_x = 0;
	float found_target_y = 0;
	
	private float rest_x = 150;
	private float rest_y = 10;
	
	float target_track_speed = 1200;
	float move_speed = 600;
	
	array<tile_clean_data> clean_tiles;
	
	Arm(int dir_x, int dir_y)
	{
		@g = get_scene();
		
		this.dir_x = dir_x;
		this.dir_y = dir_y;
	}
	
	void reset()
	{
		clean_tiles.resize(0);
	}
	
	void init(controllable@ player, dustman@ dm, float x, float y)
	{
		@this.player = player;
		@this.dm = dm;
		this.x = x;
		this.y = y;
		
		if(num_segments == 0)
		{
			sprite_group arm1_spr;
			sprite_group arm2_spr;
			sprite_group arm_end_spr;
			
			int sl = 9;
			
			uint colour = dm is null or dm.character().substr(0, 1) != 'v' ? 0xFFFFFFFF : 0x99FFFFFF;
			arm1_spr.add_sprite('props4', 'machinery_6', 0.5, 0.5, 45.2421, -0.25338, 0, 0.7, 0.521001, colour, 0, 1, 18, sl);
			arm2_spr.add_sprite('props4', 'machinery_6', 0.5, 0.5, 46.2421, -0.25338, 0, 0.7, 0.521001, colour, 0, 1, 18, sl);
			arm_end_spr.add_sprite('props4', 'machinery_6', 0.5, 0.5, 52.4127, -0.300903, 0, 0.75, 0.442637, colour, 0, 1, 18, sl);
			arm_end_spr.add_sprite('props4', 'machinery_9', 0.5, 0.5, 102.9361, -0.275932, 270, 0.230614, 0.230614, colour, 0, 1, 18, sl);
			arm_end_spr.add_sprite('props4', 'machinery_9', 0.5, 0.5, 109.9361, -0.275932, 270, 0.230614, 0.230614, colour, 0, 1, 18, sl);
			arm_end_spr.add_sprite('props2', 'boulders_15', 0.5, 0.5, 63.8831, 2.58578, 266.924, 1.2, 1.2, colour, 0, 0, 18, sl - 1);
			
			add_segment(ArmSegment(@arm1_spr));
			add_segment(ArmSegment(@arm2_spr));
			add_segment(ArmSegment(@arm_end_spr, 130));
		}
		
		for(int i = 0; i < num_segments; i++)
		{
			ArmSegment@ seg = segments[i];
			seg.prev_rotation = seg.rotation = -90;
			seg.prev_x = seg.x = x;
			seg.prev_y = seg.y = y;
		}
		
		target_x = x + (rest_x) * dir_x;
		target_y = y + rest_y;
	}
	
	void add_segment(ArmSegment@ seg)
	{
		segments.insertAt(0, seg);
		num_segments++;
		
		max_reach += seg.length;
	}
	
	void reach(ArmSegment@ seg, float x, float y, float &out out_x, float &out out_y)
	{
		float dx = x - seg.x;
		float dy = y - seg.y;
		float angle = atan2(dy, dx);
		seg.rotation = angle;

		float tx = x - cos(angle) * seg.length;
		float ty = y - sin(angle) * seg.length;

		out_x = tx;
		out_y = ty;
	}
	
	void position(ArmSegment@ seg_a, ArmSegment@ seg_b)
	{
		seg_b.x = seg_a.x + cos(seg_a.rotation) * seg_a.length;
		seg_b.y = seg_a.y + sin(seg_a.rotation) * seg_a.length;
	}
	
	void step(float x, float y)
	{
		this.x = x;
		this.y = y;
		
		if(!has_target and !locked)
		{
			target_tx = x + rest_x * dir_x;
			target_ty = y + rest_y * dir_y;
		}
		
		if(has_target)
		{
			ArmSegment@ seg = segments[0];
			raycast@ ray = g.ray_cast_tiles(
				seg.x, seg.y,
				seg.x + cos(seg.rotation) * seg.length, seg.y + sin(seg.rotation) * seg.length, 0xFF);
			if(ray.hit())
			{
				found_target = true;
				const float angle = (ray.angle() - 90) * DEG2RAD;
				found_target_x = target_tx = ray.hit_x() + cos(angle);
				found_target_y = target_ty = ray.hit_y() + sin(angle);
				puts('found_target ' + target_tx + ', ' + target_ty);
			}
		}
		else if(found_target)
		{
			puts(' attaching');
			locked = true;
			found_target = false;
			target_tx = found_target_x;
			target_ty = found_target_y;
		}
		
		float dx = target_x - x;
		float dy = target_y - y;
		float dist = dx * dx + dy * dy;
		if(dist > max_reach * max_reach) // Make sure the target doesn't get too far away
		{
			dist = sqrt(dist);
			target_x = x + dx / dist * (max_reach);
			target_y = y + dy / dist * (max_reach);
		}
		
		dx = target_tx - target_x;
		dy = target_ty - target_y;
		dist = sqrt(dx * dx + dy * dy);
		if(dist > target_track_speed * DT)
		{
			target_x = target_x + dx / dist * target_track_speed * DT;
			target_y = target_y + dy / dist * target_track_speed * DT;
			
			if(has_target and dist > max_reach + 50)
			{
				has_target = false;
				locked = false;
			}
		}
		else
		{
			target_x = target_tx;
			target_y = target_ty;
		}

		if(num_segments == 0) return;
		
		for(int i = 0; i < num_segments; i++)
		{
			ArmSegment@ seg = segments[i];
			seg.prev_rotation = seg.rotation;
			seg.prev_x = seg.x;
			seg.prev_y = seg.y;
		}
		
		float tx = x + max_reach * 0.25 * dir_x;
		float ty = y - max_reach * 1;
		for(int j = 0; j < 4; j++)
		{
			ArmSegment@ first_segment = segments[num_segments - 1];
			first_segment.x = x;
			first_segment.y = y;
			
			reach(segments[0], tx, ty, tx, ty);
			for(int i = 1; i < num_segments; i++)
			{
				reach(segments[i], tx, ty, tx, ty);
			}
			for(int i = num_segments - 1; i >= 1; i--)
			{
				position(segments[i], segments[i - 1]);
			}
			
			tx = target_x;
			ty = target_y;
		}
		
		for(uint i = 0; i < clean_tiles.size(); i++)
		{
			clean_tiles[i].timer -= 24 / 60.0;
			if(clean_tiles[i].timer <= 0)
			{
				g.set_tile(clean_tiles[i].x, clean_tiles[i].y, 19, false, 0, 0, 0, 0);
				clean_tiles[i] = clean_tiles[clean_tiles.size() - 1];
				clean_tiles.resize(clean_tiles.size() -1 );
				i--;
			}
		}
	}
	
	void draw(float sub_frame)
	{
		for(int i = 0; i < num_segments; i++)
		{
			ArmSegment@ seg = segments[i];
			const float x = lerp(seg.prev_x, seg.x, sub_frame);
			const float y = lerp(seg.prev_y, seg.y, sub_frame);
			const float rotation = lerp_angle(seg.prev_rotation, seg.rotation, sub_frame);
			
			seg.spr.draw(18, 9, x, y, rotation * RAD2DEG, player.scale());
			
			g.draw_rectangle_world(21, 21, target_tx-5,target_ty-5,target_tx+5,target_ty+5, 0, 0xFF00FF00);
			g.draw_rectangle_world(21, 21, target_x-3,target_y-3,target_x+3,target_y+3, 0, 0xFFFF0000);
			
//			g.draw_line_world(21, 7,
//				x, y,
//				x + cos(rotation) * seg.length, y + sin(rotation) * seg.length,
//				4, 0xFFFFFFFF);
		}
		
	}
	
}

class tile_clean_data
{
	int x;
	int y;
	float timer;

	tile_clean_data()
	{
		x = 0;
		y = 0;
		timer = 0;
	}

	tile_clean_data(int x, int y, float timer)
	{
		this.x = x;
		this.y = y;
		this.timer = timer;
	}
}