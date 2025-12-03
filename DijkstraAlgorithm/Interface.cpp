#include "Interface.h"
#include "ValidationAdd.h"
#include "FileProcesses.h"
#include "WindowDraw.h"
#include "ImplementationAlgorithm.h"
#include <vector>
#include <mutex>
#include <atomic>

using namespace std;

#define FILEPOINTS "points.txt"
#define FILELINES "lines.txt"

mutex dataMutex;
atomic<bool> isRunning(true);
int ANIMATION_DELAY = 0;

void cleanWorkspace(vector<Point>& points, vector<Line>& lines) {
	lock_guard<mutex> lock(dataMutex);
	for (auto& l : lines) {
		l.setIsInPath(false);
	}
	for (auto& p : points) {
		if (p.getIsStartPoint() || p.getIsEndPoint()) continue;
		p.setColor(sf::Color(210, 210, 210), true);
	}
}

bool deletePointByName(const string& name, vector<Point>& points, vector<Line>& lines) {
	auto it = remove_if(points.begin(), points.end(),
		[&name](const Point& p) { return p.getName() == name; });
	if (it != points.end()) {
		points.erase(it, points.end());
		lines.erase(remove_if(lines.begin(), lines.end(),
			[&name](const Line& l) {
				return l.getStart().getName() == name || l.getEnd().getName() == name;
			}), lines.end());
		return true;
	}
	return false;
}

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
		cout << " - 13. Setings\n";
		cout << "-------------------\n";
        cout << "Enter command: ";

        int command;
		if (!(cin >> command)) {
			cin.clear(); // clear the error flag
			cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
			cout << "Invalid input. Please enter a number corresponding to a command." << endl;
			continue;
		}

        switch(command) {
			case 1: {
				lock_guard<mutex> lock(dataMutex);
				cout << "Chose file to load points:\n";
				cout << " - " << FILEPOINTS << endl;
				cout << "Enter filename: ";
				string filename;
				cin >> filename;
				if (loadPointsFromFile(filename, points, lines)) {
					cout << "Points loaded successfully from file." << endl;
				}
				else {
					cout << "Failed to load points from file." << endl;
				}
				break;
			}
			case 2: {
				lock_guard<mutex> lock(dataMutex);
				cout << "Chose file to load lines:\n";
				cout << " - " << FILELINES << endl;
				cout << "Enter filename: ";
				string filename;
				cin >> filename;
				if (loadLinesFromFile(filename, lines, points)) {
					cout << "Lines loaded successfully from file." << endl;
				}
				else {
					cout << "Failed to load lines from file." << endl;
				}
				break;
			}
            case 3: {
                lock_guard<mutex> lock(dataMutex);
				cleanWorkspace(points, lines);
				Point newPoint(0.0, 0.0, "");
				cout << "Enter point x y name: ";
				cin >> newPoint;
                if (validateNewPoint(newPoint, points)) {
                    points.push_back(newPoint);
					cout << "Point added successfully." << endl;
                }
                break;
            }
            case 4: {
				lock_guard<mutex> lock(dataMutex);
				cleanWorkspace(points, lines);
				double weight;
				cout << "Enter start point name, end point name and weight: ";
				string startName, endName;
				cin >> startName >> endName >> weight;
				Point* start = nullptr;
				Point* end = nullptr;
                if (!validateNewLine(startName, endName, weight, lines, points)) {
                    break;
				}
				for (auto& p : points) {
					if (p.getName() == startName) {
						start = &p;
					}
					if (p.getName() == endName) {
						end = &p;
					}
				}
				if (start && end) {
					Line newLine(*start, *end, weight);
					lines.push_back(newLine);
					cout << "Line added successfully." << endl;
				}
				else {
					cout << "Line Error: One or both points not found" << endl;
				}
				break;
			}
			case 5: {
				lock_guard<mutex> lock(dataMutex);
				cleanWorkspace(points, lines);
				cout << "Enter start point name: ";
				string startName;
				cin >> startName;
				for (auto& p : points) {
					p.setIsStartPoint(false);
				}
				for (auto& p : points) {
					if (p.getName() == startName) {
						p.setIsStartPoint(true);
						cout << "Start point flagged successfully." << endl;
						break;
					}
				}
				cout << "No such point found." << endl;
				break;
			}
			case 6: {
				lock_guard<mutex> lock(dataMutex);
				cleanWorkspace(points, lines);
				cout << "Enter end point name: ";
				string endName;
				cin >> endName;
				for (auto& p : points) {
					p.setIsEndPoint(false);
				}
				for (auto& p : points) {
					if (p.getName() == endName) {
						p.setIsEndPoint(true);
						cout << "End point flagged successfully." << endl;
						break;
					}
				}
				cout << "No such point found." << endl;
				break;
			}
			case 7: {
				lock_guard<mutex> lock(dataMutex);
				cout << "Name\tx\ty\n";
				cout << "-------------------\n";
				for (const auto& p : points) {
					cout << p << "\n";
				}
				break;
			}
			case 8: {
				lock_guard<mutex> lock(dataMutex);
				cout << "Start\t->\tEnd\tWeight\n";
				cout << "-------------------\n";
				for (const auto& l : lines) {
					cout << l << "\n";
				}
				break;
			}
			case 9: {
				lock_guard<mutex> lock(dataMutex);
				cleanWorkspace(points, lines);
				cout << "Enter point name to delete: ";
				string pointName;
				cin >> pointName;
				if (deletePointByName(pointName, points, lines)) cout << "Point and its connected lines deleted successfully." << endl;
				else cout << "No such point found." << endl;
				break;
			}
			case 10: {
				lock_guard<mutex> lock(dataMutex);
				cleanWorkspace(points, lines);
				cout << "Enter start point name and end point name of line to delete: ";
				string startName, endName;
				cin >> startName >> endName;
				if (deleteLineByPoints(startName, endName, lines), true) cout << "Line deleted successfully." << endl;
				else cout << "No such line found." << endl;
				break;
			}
			case 11: {

				cleanWorkspace(points, lines);

				cout << "Finding shortest path...\n";
				auto [s, p] = findShortestPath(points, lines);

				if (p == 0) {
					cout << "No path found" << endl;
					break;
				}
				cout << "Shortest path: " << s << endl;
				cout << "Shortest path found with total weight: " << p << endl;
				break;
			}

            case 0: {
                isRunning = false;
                cout << "Exiting console loop...\n";
                break;
            }
			case 13: {
				lock_guard<mutex> lock(dataMutex);
				cout << "Settings menu: \n";
				cout << " - 1. Toggle point labels display\n";
				cout << " - 2. Toggle edge weights display\n";
				cout << " - 3. Set animation delay (current: " << ANIMATION_DELAY << " ms)\n"; // Новий пункт
				cout << "Enter command: ";
				int settingCommand;
				cin >> settingCommand;
				switch (settingCommand) {
				case 1: {
					LABEL_SHOW = !LABEL_SHOW;
					cout << "Point labels display is now " << (LABEL_SHOW ? "ON" : "OFF") << ".\n";
					break;
				}
				case 2: {
					WEIGHT_SHOW = !WEIGHT_SHOW;
					cout << "Edge weights display is now " << (WEIGHT_SHOW ? "ON" : "OFF") << ".\n";
					break;
				}
				case 3: { // Нова логіка
					cout << "Enter delay in milliseconds (e.g. 0 for instant, 500 for slow): ";
					cin >> ANIMATION_DELAY;
					if (ANIMATION_DELAY < 0) ANIMATION_DELAY = 0;
					cout << "Animation delay set to " << ANIMATION_DELAY << " ms.\n";
					break;
				}
				default:
					cout << "Invalid settings command.\n";
				}
				break;
			}
            default:

				cout << "Invalid command. Please try again.\n";
				break;
        }
      
    }
    cout << "Console thread finished.\n";
}
