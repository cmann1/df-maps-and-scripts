#include '../lib/embed_utils.cpp';

const string EMBED_BASE					= 'sk/';
const string SPRITES_BASE				= EMBED_BASE + 'sprites/';

const string EMBED_spr_texture			= SPRITES_BASE + 'texture.png';

class script
{
	
	void build_sprites(message@ msg)
	{
		build_sprite(msg, 'texture', 0, 0);
	}
	
}
