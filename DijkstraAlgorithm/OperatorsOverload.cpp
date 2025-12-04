#include <iostream>
#include "Point.h"
#include "Line.h"

using namespace std;

// Output operator for Point
ostream& operator<<(ostream& os, const Point& point)
{
    os << point.name << "\t" << point.x << "\t" << point.y;
    return os;
}

// Input operator for Point
// Reads x, y, and name from input stream
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

// Equality operator for Point
// Two points are equal if either coordinates match exactly OR names match
bool operator==(const Point& p1, const Point& p2)
{
    return ((p1.x == p2.x) && (p1.y == p2.y)) || (p1.name == p2.name);
}

// Output operator for Line
ostream& operator<<(ostream& os, const Line& line)
{
    os << line.start.getName() << "\t->\t" << line.end.getName() << " \t" << line.weight;
    return os;
}

// Equality operator for Line
bool operator==(const Line& l1, const Line& l2)
{
    return (l1.start == l2.start) && (l1.end == l2.end) && (l1.weight == l2.weight);
}

// Copy-assignment operator for Line
Line& Line::operator=(const Line& other)
{
    if (this != &other) {
        weight = other.weight;
        color = other.color;
        boldness = other.boldness;
        isInPath = other.isInPath;
    }
    return *this;
}
