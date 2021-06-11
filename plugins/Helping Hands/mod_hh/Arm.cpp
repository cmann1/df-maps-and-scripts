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
	
	float x = 0;
	float y = 0;
	float target_x = 0;
	float target_y = 0;
	float target_tx = 0;
	float target_ty = 0;
	
	int target_tile_x = 0;
	int target_tile_y = 0;
	
	int dir;
	bool has_target = false;
	
	private float rest_x = 150;
	private float rest_y = 50;
	
	float target_track_speed = 1200;
	
	array<tile_clean_data> clean_tiles;
	
	Arm(int dir)
	{
		@g = get_scene();
		
		this.dir = dir;
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
			add_segment(ArmSegment(@arm_end_spr, 160));
		}
		
		for(int i = 0; i < num_segments; i++)
		{
			ArmSegment@ seg = segments[i];
			seg.prev_rotation = seg.rotation = -90;
			seg.prev_x = seg.x = x;
			seg.prev_y = seg.y = y;
		}
		
		target_x = x + (rest_x) * dir;
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
	
	int filth_count(int tile_x, int tile_y)
	{
		int count = 0;
		tileinfo@ tile = g.get_tile(tile_x, tile_y);
		
		if(tile.solid())
		{
			if(tile.is_dustblock())
			{
				count++;
			}
			else
			{
				tilefilth@ filth = g.get_tile_filth(tile_x, tile_y);
				const uint8 top = filth.top();
				const uint8 bottom = filth.bottom();
				const uint8 left = filth.left();
				const uint8 right = filth.right();
				if(top > 0 and top <= 5) count++;
				if(bottom > 0 and bottom <= 5) count++;
				if(left > 0 and left <= 5) count++;
				if(right > 0 and right <= 5) count++;
			}
		}
		
		return count;
	}
	
	void step(float x, float y)
	{
		this.x = x;
		this.y = y;
		
		if(!has_target)
		{
			const int reach_tiles = int(floor(max_reach / 48));
			const int tile_x = int(floor(x / 48));
			const int tile_y = int(floor(y / 48));
			int x1 = tile_x + 0 * dir;
			int x2 = tile_x + reach_tiles * dir;
			int y1 = tile_y - reach_tiles;
			int y2 = tile_y + reach_tiles;
			if(x1 > x2)
			{
				int tmp = x1;
				x1 = x2;
				x2 = tmp;
			}
			
			bool flip = false;
			int t_y = 0;
			while(!has_target and t_y < reach_tiles)
			{
				for(int t_x = 0; t_x < reach_tiles; t_x++)
				{
					const int t_dx = t_x * dir;
					if(filth_count(tile_x + t_dx, tile_y + t_y) > 0)
					{
						has_target = true;
						target_tile_x = tile_x + t_dx;
						target_tile_y = tile_y + t_y;
						target_tx = target_tile_x * 48 + 24;
						target_ty = target_tile_y * 48 + 24;
						break;
					}
				}
				
				if(flip) t_y = -t_y;
				else t_y += t_y > 0 ? 1 : -1;
				flip = !flip;
			}
			
			if(!has_target)
			{
				target_tx = x + rest_x * dir;
				target_ty = y + rest_y;
			}
		}
		else
		{
			if(filth_count(target_tile_x, target_tile_y) == 0)
			{
				has_target = false;
			}
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
		if(has_target) // Make sure the target dust/dustblock doesn't get too far away
		{
			dx = target_tx - x;
			dy = target_ty - y;
			dist = dx * dx + dy * dy;
			if(dist > max_reach * 1.35 * max_reach * 1.35)
			{
				has_target = false;
			}
		}
		
		dx = target_tx - target_x;
		dy = target_ty - target_y;
		dist = sqrt(dx * dx + dy * dy);
		if(dist > target_track_speed * DT)
		{
			target_x = target_x + dx / dist * target_track_speed * DT;
			target_y = target_y + dy / dist * target_track_speed * DT;
			
			if(has_target and dist > max_reach)
			{
				has_target = false;
			}
		}
		else
		{
			target_x = target_tx;
			target_y = target_ty;
			
			if(has_target)
			{
				has_target = false;
				if(dm !is null)
				{
					int count = filth_count(target_tile_x, target_tile_y);
					dm.combo_count(dm.combo_count() + count);
					dm.skill_combo(dm.skill_combo() + count);
					dm.combo_timer(1);
				}
				g.set_tile_filth(target_tile_x, target_tile_y, 0, 0, 0, 0, false, true);
				tileinfo@ tile = g.get_tile(target_tile_x, target_tile_y);
				if(tile.solid() and tile.is_dustblock())
				{
					tile.sprite_tile(0);
					g.set_tile(target_tile_x, target_tile_y, 19, @tile, true);
					clean_tiles.insertLast(tile_clean_data(target_tile_x, target_tile_y, 5.0));
				}
			}
		}

		if(num_segments == 0) return;
		
		for(int i = 0; i < num_segments; i++)
		{
			ArmSegment@ seg = segments[i];
			seg.prev_rotation = seg.rotation;
			seg.prev_x = seg.x;
			seg.prev_y = seg.y;
		}
		
		
		float tx = x + max_reach * 0.25 * dir;
		float ty = y - max_reach * 1;
		for(int j = 0; j < 2; j++)
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
			
//			g.draw_rectangle_world(21, 21, target_tx-5,target_ty-5,target_tx+5,target_ty+5, 0, 0xFF00FF00);
//			g.draw_rectangle_world(21, 21, target_x-3,target_y-3,target_x+3,target_y+3, 0, 0xFFFF0000);
			
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