#include "ImplementationAlgorithm.h"
#include <queue>
#include <limits>

struct CompareDist {
    bool operator()(const std::pair<int, double>& a, const std::pair<int, double>& b) const {
        return a.second > b.second;  // мін-heap
    }
};

double findShortestPath(const std::vector<Point>& points, const std::vector<Line>& lines)
{
    double path = 0;

    int startIndex = -1, endIndex = -1;

    // Знаходимо індекси старту і фінішу
    for (size_t i = 0; i < points.size(); i++) {
        if (points[i].getIsStartPoint()) startIndex = i;
        if (points[i].getIsEndPoint()) endIndex = i;
    }

    if (startIndex == -1 || endIndex == -1) {
        std::cout << "Algorithm Error: Start or End point not defined\n";
        return path;
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

        // пропускаємо застарілий запис
        if (d > dist[u]) continue;

        // перебираємо тільки суміжні ребра
        for (size_t i = 0; i < lines.size(); i++) {
            const Line& line = lines[i];
            size_t v = -1;

            if (line.getStart() == points[u])
                v = std::find(points.begin(), points.end(), line.getEnd()) - points.begin();
            else if (line.getEnd() == points[u])
                v = std::find(points.begin(), points.end(), line.getStart()) - points.begin();

            if (v >= 0 && v < points.size()) {
                double alt = dist[u] + line.getWeight();
                if (alt < dist[v]) {
                    dist[v] = alt;
                    prev[v] = u;
                    pq.push({ v, alt });
                }
            }
        }
    }

    // реконструюємо шлях
    for (size_t v = endIndex; v != -1 && prev[v] != -1; v = prev[v]) {
        for (const Line& line : lines) {
            if ((line.getStart() == points[v] && line.getEnd() == points[prev[v]]) ||
                (line.getEnd() == points[v] && line.getStart() == points[prev[v]]))
            {
				path += line.getWeight();
				const_cast<Line&>(line).setIsInPath(true);
                break;
            }
        }
    }

    return path;
}
