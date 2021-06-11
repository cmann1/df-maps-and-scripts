#include "../common/ColType.cpp"
#include "../common/tile_utils.cpp"
#include "../common/math.cpp"

const array<uint8> PORTAL_ENTITIES = {
	COL_TYPE_ENEMY, // 1
	COL_TYPE_PLAYER, // 5
	COL_TYPE_PROJECTILE, // 11
};
const uint PORTAL_ENTITIES_COUNT = PORTAL_ENTITIES.length();

class Portal
{
	int id;
	scene@ g;
	PortalManager@ manager;
	
	bool active = false;
	float centre_x = 0;
	float centre_y = 0;
	float normal_x = 0;
	float normal_y = 0;
	float angle = 0;
	float length = 0;
	
	uint colour;
	
	protected Portal@ other_portal = null;
	protected bool allow_in = true;
	
	array<PortalTile@> tiles = {};
	
	Portal(scene@ g, PortalManager@ manager, Portal@ from)
	{
		@this.g = g;
		@this.manager = manager;
		this.id = from.id;
		this.colour = from.colour;
		
		@other_portal = from.other_portal;
		allow_in = from.allow_in;
	}
	
	Portal(scene@ g, PortalManager@ manager, int id, uint colour)
	{
		@this.g = g;
		@this.manager = manager;
		this.id = id;
		this.colour = colour;
	}
	
	void update(int angle_deg)
	{
		angle = deg2rad(angle_deg);
		normal_x =  sin(angle);
		normal_y = -cos(angle);
	}
	
	void add_tile(PortalTile@ tile)
	{
		tiles.insertLast(tile);
		length += len(tile.edge_x2 - tile.edge_x1, tile.edge_y2 - tile.edge_y1);
	}
	
	void link(Portal@ other, bool unlink_other=true)
	{
		if(@other == @other_portal or other is this)
		{
			return;
		}
		
		if(@other_portal != null and unlink_other)
		{
			other_portal.link(null, false);
		}
		
		@other_portal = other;
		const bool active = @other_portal != null;
		
		if(active)
		{
			other_portal.link(this);
		}
		
		uint tile_count = tiles.length();
		for(uint j = 0; j < tile_count; j++)
		{
			tiles[j].set_active(g, active);
		}
	}
	
	void step(scene@ g, array<int8>@ teleported_entities, array<int>@ teleported_entities_id, Debug@ debug=null)
	{
		if(@other_portal == null)
		{
			return;
		}
		
		uint tile_count = tiles.length();
		for(uint j = 0; j < tile_count; j++)
		{
			const PortalTile@ tile = tiles[j];
			const float p1x = tile.x1;
			const float p1y = tile.y1;
			const float p2x = tile.x2;
			const float p2y = tile.y2;
			
			for(uint k = 0; k < PORTAL_ENTITIES_COUNT; k++)
			{
				int collision_count = g.get_entity_collision(p1y, p2y, p1x, p2x, PORTAL_ENTITIES[k]);
				
				for(int l = 0; l < collision_count; l++)
				{
					entity@ e = g.get_entity_collision_index(l);
					controllable@ ec = e.as_controllable();
					
					if(@ec == null)
					{
						continue;
					}
					
					// The entity has already teleported. Skip it to prevent it from immediatelly teleporting back through portals
					int index = teleported_entities_id.find(e.id());
					if(index > -1)
					{
						continue;
					}
					
					// Find the entities centre
					rectangle@ bounds = ec.collision_rect();
					const float ec_centre_x = (bounds.left() + bounds.right()) * 0.5;
					const float ec_centre_y = (bounds.top() + bounds.bottom()) * 0.5;
					float ec_x = ec.x() + ec_centre_x;
					float ec_y = ec.y() + ec_centre_y;
					
					float x_speed = ec.x_speed();
					float y_speed = ec.y_speed();
					float speed = ec.speed();
					
					float reflect_x = 0;
					float reflect_y = 0;
					
					// "Flip" the entity's velocity so it's moving in the same direction when it comes out the other side.
					// This needs to be done differently depending on if the protals are facing away from each other or not
					// in order to keep the entity moving in the same direction along the x and y axes
					
					// The portals are facing away from each other.
					const float portal_facing = dot(normal_x, normal_y, other_portal.normal_x, other_portal.normal_y);
					if(portal_facing < 0)
					{
						reflect_x = -x_speed;
						reflect_y = -y_speed;
					}
					// Roughly in the same direction
					else
					{
						float d = dot(x_speed, y_speed, normal_x, normal_y);
						reflect_x = (x_speed - 2 * normal_x * d);
						reflect_y = (y_speed - 2 * normal_y * d);
					}
					
					// Reorient the player's velocity to match the other portal
					float offset_angle = other_portal.angle - angle;
					float entity_angle = atan2(reflect_y, reflect_x) + offset_angle;
					x_speed = cos(entity_angle) * speed;
					y_speed = sin(entity_angle) * speed;
					
					// Project the entity's position onto the portal's surface
					// This is used below to maintain the entities position relative to the portal
					float portal_dx = -normal_y;
					float portal_dy = normal_x;
					float ec_dx = ec_x - centre_x;
					float ec_dy = ec_y - centre_y;
					float dp = dot(ec_dx, ec_dy, portal_dx, portal_dy);
					ec_dx = -( dp / (portal_dx*portal_dx + portal_dy*portal_dy) ) * portal_dx;
					ec_dy = -( dp / (portal_dx*portal_dx + portal_dy*portal_dy) ) * portal_dy;
					
					if(portal_facing > 0)
					{
						ec_dx = -ec_dx;
						ec_dy = -ec_dy;
					}
					
					// Scale the relative position so that if one portal is larger or smaller than the other the player
					// won't teleport outside of it
					if(length != other_portal.length)
					{
						ec_dx = ec_dx / length * other_portal.length;
						ec_dy = ec_dy / length * other_portal.length;
					}
					
					rotate(ec_dx, ec_dy, other_portal.angle - angle, portal_dx, portal_dy);
					// Also shift the entity a little out of the portal to prevent clipping into walls behind the portal
					const float ec_width = bounds.get_width() * 0.5;
					const float ec_height = bounds.get_height() * 0.5;
					float ec_size = ((abs(ec_width) > abs(ec_height) ? abs(ec_width) : abs(ec_height))) + 24;
					ec.x(other_portal.centre_x + portal_dx + (other_portal.normal_x * ec_size) - ec_centre_x);
					ec.y(other_portal.centre_y + portal_dy + (other_portal.normal_y * ec_size) - ec_centre_y);
					
					ec.set_speed_xy(x_speed, y_speed);
					ec.face(x_speed > 0 ? 1 : -1);
					
					// Force fall state if the player is dashing through the portal and changes direction
					// st_dash = 9
					if(ec.state() == 9)
					{
						if(abs(y_speed) > abs(x_speed))
							ec.state(5); // st_fall
					}
					
					teleported_entities_id.insertLast(e.id());
					teleported_entities.insertLast(2);
				}
			}
			
		}
	}
	
	void draw(scene@ g)
	{
		for(uint i = 0; i < tiles.length(); i++)
		{
			PortalTile@ tile = tiles[i];
			draw_line(g, tile.edge_x1, tile.edge_y1, tile.edge_x2, tile.edge_y2, 19, 14, 8, colour);
			
//			g.draw_gradient_world(19, 14, tile.edge_x1, tile.edge_y1, tile.edge_x2, tile.edge_y2, 0x00FF0000, 0x00FF0000, 0xFFFF0000, 0xFFFF0000);
			// outline_rect(g, tile.x1, tile.y1, tile.x2, tile.y2, 19, 19, 1, 0xFF00FFFF);
		}
		
		// draw_line(g, centre_x, centre_y, centre_x + normal_x*100, centre_y+normal_y*100, 19, 29, 2, 0xFFFF0000);
	}
	
	bool opEquals(Portal@ b)
	{
		return this is b;
	}
	
}

class PortalTile
{
	Portal@ portal;
	int tile_x;
	int tile_y;
	int side;
	uint tile_type;
	// The original edge bits for this tile/edge
	uint8 edge_bits;
	
	float x1;
	float y1;
	float x2;
	float y2;
	
	float edge_x1;
	float edge_y1;
	float edge_x2;
	float edge_y2;
	
	// ??
	uint index;
	
	PortalTile(Portal@ portal, int tile_x, int tile_y, uint tile_type, int side, uint8 edge_bits)
	{
		@this.portal = portal;
		this.tile_x = tile_x;
		this.tile_y = tile_y;
		this.tile_type = tile_type;
		this.side = side;
		this.edge_bits = edge_bits;
		
		edge_x1 = (tile_x + TILE_SIDES[tile_type][side * 2]) * 48;
		edge_y1 = (tile_y + TILE_SIDES[tile_type][side * 2 + 1]) * 48;
		int next_side = next_side_clockwise(side) * 2;
		edge_x2 = (tile_x + TILE_SIDES[tile_type][next_side]) * 48;
		edge_y2 = (tile_y + TILE_SIDES[tile_type][next_side + 1]) * 48;
	}
	
	void set_active(scene@ g, bool active=true)
	{
		tileinfo@ tile = g.get_tile(tile_x, tile_y);
		set_tile_edge(tile, side, active ? 0 : edge_bits);
		g.set_tile(tile_x, tile_y, 19, tile, false);
	}
}