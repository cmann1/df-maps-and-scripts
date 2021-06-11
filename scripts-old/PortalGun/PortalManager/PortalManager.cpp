#include "../common/tile_utils.cpp"
#include "../common/utils.cpp"
#include "TileSideHitboxes.cpp"
#include "Portal.cpp"

class PortalManager
{

	scene@ g;
	Debug@ debug;
	
	int portal_next_id = 0;
	
	// A list of all portals in the system
	array<Portal@> portals = {};
	uint portal_count = 0;
	array<Portal@> portals_checkpoint = {};
	uint portal_count_checkpoint = 0;
	
	// Maps tile_x/tile_y to a PortalTile
	dictionary portal_tile_map = {};
	dictionary portal_tile_map_checkpoint = {};
	
	// Track entities which have been teleported to prevent them from going through multiple portals on the same frame,
	// or instantly teleporting back through the portal they came out of
	array<int8> teleported_entities = {};
	array<int8> teleported_entities_checkpoint = {};
	array<int> teleported_entities_id = {};
	array<int> teleported_entities_id_checkpoint = {};
	
	PortalManager(Debug@ debug=null)
	{
		@g = get_scene();
		@this.debug = debug;
	}
	
	void checkpoint_save()
	{
		portals_checkpoint = portals;
		portal_count_checkpoint = portal_count;
		
		teleported_entities_checkpoint = teleported_entities;
		teleported_entities_id_checkpoint = teleported_entities_id;
		
		portal_tile_map_checkpoint = portal_tile_map;
	}

	void checkpoint_load()
	{
		portals = portals_checkpoint;
		portal_count = portal_count_checkpoint;
		
		teleported_entities = teleported_entities_checkpoint;
		teleported_entities_id = teleported_entities_id_checkpoint;
		
		portal_tile_map = portal_tile_map_checkpoint;
	}
	
	void remove(Portal@ portal)
	{
		if(@portal == null) return;
		
		int index = portals.find(portal);
		if(index < 0) return;
		
		portal.link(null);
		
		@portals[index] = portals[portal_count - 1];
		@portals[--portal_count] = null;
		
		uint tile_count = portal.tiles.length();
		for(uint j = 0; j < tile_count; j++)
		{
			PortalTile@ tile = portal.tiles[j];
			portal_tile_map.delete(get_tile_key(tile.tile_x, tile.tile_y, tile.tile_type, tile.side));
		}
	}
	
	Portal@ try_create(float x1, float y1, float x2, float y2, int portal_size, uint colour=0xAAFF0000, int min_size=-1, bool allow_shift=true)
	{
		if(abs(x1 - x2) < 0.001)
		{
			x1 += 0.05;
		}
		if(abs(y1 - y2) < 0.001)
		{
			y1 += 0.05;
		}
		
		raycast@ ray = g.ray_cast_tiles(x1, y1, x2, y2);
		
		// In case the ray slips through a seam, try a another slightly offset
		if(!ray.hit())
		{
			if(abs(x1 - x2) < 0.5)
			{
				@ray = g.ray_cast_tiles(x1 + 1, y1, x2 - 1, y2);
			}
			else if(abs(y1 - y2) < 0.5)
			{
				@ray = g.ray_cast_tiles(x1, y1 + 1, x2, y2 - 1);
			}
		}
		
		Portal@ new_portal = null;
		
		if(ray.hit())
		{
			@new_portal = try_create(
				ray.tile_x(), ray.tile_y(), ray.tile_side(),
				portal_size, colour, min_size, allow_shift
			);
			
			x2 = ray.hit_x();
			y2 = ray.hit_y();
		}
		
		if(@debug != null)
		{
			debug.line(x1, y1, x2, y2, 20, 20, 30, 2, ray.hit() ? 0xAA00FF00 : 0xAAFF0000);
		}
		
		return new_portal;
	}
	
	Portal@ try_create(const int tile_x, const int tile_y, const int side, int portal_size, uint colour=0xAAFF0000, int min_size=-1, bool allow_shift=true)
	{
		if(portal_size < 1)
		{
			portal_size = 1;
		}
		if(min_size <= 0)
		{
			min_size = portal_size;
		}
		
		tileinfo@ init_tile = g.get_tile(tile_x, tile_y);
		const uint8 type = is_full_edge(init_tile.type(), side) ? TILE_SHAPE_FULL : init_tile.type();
		const int init_angle = type == TILE_SHAPE_FULL ? 0 : init_tile.angle();
		
		if(!check_tile(tile_x, tile_y, -1, side))
		{
			portal_size = 0;
		}
		
		array<int> portal_tiles = {};
		
		// -1 = left, 0 = middle, 1 = right
		int check_side = 0;
		// Tile to the left (clockwise)
		int tile1_type = type;
		int tile1_x = tile_x;
		int tile1_y = tile_y;
		// Tile to the right (clockwise)
		int tile2_type = type;
		int tile2_x = tile_x;
		int tile2_y = tile_y;
		
		int next_type = type;
		int next_x = tile_x;
		int next_y = tile_y;
		
		while(portal_size > 0)
		{
			if(check_side == -1)
			{
				if(tile1_type == -1)
				{
					if(!allow_shift or tile2_type == -1) break;
					check_side = -check_side;
					continue;
				}
				
				get_left_tile(tile1_x, tile1_y, tile1_type, side, next_x, next_y);
				next_type = get_matching_tile(tile1_type);
			}
			else if(check_side == 1)
			{
				if(tile2_type == -1)
				{
					if(!allow_shift or tile1_type == -1) break;
					check_side = -check_side;
					continue;
				}
				
				get_right_tile(tile2_x, tile2_y, tile2_type, side, next_x, next_y);
				next_type = get_matching_tile(tile2_type);
			}
			
			if(check_side != 0)
			{
				if(!check_tile(next_x, next_y, next_type, side))
				{
					next_type = -1;
				}
				else
				{
				}
				
			}
			else
			{
				check_side = 1;
			}
			
			if(@debug != null) debug.outline_tile(next_x, next_y, 19, 19, 30, 1, next_type != -1 ? 0xAA00FFFF : 0xAAFF0000);
			
			if(next_type != -1)
			{
				portal_size--;
				portal_tiles.insertLast(next_x);
				portal_tiles.insertLast(next_y);
			}
			
			if(check_side == -1)
			{
				tile1_type = next_type;
				tile1_x = next_x;
				tile1_y = next_y;
			}
			else if(check_side == 1)
			{
				tile2_type = next_type;
				tile2_x = next_x;
				tile2_y = next_y;
			}
			
			check_side = -check_side;
			
		} // end while portal_size
		
		if(portal_tiles.length() >= uint(min_size) * 2)
		{
			float portal_x = 0;
			float portal_y = 0;
			
			Portal@ new_portal = Portal(g, this, portal_next_id++, colour);
			
			new_portal.update(get_tile_angle(init_angle, side));
			
			uint tile_count = portal_tiles.length();
			for(uint i = 0; i < tile_count; i += 2)
			{
				next_x = portal_tiles[i];
				next_y = portal_tiles[i + 1];
				tileinfo@ tile = g.get_tile(next_x, next_y);
				
				PortalTile@ potral_tile = PortalTile(new_portal, next_x, next_y, tile.type(), side, get_tile_edge(tile, side));
				new_portal.add_tile(potral_tile);
				
				const array<float>@ hitboxes = TILE_SIDE_HITBOXES[tile.type()];
				int j = side * 4;
				float x = next_x * 48;
				float y = next_y * 48;
				potral_tile.x1 = x + hitboxes[j];
				potral_tile.y1 = y + hitboxes[j + 1];
				potral_tile.x2 = x + hitboxes[j + 2];
				potral_tile.y2 = y + hitboxes[j + 3];
				
				portal_x += x + 24;
				portal_y += y + 24;
				
				portal_tile_map[get_tile_key(next_x, next_y, type, side)] = @potral_tile;
			}
			
			new_portal.centre_x = portal_x / (tile_count / 2);
			new_portal.centre_y = portal_y / (tile_count / 2);
			
			if(portal_count >= portals.length())
			{
				portals.resize(portal_count + 10);
			}
			@portals[portal_count++] = new_portal;
			
			return new_portal;
		}
		
		return null;
	}
	
	bool check_tile(const int tile_x, const int tile_y, const int tile_type, const int side)
	{
		bool valid = false;
		
		if(portal_tile_map.exists(get_tile_key(tile_x, tile_y, tile_type, side)))
		{
			return false;
		}
		
		tileinfo@ tile = g.get_tile(tile_x, tile_y);
		
		if(tile.solid() and !tile.is_dustblock())
		{
			const int type = int(is_full_edge(tile.type(), side) ? TILE_SHAPE_FULL : tile.type());
			
			if((get_tile_edge(tile, side) & 8) != 0) // Is the edge solid?
			{
				if(tile_type == TILE_SHAPE_FULL)
				{
					valid = is_full_edge(tile.type(), side);
				}
				else
				{
					valid = (tile_type == -1 or tile.type() == uint(tile_type));
				}
			}
		}
		
		if(valid)
		{
			if(side == 0) // Top
			{
				return !g.get_tile(tile_x, tile_y - 1).solid();
			}
			else if(side == 1) // Bottom
			{
				return !g.get_tile(tile_x, tile_y + 1).solid();
			}
			else if(side == 2) // Left
			{
				return !g.get_tile(tile_x - 1, tile_y).solid();
			}
			else if(side == 3) // Right
			{
				return !g.get_tile(tile_x + 1, tile_y).solid();
			}
			
			return false;
		}
		
		return false;
	}
	
	string get_tile_key(const int tile_x, const int tile_y, const int tile_type, const int side)
	{
		// return tile_x + "/" + tile_y + "/" + side;
		return tile_x + "/" + tile_y;
	}
	
	void step()
	{
		for(uint i = 0; i < portal_count; i++)
		{
			portals[i].step(g, teleported_entities, teleported_entities_id, debug);
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
	
	void draw()
	{
		for(uint i = 0; i < portal_count; i++)
		{
			Portal@ portal = portals[i];
			portal.draw(g);
		}
	}
	
}