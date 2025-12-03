#include "Point.h"

void Point::setIsStartPoint(bool isStart)
{
	isStartPoint = isStart;
	if (isStart) {
		this->setColor(sf::Color::Green);
		isEndPoint = false;
	}
	else if (!isEndPoint) {
		this->setColor(sf::Color(210, 210, 210));
	}
}

void Point::setIsEndPoint(bool isEnd)
{
	isEndPoint = isEnd;
	if (isEnd) {
		this->setColor(sf::Color::Red);
		isStartPoint = false;
	}
	else if (!isStartPoint) {
		this->setColor(sf::Color(210, 210, 210));
	}
}
