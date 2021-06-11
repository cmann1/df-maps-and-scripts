#include "math.cpp"
#include "drawing_utils.cpp"
#include "utils.cpp"
#include "ColType.cpp"
#include "TileShape.cpp"
#include "Fx.cpp"
#include "Sprite.cpp"
#include "PortalGun.cpp"

class script
{
	scene@ g;
	controllable@ player;
	dustman@ dm;
	
	FxManager@ fx_manager;
	
	Sprite@ s;
	bool draw_s = false;
	float px = 0;
	float py = 0;
	float sx = 0;
	float sy = 0;
	int tx = 0;
	int ty = 0;
	int t1x = 0;
	int t1y = 0;
	int t2x = 0;
	int t2y = 0;
	bool t1 = false;
	bool t2 = false;
	
	PortalGun@ portal_gun;
	
	uint t = 0;
	
	textfield@ help_text;

	script()
	{
		@g = get_scene();
		
		@fx_manager = FxManager();
		
		@s = Sprite("props4", "machinery_18");
		
		@help_text = create_textfield();
		help_text.align_horizontal(-1);
		help_text.align_vertical(-1);
		help_text.set_font("ProximaNovaReg", 36);
		help_text.text(
			"Toggle portal gun: Taunt\n" +
			"Blue portal: Light attack\n" +
			"Orange portal: Heavy attack"
		);
		
		@portal_gun = PortalGun(g);
	}
	
	void checkpoint_save()
	{
		fx_manager.checkpoint_save();
	}

	void checkpoint_load()
	{
		@player = null;
		@dm = null;
		
		fx_manager.checkpoint_load();
	}
	
	void step(int entities)
	{
		t++;
		
		if(@player == null)
		{
			entity@e = controller_entity(0);
			if(@e != null)
			{
				@player = e.as_controllable();
				@dm = e.as_dustman();
			}
		}
		else
		{
			if(player.taunt_intent() != 0)
			{
				portal_gun.active = !portal_gun.active;
			}
			
			if(portal_gun.active)
			{
				portal_gun.update(player.x(), player.y() - 60, player.x_intent() == 0 ? player.face() : player.x_intent(), player.y_intent());
			
				if(player.light_intent() == 10)
				{
					portal_gun.shoot_1();
				}
				else if(player.heavy_intent() == 10)
				{
					portal_gun.shoot_2();
				}
				
				player.light_intent(0);
				player.heavy_intent(0);
			}
		
			portal_gun.step();
		}
	}
	
	void step_post(int entities)
	{
	}
	
	void draw(float sub_frame)
	{
		portal_gun.draw(sub_frame);
		
		if(draw_s)
		{
			draw_line(g, px, py, sx, sy, 20, 19, 2, 0x99FF0000);
			
			s.draw_world(20, 20,
				0, 0,
				sx, sy, 0,
				0.5, 0.5, 0xFFFFFFFF);
				
			outline_tile(g, t1x, t1y, 19, 17, 1, t1 ? 0xFF00FF00 : 0xFFFF0000);
			outline_tile(g, t2x, t2y, 19, 17, 1, t2 ? 0xFF0000FF : 0xFFFF0000);
			outline_tile(g, tx, ty, 19, 17, 1, 0xFF00FFFF);
		}
		
		help_text.draw_hud(19, 19, -800 + 10, -450 + 10, 0.75, 0.75, 0);
	}
	
	void entity_on_add(entity@ e) {}
	void entity_on_remove(entity@ e) {}
	
	
}