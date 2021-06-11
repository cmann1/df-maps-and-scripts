#include '../lib/embed_utils.cpp';

const string EMBED_BASE					= 'pa/';
const string SPRITES_BASE				= EMBED_BASE + 'sprites/';
const string AUDIO_BASE					= EMBED_BASE + 'audio/';

const string EMBED_spr_texture2			= SPRITES_BASE + 'texture2.png';
//const string EMBED_spr_hand				= SPRITES_BASE + 'hand.png';

//const string EMBED_snd_arcade_ambience		= AUDIO_BASE + 'arcade_ambience.ogg';

class script
{
	
	void build_sprites(message@ msg)
	{
		build_sprite(msg, 'texture2',			0, 0);
		//build_sprite(msg, 'hand',			0, 0);
	}
	
	void build_sounds(message@ msg)
	{
		//build_sound(msg, 'mall_ambience');
	}
	
}
