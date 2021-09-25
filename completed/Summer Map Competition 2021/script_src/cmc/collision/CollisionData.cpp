#include 'ControllableType.cpp';

class CollisionData
{
	
	controllable@ c;
	ControllableType type = ControllableType::Other;
	bool stepped = true;
	CollisionEdge@ collision_roof;
	CollisionEdge@ collision_ground;
	CollisionEdge@ collision_left;
	CollisionEdge@ collision_right;
	
	float contact_dx;
	float contact_dy;
	int contact_count;
	
	CollisionData(controllable@ c)
	{
		@this.c = c;
		
		if(c.player_index() != -1)
		{
			type = ControllableType::Player;
			return;
		}
		
		const string type_name = c.type_name();
		
		if(type_name == 'hittable_apple')
		{
			type = ControllableType::Apple;
		}
		else if(type_name.substr(0, 5) == 'dust_')
		{
			type = ControllableType::Player;
		}
		else if(type_name.substr(0, 6) == 'enemy_')
		{
			type = ControllableType::Enemy;
		}
	}
	
}
