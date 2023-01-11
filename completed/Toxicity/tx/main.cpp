#include '../lib/std.cpp';
#include '../lib/drawing/SpriteBatch.cpp';
#include '../lib/tiles/TileEdge.cpp';

#include 'CamDetachTrigger.cpp';

const int FilthTypeSlime = 4;

class script
{
	
	SpriteGroup SprPrismLarge(
		array<string>={'props3','backdrops_3','props3','backdrops_3','props3','backdrops_3','props3','backdrops_3',},
		array<int>={18,8,18,8,18,7,18,7,},
		array<float>={0.5,0.5,1.92185,0.201782,272,0.271442,0.271442,0.5,0.5,1.8408,2.46841,182,0.271442,0.271442,0.5,0.5,1.56215,-0.237823,182,0.195928,0.195928,0.5,0.5,2.40119,5.82814,91.9995,0.195928,0.195928,},
		array<uint>={0,0,0xffffffff,0,0,0xffffffff,0,0,0xffffffff,0,0,0xffffffff,});
	float PrismPropFadeDT = DT * 1.75;
	
	scene@ g;
	
	int num_players;
	
	array<PrismProp@> prism_props;
	
	script()
	{
		@g = get_scene();
		num_players = num_cameras();
	}
	
	void spawn_player(message@ msg)
	{
		const float x = msg.get_float('x');
		const float y = msg.get_float('y') - 90;
		for(int i = -1; i <= 1; i++)
		{
			raycast@ ray = g.ray_cast_tiles(x + 48 * i, y, x + 48 * i, y + 98, TileEdge::TopBit);
			if(ray.hit())
			{
				const int tx = ray.tile_x();
				const int ty = ray.tile_y();
				tilefilth@ t = g.get_tile_filth(tx, ty);
				// Remove spikes
				if(t.top() >= 9 || t.top() <= 13)
				{
					t.top(0);
					g.set_tile_filth(tx, ty, t);
				}
			}
		}
	}
	
	void entity_on_add(entity@ e) {
		filth_ball@ fb = e.as_filth_ball();
		if(@fb == null)
			return;
		
		fb.filth_type(FilthTypeSlime);
	}
	
	void entity_on_remove(entity@ e)
	{
		const string name = e.type_name();
		
		if(name == 'enemy_tutorial_hexagon' || name == 'enemy_tutorial_square')
		{
			PrismProp@ pp = PrismProp();
			pp.x = e.x();
			pp.y = e.y();
			pp.scale = name == 'enemy_tutorial_hexagon' ? 1.0 : 0.613485630066323;
			prism_props.insertLast(pp);
			
			int i = g.get_prop_collision(pp.y, pp.y, pp.x, pp.x);
			while(--i >= 0)
			{
				prop@ p = g.get_prop_collision_index(i);
				if(p.prop_set() != 3 || p.prop_group() != 22 || p.prop_index() != 3)
					continue;
				if(p.layer() != 18 || (p.sub_layer() != 8 && p.sub_layer() != 7))
					continue;
				
				g.remove_prop(p);
			}
		}
	}
	
	void step(int)
	{
		for(int i = int(prism_props.length) - 1; i >= 0; i--)
		{
			PrismProp@ pp = @prism_props[i];
			pp.alpha -= PrismPropFadeDT;
			if(pp.alpha <= 0)
			{
				prism_props.removeAt(i);
			}
		}
	}
	
	void draw(float sub_frame)
	{
		for(uint i = 0; i < prism_props.length; i++)
		{
			PrismProp@ pp = @prism_props[i];
			SprPrismLarge.draw(pp.x, pp.y, 0, pp.scale, 0, 0, lerp(min(pp.alpha + PrismPropFadeDT, 1.0), pp.alpha, sub_frame));
		}
	}
	
}

class PrismProp
{
	
	float x, y;
	float alpha = 1;
	float scale;
	
}
