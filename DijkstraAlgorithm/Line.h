#pragma once
#include <SFML/Graphics.hpp>
#include "Point.h"

#define BASE_COLOR_LINE sf::Color(110, 110, 110)
#define ACTIVE_COLOR_LINE sf::Color(100, 150, 255)
#define ACCEPT_COLOR_LINE sf::Color(80, 220, 120)
#define REJECTED_COLOR_LINE sf::Color(255, 100, 100)
#define IS_PATH_COLOR_LINE sf::Color::Red
#define POSSIBLE_SOLUTION_COLOR_LINE sf::Color::Blue

#define BASE_BOLDNESS_LINE 2
#define ACTIVE_BOLDNESS_LINE 3
#define IS_PATH_BOLDNESS_LINE 4

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
	void setColor(const sf::Color& newColor = BASE_COLOR_LINE, bool change = false);
	void setBoldness(double b = BASE_BOLDNESS_LINE, bool change = false);
	void setIsInPath(bool inPath);

	double calculateLength() const;

	friend ostream& operator<<(ostream& os, const Line& line);
	friend bool operator==(const Line& l1, const Line& l2);
};

