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
#include <thread>   // For creating threads
#include <mutex>    // For synchronizing shared data
#include <atomic>   // For thread-safe running flag


using namespace std;

int main()
{
    vector<Line> lines;
    vector<Point> points;

    // Create main application window
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "SFML Multithreading");

    // Initial view setup
    sf::View view;
    view.setCenter(sf::Vector2f(0.f, 0.f));
    view.setSize(sf::Vector2f(800.f, 600.f));
    window.setView(view);

    // Start console interface in a separate thread
    thread consoleThread(MainInterface, ref(points), ref(lines));

    // Main SFML loop
    while (window.isOpen())
    {
        // Handle window events
        while (const std::optional event = window.pollEvent())
        {
            // Window close event
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            // Window resize event
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                float width = static_cast<float>(resized->size.x);
                float height = static_cast<float>(resized->size.y);

                // Update view to match new window size
                view.setSize(sf::Vector2f(width, height));
                window.setView(view);
            }
        }

        // Clear window with white background
        window.clear(sf::Color::White);

        {
            // Synchronize access to shared vectors
            lock_guard<mutex> lock(dataMutex);

            // Draw lines, points, labels and weights
            drawLines(lines, points, window);
            drawPoints(points, window);
            if (LABEL_SHOW) drawLabels(points, window);
            if (WEIGHT_SHOW) drawEdgeWeights(lines, points, window);
        }

        // Display updated frame
        window.display();

        // Reduce CPU usage
        sf::sleep(sf::milliseconds(10));
    }

    // Signal console thread to stop
    isRunning = false;

    // Detach the console thread if still active
    if (consoleThread.joinable()) {
        consoleThread.detach();
    }

    return 0;
}
