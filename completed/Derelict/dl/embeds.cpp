#include '../lib/embed_utils.cpp';

/*
 * Velociraptor_Hiss.aif				https://freesound.org/people/crcavol/sounds/154636/
 * Security alarm						https://freesound.org/people/thegreatbelow/sounds/546542/
 * 2 Hours King Cobra Sound Effect		https://www.youtube.com/watch?v=z4KEqWhsvuc
*/

const string EMBED_BASE					= 'dl/';
const string SPRITES_BASE				= EMBED_BASE + 'sprites/';
const string AUDIO_BASE					= EMBED_BASE + 'audio/';

//

const string EMBED_spr_snake_head					= SPRITES_BASE + 'snake_head.png';
const string EMBED_spr_snake_jaw					= SPRITES_BASE + 'snake_jaw.png';
const string EMBED_spr_snake_teeth					= SPRITES_BASE + 'snake_teeth.png';
const string EMBED_spr_snake_gums					= SPRITES_BASE + 'snake_gums.png';
const string EMBED_spr_snake_eye					= SPRITES_BASE + 'snake_eye.png';
const string EMBED_spr_snake_segment_flat			= SPRITES_BASE + 'snake_segment_flat.png';
const string EMBED_spr_snake_segment				= SPRITES_BASE + 'snake_segment.png';
const string EMBED_spr_snake_segment_last			= SPRITES_BASE + 'snake_segment_last.png';
const string EMBED_spr_snake_segment_shadow			= SPRITES_BASE + 'snake_segment_shadow.png';
const string EMBED_spr_snake_tongue_1				= SPRITES_BASE + 'snake_tongue_1.png';
const string EMBED_spr_snake_tongue_2				= SPRITES_BASE + 'snake_tongue_2.png';

// 

const string EMBED_snd_hiss_subtle_1	= AUDIO_BASE + 'hiss_subtle_1.ogg';
const string EMBED_snd_hiss_subtle_2	= AUDIO_BASE + 'hiss_subtle_2.ogg';
const string EMBED_snd_hiss_subtle_3	= AUDIO_BASE + 'hiss_subtle_3.ogg';
const string EMBED_snd_hiss_soft		= AUDIO_BASE + 'hiss_soft.ogg';
const string EMBED_snd_hiss_med			= AUDIO_BASE + 'hiss_med.ogg';
const string EMBED_snd_hiss_hard		= AUDIO_BASE + 'hiss_hard.ogg';

const string EMBED_snd_attack_hiss_1	= AUDIO_BASE + 'attack_hiss_1.ogg';
const string EMBED_snd_attack_hiss_2	= AUDIO_BASE + 'attack_hiss_2.ogg';
const string EMBED_snd_attack_hiss_3	= AUDIO_BASE + 'attack_hiss_3.ogg';
const string EMBED_snd_attack_hiss_4	= AUDIO_BASE + 'attack_hiss_4.ogg';

const string EMBED_snd_alarm		= AUDIO_BASE + 'alarm.ogg';

class script
{
	
	void build_sprites(message@ msg)
	{
		build_sprite(msg, 'snake_head',					16, 23);
		build_sprite(msg, 'snake_jaw',					12, 7);
		build_sprite(msg, 'snake_teeth',				6, 7);
		build_sprite(msg, 'snake_gums',					11, 22);
		build_sprite(msg, 'snake_eye',					-25, 17);
		build_sprite(msg, 'snake_segment_flat',			56, 22);
		build_sprite(msg, 'snake_segment',				58, 22);
		build_sprite(msg, 'snake_segment_last',			58, 22);
		build_sprite(msg, 'snake_segment_shadow',		62, 28);
		build_sprite(msg, 'snake_tongue_1',				-44, 5);
		build_sprite(msg, 'snake_tongue_2',				-44, 5);
	}
	
	void build_sounds(message@ msg)
	{
		build_sound(msg, 'attack_hiss_1');
		build_sound(msg, 'attack_hiss_2');
		build_sound(msg, 'attack_hiss_3');
		build_sound(msg, 'attack_hiss_4');
		build_sound(msg, 'hiss_subtle_1');
		build_sound(msg, 'hiss_subtle_2');
		build_sound(msg, 'hiss_subtle_3');
		build_sound(msg, 'hiss_soft');
		build_sound(msg, 'hiss_med');
		build_sound(msg, 'hiss_hard');
		build_sound(msg, 'alarm');
	}
	
}
