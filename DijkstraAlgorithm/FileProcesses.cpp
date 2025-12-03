#include "FileProcesses.h"
#include "Line.h"	
#include "Point.h"
#include <vector>
#include <cctype>
#include <fstream>
#include "ValidationAdd.h"

using namespace std;

bool loadPointsFromFile(const std::string& filename, std::vector<Point>& points, std::vector<Line>& lines) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        return false;
    }

    points.clear();
	lines.clear();

    Point p(0.0, 0.0, "");
    while (!in.eof()) {
        if (in >> p) {
			if (validateNewPoint(p, points)) {
                points.push_back(p);
            }
        }
        else {
            in.clear();
            in.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Line Error: Format Error (" << filename << ")" << endl;
        }
    }

    return true;
}

bool loadLinesFromFile(const std::string& filename, std::vector<Line>& lines, std::vector<Point>& points)
{
    std::ifstream in(filename);
    if (!in.is_open()) {
        return false;
    }

    if (points.empty()) {
        cout << "Line Error: No points available to reference (" << filename << ")" << endl;
		return false;
    }

    lines.clear();

    string startName, endName;
    double weight = 0.0;
    while (!in.eof()) {
        if (in >> startName >> endName >> weight) {
            Point* start = nullptr;
            Point* end = nullptr;
           if (!validateNewLine(startName, endName, weight, lines, points)) {
                continue;
		   }
            for (auto& a : points) {
                if (a.getName() == startName) {
                    start = &a;
                }
                else if (a.getName() == endName) {
                    end = &a;
                }
            }
            if (start && end) {
                lines.emplace_back(*start, *end, weight);
            }
            else {
                cout << "Line Error: Points not found (" << filename << ")" << endl;
            }
        }
        else {
            in.clear();
            in.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Line Error: Format Error (" << filename << ")" << endl;
		}
    }
    return true;
}
