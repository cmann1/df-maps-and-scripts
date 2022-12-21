#include '../lib/std.cpp';
#include '../lib/drawing/common.cpp';
#include '../shared/entity_outliner/EntityOutliner.cpp';

#include 'sprites.cpp';
#include 'Recharger.cpp';
#include 'EntityOutlinerSourceExt.cpp';

const int ChargerNone = 0;
const int ChargerIdle = 1;
const int ChargerActive = 2;

class script
{
	
	[position,mode:world,layer:19,y:dust_y1] float dust_x1;
	[hidden] float dust_y1;
	[position,mode:world,layer:19,y:dust_y2] float dust_x2;
	[hidden] float dust_y2;
	[persist|label:Outliner] EntityOutlinerSettings settings;
	
	scene@ g;
	camera@ cam;
	
	RechargerSprites sprites;
	
	bool is_playing;
	float time_scale = 1;
	
	float view_x1, view_y1;
	float view_x2, view_y2;
	
	private EntityOutliner entity_outliner;
	
	private dictionary chargers;
	private dictionary chargers_cp;
	private int inactive_chargers;
	private int inactive_chargers_cp;
	
	
	script()
	{
		@g = get_scene();
		init_cam();
		
		entity_outliner.init_settings(@settings);
	}
	
	void on_level_start()
	{
		is_playing = true;
		
		init_players();
	}
	
	void on_level_end()
	{
		create_charger_dust();
	}
	
	void checkpoint_save()
	{
		entity_outliner.checkpoint_save();
		
		chargers_cp = chargers;
		inactive_chargers_cp = inactive_chargers;
	}
	
	void checkpoint_load()
	{
		entity_outliner.checkpoint_load(cam);
		
		chargers = chargers_cp;
		inactive_chargers = inactive_chargers_cp;
		
		init_players();
	}
	
	private void init_players()
	{
		const uint cam_count = num_cameras();
		for(uint i = 0; i < cam_count; i++)
		{
			controllable@ c = controller_controllable(i);
			if(@c == null)
				return;
			
			dustman@ dm = c.as_dustman();
			if(@dm == null)
				return;
			
			// Increase the dustblock clear region below the player so the two sections where you have to fall
			// into a spiked dustblock to break it are a little easier.
			float top, bottom, left, right;
			dm.get_dustblock_clear_rect(top, bottom, left, right);
			bottom += 16;
			dm.dustblock_clear_rect(top, bottom, left, right);
		}
	}
	
	void editor_var_changed(var_info@ info)
	{
		entity_outliner.editor_var_changed(info, @settings);
	}
	
	void register_charger(const uint id)
	{
		const string key = id + '';
		if(chargers.exists(key))
			return;
		
		chargers[key] = ChargerIdle;
		inactive_chargers++;
	}
	
	void activate_charger(const uint id)
	{
		const string key = id + '';
		int charger_state = ChargerNone;
		if(!chargers.get(key, charger_state))
			return;
		
		if(charger_state == ChargerIdle)
		{
			chargers[key] = ChargerActive;
			inactive_chargers--;
		}
	}
	
	void step(int)
	{
		init_cam();
		entity_outliner.step(cam);
	}
	
	void editor_step()
	{
		init_cam();
		entity_outliner.editor_step(cam);
	}
	
	void init_cam()
	{
		@cam = get_active_camera();
		float left1, top1, width1, height1;
		float left2, top2, width2, height2;
		cam.get_layer_draw_rect(0, 19, left1, top1, width1, height1);
		cam.get_layer_draw_rect(1, 19, left2, top2, width2, height2);
		view_x1 = min(left1, left2);
		view_y1 = min(top1, top2);
		view_x2 = max(left1 + width1, left2 + width2);
		view_y2 = max(top1 + height1, top2 + height2);
		//const float s = 50;
		//view_x1 += s; view_y1 += s;
		//view_x2 -= s; view_y2 -= s;
		
		time_scale = g.time_warp();
	}
	
	void draw(float sub_frame)
	{
		entity_outliner.draw(sub_frame);
		
		//outline_rect(g, 22, 22, view_x1, view_y1, view_x2, view_y2, 1, 0xffff0000);
	}
	
	void editor_draw(float sub_frame)
	{
		entity_outliner.draw(sub_frame);
		
		int x1, y1, x2, y2;
		calculate_dust_tiles(x1, y1, x2, y2);
		outline_rect_inside(g, 22, 22, x1 * 48, y1 * 48, (x2 + 1) * 48, (y2 + 1) * 48, 4, 0x9900ff00);
	}
	
	private void calculate_dust_tiles(int &out x1, int &out y1, int &out x2, int &out y2)
	{
		x1 = int(floor(dust_x1 / 48.0));
		y1 = int(floor(dust_y1 / 48.0));
		x2 = int(floor(dust_x2 / 48.0));
		y2 = int(floor(dust_y2 / 48.0));
	}
	
	private void create_charger_dust()
	{
		puts('Inactive chargers: ' + inactive_chargers);
		
		if(inactive_chargers == 0)
			return;
		
		int x1, y1, x2, y2;
		calculate_dust_tiles(x1, y1, x2, y2);
		x2++;
		y2++;
		const int w = x2 - x1;
		const int h = y2 - y1;
		int dust_count = 0;
		const int ac = h * w;
		
		for(int x = x1; x < x2; x++)
		{
			for(int y = y1; y < y2; y++)
			{
				tilefilth@ f = g.get_tile_filth(x, y);
				f.top(2);
				g.set_tile_filth(x, y, f);
				dust_count++;
				
				if(dust_count >= inactive_chargers)
					return;
			}
		}
	}
	
}
