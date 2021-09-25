#include '../../lib/embed_utils.cpp';

const string EMBED_BASE					= 'cmc/nexus/';
const string SPRITES_BASE				= EMBED_BASE + 'sprites/';
const string AUDIO_BASE					= EMBED_BASE + 'audio/';

const string EMBED_spr_vines			= SPRITES_BASE + 'vines.png';
const string EMBED_spr_vines_dead		= SPRITES_BASE + 'vines_dead.png';

const string EMBED_snd_gate_rattle		= AUDIO_BASE + 'gate_rattle.ogg';
const string EMBED_snd_elevator_move	= AUDIO_BASE + 'elevator_move.ogg';
const string EMBED_snd_power_down		= AUDIO_BASE + 'power_down.ogg';

class script
{
	
	void build_sprites(message@ msg)
	{
		
		build_sprite(msg, 'vines', 187, 330);
		build_sprite(msg, 'vines_dead', 201, 314);
	}
	
	void build_sounds(message@ msg)
	{
		build_sound(msg, 'gate_rattle');
		build_sound(msg, 'elevator_move');
		build_sound(msg, 'power_down');
	}
	
}
