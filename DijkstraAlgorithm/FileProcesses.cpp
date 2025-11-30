#include "FileProcesses.h"
#include "Line.h"	
#include "Point.h"
#include <vector>
#include <cctype>
#include <fstream>

using namespace std;

bool isFirstCharAlpha(const std::string& name) {
    if (name.empty()) {
        return false;
    }
    return std::isalpha(static_cast<unsigned char>(name[0]));
}

bool loadPointsFromFile(const std::string& filename, std::vector<Point>& points) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        return false;
    }

    points.clear();

    Point p(0.0, 0.0, "");
    while (!in.eof()) {
        if (in >> p) {
            bool found = false;

            if (!isFirstCharAlpha(p.getName())) {
                cout << "Line Error: First character of Point name must be letter (" << filename << ")" << endl;
                continue;
            }

            for (const auto& existingP : points) {
                if (existingP == p) {
                    found = true;
                    cout << "Line Error: Point Exist (" << filename << ")" << endl;
                    break;
                }
            }

            if (!found) {
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
            if (startName == endName) {
                cout << "Line Error: Start and End points are the same (" << filename << ")" << endl;
                continue;
			}
            else if (weight < 0) {
                cout << "Line Error: Weight cannot be negative (" << filename << ")" << endl;
                continue;
            }
            else if (startName == "" || endName == "") {
                cout << "Line Error: Point names cannot be empty (" << filename << ")" << endl;
                continue;
			}
            else if (!isFirstCharAlpha(startName) && !isFirstCharAlpha(endName)) {
                cout << "Line Error: First character of Point name must be letter (" << filename << ")" << endl;
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
