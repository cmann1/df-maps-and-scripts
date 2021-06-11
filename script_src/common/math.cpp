const float EPSILON = 5.3E-5;

const float PI = 3.1415926535897932384626433832795;
const float PI2 = PI * 2;
const float HALF_PI = PI / 2;
const float DEG2RAD = 1.0 / 180.0 * PI;
const float RAD2DEG = 1.0 / PI * 180.0;

const float PIXEL2TILE = 1.0 / 48;
const float TILE2PIXEL = 48;

//const float NaN = float(0x7fc00000);
//const float Infinity = float(0x7f800000);

float dot(float x1, float y1, float x2, float y2)
{
	return x1 * x2 + y1 * y2;
}

float frand()
{
	return float(rand()) / float(0x3fffffff);
}

int rand_range(int min, int max)
{
	return min + (rand() % (max - min + 1));
}
float rand_range(float min, float max)
{
	return min + (max - min) * frand();
}

float magnitude(float x, float y)
{
	return sqrt(x * x + y * y);
}
float distance(float x1, float y1, float x2, float y2)
{
	const float dx = x2 - x1;
	const float dy = y2 - y1;
	return sqrt(dx * dx + dy * dy);
}

float length_sqr(float x, float y)
{
	return x * x + y * y;
}

float dist_sqr(float x1, float y1, float x2, float y2)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	return dx * dx + dy * dy;
}

float lerp(float a, float b, float x)
{
	return a * (1.0 - x) + b * x;
}

float short_angle(float a0, float a1)
{
	float da = (a1 - a0) % PI2;
	return 2 * da % PI2 - da;
}

float lerp_angle(float a0, float a1, float t)
{
    return a0 + short_angle(a0, a1) * t;
}

void normalize(float x, float y, float &out out_x, float &out out_y)
{
	const float len = sqrt(x * x + y * y);
	out_x = len != 0 ? x / len : 0;
	out_y = len != 0 ? y / len : 0;
}

void project(float ax, float ay, float bx, float by, float &out out_x, float &out out_y)
{
	const float dp = dot(ax, ay, bx, by);
	out_x = ( dp / (bx * bx + by * by) ) * bx;
	out_y = ( dp / (bx * bx + by * by) ) * by;
}

void reflect(float x, float y, float normal_x, float normal_y, float &out out_x, float &out out_y)
{
	float d = dot(x, y, normal_x, normal_y);
	out_x = x - 2 * normal_x * d;
	out_y = y - 2 * normal_y * d;
}

void rotate(float x, float y, float angle, float &out out_x, float &out out_y)
{
	out_x = cos(angle) * x - sin(angle) * y;
	out_y = sin(angle) * x + cos(angle) * y;
}

float sgn(float x)
{
	return x < -1e-9 ? -1 : (x > 1e-9 ? 1 : 0);
}

void vec2_limit(float x, float y, float limit, float &out out_x, float &out out_y)
{
	float length = x * x + y * y;

	if(length > limit * limit && length > 0)
	{
		length = sqrt(length);
		out_x = x / length * limit;
		out_y = y / length * limit;
	}
	else
	{
		out_x = x;
		out_y = y;
	}
}

float map(float value, float from_min, float from_max, float to_min, float to_max)
{
	value = (value - from_min) / (from_max - from_min);
	return to_min + value * (to_max - to_min);
}

// Returns the z-component of the cross product of a and b
float cross_product_z(float a_x, float a_y, float b_x, float b_y) {
    return a_x * b_y - a_y * b_x;
}

// Orientation is positive if abc is counterclockwise, negative if clockwise.
// (It is actually twice the area of triangle abc, calculated using the
// Shoelace formula: http://en.wikipedia.org/wiki/Shoelace_formula .)
float orientation(float a_x, float a_y, float b_x, float b_y, float c_x, float c_y) {
    return cross_product_z(a_x, a_y, b_x, b_y) + cross_product_z(b_x, b_y, c_x, c_y) + cross_product_z(c_x, c_y, a_x, a_y);
}








