#include "ValidationAdd.h"

bool isFirstCharAlpha(const std::string& name) {
    if (name.empty()) {
        return false;
    }
    return std::isalpha(static_cast<unsigned char>(name[0]));
}

bool validateNewPoint(const Point& newPoint, const std::vector<Point>& existingPoints) {
    if (!isFirstCharAlpha(newPoint.getName())) {
        cout << "Line Error: First character of Point name must be letter" << endl;
		return false;
    }
    for (const auto& existingP : existingPoints) {
        if (existingP == newPoint) {
            cout << "Line Error: Point Exist" << endl;
			return false;
        }
    }
	return true;
}

bool validateNewLine(const string& startName, const std::string& endName, double weight, const std::vector<Line>& existingLines, const std::vector<Point>& points)
{
    if (startName == endName) {
        cout << "Line Error: Start and End points are the same" << endl;
		return false;
    }
    else if (weight < 0) {
        cout << "Line Error: Weight cannot be negative" << endl;
        return false;
    }
    else if (startName == "" || endName == "") {
        cout << "Line Error: Point names cannot be empty" << endl;
        return false;
    }
    else if (!isFirstCharAlpha(startName) && !isFirstCharAlpha(endName)) {
        cout << "Line Error: First character of Point name must be letter" << endl;
        return false;
    }
    else if (startName == "" || endName == "") {
        cout << "Line Error: Point names cannot be empty" << endl;
        return false;
	}
    for (const auto& line : existingLines) {
        if ((line.getStart().getName() == startName && line.getEnd().getName() == endName) ||
            (line.getStart().getName() == endName && line.getEnd().getName() == startName)) {
            cout << "Line Error: Line Exist" << endl;
            return false;
        }
    }
	return true;
}
