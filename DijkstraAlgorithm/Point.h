#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

using namespace std;

// Default colors used for different point states in visualization
#define BASE_COLOR_POINT sf::Color(210, 210, 210)
#define ACTIVE_COLOR_POINT sf::Color(255, 200, 0)
#define ANALYSE_COLOR_POINT sf::Color(120, 255, 150)
#define POSSIBLE_SOLUTION_COLOR_POINT sf::Color::Yellow
#define SOLUTION_COLOR_POINT sf::Color::Cyan
#define START_COLOR_POINT sf::Color::Green
#define END_COLOR_POINT sf::Color::Red
#define OUTLINE_COLOR_POINT sf::Color::Black

// Default visual sizes
#define SIZE_POINT 10.0f
#define OUTLINE_SIZE_POINT 3

class Point
{
private:
    // Coordinates of the point in the 2D plane
    double x;
    double y;

    string name; // Unique name/label of the point
    sf::Color color; // Fill color of the point (changes during algorithm visualization)
    sf::Color outlineColor; // Color of the outline around the point
    double size; // Diameter of the point
    double outlineSize; // Thickness of the outline
    bool isStartPoint; // Indicates whether this point is the algorithm's start node
    bool isEndPoint; // Indicates whether this point is the algorithm's end node

public:

    // Constructor: creates a point with coordinates and a name
    Point(double x, double y, const string& name);

    // Copy constructor
    Point(const Point& p);

    // Getters
    double getX() const { return x; }
    double getY() const { return y; }
    string getName() const { return name; }
    sf::Color getColor() const { return color; }
    sf::Color getOutlineColor() const { return outlineColor; }
    double getSize() const { return size; }
    double getOutlineSize() const { return outlineSize; }
    bool getIsStartPoint() const { return isStartPoint; }
    bool getIsEndPoint() const { return isEndPoint; }

    // Setters
    void setX(double newX) { x = newX; }
    void setY(double newY) { y = newY; }
    void setName(const string& newName) { name = newName; }

    // Updates the fill color; resets to base color if change=false
    void setColor(const sf::Color& newColor = BASE_COLOR_POINT, bool change = false);

    // Sets the outline color
    void setOutlineColor(const sf::Color& newOutlineColor) { outlineColor = newOutlineColor; }

    // Sets diameter
    void setSize(double newSize) { size = newSize; }

    // Sets outline thickness
    void setOutlineSize(double newOutlineSize) { outlineSize = newOutlineSize; }

    // Marks the point as the algorithm's start node
    void setIsStartPoint(bool isStart);

    // Marks the point as the algorithm's end node
    void setIsEndPoint(bool isEnd);

    // Overloaded operators:
    // Prints the point to an output stream
    friend ostream& operator<<(ostream& os, const Point& point);

    // Compares points by coordinates and name
    friend bool operator==(const Point& p1, const Point& p2);

    // Reads point data from file/input stream
    friend istream& operator>>(istream& in, Point& point);
};
