#include "JuicyMath.h"

bool JMath::PointInsideRect(float x, float y, RectF rect)
{
	return (
		x >= rect.x &&
		x <= rect.x + rect.w &&
		y >= rect.y &&
		y <= rect.y + rect.h);
}

