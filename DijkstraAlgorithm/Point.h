#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

using namespace std;

#define BASE_COLOR_POINT sf::Color(210, 210, 210)
#define ACTIVE_COLOR_POINT sf::Color(255, 200, 0)
#define ANALYSE_COLOR_POINT sf::Color(120, 255, 150)
#define POSSIBLE_SOLUTION_COLOR_POINT sf::Color::Yellow
#define SOLUTION_COLOR_POINT sf::Color::Cyan
#define START_COLOR_POINT sf::Color::Green
#define END_COLOR_POINT sf::Color::Red
#define OUTLINE_COLOR_POINT sf::Color::Black

#define SIZE_POINT 10.0f
#define OUTLINE_SIZE_POINT 3

class Point
{
private:
	double x;
	double y;
	string name;
	sf::Color color;
	sf::Color outlineColor;
	double size;
	double outlineSize;
	bool isStartPoint;
	bool isEndPoint;
public:
	Point(double x, double y, const string& name);
	Point(const Point& p);
	double getX() const { return x; }
	double getY() const { return y; }
	string getName() const { return name; }
	sf::Color getColor() const { return color; }
	sf::Color getOutlineColor() const { return outlineColor; }
	double getSize() const { return size; }
	double getOutlineSize() const { return outlineSize; }
	bool getIsStartPoint() const { return isStartPoint; }
	bool getIsEndPoint() const { return isEndPoint; }

	void setX(double newX) { x = newX; }
	void setY(double newY) { y = newY; }
	void setName(const string& newName) { name = newName; }
	void setColor(const sf::Color& newColor = BASE_COLOR_POINT, bool change=false);
	void setOutlineColor(const sf::Color& newOutlineColor) { outlineColor = newOutlineColor; }
	void setSize(double newSize) { size = newSize; }
	void setOutlineSize(double newOutlineSize) { outlineSize = newOutlineSize; }
	void setIsStartPoint(bool isStart);
	void setIsEndPoint(bool isEnd);

	friend ostream& operator<<(ostream& os, const Point& point);
	friend bool operator==(const Point& p1, const Point& p2);
	friend istream& operator>>(istream& in, Point& point);


};

