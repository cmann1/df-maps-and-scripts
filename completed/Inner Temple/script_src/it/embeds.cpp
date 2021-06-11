const string EMBED_spr_sun = 'it/sprites/sun.png';
const string EMBED_spr_river = 'it/sprites/river.png';
const string EMBED_spr_moon = 'it/sprites/moon.png';

void embed_build_sprites(message@ msg)
{
	build_sprite(msg, 'sun', 40, 40);
	build_sprite(msg, 'river', 40, 21);
	build_sprite(msg, 'moon', 37, 40);
}