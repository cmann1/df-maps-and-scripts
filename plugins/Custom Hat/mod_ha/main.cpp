#include '../common/utils.cpp'
#include '../common/embeds.cpp'
#include 'Hat.cpp'

class script
{
	
	scene@ g;
	
	array<Hat@> hats;
	int num_hats = 0;
	
	int sprite_origin_x = 0;
	int sprite_origin_y = 0;
	int frame_count = 0;
	float fps = 10;
	
	script()
	{
		@g = get_scene();
		
		num_hats = num_cameras();
		hats.resize(num_hats);
		
		bool has_hat_config = load_embed('dat_mod_ha_config', 'hats/config.txt');
		string hat_name = has_hat_config ? get_embed_value('dat_mod_ha_config') : 'default';
		
		load_embed('dat_mod_ha_hat', 'hats/' + hat_name + '.txt');
		
		bool flip_hit = true;
		
		float fps = 10;
		
		array<string>@ hat_data = get_embed_value('dat_mod_ha_hat').split(' ');
		if(hat_data.length() > 0) sprite_origin_x = parseInt(hat_data[0]);
		if(hat_data.length() > 1) sprite_origin_y = parseInt(hat_data[1]);
		if(hat_data.length() > 2) flip_hit = hat_data[2] != 'false';
		if(hat_data.length() > 3) frame_count = parseInt(hat_data[3]);
		if(hat_data.length() > 4) fps = parseFloat(hat_data[4]);
		
		if(frame_count > 0)
		{
			for(int i = 0; i < frame_count; i++)
				load_embed('spr_mod_ha_hat_' + (i + 1), 'hats/' + hat_name + (i + 1) + '.png');
		}
		else
		{
			load_embed('spr_mod_ha_hat', 'hats/' + hat_name + '.png');
		}
		
		fps = fps / 60;
		
		for(int i = 0; i < num_hats; i++)
		{
			Hat@ hat = Hat(i);
			@hats[i] = hat;
			hat.flip_for_facing = flip_hit;
			hat.frame_count = frame_count;
			hat.fps_step = fps;
		}
	}
	
	void build_sprites(message@ msg)
	{
		if(frame_count > 0)
		{
			for(int i = 0; i < frame_count; i++)
				build_sprite(msg, 'mod_ha_hat_' + (i + 1), sprite_origin_x, sprite_origin_y);
		}
		else
		{
			build_sprite(msg, 'mod_ha_hat', sprite_origin_x, sprite_origin_y);
		}
	}
	
	void checkpoint_load()
	{
		for(int i = 0; i < num_hats; i++)
		{
			@hats[i].player = null;
		}
	}
	
	void step(int entities)
	{
		for(int i = 0; i < num_hats; i++)
		{
			hats[i].step();
		}
	}
	
	void step_post(int entities)
	{
		for(int i = 0; i < num_hats; i++)
		{
			hats[i].step_post();
		}
	}
	
	void draw(float sub_frame)
	{
		for(int i = 0; i < num_hats; i++)
		{
			hats[i].draw(sub_frame);
		}
	}
	
}