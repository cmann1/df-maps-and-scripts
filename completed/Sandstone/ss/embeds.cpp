#include '../lib/embed_utils.cpp';

const string EMBED_BASE					= 'ss/';
const string SPRITES_BASE				= EMBED_BASE + 'sprites/';
const string AUDIO_BASE					= EMBED_BASE + 'audio/';

const string EMBED_spr_rope				= SPRITES_BASE + 'rope.png';
const string EMBED_spr_rope_joint		= SPRITES_BASE + 'rope_joint.png';
const string EMBED_spr_chain_link_1		= SPRITES_BASE + 'chain_link_1.png';
const string EMBED_spr_chain_link_2		= SPRITES_BASE + 'chain_link_2.png';
const string EMBED_spr_chain_link_3		= SPRITES_BASE + 'chain_link_3.png';
const string EMBED_spr_anchor_broken	= SPRITES_BASE + 'anchor_broken.png';
const string EMBED_spr_chirp_ring		= SPRITES_BASE + 'chirp_ring.png';

const string EMBED_snd_scarf_get_amb	= AUDIO_BASE + 'scarf_get_amb.ogg';
const string EMBED_snd_chirp_small_1	= AUDIO_BASE + 'chirp_small_1.ogg';
const string EMBED_snd_chirp_small_2	= AUDIO_BASE + 'chirp_small_2.ogg';
const string EMBED_snd_chirp_small_3	= AUDIO_BASE + 'chirp_small_3.ogg';
const string EMBED_snd_chirp_med_1		= AUDIO_BASE + 'chirp_med_1.ogg';
const string EMBED_snd_chirp_med_2		= AUDIO_BASE + 'chirp_med_2.ogg';
const string EMBED_snd_chirp_med_3		= AUDIO_BASE + 'chirp_med_3.ogg';
const string EMBED_snd_chirp_med_4		= AUDIO_BASE + 'chirp_med_4.ogg';
const string EMBED_snd_chirp_big_1		= AUDIO_BASE + 'chirp_big_1.ogg';
const string EMBED_snd_chirp_big_2		= AUDIO_BASE + 'chirp_big_2.ogg';
const string EMBED_snd_chirp_big_3		= AUDIO_BASE + 'chirp_big_3.ogg';

class script
{
	
	void build_sprites(message@ msg)
	{
		
		build_sprite(msg, 'rope', 0, 15);
		build_sprite(msg, 'rope_joint', 15, 15);
		build_sprite(msg, 'chain_link_1', 10, 13);
		build_sprite(msg, 'chain_link_2', 10, 17);
		build_sprite(msg, 'chain_link_3', 10, 15);
		build_sprite(msg, 'anchor_broken', 29, 62);
		build_sprite(msg, 'chirp_ring', 96, 96);
	}
	
	void build_sounds(message@ msg)
	{
		build_sound(msg, 'scarf_get_amb');
		
		build_sound(msg, 'chirp_small_1');
		build_sound(msg, 'chirp_small_2');
		build_sound(msg, 'chirp_small_3');
		build_sound(msg, 'chirp_med_1');
		build_sound(msg, 'chirp_med_2');
		build_sound(msg, 'chirp_med_3');
		build_sound(msg, 'chirp_med_4');
		build_sound(msg, 'chirp_big_1');
		build_sound(msg, 'chirp_big_2');
		build_sound(msg, 'chirp_big_3');
	}
	
}
