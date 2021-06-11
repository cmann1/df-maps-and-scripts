class Particle{
	float oldX;
	float oldY;
	float x;
	float y;
	
	float forceX = 0;
	float forceY = 0;
	
	Particle(float x, float y){
		this.x = x;
		this.y = y;
		oldX = x;
		oldY = y;
	}
	
	void setPosition(float newX, float newY){
		oldX = x;
		oldY = y;
		x = newX;
		y = newY;
	}
	
	void reset(float newX, float newY){
		oldX = x = newX;
		oldY = y = newY;
		forceX = forceY = 0;
	}
	
}