#include "FileProcesses.h"
#include "ValidationAdd.h"
#include "Line.h"	
#include "Point.h"
#include <vector>
#include <cctype>
#include <fstream>

using namespace std;

// ----------------------------------------------
// Load points from file into points vector
// Returns true if file was successfully read
// ----------------------------------------------
bool loadPointsFromFile(const std::string& filename, std::vector<Point>& points, std::vector<Line>& lines) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        return false; // File cannot be opened
    }

    points.clear();
    lines.clear(); // Also reset lines to avoid mismatched references

    Point p(0.0, 0.0, "");

    // Read points until EOF
    while (!in.eof()) {
        if (in >> p) { // Uses overloaded operator>>
            // Validate point before adding
            if (validateNewPoint(p, points)) {
                points.push_back(p);
            }
        }
        else {
            // Handle invalid line format
            in.clear();
            in.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Line Error: Format Error (" << filename << ")" << endl;
        }
    }

    return true;
}

// -----------------------------------------------------
// Load lines (edges) from file
// Requires that 'points' already contains valid points
// Returns true if file successfully processed
// -----------------------------------------------------
bool loadLinesFromFile(const std::string& filename, std::vector<Line>& lines, std::vector<Point>& points)
{
    std::ifstream in(filename);
    if (!in.is_open()) {
        return false; // Could not open file
    }

    // You cannot load lines if there are no points to reference
    if (points.empty()) {
        cout << "Line Error: No points available to reference (" << filename << ")" << endl;
        return false;
    }

    lines.clear();

    string startName, endName;
    double weight = 0.0;

    // Read until reaching end of file
    while (!in.eof()) {
        // Read expected format: START END WEIGHT
        if (in >> startName >> endName >> weight) {

            // Validate line before searching for point references
            if (!validateNewLine(startName, endName, weight, lines, points)) {
                continue; // Skip invalid line
            }

            Point* start = nullptr;
            Point* end = nullptr;

            // Find pointers to matching Point objects
            for (auto& a : points) {
                if (a.getName() == startName) {
                    start = &a;
                }
                else if (a.getName() == endName) {
                    end = &a;
                }
            }

            // If both points exist, create the line
            if (start && end) {
                lines.emplace_back(*start, *end, weight);
            }
            else {
                cout << "Line Error: Points not found (" << filename << ")" << endl;
            }
        }
        else {
            // Handle format errors and skip the line
            in.clear();
            in.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Line Error: Format Error (" << filename << ")" << endl;
        }
    }

    return true;
}
