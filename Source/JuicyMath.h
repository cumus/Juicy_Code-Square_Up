#ifndef __JUICY_MATH_H__
#define __JUICY_MATH_H__

#include "SDL/include/SDL_rect.h"
#include <iterator>

namespace JMath
{
	// Check Point (x,y) inside Rect
	bool PointInsideRect(int x, int y, SDL_Rect rect);
	bool PointInsideRect(float x, float y, RectF rect);

	// Get Triangle abc Area
	float TriangleArea(
		const std::pair<float, float> a,
		const std::pair<float, float> b,
		const std::pair<float, float> c);

	// Get Triangle abc Area
	float RectArea(
		 std::pair<float, float> top,
		 std::pair<float, float> bot,
		 std::pair<float, float> left,
		 std::pair<float, float> right);

	// Check Point p is inside Triangle abc
	bool PointInsideTriangle(
		const std::pair<float, float> p,
		const std::pair<float, float> a,
		const std::pair<float, float> b,
		const std::pair<float, float> c);

	// Calculate y axis angle from origin to point p
	int		HorizontalAxisAngle_I(const std::pair<int, int> origin, const std::pair<int, int> p, int offset = 0);
	float	HorizontalAxisAngle_F(const std::pair<float, float> origin, const std::pair<float, float> p, float offset = 0.0f);

	// distance between origin to point p
	float	Distance(const std::pair<float, float> origin, const std::pair<float, float> p);
	float	DistanceSquared(const std::pair<float, float> origin, const std::pair<float, float> p);

	float Cap(const float base, const float lower_bound, const float upper_bound);

	std::pair<float, float> RotatePoint(float x,float y,double degrees);

	// RNG
	float RandomF(float base = 1.0f);
	float RandomRangeF(float max = 1.0f, float min = 0.0f);
};

#endif // __JUICY_MATH_H__