#include "Interface.h"
#include "ValidationAdd.h"
#include "FileProcesses.h"
#include "WindowDraw.h"
#include "ImplementationAlgorithm.h"
#include <vector>
#include <mutex>
#include <atomic>

using namespace std;

// -------------------------------------------------------------
// Constants for file loading
// -------------------------------------------------------------
#define FILEPOINTS "points.txt"
#define FILELINES "lines.txt"

// -------------------------------------------------------------
// Global shared state for console + graphics synchronization
// -------------------------------------------------------------
mutex dataMutex;            // protects access to points and lines
atomic<bool> isRunning(true);   // controls console thread lifetime
int ANIMATION_DELAY = 0;        // delay for visualization animation

// -------------------------------------------------------------
// Reset all runtime visualization states (colors, flags)
// Does NOT delete any data, only restores neutral visual state
// -------------------------------------------------------------
void cleanWorkspace(vector<Point>& points, vector<Line>& lines) {
	for (auto& l : lines) {
		l.setIsInPath(false);
	}
	for (auto& p : points) {
		if (p.getIsStartPoint() || p.getIsEndPoint()) continue;
		p.setColor(sf::Color(210, 210, 210), true);
	}
}

// -------------------------------------------------------------
// Delete point by name, also removes all connected lines
// Returns true if a point was removed, false otherwise
// -------------------------------------------------------------
bool deletePointByName(const string& name, vector<Point>& points, vector<Line>& lines) {
	auto it = remove_if(points.begin(), points.end(),
		[&name](const Point& p) { return p.getName() == name; });

	if (it != points.end()) {
		points.erase(it, points.end());

		// Remove all lines that touch this point
		lines.erase(remove_if(lines.begin(), lines.end(),
			[&name](const Line& l) {
				return l.getStart().getName() == name || l.getEnd().getName() == name;
			}), lines.end());

		return true;
	}
	return false;
}

// -------------------------------------------------------------
// Delete a line defined by two point names
// -------------------------------------------------------------
void deleteLineByPoints(const string& startName, const string& endName, vector<Line>& lines) {
	auto it = remove_if(lines.begin(), lines.end(),
		[&startName, &endName](const Line& l) {
			return (l.getStart().getName() == startName && l.getEnd().getName() == endName) ||
				(l.getStart().getName() == endName && l.getEnd().getName() == startName);
		});
	if (it != lines.end()) {
		lines.erase(it, lines.end());
	}
}

// -------------------------------------------------------------
// Main console interface loop (runs in separate thread)
// Handles user input, data modification, and file I/O
// All operations touching shared state are mutex-protected
// -------------------------------------------------------------
void MainInterface(std::vector<Point>& points, std::vector<Line>& lines)
{
	while (isRunning) {
		cout << "\n------- Menu -------\n";
		cout << " - 1.  Load points from files\n";
		cout << " - 2.  Load lines from files\n";
		cout << "-------------------\n";
		cout << " - 3.  Add point\n";
		cout << " - 4.  Add line\n";
		cout << "-------------------\n";
		cout << " - 5.  Flag start point\n";
		cout << " - 6.  Flag end point\n";
		cout << "-------------------\n";
		cout << " - 7.  Show current points info\n";
		cout << " - 8.  Show current lines info\n";
		cout << "-------------------\n";
		cout << " - 9.  Delete point\n";
		cout << " - 10. Delete line\n";
		cout << "-------------------\n";
		cout << " - 11. Find shortest path\n";
		cout << "-------------------\n";
		cout << " - 0.  Exit console loop\n";
		cout << " - 13. Settings\n";
		cout << "-------------------\n";
		cout << "Enter command: ";

		int command;

		// Safe input reading
		if (!(cin >> command)) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid input. Please enter a number corresponding to a command.\n";
			continue;
		}

		switch (command) {

			// ---------------------- FILE LOAD ----------------------
		case 1: {
			lock_guard<mutex> lock(dataMutex);
			cout << "Choose file to load points:\n";
			cout << " - " << FILEPOINTS << endl;
			cout << "Enter filename: ";

			string filename;
			cin >> filename;

			if (loadPointsFromFile(filename, points, lines))
				cout << "Points loaded successfully.\n";
			else
				cout << "Failed to load points.\n";
			break;
		}
		case 2: {
			lock_guard<mutex> lock(dataMutex);
			cout << "Choose file to load lines:\n";
			cout << " - " << FILELINES << endl;
			cout << "Enter filename: ";

			string filename;
			cin >> filename;

			if (loadLinesFromFile(filename, lines, points))
				cout << "Lines loaded successfully.\n";
			else
				cout << "Failed to load lines.\n";
			break;
		}

			  // ---------------------- ADD POINT ----------------------
		case 3: {
			lock_guard<mutex> lock(dataMutex);
			cleanWorkspace(points, lines);

			Point newPoint(0.0, 0.0, "");
			cout << "Enter point x y name: ";
			cin >> newPoint;

			if (validateNewPoint(newPoint, points)) {
				points.push_back(newPoint);
				cout << "Point added successfully.\n";
			}
			break;
		}

			  // ---------------------- ADD LINE ----------------------
		case 4: {
			lock_guard<mutex> lock(dataMutex);
			cleanWorkspace(points, lines);

			double weight;
			cout << "Enter start point name, end point name and weight: ";

			string startName, endName;
			cin >> startName >> endName >> weight;

			// Validation handles duplicates and illegal values
			if (!validateNewLine(startName, endName, weight, lines, points)) {
				break;
			}

			Point* start = nullptr;
			Point* end = nullptr;

			// Find pointers to points
			for (auto& p : points) {
				if (p.getName() == startName) start = &p;
				if (p.getName() == endName) end = &p;
			}

			if (start && end) {
				lines.emplace_back(*start, *end, weight);
				cout << "Line added successfully.\n";
			}
			else {
				cout << "Line Error: One or both points not found.\n";
			}
			break;
		}

			  // ---------------------- FLAG START ----------------------
		case 5: {
			lock_guard<mutex> lock(dataMutex);
			cleanWorkspace(points, lines);
			cout << "Enter start point name: ";

			string startName;
			cin >> startName;

			// Reset flags
			for (auto& p : points) p.setIsStartPoint(false);

			bool found = false;
			for (auto& p : points) {
				if (p.getName() == startName) {
					p.setIsStartPoint(true);
					cout << "Start point flagged successfully.\n";
					found = true;
					break;
				}
			}
			if (!found) cout << "No such point found.\n";
			break;
		}

			  // ---------------------- FLAG END ----------------------
		case 6: {
			lock_guard<mutex> lock(dataMutex);
			cleanWorkspace(points, lines);
			cout << "Enter end point name: ";

			string endName;
			cin >> endName;

			// Reset flags
			for (auto& p : points) p.setIsEndPoint(false);

			bool found = false;
			for (auto& p : points) {
				if (p.getName() == endName) {
					p.setIsEndPoint(true);
					cout << "End point flagged successfully.\n";
					found = true;
					break;
				}
			}
			if (!found) cout << "No such point found.\n";
			break;
		}

			  // ---------------------- LIST POINTS ----------------------
		case 7: {
			lock_guard<mutex> lock(dataMutex);
			cout << "Name\tx\ty\n";
			cout << "-------------------\n";
			for (const auto& p : points) cout << p << "\n";
			break;
		}

			  // ---------------------- LIST LINES ----------------------
		case 8: {
			lock_guard<mutex> lock(dataMutex);
			cout << "Start\t->\tEnd\tWeight\n";
			cout << "-------------------\n";
			for (const auto& l : lines) cout << l << "\n";
			break;
		}

			  // ---------------------- DELETE POINT ----------------------
		case 9: {
			lock_guard<mutex> lock(dataMutex);
			cleanWorkspace(points, lines);

			cout << "Enter point name to delete: ";
			string pointName;
			cin >> pointName;

			if (deletePointByName(pointName, points, lines))
				cout << "Point and connected lines deleted.\n";
			else
				cout << "No such point.\n";
			break;
		}

			  // ---------------------- DELETE LINE ----------------------
		case 10: {
			lock_guard<mutex> lock(dataMutex);
			cleanWorkspace(points, lines);

			cout << "Enter start and end point names for line deletion: ";

			string startName, endName;
			cin >> startName >> endName;

			deleteLineByPoints(startName, endName, lines);
			cout << "Line deleted (if it existed).\n";
			break;
		}

			   // ---------------------- DIJKSTRA ----------------------
		case 11: {
			cleanWorkspace(points, lines);

			cout << "Finding shortest path...\n";
			auto [s, p] = findShortestPath(points, lines);

			if (p == 0) {
				cout << "No path found.\n";
				break;
			}

			cout << "Shortest path: " << s << "\n";
			cout << "Total weight: " << p << "\n";
			break;
		}

			   // ---------------------- EXIT MAIN LOOP ----------------------
		case 0: {
			isRunning = false;
			cout << "Exiting console loop...\n";
			break;
		}

			  // ---------------------- SETTINGS ----------------------
		case 13: {
			lock_guard<mutex> lock(dataMutex);

			cout << "Settings menu:\n";
			cout << " - 1. Toggle point labels display\n";
			cout << " - 2. Toggle edge weights display\n";
			cout << " - 3. Set animation delay (current: " << ANIMATION_DELAY << " ms)\n";
			cout << "Enter command: ";

			int settingCommand;
			cin >> settingCommand;

			switch (settingCommand) {
			case 1:
				LABEL_SHOW = !LABEL_SHOW;
				cout << "Point labels now " << (LABEL_SHOW ? "ON" : "OFF") << ".\n";
				break;

			case 2:
				WEIGHT_SHOW = !WEIGHT_SHOW;
				cout << "Edge weights now " << (WEIGHT_SHOW ? "ON" : "OFF") << ".\n";
				break;

			case 3:
				cout << "Enter animation delay in ms: ";
				cin >> ANIMATION_DELAY;
				if (ANIMATION_DELAY < 0) ANIMATION_DELAY = 0;
				cout << "Animation delay set to " << ANIMATION_DELAY << " ms.\n";
				break;

			default:
				cout << "Invalid settings command.\n";
			}
			break;
		}

			   // ---------------------- INVALID INPUT ----------------------
		default:
			cout << "Invalid command. Try again.\n";
			break;
		}
	}

	cout << "Console thread finished.\n";
}
