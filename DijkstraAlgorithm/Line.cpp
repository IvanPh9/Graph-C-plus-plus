#include "Line.h"

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
		if (isInPath) color = sf::Color::Red;
		else color = newColor;
	}
}

void Line::setBoldness(double b, bool change)
{
	if (change) boldness = b;
	else {
		if (isInPath) boldness = 4.0;
		else boldness = b;
	}
}
