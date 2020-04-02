#include "JuicyMath.h"

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