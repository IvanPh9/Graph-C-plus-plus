#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp> 
#include "Point.h"
#include "Line.h"
#include "FileProcesses.h"
#include "WindowDraw.h"
#include "Interface.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <thread> // Для створення потоків
#include <mutex>  // Для синхронізації даних
#include <atomic> // Для безпечного прапорця завершення


using namespace std;

int main()
{
    vector<Line> lines;
    vector<Point> points;


    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "SFML Multithreading");

    sf::View view;
    view.setCenter(sf::Vector2f(0.f, 0.f));
    view.setSize(sf::Vector2f(800.f, 600.f));
    window.setView(view);

    thread consoleThread(MainInterface, ref(points), ref(lines));

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
			if (LABEL_SHOW) drawLabels(points, window);
			if (WEIGHT_SHOW) drawEdgeWeights(lines, points, window);
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