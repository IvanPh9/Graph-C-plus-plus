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
public:
	Point(double x, double y, const string& name) : x(x), y(y), name(name), color(sf::Color(210, 210, 210)), outlineColor(sf::Color::Black), size(5.0), outlineSize(2.2) {}
	Point(const Point& p) : x(p.x), y(p.y), name(p.name), color(p.color), outlineColor(p.outlineColor), size(p.size), outlineSize(p.outlineSize) {}
	double getX() const { return x; }
	double getY() const { return y; }
	string getName() const { return name; }
	sf::Color getColor() const { return color; }
	sf::Color getOutlineColor() const { return outlineColor; }
	double getSize() const { return size; }
	double getOutlineSize() const { return outlineSize; }

	void setX(double newX) { x = newX; }
	void setY(double newY) { y = newY; }
	void setName(const string& newName) { name = newName; }
	void setColor(const sf::Color& newColor) { color = newColor; }
	void setOutlineColor(const sf::Color& newOutlineColor) { outlineColor = newOutlineColor; }
	void setSize(double newSize) { size = newSize; }
	void setOutlineSize(double newOutlineSize) { outlineSize = newOutlineSize; }

	friend ostream& operator<<(ostream& os, const Point& point);
	friend bool operator==(const Point& p1, const Point& p2);
	friend istream& operator>>(istream& in, Point& point);


};

