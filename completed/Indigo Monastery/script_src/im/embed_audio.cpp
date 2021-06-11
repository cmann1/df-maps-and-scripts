/*
 * Old Church Bell.wav			- https://freesound.org/people/dsp9000/sounds/76405/
 * ChurchBellKonstanz.wav		- https://freesound.org/people/edsward/sounds/341866/
 * Crystal bowl F#3 1.wav		- https://freesound.org/people/caiogracco/sounds/150454/
 * Taça Eb4 1.wav				- https://freesound.org/people/caiogracco/sounds/150455/
 * cristal grinçant V1&6-1.wav	- https://freesound.org/people/sam_x/sounds/27701/
 * Reception Bell With Strange Resonance.wav	- https://freesound.org/people/Kevzog/sounds/323000/
 * eerie glow					- https://freesound.org/people/waveplay/sounds/187499/
 * */

const string EMBED_snd_rumble = 'im/sounds/rumble.ogg';
const string EMBED_snd_resonance = 'im/sounds/resonance.ogg';

const string EMBED_snd_music_box = 'im/sounds/music_box.ogg';
const string EMBED_snd_mb_sample1 = 'im/sounds/mb_sample1.ogg';
const string EMBED_snd_mb_sample2 = 'im/sounds/mb_sample2.ogg';
const string EMBED_snd_mb_sample3 = 'im/sounds/mb_sample3.ogg';

const string EMBED_snd_bell_02 = 'im/sounds/bell-02.ogg';
const string EMBED_snd_bell_02_light = 'im/sounds/bell-02-light.ogg';

const string EMBED_snd_bell_a2  = 'im/sounds/bell_note01_a2.ogg';
const string EMBED_snd_bell_b2  = 'im/sounds/bell_note16_b2.ogg';
const string EMBED_snd_bell_c3  = 'im/sounds/bell_note02_c3.ogg';
const string EMBED_snd_bell_cs3 = 'im/sounds/bell_note13_cs3.ogg';
const string EMBED_snd_bell_d3  = 'im/sounds/bell_note03_d3.ogg';
const string EMBED_snd_bell_ds3 = 'im/sounds/bell_note17_ds3.ogg';
const string EMBED_snd_bell_e3  = 'im/sounds/bell_note04_e3.ogg';
const string EMBED_snd_bell_fs3  = 'im/sounds/bell_note00_fs3.ogg';
const string EMBED_snd_bell_g3  = 'im/sounds/bell_note05_g3.ogg';
const string EMBED_snd_bell_gs3 = 'im/sounds/bell_note18_gs3.ogg';
const string EMBED_snd_bell_a3  = 'im/sounds/bell_note06_a3.ogg';
const string EMBED_snd_bell_b3  = 'im/sounds/bell_note19_b3.ogg';
const string EMBED_snd_bell_c4  = 'im/sounds/bell_note07_c4.ogg';
const string EMBED_snd_bell_cs4 = 'im/sounds/bell_note14_cs4.ogg';
const string EMBED_snd_bell_d4  = 'im/sounds/bell_note08_d4.ogg';
const string EMBED_snd_bell_ds4 = 'im/sounds/bell_note20_ds4.ogg';
const string EMBED_snd_bell_e4  = 'im/sounds/bell_note09_e4.ogg';
const string EMBED_snd_bell_fs4 = 'im/sounds/bell_note15_fs4.ogg';
const string EMBED_snd_bell_g4  = 'im/sounds/bell_note10_g4.ogg';
const string EMBED_snd_bell_gs4 = 'im/sounds/bell_note21_gs4.ogg';
const string EMBED_snd_bell_a4  = 'im/sounds/bell_note11_a4.ogg';
const string EMBED_snd_bell_c5  = 'im/sounds/bell_note12_c5.ogg';

const string EMBED_snd_crystal_a3  = 'im/sounds/crystal/a3.ogg';
const string EMBED_snd_crystal_b3  = 'im/sounds/crystal/b3.ogg';
const string EMBED_snd_crystal_cs2 = 'im/sounds/crystal/cs2.ogg';
const string EMBED_snd_crystal_cs3 = 'im/sounds/crystal/cs3.ogg';
const string EMBED_snd_crystal_cs4 = 'im/sounds/crystal/cs4.ogg';
const string EMBED_snd_crystal_ds3 = 'im/sounds/crystal/ds3.ogg';
const string EMBED_snd_crystal_ds4 = 'im/sounds/crystal/ds4.ogg';
const string EMBED_snd_crystal_e3  = 'im/sounds/crystal/e3.ogg';
const string EMBED_snd_crystal_e4  = 'im/sounds/crystal/e4.ogg';
const string EMBED_snd_crystal_fs2 = 'im/sounds/crystal/fs2.ogg';
const string EMBED_snd_crystal_fs3 = 'im/sounds/crystal/fs3.ogg';
const string EMBED_snd_crystal_gs2 = 'im/sounds/crystal/gs2.ogg';
const string EMBED_snd_crystal_gs3 = 'im/sounds/crystal/gs3.ogg';
const string EMBED_snd_crystal_cs1_long  = 'im/sounds/crystal/cs1_long.ogg';
const string EMBED_snd_crystal_a1_long   = 'im/sounds/crystal/a1_long.ogg';
const string EMBED_snd_crystal_cs2_chord = 'im/sounds/crystal/cs2_chord.ogg';
const string EMBED_snd_crystal_e1_long   = 'im/sounds/crystal/e1_long.ogg';

const string EMBED_snd_lock_open_strings = 'im/sounds/lock_open_strings.ogg';


void embed_build_sounds(message@ msg)
{
	build_sound(msg, 'rumble');
	build_sound(msg, 'resonance');
	build_sound(msg, 'lock_open_strings');
	
	build_sound(msg, 'music_box');
	build_sound(msg, 'mb_sample1');
	build_sound(msg, 'mb_sample2');
	build_sound(msg, 'mb_sample3');
	
	build_sound(msg, 'bell_02');
	build_sound(msg, 'bell_02_light');
	
	build_sound(msg, 'crystal_a3');
	build_sound(msg, 'crystal_b3');
	build_sound(msg, 'crystal_cs2');
	build_sound(msg, 'crystal_cs3');
	build_sound(msg, 'crystal_cs4');
	build_sound(msg, 'crystal_ds3');
	build_sound(msg, 'crystal_ds4');
	build_sound(msg, 'crystal_e3');
	build_sound(msg, 'crystal_e4');
	build_sound(msg, 'crystal_fs2');
	build_sound(msg, 'crystal_fs3');
	build_sound(msg, 'crystal_gs2');
	build_sound(msg, 'crystal_gs3');
	build_sound(msg, 'crystal_cs1_long');
	build_sound(msg, 'crystal_a1_long');
	build_sound(msg, 'crystal_cs2_chord');
	build_sound(msg, 'crystal_e1_long');
	
	build_sound(msg, 'bell_a2');
	build_sound(msg, 'bell_c3');
	build_sound(msg, 'bell_d3');
	build_sound(msg, 'bell_e3');
	build_sound(msg, 'bell_fs3');
	build_sound(msg, 'bell_g3');
	build_sound(msg, 'bell_a3');
	build_sound(msg, 'bell_c4');
	build_sound(msg, 'bell_d4');
	build_sound(msg, 'bell_e4');
	build_sound(msg, 'bell_g4');
	build_sound(msg, 'bell_a4');
	build_sound(msg, 'bell_c5');
	build_sound(msg, 'bell_cs3');
	build_sound(msg, 'bell_cs4');
	build_sound(msg, 'bell_fs4');
	build_sound(msg, 'bell_b2');
	build_sound(msg, 'bell_ds3');
	build_sound(msg, 'bell_gs3');
	build_sound(msg, 'bell_b3');
	build_sound(msg, 'bell_ds4');
	build_sound(msg, 'bell_gs4');
}