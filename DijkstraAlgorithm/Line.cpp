#include "Line.h"

double Line::calculateLength() const
{
	double deltaX = end.getX() - start.getX();
	double deltaY = end.getY() - start.getY();
	return sqrt(deltaX * deltaX + deltaY * deltaY);
}
