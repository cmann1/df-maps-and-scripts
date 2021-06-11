const string EMBED_spr_blade = "tb/sprites/blade.png";
const string EMBED_spr_pentagram = "tb/sprites/pentagram.png";
const string EMBED_spr_pent_glow = "tb/sprites/pent_glow.png";

const string EMBED_spr_dm_gib_01 = "tb/sprites/dm_gib_01.png";
const string EMBED_spr_dm_gib_02 = "tb/sprites/dm_gib_02.png";
const string EMBED_spr_dm_gib_03 = "tb/sprites/dm_gib_03.png";
const string EMBED_spr_dm_gib_04 = "tb/sprites/dm_gib_04.png";
const string EMBED_spr_dm_gib_05 = "tb/sprites/dm_gib_05.png";
const string EMBED_spr_dm_gib_06 = "tb/sprites/dm_gib_06.png";

const string EMBED_spr_dk_gib_01 = "tb/sprites/dk_gib_01.png";
const string EMBED_spr_dk_gib_02 = "tb/sprites/dk_gib_02.png";
const string EMBED_spr_dk_gib_03 = "tb/sprites/dk_gib_03.png";
const string EMBED_spr_dk_gib_04 = "tb/sprites/dk_gib_04.png";
const string EMBED_spr_dk_gib_05 = "tb/sprites/dk_gib_05.png";
const string EMBED_spr_dk_gib_06 = "tb/sprites/dk_gib_06.png";

const string EMBED_spr_dg_gib_01 = "tb/sprites/dg_gib_01.png";
const string EMBED_spr_dg_gib_02 = "tb/sprites/dg_gib_02.png";
const string EMBED_spr_dg_gib_03 = "tb/sprites/dg_gib_03.png";
const string EMBED_spr_dg_gib_04 = "tb/sprites/dg_gib_04.png";
const string EMBED_spr_dg_gib_05 = "tb/sprites/dg_gib_05.png";
const string EMBED_spr_dg_gib_06 = "tb/sprites/dg_gib_06.png";

const string EMBED_spr_dw_gib_01 = "tb/sprites/dw_gib_01.png";
const string EMBED_spr_dw_gib_02 = "tb/sprites/dw_gib_02.png";
const string EMBED_spr_dw_gib_03 = "tb/sprites/dw_gib_03.png";
const string EMBED_spr_dw_gib_04 = "tb/sprites/dw_gib_04.png";
const string EMBED_spr_dw_gib_05 = "tb/sprites/dw_gib_05.png";
const string EMBED_spr_dw_gib_06 = "tb/sprites/dw_gib_06.png";

void embed_build_sprites(message@ msg)
{
	build_sprite(msg, "blade", 59, 62);
	build_sprite(msg, "pentagram", 132, 132);
	build_sprite(msg, "pent_glow", 198, 198);
	
	build_sprite(msg, "dm_gib_01", 12, 15);
	build_sprite(msg, "dm_gib_02", 12, 15);
	build_sprite(msg, "dm_gib_03", 12, 14);
	build_sprite(msg, "dm_gib_04", 08, 13);
	build_sprite(msg, "dm_gib_05", 10, 26);
	build_sprite(msg, "dm_gib_06", 22, 22);
	
	build_sprite(msg, "dk_gib_01", 12, 15);
	build_sprite(msg, "dk_gib_02", 16, 19);
	build_sprite(msg, "dk_gib_03", 10, 10);
	build_sprite(msg, "dk_gib_04", 11, 11);
	build_sprite(msg, "dk_gib_05", 18, 13);
	build_sprite(msg, "dk_gib_06", 15, 14);
	
	build_sprite(msg, "dg_gib_01", 20, 25);
	build_sprite(msg, "dg_gib_02", 19, 15);
	build_sprite(msg, "dg_gib_03", 5, 24);
	build_sprite(msg, "dg_gib_04", 7, 17);
	build_sprite(msg, "dg_gib_05", 19, 21);
	build_sprite(msg, "dg_gib_06", 19, 26);
	
	build_sprite(msg, "dw_gib_01", 18, 21);
	build_sprite(msg, "dw_gib_02", 13, 14);
	build_sprite(msg, "dw_gib_03", 14, 15);
	build_sprite(msg, "dw_gib_04", 9, 16);
	build_sprite(msg, "dw_gib_05", 19, 21);
	build_sprite(msg, "dw_gib_06", 24, 20);
}
