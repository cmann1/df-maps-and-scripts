#include "SAT.cpp"

// 0 Top, 1 Bottom, 2 Left, 3 Right
const array<TilePolygon@> TILE_POLYGONS = {
	// FULL
	TilePolygon(array<int> = {0, 3, 1, 2}, array<float> = {-24, -24, 24, -24, 24, 24, -24, 24}),
	// BIG_1
	TilePolygon(array<int> = {0, 3, 1, 2}, array<float> = {-24, -24, 24, 0, 24, 24, -24, 24}),
	// SMALL_1
	TilePolygon(array<int> = {0, 1, 2}, array<float> = {-24, 0, 24, 24, -24, 24}),
	// BIG_2
	TilePolygon(array<int> = {0, 3, 1, 2}, array<float> = {-24, -24, 24, -24, 0, 24, -24, 24}),
	// SMALL_2
	TilePolygon(array<int> = {0, 3, 2}, array<float> = {-24, -24, 0, -24, -24, 24}),
	// BIG_3
	TilePolygon(array<int> = {0, 3, 1, 2}, array<float> = {-24, -24, 24, -24, 24, 24, -24, 0}),
	// SMALL_3
	TilePolygon(array<int> = {0, 3, 1}, array<float> = {-24, -24, 24, -24, 24, 0}),
	// BIG_4
	TilePolygon(array<int> = {0, 3, 1, 2}, array<float> = {0, -24, 24, -24, 24, 24, -24, 24}),
	// SMALL_4
	TilePolygon(array<int> = {3, 1, 2}, array<float> = {24, -24, 24, 24, 0, 24}),
	// BIG_5
	TilePolygon(array<int> = {0, 3, 1, 2}, array<float> = {-24, 0, 24, -24, 24, 24, -24, 24}),
	// SMALL_5
	TilePolygon(array<int> = {3, 1, 0}, array<float> = {24, 0, 24, 24, -24, 24}),
	// BIG_6
	TilePolygon(array<int> = {0, 3, 1, 2}, array<float> = {-24, -24, 24, -24, 24, 24, 0, 24}),
	// SMALL_6
	TilePolygon(array<int> = {0, 3, 2}, array<float> = {0, -24, 24, -24, 24, 24}),
	// BIG_7
	TilePolygon(array<int> = {0, 3, 1, 2}, array<float> = {-24, -24, 24, -24, 24, 0, -24, 24}),
	// SMALL_7
	TilePolygon(array<int> = {0, 1, 2}, array<float> = {-24, -24, 24, -24, -24, 0}),
	// BIG_8
	TilePolygon(array<int> = {0, 3, 1, 2}, array<float> = {-24, -24, 0, -24, 24, 24, -24, 24}),
	// SMALL_8
	TilePolygon(array<int> = {3, 1, 2}, array<float> = {-24, -24, 0, 24, -24, 24}),
	// HALF_A
	TilePolygon(array<int> = {0, 1, 2}, array<float> = {-24, -24, 24, 24, -24, 24}),
	// HALF_B
	TilePolygon(array<int> = {0, 1, 2}, array<float> = {-24, -24, 24, -24, -24, 24}),
	// HALF_C
	TilePolygon(array<int> = {0, 3, 1}, array<float> = {-24, -24, 24, -24, 24, 24}),
	// HALF_D
	TilePolygon(array<int> = {3, 1, 0}, array<float> = {24, -24, 24, 24, -24, 24})
};

// 0 Top, 1 Bottom, 2 Left, 3 Right
//const array<TilePolygon@> TILE_POLYGONS = {
//	// FULL
//	TilePolygon(0, 0, array<int> = {0, 3, 1, 2}, array<Vec2@> = {Vec2(-24, -24), Vec2(24, -24), Vec2(24, 24), Vec2(-24, 24)}, 4),
//	// BIG_1
//	TilePolygon(0, 0, array<int> = {0, 3, 1, 2}, array<Vec2@> = {Vec2(-24, -24), Vec2(24, 0), Vec2(24, 24), Vec2(-24, 24)}, 4),
//	// SMALL_1
//	TilePolygon(0, 0, array<int> = {0, 1, 2}, array<Vec2@> = {Vec2(-24, 0), Vec2(24, 24), Vec2(-24, 24)}, 3),
//	// BIG_2
//	TilePolygon(0, 0, array<int> = {0, 3, 1, 2}, array<Vec2@> = {Vec2(-24, -24), Vec2(24, -24), Vec2(0, 24), Vec2(-24, 24)}, 4),
//	// SMALL_2
//	TilePolygon(0, 0, array<int> = {0, 3, 2}, array<Vec2@> = {Vec2(-24, -24), Vec2(0, -24), Vec2(-24, 24)}, 3),
//	// BIG_3
//	TilePolygon(0, 0, array<int> = {0, 3, 1, 2}, array<Vec2@> = {Vec2(-24, -24), Vec2(24, -24), Vec2(24, 24), Vec2(-24, 0)}, 4),
//	// SMALL_3
//	TilePolygon(0, 0, array<int> = {0, 3, 1}, array<Vec2@> = {Vec2(-24, -24), Vec2(24, -24), Vec2(24, 0)}, 3),
//	// BIG_4
//	TilePolygon(0, 0, array<int> = {0, 3, 1, 2}, array<Vec2@> = {Vec2(0, -24), Vec2(24, -24), Vec2(24, 24), Vec2(-24, 24)}, 4),
//	// SMALL_4
//	TilePolygon(0, 0, array<int> = {3, 1, 2}, array<Vec2@> = {Vec2(24, -24), Vec2(24, 24), Vec2(0, 24)}, 3),
//	// BIG_5
//	TilePolygon(0, 0, array<int> = {0, 3, 1, 2}, array<Vec2@> = {Vec2(-24, 0), Vec2(24, -24), Vec2(24, 24), Vec2(-24, 24)}, 4),
//	// SMALL_5
//	TilePolygon(0, 0, array<int> = {3, 1, 0}, array<Vec2@> = {Vec2(24, 0), Vec2(24, 24), Vec2(-24, 24)}, 3),
//	// BIG_6
//	TilePolygon(0, 0, array<int> = {0, 3, 1, 2}, array<Vec2@> = {Vec2(-24, -24), Vec2(24, -24), Vec2(24, 24), Vec2(0, 24)}, 4),
//	// SMALL_6
//	TilePolygon(0, 0, array<int> = {0, 3, 2}, array<Vec2@> = {Vec2(0, -24), Vec2(24, -24), Vec2(24, 24)}, 3),
//	// BIG_7
//	TilePolygon(0, 0, array<int> = {0, 3, 1, 2}, array<Vec2@> = {Vec2(-24, -24), Vec2(24, -24), Vec2(24, 0), Vec2(-24, 24)}, 4),
//	// SMALL_7
//	TilePolygon(0, 0, array<int> = {0, 1, 2}, array<Vec2@> = {Vec2(-24, -24), Vec2(24, -24), Vec2(-24, 0)}, 3),
//	// BIG_8
//	TilePolygon(0, 0, array<int> = {0, 3, 1, 2}, array<Vec2@> = {Vec2(-24, -24), Vec2(0, -24), Vec2(24, 24), Vec2(-24, 24)}, 4),
//	// SMALL_8
//	TilePolygon(0, 0, array<int> = {3, 1, 2}, array<Vec2@> = {Vec2(-24, -24), Vec2(0, 24), Vec2(-24, 24)}, 3),
//	// HALF_A
//	TilePolygon(0, 0, array<int> = {0, 1, 2}, array<Vec2@> = {Vec2(-24, -24), Vec2(24, 24), Vec2(-24, 24)}, 3),
//	// HALF_B
//	TilePolygon(0, 0, array<int> = {0, 1, 2}, array<Vec2@> = {Vec2(-24, -24), Vec2(24, -24), Vec2(-24, 24)}, 3),
//	// HALF_C
//	TilePolygon(0, 0, array<int> = {0, 3, 1}, array<Vec2@> = {Vec2(-24, -24), Vec2(24, -24), Vec2(24, 24)}, 3),
//	// HALF_D
//	TilePolygon(0, 0, array<int> = {3, 1, 0}, array<Vec2@> = {Vec2(24, -24), Vec2(24, 24), Vec2(-24, 24)}, 3)
//};