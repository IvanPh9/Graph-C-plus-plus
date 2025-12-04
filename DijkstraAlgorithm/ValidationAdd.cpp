#include "ValidationAdd.h"

// Checks if the first character of a name is an alphabet letter
bool isFirstCharAlpha(const std::string& name) {
    if (name.empty()) {
        return false; // Empty string is invalid
    }
    return isalpha(static_cast<unsigned char>(name[0])); // Cast to avoid undefined behavior
}

// Validates a new point before adding it to the existing points
bool validateNewPoint(const Point& newPoint, const std::vector<Point>& existingPoints) {
    // First character of point name must be a letter
    if (!isFirstCharAlpha(newPoint.getName())) {
        cout << "Line Error: First character of Point name must be letter" << endl;
        return false;
    }

    // Check for duplicate points (by coordinates or name)
    for (const auto& existingP : existingPoints) {
        if (existingP == newPoint) {
            cout << "Line Error: Point Exist" << endl;
            return false;
        }
    }

    return true; // Point is valid
}

// Validates a new line before adding it to the existing lines
bool validateNewLine(const string& startName, const std::string& endName, double weight,
    const std::vector<Line>& existingLines, const std::vector<Point>& points)
{
    // Start and end points cannot be the same
    if (startName == endName) {
        cout << "Line Error: Start and End points are the same" << endl;
        return false;
    }

    // Weight cannot be negative
    if (weight < 0) {
        cout << "Line Error: Weight cannot be negative" << endl;
        return false;
    }

    // Point names cannot be empty
    if (startName.empty() || endName.empty()) {
        cout << "Line Error: Point names cannot be empty" << endl;
        return false;
    }

    // First character of point names must be letters
    if (!isFirstCharAlpha(startName) && !isFirstCharAlpha(endName)) {
        cout << "Line Error: First character of Point name must be letter" << endl;
        return false;
    }

    // Check if line already exists (both directions considered)
    for (const auto& line : existingLines) {
        if ((line.getStart().getName() == startName && line.getEnd().getName() == endName) ||
            (line.getStart().getName() == endName && line.getEnd().getName() == startName)) {
            cout << "Line Error: Line Exist" << endl;
            return false;
        }
    }

    return true; // Line is valid
}
