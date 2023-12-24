class CollisionData
{
	
	controllable@ c;
	ControllableType type = ControllableType::Other;
	bool stepped = true;
	CollisionEdge@ collision_roof;
	CollisionEdge@ collision_ground;
	CollisionEdge@ collision_left;
	CollisionEdge@ collision_right;
	
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

enum ControllableType
{
	
	Other	= 0x1,
	Player	= 0x2,
	Enemy	= 0x4,
	Apple	= 0x8,
	
}
