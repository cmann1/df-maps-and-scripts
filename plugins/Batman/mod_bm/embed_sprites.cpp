const string EMBED_spr_bat_ears = R + 'bat_ears.png';
const string EMBED_spr_cape = R + 'cape.png';

const string EMBED_spr_pow = R + 'pow.png';
const string EMBED_spr_zap = R + 'zap.png';
const string EMBED_spr_crash = R + 'crash.png';
const string EMBED_spr_bang = R + 'bang.png';

void embed_build_sprites(message@ msg)
{
	build_sprite(msg, 'bat_ears', 10, 16);
	build_sprite(msg, 'cape', 30, 3);
	
	build_sprite(msg, 'pow', 64, 55);
	build_sprite(msg, 'zap', 54, 44);
	build_sprite(msg, 'crash', 54, 44);
	build_sprite(msg, 'bang', 51, 57);
}
