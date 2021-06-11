const string EMBED_spr_pumpkin = "gs/sprites/pumpkin.png";
const string EMBED_spr_pumpkin2 = "gs/sprites/pumpkin2.png";

const string EMBED_spr_bat_01 = "gs/sprites/bat_01.png";
const string EMBED_spr_bat_02 = "gs/sprites/bat_02.png";
const string EMBED_spr_bat_hanging = "gs/sprites/bat_hanging.png";

const string EMBED_spr_crow_01 = "gs/sprites/crow_01.png";
const string EMBED_spr_crow_02 = "gs/sprites/crow_02.png";
const string EMBED_spr_crow_03 = "gs/sprites/crow_03.png";

const string EMBED_spr_skeleton_sitting = "gs/sprites/skeleton_sitting.png";
const string EMBED_spr_skeleton_stand = "gs/sprites/skeleton_stand.png";

const string EMBED_spr_skull = "gs/sprites/skull.png";
const string EMBED_spr_skulls = "gs/sprites/skulls.png";
const string EMBED_spr_ghost_01 = "gs/sprites/ghost_01.png";
const string EMBED_spr_ghost_02 = "gs/sprites/ghost_02.png";
const string EMBED_spr_ghost_03 = "gs/sprites/ghost_03.png";

const string EMBED_spr_caustics_01 = "gs/sprites/caustics_01.png";
const string EMBED_spr_caustics_02 = "gs/sprites/caustics_02.png";
const string EMBED_spr_caustics_03 = "gs/sprites/caustics_03.png";
const string EMBED_spr_caustics_h_01 = "gs/sprites/caustics_h_01.png";
const string EMBED_spr_caustics_h_02 = "gs/sprites/caustics_h_02.png";
const string EMBED_spr_caustics_h_03 = "gs/sprites/caustics_h_03.png";

const string EMBED_spr_secret = "gs/sprites/secret.png";
const string EMBED_spr_secret_front = "gs/sprites/secret_front.png";

const string EMBED_spr_pumpkin_small = "gs/sprites/pumpkin_small.png";
const string EMBED_spr_feather = "gs/sprites/feather.png";
const string EMBED_spr_feather_outline = "gs/sprites/feather_outline.png";
const string EMBED_spr_tear = "gs/sprites/tear.png";
const string EMBED_spr_cauldron = "gs/sprites/cauldron.png";

void embed_build_sprites(message@ msg)
{
	build_sprite(msg, "pumpkin", 32, 62);
	build_sprite(msg, "pumpkin2", 92, 107);
	
	build_sprite(msg, "bat_01", 41, 33);
	build_sprite(msg, "bat_02", 12, 16);
	build_sprite(msg, "bat_hanging", 6, 3);
	
	build_sprite(msg, "crow_01", 22, 23);
	build_sprite(msg, "crow_02", 31, 42);
	build_sprite(msg, "crow_03", 35, 17);
	
	build_sprite(msg, "skeleton_sitting", 57, 58);
	build_sprite(msg, "skeleton_stand", 19, 8);
	
	build_sprite(msg, "skull", 11, 16);
	build_sprite(msg, "skulls", 38, 38);
	
	build_sprite(msg, "ghost_01", 58, 215);
	build_sprite(msg, "ghost_02", 52, 215);
	build_sprite(msg, "ghost_03", 52, 215);
	
	build_sprite(msg, "caustics_01", 0, 0);
	build_sprite(msg, "caustics_02", 0, 0);
	build_sprite(msg, "caustics_03", 0, 0);
	build_sprite(msg, "caustics_h_01", 0, 0);
	build_sprite(msg, "caustics_h_02", 0, 0);
	build_sprite(msg, "caustics_h_03", 0, 0);
		
	build_sprite(msg, "secret");
	build_sprite(msg, "secret_front");
	
	build_sprite(msg, "pumpkin_small", 36, 34);
	build_sprite(msg, "feather", 22, 28);
	build_sprite(msg, "feather_outline", 22, 28);
	build_sprite(msg, "tear", 45, 56);
	build_sprite(msg, "cauldron", 46, 72);
}
