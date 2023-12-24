namespace Player
{
	
	const float SignPulsePeriod = 120;
	const float SignPulseInTime = 0.15;
	const float SignSizeMin = 4.5;
	const float SignSizeMax = 5.5;
	const float ChirpSignSizeMax = 9;
	const float ChirpBigSignSizeMax = 23;
	const float ChirpSignGrow = 0.35;
	const float ChirpSmallTime = 40;
	const float ChirpMedTime = 65;
	const float ChirpBigTime = 90;
	const float ChirpFadeTime = 12;
	const float ChirpSmallRingSize = 120;
	const float ChirpMedRingSize = 300;
	const float ChirpBigRingSize = 580;
	const float ChirpRingMin = 0.35;
	const float ChirpRingMax = 1.35;
	
	const int SignLayer = 18;
	const int SignSubLayer = 9;
	const int SignFxLayer = 18;
	const int SignFxSubLayer = 13;
	
	const int ScarfGrowLayer = 18;
	const int ScarfGrowSubLayer = 5;
	const uint ScarfGlowClr = 0xffe4b6;
	const float ScarfGrowRate = 6;
	
	const uint ScarfLightClr = 0xffb3583d;
	const uint ScarfDarkClr = 0xff993718;
	const uint ScarfTrimClr = 0xffc89f2c;
	const uint ScarfTrimGlowClr = 0xffffe4b6;
	
	const int ScarfGetEmitterId = EmitterId::TrashGround;
	
}
