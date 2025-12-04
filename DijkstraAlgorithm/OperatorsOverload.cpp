#include <iostream>
#include "Point.h"
#include "Line.h"

using namespace std;

ostream& operator<<(ostream& os, const Point& point)
{
    os << point.name << "\t" << point.x << "\t" << point.y;
    return os;
}

istream& operator>>(istream& in, Point& point)
{
    double x, y;
    string name;

    if (in >> x >> y >> name) {
        point.setX(x);
        point.setY(y);
        point.setName(name);
    }
    return in;
}


bool operator==(const Point& p1, const Point& p2)
{
    return ((p1.x == p2.x) && (p1.y == p2.y)) || (p1.name == p2.name);
}

ostream& operator<<(ostream& os, const Line& line)
{
	os << line.start.getName() << "\t->\t" << line.end.getName() << " \t" << line.weight;
    return os;
}

bool operator==(const Line& l1, const Line& l2)
{
    return (l1.start == l2.start) && (l1.end == l2.end) && (l1.weight == l2.weight);
}
// Copy-assignment: do not (and cannot) rebind the const reference members `start` and `end`.
// We only copy the assignable state from `other`.
Line& Line::operator=(const Line& other)
{
    if (this != &other) {
        // start and end are const references and must keep their original bindings.
        weight = other.weight;
        color = other.color;
        boldness = other.boldness;
        isInPath = other.isInPath;
    }
    return *this;
}
