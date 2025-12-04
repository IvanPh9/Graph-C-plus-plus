#include "Line.h"
#include <cmath>

Line::Line(const Point& start, const Point& end, double weight) : start(start), end(end), weight(weight)
{
	setColor();
	setBoldness();
	setIsInPath(false);
}

void Line::setColor(const sf::Color& newColor, bool change)
{
	if (change) color = newColor;
	else {
		if (isInPath) color = IS_PATH_COLOR_LINE;
		else color = BASE_COLOR_LINE;
	}
}

void Line::setBoldness(double b, bool change)
{
	if (change) boldness = b;
	else {
		if (isInPath) boldness = IS_PATH_BOLDNESS_LINE;
		else boldness = BASE_BOLDNESS_LINE;
	}
}

void Line::setIsInPath(bool inPath)
{
	isInPath = inPath;
	this->setColor();
	this->setBoldness();
}

double Line::calculateLength() const
{
	double deltaX = end.getX() - start.getX();
	double deltaY = end.getY() - start.getY();
	return sqrt(deltaX * deltaX + deltaY * deltaY);
}

