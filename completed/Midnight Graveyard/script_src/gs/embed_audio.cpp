/*
 * Eerie Atmosphere
 * https://freesound.org/people/officialfourge/sounds/341977/
 * 
 * hanging bats (two) with wings.wav
 * https://freesound.org/people/sastoryteller/sounds/352899/
 * 
 * cry.wav
 * https://freesound.org/people/silversatyr/sounds/113364/
 * 
 * Bird flying away.wav
 * https://freesound.org/people/flashdeejay/sounds/241032/
 * 
 * Monster Snarl 2
 * Monster Snarl 4
 * https://freesound.org/people/pikachu09/sounds/204609/
 * https://freesound.org/people/pikachu09/sounds/204611
 * 
 * evil_laugh_departing_0m38s.wav
 * https://freesound.org/people/MadamVicious/sounds/218172/
 * */

const string EMBED_snd_ambience = "gs/audio/ambience.ogg";
const string EMBED_snd_bat_01 = "gs/audio/bat_01.ogg";
const string EMBED_snd_bat_02 = "gs/audio/bat_02.ogg";
const string EMBED_snd_crying = "gs/audio/crying2.ogg";
const string EMBED_snd_crow_fade = "gs/audio/crow_fade.ogg";
const string EMBED_snd_crow_01 = "gs/audio/crow_01.ogg";
const string EMBED_snd_crow_02 = "gs/audio/crow_02.ogg";
const string EMBED_snd_crow_03 = "gs/audio/crow_03.ogg";
const string EMBED_snd_crow_04 = "gs/audio/crow_04.ogg";
const string EMBED_snd_crow_05 = "gs/audio/crow_04.ogg";
const string EMBED_snd_bird_flapping_01 = "gs/audio/bird_flapping_01.ogg";
const string EMBED_snd_bird_flapping_02 = "gs/audio/bird_flapping_02.ogg";
const string EMBED_snd_monster_snarl = "gs/audio/monster_snarl.ogg";
const string EMBED_snd_witch_laugh = "gs/audio/witch_laugh.ogg";

void embed_build_sounds(message@ msg)
{
	build_sound(msg, "ambience");
	build_sound(msg, "bat_01");
	build_sound(msg, "bat_02");
	build_sound(msg, "crying");
	build_sound(msg, "crow_fade");
	build_sound(msg, "crow_01");
	build_sound(msg, "crow_02");
	build_sound(msg, "crow_03");
	build_sound(msg, "crow_04");
	build_sound(msg, "crow_05");
	build_sound(msg, "bird_flapping_01");
	build_sound(msg, "bird_flapping_02");
	build_sound(msg, "monster_snarl");
	build_sound(msg, "witch_laugh");
}