#include "math.cpp"
#include "utils.cpp"
#include "Sprite.cpp"
#include "TileShape.cpp"
#include "TileSides.cpp"

const array<uint8> PORTAL_ENTITIES = {
	COL_TYPE_ENEMY, // 1
	COL_TYPE_PARTICLE, // 3
	COL_TYPE_PLAYER, // 5
	COL_TYPE_PROJECTILE, // 11
};
const uint PORTAL_ENTITIES_COUNT = PORTAL_ENTITIES.length();

class PortalGun
{
	scene@ g;
	
	Portal@ portal1;
	Portal@ portal2;
	
	bool active = false;
	
	float x;
	float y;
	float dir_x = 0;
	float dir_y = 0;
	float throw_dir = 0;
	float throw_angle = 0;
	float trace_length = 2000;
	
	Sprite @barrel_sprite;
	
	PortalGun(scene@ g)
	{
		@this.g = g;
		
		@barrel_sprite = Sprite("props4", "machinery_6");
		barrel_sprite.origin_x = 0.2;
		
		@portal1 = Portal(g, 0xFF274bd4);
		@portal2 = Portal(g, 0xFFff8a0d);
		@portal1.linked_portal = portal2;
		@portal2.linked_portal = portal1;
	}
	
	void update(float x, float y, int intent_x, int intent_y)
	{
		this.x = x;
		this.y = y;
		
		throw_dir = 0;
		
		if(intent_y == -1)
		{
			throw_dir = 45;
		}
		else if(intent_y == 1)
		{
			throw_dir = 135;
		}
		else
		{
			throw_dir = 90;
		}
		
		throw_dir *= intent_x;
		
		throw_angle = deg2rad(throw_dir);
		dir_x = sin(throw_angle);
		dir_y = -cos(throw_angle);
	}
	
	void step()
	{
		if(portal1.active)
		{
			portal1.step();
		}
		if(portal2.active)
		{
			portal2.step();
		}
	}
	
	void shoot_1()
	{
		shoot(portal1);
	}
	
	void shoot_2()
	{
		shoot(portal2);
	}
	
	void shoot(Portal@ portal)
	{
		raycast@ ray = g.ray_cast_tiles(x, y, x + dir_x * trace_length, y + dir_y * trace_length);
		
		if(ray.hit())
		{
			const float hit_x = ray.hit_x();
			const float hit_y = ray.hit_y();
			// const int side = ray.tile_side();
			
			
			if( try_place_portal(ray.hit_x(), ray.hit_y(), ray.angle(), ray.tile_x(), ray.tile_y(), ray.tile_side(), portal, true) )
			{
				
			}
		}
	}
	
	bool try_place_portal(float hit_x, float hit_y, const int hit_angle, const int tile_x, const int tile_y, const int side, Portal@ portal, bool try_adjacent=false)
	{
		// Tile to the left (clockwise)
		int tile1_type = TILE_SHAPE_FULL;
		int tile1_x = tile_x;
		int tile1_y = tile_y;
		// Tile to the right (clockwise)
		int tile2_type = TILE_SHAPE_FULL;
		int tile2_x = tile_x;
		int tile2_y = tile_y;
		
		tileinfo@ tile = g.get_tile(tile_x, tile_y);
		const uint8 type = tile.type();
		
		if(side == 0) // Top
		{
			tile1_x--;
			tile2_x++;
			
			if(
				type == TILE_SHAPE_BIG_1 or
				type == TILE_SHAPE_HALF_A)
			{
				tile1_y--;
			}
			else if(
				type == TILE_SHAPE_SMALL_5 or
				type == TILE_SHAPE_HALF_D)
			{
				tile1_y++;
			}
			
			if(
				type == TILE_SHAPE_BIG_5 or
				type == TILE_SHAPE_HALF_D)
			{
				tile2_y--;
			}
			else if(
				type == TILE_SHAPE_SMALL_1 or
				type == TILE_SHAPE_HALF_A)
			{
				tile2_y++;
			}
		}
		else if(side == 1) // Bottom
		{
			tile1_x++;
			tile2_x--;
			
			if(
				type == TILE_SHAPE_SMALL_7 or
				type == TILE_SHAPE_HALF_B)
			{
				tile1_y--;
			}
			else if(
				type == TILE_SHAPE_BIG_3 or
				type == TILE_SHAPE_HALF_C)
			{
				tile1_y++;
			}
			
			if(
				type == TILE_SHAPE_SMALL_3 or
				type == TILE_SHAPE_HALF_C)
			{
				tile2_y--;
			}
			else if(
				type == TILE_SHAPE_BIG_7 or
				type == TILE_SHAPE_HALF_B)
			{
				tile2_y++;
			}
		}
		else if(side == 2) // Left
		{
			tile1_y++;
			tile2_y--;
			
			if(type == TILE_SHAPE_BIG_4)
			{
				tile1_x--;
			}
			else if(type == TILE_SHAPE_SMALL_6)
			{
				tile1_x++;
			}
			
			if(type == TILE_SHAPE_BIG_6)
			{
				tile2_x--;
			}
			else if(type == TILE_SHAPE_SMALL_4)
			{
				tile2_x++;
			}
		}
		else if(side == 3) // Right
		{
			tile1_y--;
			tile2_y++;
			
			if(type == TILE_SHAPE_SMALL_8)
			{
				tile1_x--;
			}
			else if(type == TILE_SHAPE_BIG_2)
			{
				tile1_x++;
			}
			
			if(type == TILE_SHAPE_SMALL_2)
			{
				tile2_x--;
			}
			else if(type == TILE_SHAPE_BIG_8)
			{
				tile2_x++;
			}
		}
		
		tile1_type = get_matching_tile(type);
		tile2_type = get_matching_tile(type);
		
		tileinfo@ tile1 = g.get_tile(tile1_x, tile1_y);
		tileinfo@ tile2 = g.get_tile(tile2_x, tile2_y);
		
		bool tile1_valid = check_adjacent_tile(tile1_x, tile1_y, tile1_type, side);
		bool tile2_valid = check_adjacent_tile(tile2_x, tile2_y, tile2_type, side);
		
		if(try_adjacent)
		{
			if(tile1_valid and !tile2_valid)
			{
				portal.update((tile_x + TILE_SIDES[type][side * 2]) * 48, (tile_y + TILE_SIDES[type][side * 2 + 1]) * 48, hit_angle);
				return true;
				
				// if(try_place_portal(
					// hit_x, hit_y,
					// hit_angle, tile1_x, tile1_y, side, portal)
				// )
				// {
					// return true;
				// }
				
			}
			else if(tile2_valid and !tile1_valid)
			{
				int next_side = next_side_clockwise(side);
				portal.update((tile_x + TILE_SIDES[type][next_side * 2]) * 48, (tile_y + TILE_SIDES[type][next_side * 2 + 1]) * 48, hit_angle);
				return true;
			}
		}
		
		if(tile1_valid and tile2_valid)
		{
			portal.update(hit_x, hit_y, hit_angle);
			return true;
		}
		
		portal.active = false;
		
		return false;
	}
	
	bool check_adjacent_tile(const int tile_x, const int tile_y, const uint tile_type, const int side)
	{
		tileinfo@ tile = g.get_tile(tile_x, tile_y);
		bool valid = false;
		
		if(tile.solid())
		{
			const int type = int(tile.type());
			
			if(tile_type == TILE_SHAPE_FULL)
			{
				valid = is_full_edge(tile.type(), side);
			}
			else
			{
				valid = (tile.type() == tile_type);
			}
		}
		
		if(valid)
		{
			if(side == 0) // Top
			{
				@tile = g.get_tile(tile_x, tile_y - 1);
			}
			else if(side == 1) // Bottom
			{
				@tile = g.get_tile(tile_x, tile_y + 1);
			}
			else if(side == 2) // Left
			{
				@tile = g.get_tile(tile_x - 1, tile_y);
			}
			else if(side == 3) // Right
			{
				@tile = g.get_tile(tile_x + 1, tile_y);
			}
			
			if(tile.solid())
			{
				valid = false;
			}
		}
		
		return valid;
	}
	
	void draw(float sub_frame)
	{
		if(portal1.active)
		{
			portal1.draw(sub_frame);
		}
		
		if(portal2.active)
		{
			portal2.draw(sub_frame);
		}
		
		if(active)
		{
			barrel_sprite.draw_world(
				20, 20,
				0, 0, x, y, throw_dir - 90,
				0.5, 0.5, 0xFFFFFFFF);
		}
	}
}

class Portal
{
	scene@ g;
	
	bool active = false;
	float x = 0;
	float y = 0;
	float normal_x = 0;
	float normal_y = 0;
	float angle = 0; // Radians
	
	float p1x;
	float p1y;
	float p2x;
	float p2y;
	
	array<int> teleported_entities_id = {};
	array<int> teleported_entities = {};
	
	Portal@ linked_portal = null;
	
	uint colour;
	
	Portal(scene@ g, uint colour)
	{
		@this.g = g;
		this.colour = colour;
	}
	
	void update(float x, float y, int angle_deg)
	{
		active = true;
		this.x = x;
		this.y = y;
		angle = deg2rad(angle_deg);
		normal_x =  sin(angle);
		normal_y = -cos(angle);
		
		float dx = -normal_y * 48;
		float dy =  normal_x * 46;
		float x1 = x - dx;
		float y1 = y - dy;
		float x2 = x + dx;
		float y2 = y + dy;
		
		if(x1 < x2)
		{
			p1x = x1;
			p2x = x2;
		}
		else
		{
			p1x = x2;
			p2x = x1;
		}
		
		if(y1 < y2)
		{
			p1y = y1;
			p2y = y2;
		}
		else
		{
			p1y = y2;
			p2y = y1;
		}
		
		float min_thickness = 135;
		
		if(p2x - p1x < min_thickness)
		{
			float expand = (min_thickness - (p2x - p1x)) * 0.5;
			p1x -= expand;
			p2x += expand;
		}
		
		if(p2y - p1y < min_thickness)
		{
			float expand = (min_thickness - (p2y - p1y)) * 0.5;
			p1y -= expand;
			p2y += expand;
		}
		
		p1x += 30;
		p1y += 30;
		p2x -= 30;
		p2y -= 30;
	}
	
	void deactivate()
	{
		teleported_entities_id.resize(0);
		teleported_entities.resize(0);
		active = false;
	}
	
	void step()
	{
		if(@linked_portal != null and linked_portal.active)
		{
			// puts("---");
			for(uint i = 0; i < PORTAL_ENTITIES_COUNT; i++)
			{
				int collision_count = g.get_entity_collision(p1y, p2y, p1x, p2x, PORTAL_ENTITIES[i]);
				
				for(int j = 0; j < collision_count; j++)
				{
					entity@ e = g.get_entity_collision_index(j);
					controllable@ ec = e.as_controllable();
					
					if(@ec == null)
					{
						continue;
					}
					
					
					int index = teleported_entities_id.find(e.id());
					if(index > -1)
					{
						teleported_entities[index]++;
						continue;
					}
					
					float x_speed = ec.x_speed();
					float y_speed = ec.y_speed();
					// float speed = sqrt(x_speed * x_speed + y_speed * y_speed);
					float speed = ec.speed();
					
					float d = dot(x_speed, y_speed, normal_x, normal_y);
					float reflect_x = -(x_speed - 2 * normal_x * d);
					float reflect_y = -(y_speed - 2 * normal_y * d);
					
					float offset_angle = angle - atan2(reflect_y, reflect_x);
					x_speed = cos(linked_portal.angle + offset_angle) * speed;
					y_speed = sin(linked_portal.angle + offset_angle) * speed;
					
					// Project the players position onto the portal's surface
					float portal_dx = -normal_y;
					float portal_dy = normal_x;
					float ec_dx = ec.x() - x;
					float ec_dy = (ec.y() - 48) - y;
					float dp = dot(ec_dx, ec_dy, portal_dx, portal_dy);
					ec_dx = -( dp / (portal_dx*portal_dx + portal_dy*portal_dy) ) * portal_dx;
					ec_dy = -( dp / (portal_dx*portal_dx + portal_dy*portal_dy) ) * portal_dy;
					
					// Rotate the projected player's position to orient it to the other portal so that the player maintains it's relative position
					// when going through portals
					rotate(ec_dx, ec_dy, linked_portal.angle - angle, portal_dx, portal_dy);
					// Also shift the player a little out of the portal to prevent clipping into walls behind the portal
					ec.x(linked_portal.x + portal_dx + (linked_portal.normal_x * 48));
					ec.y(linked_portal.y + 48 + portal_dy + (linked_portal.normal_y * 48));
					
					ec.set_speed_xy(x_speed, y_speed);
					
					linked_portal.teleported_entities_id.insertLast(e.id());
					linked_portal.teleported_entities.insertLast(10);
				}
			}
			
			for(int i = teleported_entities.length() - 1; i >= 0; i--)
			{
				if(teleported_entities[i]-- <= 0)
				{
					teleported_entities_id.removeAt(i);
					teleported_entities.removeAt(i);
				}
			}
		}
	}
	
	void draw(float sub_frame)
	{
		outline_rect(g, p1x, p1y, p2x, p2y, 20, 20, 2, 0xFF00FF00);
		
		// draw_line(g, x, y, x+normal_x*50, y+normal_y*50, 20, 20, 3, 0xFF00FF00);
		draw_line(g, x-normal_y*48, y+normal_x*48, x+normal_y*48, y-normal_x*48, 20, 20, 10, colour);
	}
}