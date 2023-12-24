namespace BreakableFloor
{
	
	const int EmitterId = EmitterId::TrashWall;
	const int EmitterLayer = 19;
	const int EmitterSubLayer = 7;
	const int RemoveEmitterId = EmitterId::Rain;
	
	/// Layer, sublayer, set, group, index
	const array<uint> RemoveProps = {
		// Sun/moon shadow props
		19, 9, 2, 22, 4,
		// Book cracks
		19, 23, 1, 0, 10,};
	
	const float DebrisLayer = 19;
	const float DebrisSubLayer = 13;
	const float DebrisGravity = 500;
	const float DebrisFric = 0.5;
	const float DebrisRotFric = 1;
	const float DebrisLifeMin = 1.25;
	const float DebrisLifeMax = 3.5;
	const float DebrisRotVel = 5;
	const float DebrisMaxDistForce = 48 * 4;
	const float DebrisForce = 16000;
	
	const float BlockDebrisChance = 0.8;
	const float DebrisOffset = 5;
	const float DebrisRot = 5;
	const float DebrisScaleMin = 0.45;
	const float DebrisScaleMax = 0.52;
	const float BlockDebrisScaleMin = 0.45;
	const float BlockDebrisScaleMax = 0.52;
	const float Boulder1DebrisScaleMin = 0.81;
	const float Boulder1DebrisScaleMax = 0.84;
	const float Boulder2DebrisScaleMin = 0.37;
	const float Boulder2DebrisScaleMax = 0.45;
	
}
