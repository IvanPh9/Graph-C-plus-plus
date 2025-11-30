#include "Interface.h"
#include "ValidationAdd.h"
#include "FileProcesses.h"
#include "WindowDraw.h"
#include <vector>
#include <mutex>
#include <atomic>

using namespace std;

#define FILEPOINTS "points.txt"
#define FILELINES "lines.txt"

mutex dataMutex;
atomic<bool> isRunning(true);

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
		cout << " - 5.  Update point\n";
		cout << " - 6.  Update line\n";
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
        cin >> command;
        switch(command) {
			case 1: {
				lock_guard<mutex> lock(dataMutex);
				cout << "Chose file to load points:\n";
				cout << " - " << FILEPOINTS << endl;
				cout << "Enter filename: ";
				string filename;
				cin >> filename;
				if (loadPointsFromFile(filename, points)) {
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
					default:
						cout << "Invalid settings command. Please try again.\n";
				}
				break;
			}
            default:
				cout << "Invalid command. Please try again.\n";
        }
      
    }
    cout << "Console thread finished.\n";
}
