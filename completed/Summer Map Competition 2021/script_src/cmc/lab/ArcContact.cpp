#include '../../lib/utils/colour.cpp';
#include '../../lib/drawing/circle.cpp';
#include '../../lib/tiles/common.cpp';
#include '../../lib/tiles/EdgeFlags.cpp';
#include '../../lib/tiles/get_tile_edge_points.cpp';

#include 'ArcAreas.cpp';
#include 'ArcContactArea.cpp';

namespace ArcContact
{
	
	const int Point = 0;
	const int Line = 1;
	const int Rect = 2;
	const int Tiles = 3;
	
	const uint SrcClr = 0xffff5555;
	const uint DstClr = 0xff55ff55;
	
}

class ArcContact : ArcAreas
{
	
	// I had to remove the annotations from fields marked with "LAB UNUSED" to try and help with
	// checkpoint lag spikes in the cmclab map
	
	[option,0:Point,1:Line,2:Rect,3:Tiles] int type = ArcContact::Point;
	[persist] float radius;
	[position,mode:world,layer:19,y:y1] float x1;
	[hidden] float y1;
	[position,mode:world,layer:19,y:y2] float x2;
	[hidden] float y2;
	[persist|tooltip:'Affects the relative chances of an arc\nspawning from/touching this surface.'] /* LAB UNUSED */
	float weight = 1;
	
	script@ script;
	
	void initialise(script@ script) 
	{
		@this.script = script;
		
		areas.resize(0);
		
		switch(type)
		{
			case ArcContact::Point: initialise_point(); break;
			case ArcContact::Line:  initialise_line(); break;
			case ArcContact::Rect:  initialise_rect(); break;
			case ArcContact::Tiles: initialise_tiles(); break;
		}
		
		initialise();
	}
	
	protected ArcContactArea@ create_area(const int type)
	{
		ArcContactArea@ area = ArcContactArea(this, type, weight);
		return area;
	}
	
	private void initialise_point()
	{
		ArcContactArea@ area = create_area(ArcContact::Point);
		area.x1 = x1;
		area.y1 = y1;
		area.x2 = radius;
		area.size = max(2 * PI * radius, 10.0);
		areas.insertLast(area);
	}
	
	private void initialise_line()
	{
		ArcContactArea@ area = create_area(ArcContact::Line);
		area.x1 = x1;
		area.y1 = y1;
		area.x2 = x2;
		area.y2 = y2;
		area.size = distance(area.x1, area.y1, area.x2, area.y2);
		areas.insertLast(area);
	}
	
	private void initialise_rect()
	{
		ArcContactArea@ area = create_area(ArcContact::Rect);
		area.x1 = x1;
		area.y1 = y1;
		area.x2 = x2;
		area.y2 = y2;
		area.size = 2 * abs(x2 - x1) + 2 * abs(y2 - y1);
		areas.insertLast(area);
	}
	
	private void initialise_tiles()
	{
		const float x1 = min(this.x1, this.x2);
		const float y1 = min(this.y1, this.y2);
		const float x2 = max(this.x1, this.x2);
		const float y2 = max(this.y1, this.y2);
		const int tx1 = floor_int(x1 / 48);
		const int ty1 = floor_int(y1 / 48);
		const int tx2 = floor_int(x2 / 48);
		const int ty2 = floor_int(y2 / 48);
		
		for(int tx = tx1; tx <= tx2; tx++)
		{
			for(int ty = ty1; ty <= ty2; ty++)
			{
				tileinfo@ tile = script.g.get_tile(tx, ty);
				if(!tile.solid())
					continue;
				
				const int shape = tile.type();
				const float x = tx * 48;
				const float y = ty * 48;
				for(int side = TileEdge::Top; side <= TileEdge::Right; side++)
				{
					const uint8 edge = get_tile_edge(tile, side);
					if(edge & EdgeFlags::Collision == 0)
						continue;
					
					float ex1, ey1, ex2, ey2;
					const bool is_valid = get_edge_points(shape, side, ex1, ey1, ex2, ey2, x, y);
					if(!is_valid)
						continue;
					
					float t_min, t_max;
					if(!line_aabb_intersection(
						ex1, ey1, ex2, ey2,
						x1, y1, x2, y2,
						t_min, t_max)
					)
						continue;
					
					const float dx = ex2 - ex1;
					const float dy = ey2 - ey1;
					ArcContactArea@ area = create_area(ArcContact::Tiles);
					area.x1 = ex1 + dx * max(t_min, 0.0);
					area.y1 = ey1 + dy * max(t_min, 0.0);
					area.x2 = ex1 + dx * min(t_max, 1.0);
					area.y2 = ey1 + dy * min(t_max, 1.0);
					area.size = distance(area.x1, area.y1, area.x2, area.y2);
					areas.insertLast(area);
				}
			}
		}
	}
	
	void editor_draw(const bool is_source)
	{
		switch(type)
		{
			case ArcContact::Point: draw_point(is_source); break;
			case ArcContact::Line: draw_line(is_source); break;
			case ArcContact::Rect: draw_rect(is_source); break;
			case ArcContact::Tiles: draw_tiles(is_source); break;
		}
	}
	
	private void draw_point(const bool is_source)
	{
		const uint clr = is_source ? ArcContact::SrcClr : ArcContact::DstClr;
		draw_dot(script.g, 22, 22, x1, y1, 4 * script.ed_zoom, clr, 45);
		
		if(radius > 0)
		{
			drawing::circle(script.g, 22, 22,
				x1, y1, radius, 32,
				1 * script.ed_zoom, is_source ? ArcContact::SrcClr : ArcContact::DstClr);
		}
	}
	
	private void draw_line(const bool is_source)
	{
		const uint clr = is_source ? ArcContact::SrcClr : ArcContact::DstClr;
		::draw_line(script.g, 22, 22, x1, y1, x2, y2, 1 * script.ed_zoom, clr);
		draw_dot(script.g, 22, 22, x1, y1, 4 * script.ed_zoom, clr, 45);
		draw_dot(script.g, 22, 22, x2, y2, 4 * script.ed_zoom, clr, 45);
	}
	
	private void draw_rect(const bool is_source)
	{
		const uint clr = is_source ? ArcContact::SrcClr : ArcContact::DstClr;
		outline_rect_inside(script.g, 22, 22, x1, y1, x2, y2, 1 * script.ed_zoom, clr);
	}
	
	private void draw_tiles(const bool is_source)
	{
		const uint clr = is_source ? ArcContact::SrcClr : ArcContact::DstClr;
		const uint e_clr = multiply_alpha(clr, 0.5);
		
		for(uint i = 0; i < areas.length; i++)
		{
			ArcContactArea@ area = @areas[i];
			::draw_line(script.g, 22, 22, area.x1, area.y1, area.x2, area.y2, 1 * script.ed_zoom, e_clr);
		}
		
		outline_rect_inside(script.g, 22, 22, x1, y1, x2, y2, 2 * script.ed_zoom, clr);
	}
	
}
