#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp> 
#include "Point.h"
#include "Line.h"
#include "FileProcesses.h"
#include "WindowDraw.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <thread> // Для створення потоків
#include <mutex>  // Для синхронізації даних
#include <atomic> // Для безпечного прапорця завершення

#define FILEPOINTS "points.txt"
#define FILELINES "lines.txt"

using namespace std;

mutex dataMutex;
atomic<bool> isRunning(true);

void consoleProcess(vector<Point>& points, vector<Line>& lines) {
    while (isRunning) {
        cout << "\n--- Menu ---\n";
        cout << "1. Show current points info\n";
        cout << "2. Add dummy point (Example dynamic update)\n";
        cout << "3. Exit console loop\n";
        cout << "Enter command: ";

        int command;
        if (cin >> command) {
            if (command == 1) {

                lock_guard<mutex> lock(dataMutex);
                cout << "\nCurrent Points:\n";
                for (const auto& p : points) {
                    cout << p << endl;
                }
            }
            else if (command == 2) {

                double x, y;
                string name;
                cout << "Enter X Y Name: ";
                cin >> x >> y >> name;

                lock_guard<mutex> lock(dataMutex);
                points.emplace_back(x, y, name);
                cout << "Point added! Check the window.\n";
            }
            else if (command == 3) {
                break;
            }
        }
        else {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    cout << "Console thread finished.\n";
}

int main()
{
    vector<Line> lines;
    vector<Point> points;

    loadPointsFromFile(FILEPOINTS, points);
    loadLinesFromFile(FILELINES, lines, points);

    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "SFML Multithreading");

    sf::View view;
    view.setCenter(sf::Vector2f(0.f, 0.f));
    view.setSize(sf::Vector2f(800.f, 600.f));
    window.setView(view);

    thread consoleThread(consoleProcess, ref(points), ref(lines));

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                float width = static_cast<float>(resized->size.x);
                float height = static_cast<float>(resized->size.y);
                view.setSize(sf::Vector2f(width, height));
                window.setView(view);
            }
        }

        window.clear(sf::Color::White);

        {

            lock_guard<mutex> lock(dataMutex);

            drawLines(lines, points, window);
            drawPoints(points, window);
        } 

        window.display();

        sf::sleep(sf::milliseconds(10));
    }

    isRunning = false;

    if (consoleThread.joinable()) {
        consoleThread.detach();
    }

    return 0;
}