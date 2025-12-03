#include "Line.h"

void Line::setIsInPath(bool inPath)
{
	if (inPath) {
		color = sf::Color::Red;
		boldness = 4.0;
		isInPath = true;
	}
	else {
		color = sf::Color(110, 110, 110);
		boldness = 2.0;
		isInPath = false;
	}
}

double Line::calculateLength() const
{
	double deltaX = end.getX() - start.getX();
	double deltaY = end.getY() - start.getY();
	return sqrt(deltaX * deltaX + deltaY * deltaY);
}
