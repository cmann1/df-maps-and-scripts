#include '../lib/drawing/SpriteBatch.cpp';
#include '../lib/drawing/SpriteGroup.cpp';

class SpriteData
{
	
	SpriteGroup PrismLarge(
		array<string>={'props3','backdrops_3','props3','backdrops_3',},
		array<int>={18,1,18,1,},
		array<float>={0.5,0.5,0.461304,1.23576,182,0.12015,0.12015,0.5,0.5,0.825911,4.17493,91.9995,0.12015,0.12015,},
		array<uint>={0,0,0xffffffff,0,0,0xffffffff,});
	
	SpriteBatch AlarmOff(
		array<string>={'props4','machinery_6','props4','machinery_6','props2','backdrops_4','props2','backdrops_4','props4','machinery_6','props4','machinery_6','props4','machinery_6','props4','machinery_12','props4','machinery_9',},
		array<int>={17,9,17,9,17,4,17,4,17,9,17,10,17,10,17,4,17,11,},
		array<float>={46,112,38,94,1,63,3,75,40,74,70,-16,36,-16,-47,23,-205,-148,},
		array<float>={0.230614,0.230614,0.195928,0.195928,0.141421,0.141421,0.271442,0.271442,0.195928,0.195928,0.319497,0.319497,0.319497,0.319497,0.230614,0.230614,1,1,},
		array<float>={180,180,0,0,180,90,90,0,0,},
		array<uint>={1,1,0,0,1,1,1,0,1,});
	SpriteBatch AlarmOn(
		array<string>={'props3','backdrops_3','props3','backdrops_3','props2','backdrops_4','props4','machinery_6','props2','backdrops_4','props4','machinery_6','props4','machinery_6','props3','backdrops_3','props4','machinery_6','props4','machinery_6','props4','machinery_12','props3','backdrops_3','props4','machinery_9',},
		array<int>={17,7,17,7,17,7,17,9,17,6,17,9,17,9,17,6,17,10,17,10,17,6,17,6,17,11,},
		array<float>={6,34,6,34,1,63,40,74,3,75,38,94,46,112,-44,33,70,-16,36,-16,-47,23,23,-24,-205,-148,},
		array<float>={0.053183,0.053183,0.053183,0.053183,0.141421,0.141421,0.195928,0.195928,0.271442,0.271442,0.195928,0.195928,0.230614,0.230614,0.141421,0.141421,0.319497,0.319497,0.319497,0.319497,0.230614,0.230614,0.195928,0.195928,1,1,},
		array<float>={91.5216,91.5216,0,180,0,180,180,1.99951,90,90,0,91.9995,0,},
		array<uint>={0,0,0,1,0,1,1,0,1,1,0,0,1,});
	SpriteBatch AlarmLightGlow(
		array<string>={'props3','backdrops_3',},
		array<int>={19,4,},
		array<float>={-96,490,},
		array<float>={0.721804,0.721804,},
		array<float>={272.999,},
		array<uint>={0,});
	
	SpriteBatch SwitchInactive(
		array<string>={'props2','backdrops_4','props2','backdrops_4','props4','machinery_9','props3','facade_11','props3','facade_11','props3','facade_11','props3','facade_11','props4','machinery_6','props4','machinery_6','props4','machinery_6','props4','machinery_6','props4','machinery_6','props3','facade_11','props3','facade_11','props3','facade_11','props3','facade_11','props4','machinery_19',},
		array<int>={17,1,17,1,17,2,17,2,17,2,17,2,17,2,17,5,17,5,17,5,17,5,17,5,17,5,17,5,17,5,17,5,17,17,},
		array<float>={6,48,6,48,-175,-139,179,126,179,-125,-180,126,-180,-125,-35,-48,-35,-40,-35,-6,-35,-14,73,91,58,-40,-59,-40,-59,40,58,40,-83,-108,},
		array<float>={0.521001,0.521001,0.521001,0.521001,0.84959,0.84959,0.84959,0.84959,-0.84959,0.84959,-0.84959,0.84959,0.84959,0.84959,0.166458,0.166458,0.166458,0.166458,0.166458,0.166458,0.166458,0.166458,-0.442637,0.442637,-0.271442,0.271442,0.271442,0.271442,-0.271442,0.271442,0.271442,0.271442,0.442637,0.442637,},
		array<float>={0,0,0,180,0,180,0,0,0,0,0,90,0,0,180,180,0,},
		array<uint>={0,0,1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,});
	SpriteBatch Switch(
		array<string>={'props2','backdrops_4','props2','backdrops_4','props4','machinery_6','props4','machinery_6','props4','machinery_6','props4','machinery_6','props4','machinery_6','props4','machinery_9',},
		array<int>={17,1,17,1,17,13,17,13,17,13,17,13,17,13,17,3,},
		array<float>={7,48,7,48,74,91,-34,-12,-34,-4,-34,-42,-34,-50,-205,-164,},
		array<float>={0.521001,0.521001,0.521001,0.521001,-0.442637,0.442637,0.166458,0.166458,0.166458,0.166458,0.166458,0.166458,0.166458,0.166458,1,1,},
		array<float>={0,0,90,0,0,0,0,0,},
		array<uint>={0,0,1,1,1,1,1,1,});
	SpriteBatch SwitchLightLocked(
		array<string>={'props4','machinery_9','props2','backdrops_4','props2','backdrops_4','props2','backdrops_4','props3','backdrops_3','props3','backdrops_3','props4','machinery_9','props2','backdrops_4','props2','backdrops_4','props2','backdrops_4','props3','backdrops_3','props3','backdrops_3','props3','facade_11','props3','facade_11','props3','facade_11','props3','facade_11','props2','backdrops_4','props2','backdrops_4',},
		array<int>={17,6,17,6,17,6,17,6,17,7,17,7,17,6,17,6,17,6,17,6,17,7,17,7,17,13,17,13,17,13,17,13,17,20,17,21,},
		array<float>={78,-76,2,0,4,7,4,3,19,-6,19,-6,46,-77,-29,1,-28,8,-28,0,-13,-5,-13,-5,-58,-40,58,-40,-58,40,58,40,2,15,1,8,},
		array<float>={0.37606,0.37606,0.141421,0.141421,0.141421,0.141421,0.141421,0.141421,0.0235408,0.0235408,0.0235408,0.0235408,0.37606,0.37606,0.141421,0.141421,0.141421,0.141421,0.141421,0.141421,0.0235408,0.0235408,0.0235408,0.0235408,0.271442,0.271442,-0.271442,0.271442,-0.271442,0.271442,0.271442,0.271442,0.166458,0.166458,0.0867249,0.0867249,},
		array<float>={90,90,90,90,91.9995,91.9995,90,90,90,90,91.9995,91.9995,0,0,180,180,0,0,},
		array<uint>={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,});
	SpriteBatch SwitchLightUnlocked(
		array<string>={'props3','facade_11','props3','facade_11','props3','facade_11','props3','facade_11','props2','backdrops_4','props2','backdrops_4',},
		array<int>={17,13,17,13,17,13,17,13,17,16,17,15,},
		array<float>={-60,42,60,42,60,-41,-60,-41,1,8,2,15,},
		array<float>={-0.271442,0.271442,0.271442,0.271442,-0.271442,0.271442,0.271442,0.271442,0.0867249,0.0867249,0.166458,0.166458,},
		array<float>={180,180,0,0,0,0,},
		array<uint>={0,0,0,0,0,0,});
	SpriteGroup SwitchRotator(
		array<string>={'props3','facade_11','props3','facade_11','props3','facade_11','props3','facade_11','props4','machinery_6',},
		array<int>={17,9,17,9,17,9,17,9,17,10,},
		array<float>={0.5,0.5,19.2037,-15.7059,90,0.84959,0.84959,0.5,0.5,-19.2037,-15.7059,270,-0.84959,0.84959,0.5,0.5,-19.2037,15.7059,270,0.84959,0.84959,0.5,0.5,19.2037,15.7059,90,-0.84959,0.84959,0.5,0.5,-0.53582,0.11404,0,0.37606,0.37606,},
		array<uint>={0,0,0xffffffff,0,0,0xffffffff,0,0,0xffffffff,0,0,0xffffffff,0,1,0xffffffff,});
	
}
