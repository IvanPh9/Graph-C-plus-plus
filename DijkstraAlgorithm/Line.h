#pragma once
#include <SFML/Graphics.hpp>
#include "Point.h"
class Line
{
private:
	Point start;
	Point end;
	double weight;
	sf::Color color;
	double boldness;
public:
	Line(const Point& start, const Point& end, double weight) : start(start), end(end), weight(weight), color(sf::Color(110, 110, 110)), boldness(2.0) {}
	Point getStart() const { return start; }
	Point getEnd() const { return end; }
	double getWeight() const { return weight; }
	sf::Color getColor() const { return color; }
	double getBoldness() const { return boldness; }

	void setStart(Point p) { start = p; }
	void setEnd(Point p) { end = p; }
	void setWeight(double w) { weight = w; }
	void setColor(const sf::Color& c) { color = c; }
	void setBoldness(double b) { boldness = b; }

	double calculateLength() const;

	friend ostream& operator<<(ostream& os, const Line& line);
	friend bool operator==(const Line& l1, const Line& l2);
};

