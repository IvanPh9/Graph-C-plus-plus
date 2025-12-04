#include "ImplementationAlgorithm.h"
#include "Interface.h"
#include <queue>
#include <limits>

using namespace std;

// -----------------------------------------------------------
// Sleep function used to slow down animation
// -----------------------------------------------------------
void visualizationSleep() {
    if (ANIMATION_DELAY > 0) {
        this_thread::sleep_for(chrono::milliseconds(ANIMATION_DELAY));
    }
}

// -----------------------------------------------------------
// Custom comparator for priority queue
// Implements a min-heap based on distance (second value)
// -----------------------------------------------------------
struct CompareDist {
    bool operator()(const pair<int, double>& a, const pair<int, double>& b) const {
        return a.second > b.second;  // min-heap
    }
};

// -----------------------------------------------------------
// Main Dijkstra shortest path implementation with visualization
// Returns (pathString, totalPathWeight)
// -----------------------------------------------------------
pair<string, double> findShortestPath(const vector<Point>& points, const vector<Line>& lines)
{
    double path = 0;
    string pathStr;
    int startIndex = -1, endIndex = -1;

    // -------------------------------------
    // Locate start and end points
    // -------------------------------------
    {
        lock_guard<mutex> lock(dataMutex);
        for (size_t i = 0; i < points.size(); i++) {
            if (points[i].getIsStartPoint()) startIndex = i;
            if (points[i].getIsEndPoint())   endIndex = i;
        }
    }

    if (startIndex == -1 || endIndex == -1) {
        cout << "Algorithm Error: Start or End point not defined\n";
        return make_pair(string(), 0.0);
    }

    // -------------------------------------
    // Initialize Dijkstra data structures
    // -------------------------------------
    vector<double> dist(points.size(), numeric_limits<double>::infinity());
    vector<int> prev(points.size(), -1);

    priority_queue<pair<int, double>, vector<pair<int, double>>, CompareDist> pq;

    dist[startIndex] = 0.0;
    pq.push({ startIndex, 0.0 });

    // -------------------------------------
    // Dijkstra main loop
    // -------------------------------------
    while (!pq.empty()) {
        auto [u, d] = pq.top();
        pq.pop();

        // If reached the destination — stop early
        if (points[u].getIsEndPoint()) break;
        if (d > dist[u]) continue;  // Skip outdated values

        // Animate current active point
        if (ANIMATION_DELAY > 0) {
            lock_guard<mutex> lock(dataMutex);
            const_cast<Point&>(points[u]).setColor(ACTIVE_COLOR_POINT, true);
        }
        visualizationSleep();

        // -------------------------------------
        // Iterate over all edges to find neighbors
        // -------------------------------------
        for (size_t i = 0; i < lines.size(); i++) {
            const Line& line = lines[i];
            size_t v = -1;

            {
                lock_guard<mutex> lock(dataMutex);
                if (line.getStart() == points[u])
                    v = find(points.begin(), points.end(), line.getEnd()) - points.begin();
                else if (line.getEnd() == points[u])
                    v = find(points.begin(), points.end(), line.getStart()) - points.begin();
            }

            if (v >= points.size()) continue;

            // Highlight current edge being tested
            if (ANIMATION_DELAY > 0) {
                lock_guard<mutex> lock(dataMutex);
                const_cast<Line&>(line).setColor(ACTIVE_COLOR_LINE, true);
                const_cast<Line&>(line).setBoldness(ACTIVE_BOLDNESS_LINE, true);
            }
            visualizationSleep();

            // Relaxation step
            double alt = dist[u] + line.getWeight();

            if (alt < dist[v]) {
                // Found a better path
                dist[v] = alt;
                prev[v] = u;
                pq.push({ v, alt });

                // Accepted edge animation
                if (ANIMATION_DELAY > 0) {
                    lock_guard<mutex> lock(dataMutex);
                    const_cast<Line&>(line).setColor(ACCEPT_COLOR_LINE, true);
                    const_cast<Line&>(line).setBoldness(ACTIVE_BOLDNESS_LINE, true);
                    const_cast<Point&>(points[v]).setColor(ANALYSE_COLOR_POINT, true);
                }
            }
            else {
                // Rejected edge animation
                if (ANIMATION_DELAY > 0) {
                    lock_guard<mutex> lock(dataMutex);
                    const_cast<Line&>(line).setColor(REJECTED_COLOR_LINE, true);
                    const_cast<Line&>(line).setBoldness(ACTIVE_BOLDNESS_LINE, true);
                }
            }

            visualizationSleep();

            // -------------------------------------
            // Reset and redraw all edges and points
            // to visualize current shortest-path tree
            // -------------------------------------
            if (ANIMATION_DELAY > 0) {
                lock_guard<mutex> lock(dataMutex);

                // Reset all edges to base color
                for (const auto& l : lines) {
                    const_cast<Line&>(l).setColor(BASE_COLOR_LINE, true);
                    const_cast<Line&>(l).setBoldness(BASE_BOLDNESS_LINE, true);
                }

                // Rebuild partial shortest-path tree visualization
                for (size_t j = 0; j < points.size(); j++) {

                    // Color visited and non-visited nodes
                    if (dist[j] != numeric_limits<double>::infinity() && j != startIndex && j != endIndex)
                        const_cast<Point&>(points[j]).setColor(POSSIBLE_SOLUTION_COLOR_POINT, true);
                    else if (j != startIndex && j != endIndex)
                        const_cast<Point&>(points[j]).setColor(BASE_COLOR_POINT, true);
                    else if (j == startIndex)
                        const_cast<Point&>(points[j]).setColor(START_COLOR_POINT, true);
                    else if (j == endIndex)
                        const_cast<Point&>(points[j]).setColor(END_COLOR_POINT, true);

                    // Draw blue edges representing the current predecessor tree
                    if (prev[j] != -1) {
                        for (const Line& l : lines) {
                            if ((l.getStart() == points[j] && l.getEnd() == points[prev[j]]) ||
                                (l.getEnd() == points[j] && l.getStart() == points[prev[j]])) {

                                const_cast<Line&>(l).setColor(POSSIBLE_SOLUTION_COLOR_LINE, true);
                                const_cast<Line&>(l).setBoldness(ACTIVE_BOLDNESS_LINE, true);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    // -----------------------------------------------------------
    // Final path reconstruction (with animation)
    // -----------------------------------------------------------
    {
        lock_guard<mutex> lock(dataMutex);
        for (size_t v = endIndex; v != -1 && prev[v] != -1; v = prev[v]) {

            // Build path string backwards
            pathStr = points[v].getName() + (pathStr.empty() ? "" : "->") + pathStr;

            // Find and mark edges belonging to final shortest path
            for (const Line& line : lines) {
                if ((line.getStart() == points[v] && line.getEnd() == points[prev[v]]) ||
                    (line.getEnd() == points[v] && line.getStart() == points[prev[v]])) {

                    path += line.getWeight();
                    const_cast<Line&>(line).setIsInPath(true);

                    if (ANIMATION_DELAY > 0) {
                        const_cast<Line&>(line).setColor(IS_PATH_COLOR_LINE, true);
                        const_cast<Line&>(line).setBoldness(IS_PATH_BOLDNESS_LINE, true);
                        visualizationSleep();
                    }

                    break;
                }
            }

            if (ANIMATION_DELAY > 0) {
                const_cast<Point&>(points[v]).setColor(SOLUTION_COLOR_POINT, true);
                visualizationSleep();
            }
        }

        // Add start point to the string
        if (startIndex != -1)
            pathStr = points[startIndex].getName() + (pathStr.empty() ? "" : "->") + pathStr;

        // Highlight start and end
        if (ANIMATION_DELAY > 0) {
            const_cast<Point&>(points[startIndex]).setColor(START_COLOR_POINT, true);
            const_cast<Point&>(points[endIndex]).setColor(END_COLOR_POINT, true);
        }
    }

    return make_pair(pathStr, path);
}
