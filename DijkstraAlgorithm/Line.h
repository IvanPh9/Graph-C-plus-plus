#pragma once
#include "Point.h"
class Line
{
private:
	Point start;
	Point end;
	double weight;
public:
	Line(const Point& start, const Point& end, double weight) : start(start), end(end), weight(weight) {}
	Point getStart() const { return start; }
	Point getEnd() const { return end; }
	double getWeight() const { return weight; }

	void setStart(Point p) { start = p; }
	void setEnd(Point p) { end = p; }
	void setWeight(double w) { weight = w; }

	double calculateLength() const;

	friend ostream& operator<<(ostream& os, const Line& line);
	friend bool operator==(const Line& l1, const Line& l2);
};

