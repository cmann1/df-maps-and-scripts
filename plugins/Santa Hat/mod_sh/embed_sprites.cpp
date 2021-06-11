const string EMBED_spr_hat_rim = R + 'hat_rim.png';
const string EMBED_spr_hat_bob = R + 'hat_bob.png';

void embed_build_sprites(message@ msg)
{
	build_sprite(msg, 'hat_rim', 17, 6);
	build_sprite(msg, 'hat_bob', 8, 8);
}
