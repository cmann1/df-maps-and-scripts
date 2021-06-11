class EnemyData
{
	entity@ enemy;
	uint id;
	float x;
	float y;
	float l;
	float t;
	float r;
	float b;

	EnemyData(entity@ enemy, float l, float t, float r, float b)
	{
		@this.enemy = enemy;
		this.id = enemy.id();
		this.x = enemy.x();
		this.y = enemy.y();
		this.l = l;
		this.t = t;
		this.r = r;
		this.b = b;
	}
	
	float ex(float offset = 0) const
	{
		if(enemy is null)
			return x + offset;
		
		return enemy.x() + offset;
	}
	
	float ey(float offset = 0) const
	{
		if(enemy is null)
			return y + offset;
		
		return enemy.y() + offset;
	}
}










