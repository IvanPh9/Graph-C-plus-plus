#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

using namespace std;

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
	void setColor(const sf::Color& newColor = sf::Color(210, 210, 210), bool change=false);
	void setOutlineColor(const sf::Color& newOutlineColor) { outlineColor = newOutlineColor; }
	void setSize(double newSize) { size = newSize; }
	void setOutlineSize(double newOutlineSize) { outlineSize = newOutlineSize; }
	void setIsStartPoint(bool isStart);
	void setIsEndPoint(bool isEnd);

	friend ostream& operator<<(ostream& os, const Point& point);
	friend bool operator==(const Point& p1, const Point& p2);
	friend istream& operator>>(istream& in, Point& point);


};

