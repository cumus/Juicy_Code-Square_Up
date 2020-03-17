#ifndef __JUICY_MATH_H__
#define __JUICY_MATH_H__

#include "SDL/include/SDL_rect.h"
#include <iterator>

namespace JMath
{
	// Check Point (x,y) inside Rect
	bool PointInsideRect(float x, float y, RectF rect);

	// Get Triangle abc Area
	float TriangleArea(
		const std::pair<float, float> a,
		const std::pair<float, float> b,
		const std::pair<float, float> c);

	// Check Point p is inside Triangle abc
	bool PointInsideTriangle(
		const std::pair<float, float> p,
		const std::pair<float, float> a,
		const std::pair<float, float> b,
		const std::pair<float, float> c);
};


#endif // __JUICY_MATH_H__