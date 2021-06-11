#include '../lib/triggers/InstancedSprite.cpp';
#include '../lib/embed_utils.cpp';
#include 'Mirror.cpp';
#include 'Gravestone.cpp';

const string EMBED_spr_wallpaper	= 'cmr59/wallpaper.png';
const string EMBED_spr_mirror_surface	= 'cmr59/mirror_surface.png';

class script
{
	
	bool in_game;
	
	[colour,alpha] uint text_clr = 0xff999999;
	[colour,alpha] uint light_clr = 0xffffffaa;
	[colour,alpha] uint dark_clr = 0xff000000;
	
	void on_level_start()
	{
		in_game = true;
	}
	
	void build_sprites(message@ msg)
	{
		build_sprite(msg, 'wallpaper', 240, 144);
		build_sprite(msg, 'mirror_surface', 240, 144);
	}
	
}