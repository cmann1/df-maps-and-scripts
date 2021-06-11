/*
 * Copied from https://github.com/jriecken/sat-js
 */

#include "math.cpp"
#include "tile_utils.cpp"
#include "TileShape.cpp"
#include "TilePolygons.cpp"

const int LEFT_VORONOI_REGION = 0;
const int RIGHT_VORONOI_REGION = 1;
const int MIDDLE_VORONOI_REGION = 2;

class Circle
{
	float x;
	float y;
	float radius;
	
	Circle(float x=0, float y=0, float radius=0)
	{
		this.x = x;
		this.y = y;
		this.radius = radius;
	}
}

class Polygon
{
	float x = 0;
	float y = 0;
	array<float> points;
	array<float> edges = {};
	array<float> normals = {};
	
	Polygon(array<float> p)
	{
		points = p;
		calculate_edges();
	}
	
	void calculate_edges()
	{
		int p_len = points.length();
		edges.resize(p_len);
		normals.resize(p_len);
		
		for(int i = 0; i < p_len; i += 2)
		{
			int ni = i == p_len - 2 ? 0 : i + 2;
			float ex = points[ni] - points[i];
			float ey = points[ni + 1] - points[i + 1];
			edges[i] = ex;
			edges[i + 1] = ey;
			
			float normal_x;
			float normal_y;
			normalize(ey, -ex, normal_x, normal_y);
			normals[i] = normal_x;
			normals[i + 1] = normal_y;
		}
	}
	
	void draw(scene@ g, uint layer, uint sub_layer, float thickness=2, uint colour=0xFFFFFFFF, bool world=true)
	{
		for(uint i1 = 0; i1 < points.length(); i1 += 2)
		{
			float v1_x = points[i1];
			float v1_y = points[i1 + 1];
			int i2 = i1 + 2 < points.length() ? i1 + 2 : 0;
			float v2_x = points[i2];
			float v2_y = points[i2 + 1];
			
			g.draw_line(
				layer, sub_layer,
				x + v1_x, y + v1_y,
				x + v2_x, y + v2_y,
				thickness, colour);
		}
	}
}

class TilePolygon : Polygon
{
	array<int> sides;
	
	TilePolygon(array<int>@ sides, array<float>@ p)
	{
		super(p);
		this.sides = sides;
	}
}

class CollisionResponse
{
	bool aInB = false;
	bool bInA = false;
	float overlapV_x = 0;
	float overlapV_y = 0;
	float overlapN_x = 0;
	float overlapN_y = 0;
	float overlap = 9999999999.0;
	float x1;
	float y1;
	float x2;
	float y2;
	int type;
	
	void reset()
	{
		aInB = false;
		bInA = false;
		overlapV_x = 0;
		overlapV_y = 0;
		overlapN_x = 0;
		overlapN_y = 0;
		overlap = 9999999999.0;
		type = 0;
	}
}

int voronoi_region(float line_x, float line_y, float point_x, float point_y)
{
	float dp = dot(point_x, point_y, line_x, line_y);
	// If the point is beyond the start of the line, it is in the
	// left voronoi region.
	if (dp < 0) { return LEFT_VORONOI_REGION; }
	// If the point is beyond the end of the line, it is in the
	// right voronoi region.
	else if (dp > length_sqr(line_x, line_y)) { return RIGHT_VORONOI_REGION; }
	// Otherwise, it's in the middle one.
	else { return MIDDLE_VORONOI_REGION; }
}

bool test_tile_circle(scene@ g, int tile_x, int tile_y, Circle@ circle, CollisionResponse@ response)
{
	tileinfo@ tile = g.get_tile(tile_x, tile_y);
	if(!tile.solid()) return false;
	
	const uint8 tile_type = tile.type();
	int next_tile_x;
	int next_tile_y;
	int next_type;
	
	int collision_count = 0;
	
	TilePolygon@ polygon = TILE_POLYGONS[tile_type];
	polygon.x = tile_x * 48 + 24;
	polygon.y = tile_y * 48 + 24;
	
    // Get the position of the circle relative to the polygon.
	const float circlePos_x = circle.x - polygon.x;
	const float circlePos_y = circle.y - polygon.y;
    const float radius = circle.radius;
    const float radius2 = radius * radius;
	
	const array<float>@ points = @polygon.points;
	const array<float>@ edges = @polygon.edges;
	const array<float>@ normals = @polygon.normals;
	const array<int>@ sides = @polygon.sides;
	const int p_len = points.length();
	
    // For each edge in the polygon:
    for(int i = 0; i < p_len; i += 2)
	{
		int side = sides[i / 2];
		if((get_tile_edge(tile, side) & 8) == 0) continue;
		
		int next = i == p_len - 2 ? 0 : i + 2;
		int prev = i == 0 ? p_len - 2 : i - 2;
		float overlap = 0;
		bool overlapN = false;
		float overlapN_x;
		float overlapN_y;
		int edge_type = -1;
		
		float p1x = points[i];
		float p1y = points[i + 1];
		float p2x = points[next];
		float p2y = points[next + 1];
		
		// Get the edge.
		float edge_x = edges[i];
		float edge_y = edges[i + 1];
		float edge_dx = edge_x;
		float edge_dy = edge_y;
		
		// Expand tile edge
		get_left_tile(tile_x, tile_y, tile_type, side, next_tile_x, next_tile_y);
		next_type = get_matching_tile(tile_type);
		if(check_tile(g, next_tile_x, next_tile_y, next_type, side))
		{
			p1x -= edge_dx;
			p1y -= edge_dy;
			edge_x += edge_dx;
			edge_y += edge_dy;
		}
		
		get_right_tile(tile_x, tile_y, tile_type, side, next_tile_x, next_tile_y);
		next_type = get_matching_tile(tile_type);
		if(check_tile(g, next_tile_x, next_tile_y, next_type, side))
		{
			p2x += edge_dx;
			p2y += edge_dy;
			edge_x += edge_dx;
			edge_y += edge_dy;
		}

		// Calculate the center of the circle relative to the starting point of the edge.
		float point_x = circlePos_x - p1x;
		float point_y = circlePos_y - p1y;

		// If the distance between the center of the circle and the point
		// is bigger than the radius, the polygon is definitely not fully in
		// the circle.
		if(length_sqr(point_x, point_y) > radius2)
		{
			response.aInB = false;
		}

		// Calculate which Voronoi region the center of the circle is in.
		int region = voronoi_region(edge_x, edge_y, point_x, point_y);
		
		// If it's the left region:
		if(region == LEFT_VORONOI_REGION)
		{
			// We need to make sure we're in the RIGHT_VORONOI_REGION of the previous edge.
			edge_x = edges[prev];
			edge_y = edges[prev + 1]; 
			// Calculate the center of the circle relative the starting point of the previous edge
			float point2_x = circlePos_x - points[prev];
			float point2_y = circlePos_y - points[prev + 1];
			region = voronoi_region(edge_x, edge_y, point2_x, point2_y);
			
			if(region == RIGHT_VORONOI_REGION)
			{
				// It's in the region we want.  Check if the circle intersects the point.
				float dist = magnitude(point_x, point_y);
				if(dist > radius)
				{
					// No intersection
					return false;
				}
				else
				{
					// It intersects, calculate the overlap.
					response.bInA = false;
					overlapN = true;
					overlapN_x = point_x;
					overlapN_y = point_y;
					overlap = radius - dist;
					edge_type = 0;
				}
			}
		}
		// If it's the right region:
		else if(region == RIGHT_VORONOI_REGION)
		{
			// We need to make sure we're in the left region on the next edge
			edge_x = edges[next];
			edge_y = edges[next + 1]; 
			// Calculate the center of the circle relative to the starting point of the next edge.
			point_x = circlePos_x - p2x;
			point_y = circlePos_y - p2y;
			region = voronoi_region(edge_x, edge_y, point_x, point_y);
			
			if (region == LEFT_VORONOI_REGION) {
				// It's in the region we want.  Check if the circle intersects the point.
				float dist = magnitude(point_x, point_y);
				if(dist > radius)
				{
					// No intersection
					return false;
				}
				else
				{
					// It intersects, calculate the overlap.
					response.bInA = false;
					overlapN = true;
					overlapN_x = point_x;
					overlapN_y = point_y;
					overlap = radius - dist;
					edge_type = 0;
				}
			}
		}
			// Otherwise, it's the middle region:
		else
		{
			// Need to check if the circle is intersecting the edge,
			// Change the edge into its "edge normal".
			float normal_x = normals[i];
			float normal_y = normals[i + 1];
			// Find the perpendicular distance between the center of the 
			// circle and the edge.
			float dist = dot(point_x, point_y, normal_x, normal_y);
			float distAbs = abs(dist);
			// If the circle is on the outside of the edge, there is no intersection.
			if(dist > 0 && distAbs > radius)
			{
				// No intersection
				return false;
			}
			else
			{
				// It intersects, calculate the overlap.
				overlapN = true;
				overlapN_x = normal_x;
				overlapN_y = normal_y;
				overlap = radius - dist;
				edge_type = 1;
				// If the center of the circle is on the outside of the edge, or part of the
				// circle is on the outside, the circle is not fully inside the polygon.
				if(dist >= 0 || overlap < 2 * radius)
				{
					response.bInA = false;
				}
			}
		}

		// If this is the smallest overlap we've seen, keep it. 
		// (overlapN may be null if the circle was in the wrong Voronoi region).
		if(overlapN and abs(overlap) < abs(response.overlap))
		{
			response.overlap = overlap;
			response.overlapN_x = overlapN_x;
			response.overlapN_y = overlapN_y;
			response.x1 = polygon.x + p1x;
			response.y1 = polygon.y + p1y;
			response.x2 = polygon.x + p2x;
			response.y2 = polygon.y + p2y;
			response.type = edge_type;
			
			collision_count++;
		}
	}

	if(collision_count > 0)
	{
		if(response.type == 0)
		{
			normalize(response.overlapN_x, response.overlapN_y, response.overlapN_x, response.overlapN_y);
		}
		
		// Calculate the final overlap vector - based on the smallest overlap.
		response.overlapV_x = response.overlapN_x * response.overlap;
		response.overlapV_y = response.overlapN_y * response.overlap;
		
		return true;
	}
	
	return false;
}

bool test_polygon_circle(Polygon@ polygon, Circle@ circle, CollisionResponse@ response)
{
    // Get the position of the circle relative to the polygon.
	const float circlePos_x = circle.x - polygon.x;
	const float circlePos_y = circle.y - polygon.y;
    const float radius = circle.radius;
    const float radius2 = radius * radius;
	
	const array<float>@ points = @polygon.points;
	const array<float>@ edges = @polygon.edges;
	const array<float>@ normals = @polygon.normals;
	const int p_len = points.length();
	
    // For each edge in the polygon:
    for(int i = 0; i < p_len; i += 2)
	{
		int next = i == p_len - 2 ? 0 : i + 2;
		int prev = i == 0 ? p_len - 2 : i - 2;
		float overlap = 0;
		bool overlapN = false;
		float overlapN_x;
		float overlapN_y;
		
		// Get the edge.
		float edge_x = edges[i];
		float edge_y = edges[i + 1];

		// Calculate the center of the circle relative to the starting point of the edge.
		float point_x = circlePos_x - points[i];
		float point_y = circlePos_y - points[i + 1];

		// If the distance between the center of the circle and the point
		// is bigger than the radius, the polygon is definitely not fully in
		// the circle.
		if(length_sqr(point_x, point_y) > radius2)
		{
			response.aInB = false;
		}

		// Calculate which Voronoi region the center of the circle is in.
		int region = voronoi_region(edge_x, edge_y, point_x, point_y);
		
		// If it's the left region:
		if(region == LEFT_VORONOI_REGION)
		{
			// We need to make sure we're in the RIGHT_VORONOI_REGION of the previous edge.
			edge_x = edges[prev];
			edge_y = edges[prev + 1]; 
			// Calculate the center of the circle relative the starting point of the previous edge
			float point2_x = circlePos_x - points[prev];
			float point2_y = circlePos_y - points[prev + 1];
			region = voronoi_region(edge_x, edge_y, point2_x, point2_y);
			
			if(region == RIGHT_VORONOI_REGION)
			{
				// It's in the region we want.  Check if the circle intersects the point.
				float dist = magnitude(point_x, point_y);
				if(dist > radius)
				{
					// No intersection
					return false;
				}
				else
				{
					// It intersects, calculate the overlap.
					response.bInA = false;
					overlapN = true;
					normalize(point_x, point_y, overlapN_x, overlapN_y);
					overlap = radius - dist;
				}
			}
		}
		// If it's the right region:
		else if(region == RIGHT_VORONOI_REGION)
		{
			// We need to make sure we're in the left region on the next edge
			edge_x = edges[next];
			edge_y = edges[next + 1]; 
			// Calculate the center of the circle relative to the starting point of the next edge.
			point_x = circlePos_x - points[next];
			point_y = circlePos_y - points[next + 1];
			region = voronoi_region(edge_x, edge_y, point_x, point_y);
			
			if (region == LEFT_VORONOI_REGION) {
				// It's in the region we want.  Check if the circle intersects the point.
				float dist = magnitude(point_x, point_y);
				if(dist > radius)
				{
					// No intersection
					return false;
				}
				else
				{
					// It intersects, calculate the overlap.
					response.bInA = false;
					overlapN = true;
					normalize(point_x, point_y, overlapN_x, overlapN_y);
					overlap = radius - dist;
				}
			}
		}
			// Otherwise, it's the middle region:
		else
		{
			// Need to check if the circle is intersecting the edge,
			// Change the edge into its "edge normal".
			float normal_x = normals[i];
			float normal_y = normals[i + 1];
			// Find the perpendicular distance between the center of the 
			// circle and the edge.
			float dist = dot(point_x, point_y, normal_x, normal_y);
			float distAbs = abs(dist);
			// If the circle is on the outside of the edge, there is no intersection.
			if(dist > 0 && distAbs > radius)
			{
				// No intersection
				return false;
			}
			else
			{
				// It intersects, calculate the overlap.
				overlapN = true;
				overlapN_x = normal_x;
				overlapN_y = normal_y;
				overlap = radius - dist;
				// If the center of the circle is on the outside of the edge, or part of the
				// circle is on the outside, the circle is not fully inside the polygon.
				if(dist >= 0 || overlap < 2 * radius)
				{
					response.bInA = false;
				}
			}
		}

		// If this is the smallest overlap we've seen, keep it. 
		// (overlapN may be null if the circle was in the wrong Voronoi region).
		if(overlapN and abs(overlap) < abs(response.overlap))
		{
			response.overlap = overlap;
			response.overlapN_x = overlapN_x;
			response.overlapN_y = overlapN_y;
			response.x1 = polygon.x + points[i];
			response.y1 = polygon.y + points[i + 1];
			response.x2 = polygon.x + points[next];
			response.y2 = polygon.y + points[next + 1];
		}
	}

	// Calculate the final overlap vector - based on the smallest overlap.
	response.overlapV_x = response.overlapN_x * response.overlap;
	response.overlapV_y = response.overlapN_y * response.overlap;
	
	return true;
}

bool check_tile(scene@ g, const int tile_x, const int tile_y, const int tile_type, const int side)
{
	bool valid = false;
	
	tileinfo@ tile = g.get_tile(tile_x, tile_y);
	
	if(tile.solid() and !tile.is_dustblock())
	{
		const int type = int(is_full_edge(tile.type(), side) ? TILE_SHAPE_FULL : tile.type());
		
		if((get_tile_edge(tile, side) & 8) != 0) // Is the edge solid?
		{
			if(tile_type == TILE_SHAPE_FULL)
			{
				return is_full_edge(tile.type(), side);
			}
			else
			{
				return (tile_type == -1 or tile.type() == uint(tile_type));
			}
		}
	}
	
//	if(valid)
//	{
//		if(side == 0) // Top
//		{
//			return !g.get_tile(tile_x, tile_y - 1).solid();
//		}
//		else if(side == 1) // Bottom
//		{
//			return !g.get_tile(tile_x, tile_y + 1).solid();
//		}
//		else if(side == 2) // Left
//		{
//			return !g.get_tile(tile_x - 1, tile_y).solid();
//		}
//		else if(side == 3) // Right
//		{
//			return !g.get_tile(tile_x + 1, tile_y).solid();
//		}
//		
//		return false;
//	}
	
	return false;
}
