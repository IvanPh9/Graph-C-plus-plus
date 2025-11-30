#pragma once

#include <string>

#include <iostream>

using namespace std;

class Point
{
private:
	double x;
	double y;
	string name;
public:
	Point(double x, double y, const string& name) : x(x), y(y), name(name) {}
	Point(const Point& p): x(p.x), y(p.y), name(p.name) {}
	double getX() const { return x; }
	double getY() const { return y; }
	string getName() const { return name; }

	void setX(double newX) { x = newX; }
	void setY(double newY) { y = newY; }
	void setName(const string& newName) { name = newName; }

	friend ostream& operator<<(ostream& os, const Point& point);
	friend bool operator==(const Point& p1, const Point& p2);
	friend istream& operator>>(istream& in, Point& point);


};

