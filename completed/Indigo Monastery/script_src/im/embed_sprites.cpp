const string EMBED_spr_clock_hand_small = "im/sprites/clock_hand_small.png";
const string EMBED_spr_clock_hand_large = "im/sprites/clock_hand_large.png";
const string EMBED_spr_clock_back = "im/sprites/clock_back.png";
const string EMBED_spr_note_1 = "im/sprites/note_1.png";
const string EMBED_spr_note_2 = "im/sprites/note_2.png";

void embed_build_sprites(message@ msg)
{
	build_sprite(msg, 'clock_back', 96, 96);
	build_sprite(msg, 'clock_hand_large', 21, 20);
	build_sprite(msg, 'clock_hand_large', 21, 20);
	build_sprite(msg, 'note_1', 11, 20);
	build_sprite(msg, 'note_2', 20, 20);
}
