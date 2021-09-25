#include '../../lib/embed_utils.cpp';

const string EMBED_BASE					= 'cmc/lab/';
const string SPRITES_BASE				= EMBED_BASE + 'sprites/';

const string EMBED_spr_arc11	= SPRITES_BASE + 'arc11.png';
const string EMBED_spr_arc12	= SPRITES_BASE + 'arc12.png';
const string EMBED_spr_arc13	= SPRITES_BASE + 'arc13.png';
const string EMBED_spr_arc14	= SPRITES_BASE + 'arc14.png';
const string EMBED_spr_arc21	= SPRITES_BASE + 'arc21.png';
const string EMBED_spr_arc22	= SPRITES_BASE + 'arc22.png';
const string EMBED_spr_arc23	= SPRITES_BASE + 'arc23.png';
const string EMBED_spr_arc24	= SPRITES_BASE + 'arc24.png';
const string EMBED_spr_arc_1	= SPRITES_BASE + 'arc21.png';
const string EMBED_spr_arc_2	= SPRITES_BASE + 'arc22.png';
const string EMBED_spr_arc_3	= SPRITES_BASE + 'arc23.png';
const string EMBED_spr_arc_4	= SPRITES_BASE + 'arc24.png';

class script
{
	
	void build_sprites(message@ msg)
	{
		
		build_sprite(msg, 'arc11', 21, 30);
		build_sprite(msg, 'arc12', 21, 30);
		build_sprite(msg, 'arc13', 21, 30);
		build_sprite(msg, 'arc14', 21, 30);
		build_sprite(msg, 'arc21', 21, 30);
		build_sprite(msg, 'arc22', 21, 30);
		build_sprite(msg, 'arc23', 21, 30);
		build_sprite(msg, 'arc24', 21, 30);

		build_sprite(msg, 'arc_1', 21, 30);
		build_sprite(msg, 'arc_2', 21, 30);
		build_sprite(msg, 'arc_3', 21, 30);
		build_sprite(msg, 'arc_4', 21, 30);
	}
	
}
