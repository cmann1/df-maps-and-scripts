namespace Snake
{
	
	const int HissSubtle = 0;
	const int HissSoft = 1;
	const int HissMed = 2;
	const int HissHard = 3;
	
	const int ChrHissSubtle = 115;
	const int ChrHissSoft = 83;
	const int ChrHissMed = 77;
	const int ChrHissHard = 72;
	const int ChrHissAttrib = 104;
	
	const float HeadStrength = 9000;
	const float BodyStrength = 3000;
	const float BodyStrengthFull = 6500;
	/// The force applied to body joints to make them cling to walls when climbing.
	const float HeadClingForceWall = 25;
	/// The same as HeadClingForceWall, except a larger force applied when the snake climbing a narrow tunnel.
	const float HeadClingForceTunnel = 50;
	
	const float AttackCooldown = 120;
	const float AttackHeadImpulse = 170;
	const float AttackHeadForce = 200;
	const float AttackRecoilForce = 1000;
	const float AttackStrength = 800;
	const int AttackDamage = 3;
	
	const int JumpMinGroundContactJoints = 3;
	
	const float TauntStiffness = 0.15;
	const float TauntTransitionTime = 45;
	
	const EmitterBurstSettings AttackBloodEmitterBurst(
		EmitterId::LeafGroundCreate, 18, 23,
		12, 12, 12
	);
	const EmitterBurstSettings AttackPolyEmitterBurst(
		EmitterId::CleansedFb5, 18, 3,
		12, 12, 12
	);
	
	const float LandMin = 7;
	const float LandMax = 16;
	
	const float SlideMaxVolume = 0.4;
	const float SlideMaxForce = 4.5;
	const float SlidePitchUnderMin = 0.1;
	const float SlidePitchOverForce = 6.5;
	const float SlidePitchOver = 1.5;
	
	const float SlideVolumeWobbleSpeed = 4;
	const float SlideVolumeSpeed = 0.5;
	const float SlideFadeSpeed = 0.5;
	const float SlidePitchSpeed = 0.5;
	
	const float TongueCooldownMin = 1 * 60;
	const float TongueCooldownMax = 8 * 60;
	const float TongueSpeedStart = 14;
	const float TongueSpeed = 10;
	const int TongueFlicksMin = 2;
	const int TongueFlicksMax = 5;
	
	const EmitterBurstSettings AcidSplashEmitterBurst(
		EmitterId::ChestScrolls, 18, 16,
		44, 44, 12
	);
	
	class SpeechMeta
	{
		
		int index;
		int hiss_level;
		
	}
	
}
