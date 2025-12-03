#include "ImplementationAlgorithm.h"
#include "Interface.h"
#include <queue>
#include <limits>

void visualizationSleep() {
    if (ANIMATION_DELAY > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ANIMATION_DELAY));
    }
}

struct CompareDist {
    bool operator()(const std::pair<int, double>& a, const std::pair<int, double>& b) const {
        return a.second > b.second;  // мін-heap
    }
};

std::pair<std::string, double> findShortestPath(const std::vector<Point>& points, const std::vector<Line>& lines)
{
    double path = 0;
    std::string pathStr;
    int startIndex = -1, endIndex = -1;

    {
        std::lock_guard<std::mutex> lock(dataMutex);
        for (size_t i = 0; i < points.size(); i++) {
            if (points[i].getIsStartPoint()) startIndex = i;
            if (points[i].getIsEndPoint()) endIndex = i;
        }
    }

    if (startIndex == -1 || endIndex == -1) {
        std::cout << "Algorithm Error: Start or End point not defined\n";
        return std::make_pair(std::string(), 0.0);
    }

    // Dijkstra
    std::vector<double> dist(points.size(), std::numeric_limits<double>::infinity());
    std::vector<int> prev(points.size(), -1);

    std::priority_queue<
        std::pair<int, double>,
        std::vector<std::pair<int, double>>,
        CompareDist
    > pq;

    dist[startIndex] = 0.0;
    pq.push({ startIndex, 0.0 });

    while (!pq.empty()) {
        auto [u, d] = pq.top();
        pq.pop();

        if (points[u].getIsEndPoint()) break;
        if (d > dist[u]) continue;

        if (ANIMATION_DELAY > 0) {
            std::lock_guard<std::mutex> lock(dataMutex);
            const_cast<Point&>(points[u]).setColor(sf::Color(255, 200, 0), true); // оранжевий = активний
        }
        visualizationSleep();

        for (size_t i = 0; i < lines.size(); i++) {
            const Line& line = lines[i];
            size_t v = -1;

            {
                std::lock_guard<std::mutex> lock(dataMutex);
                if (line.getStart() == points[u])
                    v = std::find(points.begin(), points.end(), line.getEnd()) - points.begin();
                else if (line.getEnd() == points[u])
                    v = std::find(points.begin(), points.end(), line.getStart()) - points.begin();
            }

            if (v >= points.size()) continue;

            if (ANIMATION_DELAY > 0) {
                std::lock_guard<std::mutex> lock(dataMutex);
                const_cast<Line&>(line).setColor(sf::Color(100, 150, 255), true); // блакитний
                const_cast<Line&>(line).setBoldness(3.0, true);
            }
            visualizationSleep();

            double alt = dist[u] + line.getWeight();

            if (alt < dist[v]) {
                dist[v] = alt;
                prev[v] = u;
                pq.push({ v, alt });

                if (ANIMATION_DELAY > 0) {
                    std::lock_guard<std::mutex> lock(dataMutex);
                    const_cast<Line&>(line).setColor(sf::Color(80, 220, 120), true); // зелений
                    const_cast<Line&>(line).setBoldness(4.0, true);
                    const_cast<Point&>(points[v]).setColor(sf::Color(120, 255, 150), true);
                }
            }
            else {
                if (ANIMATION_DELAY > 0) {
                    std::lock_guard<std::mutex> lock(dataMutex);
                    const_cast<Line&>(line).setColor(sf::Color(255, 100, 100), true); // червоний
                    const_cast<Line&>(line).setBoldness(3.0, true);
                }
            }
            
            visualizationSleep();


            if (ANIMATION_DELAY > 0) {
             
                std::lock_guard<std::mutex> lock(dataMutex);

                // Скидаємо ВСІ лінії у сірий колір (очистка перед малюванням дерева)
                // Використовуємо іншу назву змінної 'l', щоб не переплутати з 'line' з зовнішнього циклу
                for (const auto& l : lines) {
                    const_cast<Line&>(l).setColor(sf::Color(180, 180, 180), true);
                    const_cast<Line&>(l).setBoldness(2.0, true);
                }

                // Оновлюємо кольори точок та малюємо сині лінії (дерево найкоротших шляхів)
                for (size_t j = 0; j < points.size(); j++) {
                   
                    // Фарбування відвіданих точок у жовтий
                    if (dist[j] != std::numeric_limits<double>::infinity() && j != startIndex && j != endIndex) {
                        const_cast<Point&>(points[j]).setColor(sf::Color::Yellow, true);
                    }
                    else if (j != startIndex && j != endIndex) {
                        const_cast<Point&>(points[j]).setColor(sf::Color(180, 180, 180), true);
                    }

                    // Відновлення синіх ліній (шляхів до попередників)
                    if (prev[j] != -1) {
                        // Шукаємо лінію між поточною точкою j та її попередником prev[j]
                        for (const Line& l : lines) {
                            if ((l.getStart() == points[j] && l.getEnd() == points[prev[j]]) ||
                                (l.getEnd() == points[j] && l.getStart() == points[prev[j]]))
                            {
                                const_cast<Line&>(l).setColor(sf::Color::Blue, true);
                                const_cast<Line&>(l).setBoldness(3.0, true);
                                break; // Лінію знайдено, переходимо до наступної точки
                            }
                        }
                    }
                }
            }
        }
    }

    // Реконструкція шляху (анімаційна)
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        for (size_t v = endIndex; v != -1 && prev[v] != -1; v = prev[v]) {

            pathStr = points[v].getName() + (pathStr.empty() ? "" : "->") + pathStr;

            for (const Line& line : lines) {   
                if ((line.getStart() == points[v] && line.getEnd() == points[prev[v]]) ||
                    (line.getEnd() == points[v] && line.getStart() == points[prev[v]])) {

                    path += line.getWeight();
                    const_cast<Line&>(line).setIsInPath(true);

                    if (ANIMATION_DELAY > 0) {
                        const_cast<Line&>(line).setColor(sf::Color::Red, true);
                        const_cast<Line&>(line).setBoldness(5.0, true);
                        visualizationSleep();
                    }

                    break;
                }
            }

            if (ANIMATION_DELAY > 0) {
                const_cast<Point&>(points[v]).setColor(sf::Color::Cyan, true);
                visualizationSleep();
            }
        }

        if (startIndex != -1) {
            pathStr = points[startIndex].getName() + (pathStr.empty() ? "" : "->") + pathStr;
        }

        if (ANIMATION_DELAY > 0) {
            const_cast<Point&>(points[startIndex]).setColor(sf::Color::Green, true);
            const_cast<Point&>(points[endIndex]).setColor(sf::Color::Green, true);
        }
    }

    return std::make_pair(pathStr, path);
}
