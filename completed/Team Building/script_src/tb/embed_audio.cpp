/*
 * Angle grinder on/off grinding
 * https://freesound.org/people/Kinoton/sounds/347446/
 * 
 * ambience06.wav
 * https://freesound.org/people/yewbic/sounds/66015/
 * */

const string EMBED_snd_hell_amb = "tb/audio/hell_amb.ogg";

const string EMBED_snd_grind_loop = "tb/audio/grind_loop.ogg";
const string EMBED_snd_grind_01 = "tb/audio/grind_01.ogg";
const string EMBED_snd_grind_02 = "tb/audio/grind_02.ogg";

const string EMBED_snd_rumble = "tb/audio/rumble.ogg";

void embed_build_sounds(message@ msg)
{
	build_sound(msg, "hell_amb", 6);
	
	build_sound(msg, "grind_loop");
	build_sound(msg, "grind_01");
	build_sound(msg, "grind_02");
	
	build_sound(msg, "rumble");
}