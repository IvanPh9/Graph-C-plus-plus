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
	bool isInPath;
public:
	Line(const Point& start, const Point& end, double weight);
	Point getStart() const { return start; }
	Point getEnd() const { return end; }
	double getWeight() const { return weight; }
	sf::Color getColor() const { return color; }
	double getBoldness() const { return boldness; }
	bool getIsInPath() const { return isInPath; }

	void setStart(Point p) { start = p; }
	void setEnd(Point p) { end = p; }
	void setWeight(double w) { weight = w; }
	void setColor(const sf::Color& newColor =sf::Color(110, 110, 110), bool change = false);
	void setBoldness(double b = 2, bool change = false);
	void setIsInPath(bool inPath);

	double calculateLength() const;

	friend ostream& operator<<(ostream& os, const Line& line);
	friend bool operator==(const Line& l1, const Line& l2);
};

