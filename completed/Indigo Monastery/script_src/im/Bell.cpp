#include '../common/SpriteBatch.cpp'
#include '../common/sprite_group.cpp'
#include '../common/ColType.cpp'

class Bell : enemy_base, callback_base
{
	
	script@ script;
	scene@ g;
	scriptenemy@ self;
	
	SpriteBatch@ spr_batch;
	sprite_group@ spr_group;
	float spr_offset_x = 0;
	float spr_offset_y = 0;
	float spr_offset_prev_x = 0;
	float spr_offset_prev_y = 0;
	
	float coll_left = -50;
	float coll_top = -40;
	float coll_right = 50;
	float coll_bottom = 50;
	
	float hit_timer = 0;
	float hit_timer_max = 20;
	float shake_amount = 3;
	float shake_timer = 0;
	float shake_timer_max = 350;
	
	[angle] float rotation = 0;
	
	Bell()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scriptenemy@ self)
	{
		@this.script = s;
		@this.self = self;
		
		self.on_hurt_callback(this, 'on_hurt', 0);
		self.auto_physics(false);
		set_collision();
		
		create_bell_sprites();
	}
	
	void create_bell_sprites()
	{
		if(rotation == 0)
		{
			@spr_batch = SpriteBatch(
				array<string>={'props3','facade_12','props3','facade_11','props3','facade_11','props3','facade_12','props3','facade_12','props1','chains_5','props2','foliage_16','props3','facade_12','props1','foliage_18','props1','foliage_18','props1','foliage_18','props2','foliage_16','props2','foliage_16','props2','foliage_16','props2','foliage_16','props1','facade_2','props1','facade_2','props1','foliage_18','props1','facade_2','props1','foliage_6','props2','foliage_16','props1','foliage_18','props1','foliage_18','props1','foliage_18','props1','foliage_18','props1','foliage_18','props2','foliage_16','props2','foliage_16','props1','foliage_18','props1','foliage_18','props1','foliage_18','props1','foliage_18','props2','foliage_16','props2','foliage_16','props2','foliage_16','props1','foliage_18','props1','foliage_18','props2','foliage_16','props2','foliage_17','props2','foliage_16','props1','foliage_18','props2','foliage_16','props2','foliage_16','props1','foliage_18','props2','foliage_16','props2','foliage_16','props2','foliage_16','props1','foliage_18','props2','foliage_16','props2','foliage_16','props1','foliage_18','props1','foliage_18','props1','foliage_18','props2','foliage_16','props2','foliage_16','props2','foliage_16','props2','foliage_16','props1','foliage_18','props2','foliage_16','props1','foliage_18','props3','facade_11','props3','facade_11',},
				array<int>={12,16,12,15,12,15,12,15,12,16,12,16,12,18,12,15,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,14,12,14,12,18,12,14,12,17,12,17,12,17,12,17,12,18,12,18,12,17,12,17,12,17,12,18,12,18,12,18,12,17,12,18,12,17,12,17,12,18,12,17,12,18,12,17,12,18,12,17,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,17,12,18,12,17,12,17,12,17,12,17,12,18,12,15,12,15,},
				array<float>={-239.849,-131.253,-224.849,-91.2529,-196.849,-91.2529,126.151,-169.253,237.151,-131.253,-190.849,-215.253,41.1514,42.7471,211.151,199.747,56.1514,52.7471,16.1514,1.74707,22.1514,2.74707,-12.8486,0.74707,-22.8486,-5.25293,-22.8486,-2.25293,19.1514,9.74707,-3.84863,15.7471,-3.84863,19.7471,9.15137,22.7471,-3.84863,25.7471,-7.84863,25.7471,-44.8486,29.7471,-45.8486,21.7471,-53.8486,30.7471,25.1514,33.7471,-17.8486,32.7471,-38.8486,31.7471,-44.8486,36.7471,-46.8486,40.7471,-57.8486,31.7471,27.1514,52.7471,-10.8486,52.7471,-45.8486,51.7471,-13.8486,-10.2529,-34.8486,-35.2529,-21.8486,-30.2529,17.1514,-26.2529,-38.8486,-22.2529,-12.8486,-17.2529,-36.8486,-18.2529,40.1514,11.7471,41.1514,26.7471,47.1514,39.7471,35.1514,37.7471,48.1514,-29.2529,41.1514,-26.2529,43.1514,-16.2529,45.1514,-4.25293,50.1514,-8.25293,40.1514,-0.25293,36.1514,-34.2529,54.1514,31.7471,1.15137,-77.2529,-6.84863,-58.2529,-5.84863,-66.2529,25.1514,-47.2529,-17.8486,-49.2529,-26.8486,-45.2529,-27.8486,-45.2529,-33.8486,-38.2529,35.1514,-49.2529,-172.849,-92.2529,235.151,-91.2529,},
				array<float>={1,1,1,1,1,1,1,1,-1,1,1,1,1,1,-1,1,1,1,1,1,-1,1,1,1,1,1,1,1,1,1,1,1,1,1,-1,1,1,1,1,1,1,1,1,1,1,1,1,-1,1,1,1,-1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,-1,1,1,1,1,1,1,1,-1,1,1,1,1,1,-1,-1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,-1,1,1,-1,1,},
				array<float>={0,0,0,90,0,0,0,90,0,299.998,259.997,0,0,0,0,0,0,0,0,90,0,339.999,99.9976,0,180,90,0,0,180,0,0,0,0,0,0,270,99.9976,0,279.998,0,0,0,0,69.9994,0,0,0,90,0,0,0,180,9.99756,0,0,0,0,139.999,0,39.9957,0,0,},
				array<uint>={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,}
			);
			@spr_group = null;
		}
		else
		{
			@spr_group = sprite_group(
				array<string>={'props3','facade_12','props3','facade_11','props3','facade_11','props3','facade_12','props3','facade_12','props1','chains_5','props2','foliage_16','props3','facade_12','props1','foliage_18','props1','foliage_18','props1','foliage_18','props2','foliage_16','props2','foliage_16','props2','foliage_16','props2','foliage_16','props1','facade_2','props1','facade_2','props1','foliage_18','props1','facade_2','props1','foliage_6','props2','foliage_16','props1','foliage_18','props1','foliage_18','props1','foliage_18','props1','foliage_18','props1','foliage_18','props2','foliage_16','props2','foliage_16','props1','foliage_18','props1','foliage_18','props1','foliage_18','props1','foliage_18','props2','foliage_16','props2','foliage_16','props2','foliage_16','props1','foliage_18','props1','foliage_18','props2','foliage_16','props2','foliage_17','props2','foliage_16','props1','foliage_18','props2','foliage_16','props2','foliage_16','props1','foliage_18','props2','foliage_16','props2','foliage_16','props2','foliage_16','props1','foliage_18','props2','foliage_16','props2','foliage_16','props1','foliage_18','props1','foliage_18','props1','foliage_18','props2','foliage_16','props2','foliage_16','props2','foliage_16','props2','foliage_16','props1','foliage_18','props2','foliage_16','props1','foliage_18','props3','facade_11','props3','facade_11',},
				array<int>={12,16,12,15,12,15,12,15,12,16,12,16,12,18,12,15,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,14,12,14,12,18,12,14,12,17,12,17,12,17,12,17,12,18,12,18,12,17,12,17,12,17,12,18,12,18,12,18,12,17,12,18,12,17,12,17,12,18,12,17,12,18,12,17,12,18,12,17,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,18,12,17,12,18,12,17,12,17,12,17,12,17,12,18,12,15,12,15,},
				array<float>={0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,},
				array<float>={-57,37,-27.5,40,0.5,40,-42,14,54,37,-1,-27.5,38.5,44,43,17,54.5,46,9.18775,-0.200744,14.8459,2.73871,-15.5,2,-25.5,-4,-25.5,-1,16.5,11,-2.5,30,-2.5,34,10.5,16,-2.5,40,-20,27,-47.5,31,-49.8037,15.9352,-46.8459,30.738,23.5,41,-16.5,40,-46,30.5,-47.5,38,-49.5,42,-56.5,39,25.5,46,-12.5,46,-47.5,45,-16.5,-9,-37.5,-34,-24.5,-29,10,-24.5,-31.8459,-22.262,-15.5,-16,-34.7968,-7.45742,37.5,13,39.5,20,44.5,41,32.5,39,40.9091,-28.0153,38.5,-25,40.5,-15,42.5,-3,43,-9.5,37.5,1,33.5,-33,52.5,39,-0.500001,-70,-7.26198,-65.1541,-8.5,-65,22.5,-46,-20.5,-48,-29.5,-44,-22.3513,-40.602,-36.5,-37,29.3517,-44.6014,24.5,39,37.5,40,},
				array<float>={0,0,0,90,0,0,0,90,0,299.998,259.997,0,0,0,0,0,0,0,0,90,0,339.999,99.9976,0,180,90,0,0,180,0,0,0,0,0,0,270,99.9976,0,279.998,0,0,0,0,69.9994,0,0,0,90,0,0,0,180,9.99756,0,0,0,0,139.999,0,39.9957,0,0,},
				array<float>={1,1,1,1,1,1,1,1,-1,1,1,1,1,1,-1,1,1,1,1,1,-1,1,1,1,1,1,1,1,1,1,1,1,1,1,-1,1,1,1,1,1,1,1,1,1,1,1,1,-1,1,1,1,-1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,-1,1,1,1,1,1,1,1,-1,1,1,1,1,1,-1,-1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,-1,1,1,-1,1,},
				array<uint>={0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,},
				array<uint>={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
				array<uint>={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,}
			);
			sprite_group@ spr = @spr_group;
//			spr.add_sprite('props3', 'facade_12', 0.5, 0.5, -60, 42, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 16);
//spr.add_sprite('props3', 'facade_11', 0.5, 0.5, -30.5, 45, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 15);
//spr.add_sprite('props3', 'facade_11', 0.5, 0.5, -2.5, 45, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 15);
//spr.add_sprite('props3', 'facade_12', 0.5, 0.5, -45, 19, 90, 1, 1, 0xFFFFFFFF, 0, 0, 12, 15);
//spr.add_sprite('props3', 'facade_12', 0.5, 0.5, 51, 42, 0, -1, 1, 0xFFFFFFFF, 0, 0, 12, 16);
//spr.add_sprite('props1', 'chains_5', 0.5, 0.5, -4, -22.5, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 16);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, 35.5, 49, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props3', 'facade_12', 0.5, 0.5, 40, 22, 90, -1, 1, 0xFFFFFFFF, 0, 0, 12, 15);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, 51.5, 51, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, 6.18775, 4.79926, 299.998, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, 11.8459, 7.73871, 259.997, -1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, -18.5, 7, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, -28.5, 1, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, -28.5, 4, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, 13.5, 16, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props1', 'facade_2', 0.5, 0.5, -5.5, 35, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 14);
//spr.add_sprite('props1', 'facade_2', 0.5, 0.5, -5.5, 39, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 14);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, 7.5, 21, 0, -1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props1', 'facade_2', 0.5, 0.5, -5.5, 45, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 14);
//spr.add_sprite('props1', 'foliage_6', 0.5, 0.5, -23, 32, 90, 1, 1, 0xFFFFFFFF, 0, 0, 12, 17);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, -50.5, 36, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 17);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, -52.8037, 20.9352, 339.999, 1, 1, 0xFFFFFFFF, 0, 0, 12, 17);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, -49.8459, 35.738, 99.9976, 1, 1, 0xFFFFFFFF, 0, 0, 12, 17);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, 20.5, 46, 0, 1, -1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, -19.5, 45, 180, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, -49, 35.5, 90, 1, -1, 0xFFFFFFFF, 0, 0, 12, 17);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, -50.5, 43, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 17);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, -52.5, 47, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 17);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, -59.5, 44, 180, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, 22.5, 51, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, -15.5, 51, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, -50.5, 50, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 17);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, -19.5, -4, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, -40.5, -29, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 17);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, -27.5, -24, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 17);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, 7, -19.5, 270, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, -34.8459, -17.262, 99.9976, 1, 1, 0xFFFFFFFF, 0, 0, 12, 17);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, -18.5, -11, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props2', 'foliage_17', 0.5, 0.5, -37.7968, -2.45742, 279.998, 1, 1, 0xFFFFFFFF, 0, 0, 12, 17);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, 34.5, 18, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, 36.5, 25, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 17);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, 41.5, 46, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, 29.5, 44, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, 37.9091, -23.0153, 69.9994, 1, -1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, 35.5, -20, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, 37.5, -10, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, 39.5, 2, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, 40, -4.5, 90, 1, -1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, 34.5, 6, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, 30.5, -28, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, 49.5, 44, 0, 1, -1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, -3.5, -65, 180, -1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, -10.262, -60.1541, 9.99756, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, -11.5, -60, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 17);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, 19.5, -41, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, -23.5, -43, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 17);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, -32.5, -39, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 17);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, -25.3513, -35.602, 139.999, 1, 1, 0xFFFFFFFF, 0, 0, 12, 17);
//spr.add_sprite('props2', 'foliage_16', 0.5, 0.5, -39.5, -32, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 17);
//spr.add_sprite('props1', 'foliage_18', 0.5, 0.5, 26.3517, -39.6014, 39.9957, 1, -1, 0xFFFFFFFF, 0, 0, 12, 18);
//spr.add_sprite('props3', 'facade_11', 0.5, 0.5, 21.5, 44, 0, 1, 1, 0xFFFFFFFF, 0, 0, 12, 15);
//spr.add_sprite('props3', 'facade_11', 0.5, 0.5, 34.5, 45, 0, -1, 1, 0xFFFFFFFF, 0, 0, 12, 15);
			@spr_batch = null;
//			puts(spr_group.sprite_names.size());
		}
	}
	
	void set_collision()
	{
		self.base_rectangle(
			coll_top, coll_bottom,
			coll_left, coll_right);
		self.hit_rectangle(
			coll_top, coll_bottom,
			coll_left, coll_right);
	}
	
	bool check = false;
	controllable@ checkee = null;
	void on_hurt(controllable@ attacked, controllable@ attacker, hitbox@ attack_hitbox, int arg)
	{
		if(hit_timer == 0)
		{
			audio@ ring_sound = g.play_script_stream('bell_02', 0, self.x(), self.y(), false, 1);
			ring_sound.positional(true);
			hit_timer = hit_timer_max;
			shake_timer = shake_timer_max;
		}
		
		if(attacker.player_index() != -1)
		{
			dustman@ dm = attacker.as_dustman();
//			puts(attacker.freeze_frame_timer());
			@script.check = attacker;
			script.check_freeze = attack_hitbox.damage();
			if(@dm != null)
			{
				check = true;
//				puts(dm.combo_count()+' d:'+attack_hitbox.damage());
//				dm.combo_count(dm.combo_count() - attack_hitbox.damage());
//				dm.skill_combo(dm.skill_combo() - attack_hitbox.damage());
//				puts(g.combo_break_count());
				script.check_combo = attack_hitbox.damage();
//				puts(dm.combo_count()+' d:'+attack_hitbox.damage());
			}
		}
	}
	
	protected void draw_coll()
	{
		const float x = self.x();
		const float y = self.y();
		g.draw_rectangle_world(21, 10, x + coll_left, y + coll_top, x + coll_right, y + coll_bottom, 0, 0x77FFFFFF);
	}
	
	void step()
	{
//		if(check)
//		{
//			puts('   > ' + checkee.freeze_frame_timer());
//			@checkee = null;
//			check = false;
//		}
		
		if(hit_timer > 0)
		{
			hit_timer--;
		}
		
		if(shake_timer > 0)
		{
			if(--shake_timer == 0)
			{
				spr_offset_x = spr_offset_prev_x = 0;
				spr_offset_y = spr_offset_prev_y = 0;
			}
			else
			{
				spr_offset_prev_x = spr_offset_x;
				spr_offset_prev_y = spr_offset_y;
				const float shake_factor = shake_timer / shake_timer_max * shake_amount;
				spr_offset_x = rand_range(-shake_factor, shake_factor);
//				spr_offset_y = rand_range(-shake_factor, shake_factor);
			}
		}
	}
	
//	void editor_step()
//	{
//		if((rotation == 0 && @spr_batch == null) || (rotation != 0 && @spr_group == null))
//		{
//			create_bell_sprites();
//		}
//	}
	
	void draw(float sub_frame)
	{
		const float sprite_x = self.x() + lerp(spr_offset_prev_x, spr_offset_x, sub_frame);
		const float sprite_y = self.y() + lerp(spr_offset_prev_y, spr_offset_y, sub_frame);
		
		if(@spr_group != null)
			spr_group.draw(-1, -1, sprite_x, sprite_y, rotation, 1);
		else
			spr_batch.draw(sprite_x, sprite_y);
		
//		draw_coll();
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}