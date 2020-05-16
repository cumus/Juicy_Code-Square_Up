#include "JuicyMath.h"
#include "Defs.h"

#include <math.h>  

bool JMath::PointInsideRect(int x, int y, SDL_Rect rect)
{
	return (
		x >= rect.x &&
		x <= rect.x + rect.w &&
		y >= rect.y &&
		y <= rect.y + rect.h);
}

bool JMath::PointInsideRect(float x, float y, RectF rect)
{
	return (
		x >= rect.x &&
		x <= rect.x + rect.w &&
		y >= rect.y &&
		y <= rect.y + rect.h);
}


float JMath::TriangleArea(const std::pair<float, float> a, const std::pair<float, float> b, const std::pair<float, float> c)
{
	return abs((
		a.first * (b.second - c.second)
		+ b.first * (c.second - a.second)
		+ c.first * (a.second - b.second)) / 2.0f);
}

bool JMath::PointInsideTriangle(const std::pair<float, float> p, const std::pair<float, float> a, const std::pair<float, float> b, const std::pair<float, float> c)
{
	float abc = TriangleArea(a, b, c);
	float pbc = TriangleArea(p, b, c);
	float apc = TriangleArea(a, p, c);
	float abp = TriangleArea(a, b, p);

	return (abc == pbc + apc + abp);
}

int JMath::HorizontalAxisAngle_I(const std::pair<int, int> origin, const std::pair<int, int> p, int offset)
{
	int angle = int(atan2(float(origin.second - p.second), float(origin.first - p.first)) * RADTODEG) + offset;

	if (angle < 0) angle += 360;
	angle %= 360;

	return angle;
}

float JMath::HorizontalAxisAngle_F(const std::pair<float, float> origin, const std::pair<float, float> p, float offset)
{
	float angle = (atan2(origin.second - p.second, origin.first - p.first) * RADTODEG) + offset;

	if (angle < 0) angle += 360.0f;
	else if (angle > 360.0f) angle -= 360.0f;

	return angle;
}

float JMath::Distance(const std::pair<float, float> origin, const std::pair<float, float> p)
{
	return sqrtf(DistanceSquared(origin, p));
}

float JMath::DistanceSquared(const std::pair<float, float> origin, const std::pair<float, float> p)
{
	std::pair<float, float> dist = { origin.second - p.second, origin.first - p.first };
	return (dist.first * dist.first) + (dist.second * dist.second);
}

float JMath::Cap(const float base, const float lower_bound, const float upper_bound)
{
	if (base <= lower_bound)
		return lower_bound;
	else if (base >= upper_bound)
		return upper_bound;
	else
		return base;
}

std::pair<float, float> JMath::RotatePoint(float x, float y, double degrees)
{
	std::pair<float, float> point;
	//degrees = x * PI / 180;
	point.second = y * cos(degrees) - x * sin(degrees);
	point.first = y * sin(degrees) + x * cos(degrees);
	return point;
}

float JMath::RandomF(float base)
{
	return base * (32767.0f / float(std::rand()));
}

float JMath::RandomRangeF(float max, float min)
{
	return min + RandomF(max - min);
}
