const string EMBED_BASE		= 'mod_be/';
const string SPRITES_BASE	= EMBED_BASE + 'sprites/';

const string APPLE_BASE			= SPRITES_BASE + 'apple/';
const string EMBED_spr_fall		= APPLE_BASE + 'fall.png';
const string EMBED_spr_hitwall1	= APPLE_BASE + 'hitwall1.png';
const string EMBED_spr_hitwall2	= APPLE_BASE + 'hitwall2.png';
const string EMBED_spr_hover	= APPLE_BASE + 'hover.png';
const string EMBED_spr_idle		= APPLE_BASE + 'idle.png';
const string EMBED_spr_land		= APPLE_BASE + 'land.png';
const string EMBED_spr_stun1	= APPLE_BASE + 'stun1.png';
const string EMBED_spr_stun2	= APPLE_BASE + 'stun2.png';

const string EMBED_spr_egg_1	= SPRITES_BASE + 'egg_1.png';
const string EMBED_spr_egg_2	= SPRITES_BASE + 'egg_2.png';
const string EMBED_spr_egg_3	= SPRITES_BASE + 'egg_3.png';
const string EMBED_spr_egg_4	= SPRITES_BASE + 'egg_4.png';
const string EMBED_spr_egg_5	= SPRITES_BASE + 'egg_5.png';

void embed_build_sprites(message@ msg)
{
	build_sprite(msg, 'fall',		43, 37);
	build_sprite(msg, 'hitwall1',	24, 46);
	build_sprite(msg, 'hitwall2',	22, 42);
	build_sprite(msg, 'hover',		39, 40);
	build_sprite(msg, 'idle',		25, 47);
	build_sprite(msg, 'land',		46, 31);
	build_sprite(msg, 'stun1',		47, 25);
	build_sprite(msg, 'stun2',		25, 19);
	
	build_sprite(msg, 'egg_1',		24, 30);
	build_sprite(msg, 'egg_2',		24, 30);
	build_sprite(msg, 'egg_3',		24, 30);
	build_sprite(msg, 'egg_4',		24, 30);
	build_sprite(msg, 'egg_5',		24, 30);
}
