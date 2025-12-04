#pragma once
#include <SFML/Graphics.hpp>
#include "Point.h"

// Default colors for different line states
#define BASE_COLOR_LINE sf::Color(110, 110, 110)
#define ACTIVE_COLOR_LINE sf::Color(100, 150, 255)
#define ACCEPT_COLOR_LINE sf::Color(80, 220, 120)
#define REJECTED_COLOR_LINE sf::Color(255, 100, 100)
#define IS_PATH_COLOR_LINE sf::Color::Red
#define POSSIBLE_SOLUTION_COLOR_LINE sf::Color::Blue

// Default thickness values for different states
#define BASE_BOLDNESS_LINE 2
#define ACTIVE_BOLDNESS_LINE 3
#define IS_PATH_BOLDNESS_LINE 4

class Line
{
private:
    // References to the start and end points of the line.
    const Point& start;
    const Point& end;
    double weight; // Weight of the edge (used in algorithms such as Dijkstra)

    sf::Color color; // Current color of the line (depends on algorithm visualization state)
    double boldness; // Visual thickness of the line when rendered

    bool isInPath; // Indicates if the line is part of the final path solution

public:
    // Constructor: creates a line between two points with a specific weight
    Line(const Point& start, const Point& end, double weight);

    // Getters
    const Point& getStart() const { return start; }
    const Point& getEnd() const { return end; }
    double getWeight() const { return weight; }
    sf::Color getColor() const { return color; }
    double getBoldness() const { return boldness; }
    bool getIsInPath() const { return isInPath; }

    // Setters
    void setWeight(double w) { weight = w; }

    // Updates the line color; if 'change' is false, resets to base color
    void setColor(const sf::Color& newColor = BASE_COLOR_LINE, bool change = false);

    // Updates line thickness; if 'change' is false, resets to base thickness
    void setBoldness(double b = BASE_BOLDNESS_LINE, bool change = false);

    // Marks the line as part of the resulting shortest path
    void setIsInPath(bool inPath);

    // Calculates the geometric pixel length of the line
    double calculateLength() const;

    // Assignment operator (copies line visual/weight data but keeps references to the same points)
    Line& operator=(const Line& other);

    // Output operator for saving the line to a file or printing it
    friend ostream& operator<<(ostream& os, const Line& line);

    // Compares two lines by their endpoints and weight
    friend bool operator==(const Line& l1, const Line& l2);
};
