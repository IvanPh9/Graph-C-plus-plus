#include "Point.h"

void Point::setIsStartPoint(bool isStart)
{
	isStartPoint = isStart;
	if (isStart) isEndPoint = false;
	this->setColor();
}

void Point::setIsEndPoint(bool isEnd)
{
	isEndPoint = isEnd;
	if (isEnd) isStartPoint = false;
	this->setColor();
}

Point::Point(double x, double y, const string& name) : x(x), y(y), name(name), isStartPoint(false), isEndPoint(false)
{
	setColor();
	setSize(SIZE_POINT);
	setOutlineColor(OUTLINE_COLOR_POINT);
	setOutlineSize(OUTLINE_SIZE_POINT);
}

Point::Point(const Point& p) : x(p.x), y(p.y), name(p.name), color(p.color), outlineColor(p.outlineColor), size(p.size), outlineSize(p.outlineSize), isStartPoint(p.isStartPoint), isEndPoint(p.isEndPoint) {}

void Point::setColor(const sf::Color& newColor, bool change) {
	if (change) color = newColor;
	else {
		if (isEndPoint) color = END_COLOR_POINT;
		else if (isStartPoint) color = START_COLOR_POINT;
		else color = BASE_COLOR_POINT;
	}
}
