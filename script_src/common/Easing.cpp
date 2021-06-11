// No easing, no acceleration
float easeLinear(float t)
{
	return t;
}

// Accelerating from zero velocity
float easeInQuad(float t)
{
	return t * t;
}

// Decelerating to zero velocity
float easeOutQuad(float t)
{
	return t * (2 - t);
}

// Acceleration until halfway, then deceleration
float easeInOutQuad(float t)
{
	return t < .5 ? 2 * t * t : -1 + (4 - 2 * t) * t;
}

// Accelerating from zero velocity
float easeInCubic(float t)
{
	return t * t * t;
}

// Decelerating to zero velocity
float easeOutCubic(float t)
{
	return (--t) * t * t + 1;
}

// Acceleration until halfway, then deceleration
float easeInOutCubic(float t)
{
	return t < .5 ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1;
}

// Accelerating from zero velocity
float easeInQuart(float t)
{
	return t * t * t * t;
}

// Decelerating to zero velocity
float easeOutQuart(float t)
{
	return 1 - (--t) * t * t * t;
}

// Acceleration until halfway, then deceleration
float easeInOutQuart(float t)
{
	return t < .5 ? 8 * t * t * t * t : 1 - 8 * (--t) * t * t * t;
}

// Accelerating from zero velocity
float easeInQuint(float t)
{
	return t * t * t * t * t;
}

// Decelerating to zero velocity
float easeOutQuint(float t)
{
	return 1 + (--t) * t * t * t * t;
}

// Acceleration until halfway, then deceleration
float easeInOutQuint(float t)
{
	return t < .5 ? 16 * t * t * t * t * t : 1 + 16 * (--t) * t * t * t * t;
}