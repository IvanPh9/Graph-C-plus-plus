#include "WindowDraw.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>

#define M_PI 3.14159265358979323846

// Constants to control visual scaling and margins
const double MAX_SCALE_ELEMENTS = 3.0;
const double paddingFactor = 0.9; // Keeps the drawing 90% of the window size (10% margin)

// Global font resources
sf::Font globalFont;
bool fontLoaded = false; // Lazy loading flag to ensure font is loaded only once

// Visual offsets to prevent labels from overlapping the points
float LABEL_OFFSET_X = 5.0f;
float LABEL_OFFSET_Y = -5.0f;

// Base font sizes
const unsigned int FONT_SIZE = 24;
const unsigned int WEIGHT_FONT_SIZE = 20;

// Toggles for rendering optional elements
bool LABEL_SHOW = true;
bool WEIGHT_SHOW = true;

// Structure to hold calculated scaling parameters
// This ensures coordinates map correctly from "Data Space" to "Screen Space"
struct ScaleInfo {
    double scaleX;       // Horizontal scaling factor
    double scaleY;       // Vertical scaling factor
    double minX;         // Minimum X in data
    double minY;         // Minimum Y in data
    double centerX;      // Center X of the data cloud
    double centerY;      // Center Y of the data cloud
    double visualScale;  // A factor to resize visual elements (circles/lines) based on window size
};

//------------------------------------------------------------------------
// Calculates how to fit the given points into the current window.
// Determines the bounding box of the data and calculates scale factors
// so that the graph fills the screen while maintaining a margin.
//------------------------------------------------------------------------
ScaleInfo getScaleAndBounds(const std::vector<Point>& points, double windowSizeX, double windowSizeY) {
    // Edge case: No points to draw
    if (points.empty()) {
        return { 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
    }

    double minX = points[0].getX(), maxX = points[0].getX();
    double minY = points[0].getY(), maxY = points[0].getY();

    // Iterate through all points to find the bounding box (min/max coordinates)
    for (const auto& p : points) {
        if (p.getX() < minX) minX = p.getX();
        if (p.getX() > maxX) maxX = p.getX();
        if (p.getY() < minY) minY = p.getY();
        if (p.getY() > maxY) maxY = p.getY();
    }

    double rangeX = maxX - minX;
    double rangeY = maxY - minY;

    // Calculate the geometric center of the data set
    double centerX = (minX + maxX) / 2.0;
    double centerY = (minY + maxY) / 2.0;

    // Handle degenerate cases (e.g., all points are the same, or form a straight horizontal/vertical line)
    // to avoid division by zero later.
    if (rangeX < 1e-6 && rangeY < 1e-6) {
        rangeX = 1;
        rangeY = 1;
    }
    else if (rangeX < 1e-6 || rangeY < 1e-6) {
        rangeX = std::max(rangeX, 1.);
        rangeY = std::max(rangeY, 1.);
    }

    // Ensure ranges are at least 1.0 to prevent scaling issues
    rangeX = std::max(rangeX, 1.);
    rangeY = std::max(rangeY, 1.);

    // Calculate scale factors: (Half Window Size / Half Data Range) * Padding
    double scaleX = ((windowSizeX / 2.0) * paddingFactor) / (rangeX / 2.0);
    double scaleY = ((windowSizeY / 2.0) * paddingFactor) / (rangeY / 2.0);

    // Aspect Ratio clamping: prevents the graph from looking extremely stretched
    // if the data is very tall and thin or short and wide.
    if (scaleX > scaleY * 2) scaleX = scaleY * 2;
    else if (scaleY > scaleX * 2) scaleY = scaleX * 2;

    // Calculate a "visual scale" based on window dimensions
    // This helps size circles and text relative to the overall window size.
    double windowFactor = std::min(windowSizeX, windowSizeY) / 100.0;
    double visualScale = 0.2 * windowFactor;

    return { scaleX, scaleY, minX, minY, centerX, centerY, visualScale };
}

//------------------------------------------------------------------------
// Converts a point from "Data Space" (arbitrary coordinates) to "Screen Space" (pixels).
//
// SFML coordinate system has (0,0) at the Top-Left.
// Mathematical coordinate systems usually have Y increasing upwards.
// We must invert the Y-axis here.
//------------------------------------------------------------------------
sf::Vector2f transformPoint(const Point& p, const ScaleInfo& info, double scaleX, double scaleY) {
    // Center the point relative to the data's center
    double centeredX = p.getX() - info.centerX;
    double centeredY = p.getY() - info.centerY;

    // Scale the coordinates to pixels
    // We start drawing from the center of the window

    // Assuming standard logic:
    float x = static_cast<float>(centeredX * scaleX);
    float y = static_cast<float>(-centeredY * scaleY); // Negative Y flips the axis so +Y goes Up

    return sf::Vector2f(x, y);
}

// Renders the vertices (nodes) of the graph
void drawPoints(const std::vector<Point>& points, sf::RenderWindow& window) {
    if (points.empty()) return;

    double width = static_cast<double>(window.getSize().x);
    double height = static_cast<double>(window.getSize().y);

    // Get the scaling factors based on current window size
    ScaleInfo scaleData = getScaleAndBounds(points, width, height);
    double sharedScaleX = scaleData.scaleX;
    double sharedScaleY = scaleData.scaleY;
    double visualScale = scaleData.visualScale * 0.7; // Slightly reduce point size

    // We must manually shift coordinates to the center of the screen
    // because transformPoint returns coords relative to (0,0).
    sf::Vector2f screenCenter(width / 2.0f, height / 2.0f);

    for (const auto& p : points) {
        // Calculate radius based on visual scale, clamping to min/max reasonable sizes
        float calculatedRadius = static_cast<float>(p.getSize() * visualScale);
        float radius = std::max(std::min(calculatedRadius, static_cast<float>(p.getSize())), 3.0f);

        sf::CircleShape shape(radius);

        // Calculate outline thickness
        float calculatedOutlineOffset = static_cast<float>(p.getOutlineSize() * visualScale);
        float outlineOffset = std::max(std::min(calculatedOutlineOffset, static_cast<float>(p.getOutlineSize())), 1.0f);
        shape.setOutlineThickness(outlineOffset);

        shape.setFillColor(p.getColor());
        shape.setOutlineColor(p.getOutlineColor());

        // Get relative position and shift to screen center
        sf::Vector2f pos = transformPoint(p, scaleData, sharedScaleX, sharedScaleY);
        pos += screenCenter;

        // Offset by radius so the point is centered exactly on the coordinate
        shape.setPosition(sf::Vector2f(pos.x - shape.getRadius(), pos.y - shape.getRadius()));

        window.draw(shape);
    }
}

// Renders the edges (connections) between nodes
void drawLines(const std::vector<Line>& lines, const std::vector<Point>& points, sf::RenderWindow& window) {
    if (points.empty() || lines.empty()) return;

    double width = static_cast<double>(window.getSize().x);
    double height = static_cast<double>(window.getSize().y);
    sf::Vector2f screenCenter(width / 2.0f, height / 2.0f);

    ScaleInfo scaleData = getScaleAndBounds(points, width, height);
    double sharedScaleX = scaleData.scaleX;
    double sharedScaleY = scaleData.scaleY;
    double visualScale = scaleData.visualScale;

    for (const auto& line : lines) {
        // Calculate start and end positions relative to center
        sf::Vector2f startPos = transformPoint(line.getStart(), scaleData, sharedScaleX, sharedScaleY);
        sf::Vector2f endPos = transformPoint(line.getEnd(), scaleData, sharedScaleX, sharedScaleY);

        // Shift to absolute screen coordinates
        startPos += screenCenter;
        endPos += screenCenter;

        // Calculate the vector from Start to End
        sf::Vector2f direction = endPos - startPos;
        // Calculate length (Pythagorean theorem)
        float lineLength = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        // Determine line thickness (boldness)
        float calculatedBoldness = static_cast<float>(line.getBoldness() * visualScale);
        float boldness = std::max(std::min(calculatedBoldness, static_cast<float>(line.getBoldness())), 1.0f);

        // We use a RectangleShape instead of SFML lines because Rectangles support thickness/rotation better
        sf::RectangleShape rect(sf::Vector2f(lineLength, boldness));
        rect.setFillColor(line.getColor());
        rect.setPosition(startPos);

        // Compute rotation angle using atan2 (arc tangent)
        // atan2 returns radians, we convert to degrees for SFML
        sf::Angle angle = sf::degrees(std::atan2(direction.y, direction.x) * 180.f / static_cast<float>(M_PI));
        rect.setRotation(angle);

        // Set the origin (pivot point) to the vertical center of the line's start
        // This ensures the line is drawn centered on the mathematical coordinates
        rect.setOrigin(sf::Vector2f(0.0f, line.getBoldness() / 2.0f));

        window.draw(rect);
    }
}

// Renders text names next to the points
void drawLabels(const std::vector<Point>& points, sf::RenderWindow& window) {
    if (points.empty()) return;

    // Load font only once (Lazy Loading)
    if (!fontLoaded) {
        if (!globalFont.openFromFile("arial.ttf")) {
            std::cerr << "Error loading font! The labels will not be displayed." << std::endl;
            return;
        }
        fontLoaded = true;
    }

    double width = static_cast<double>(window.getSize().x);
    double height = static_cast<double>(window.getSize().y);
    sf::Vector2f screenCenter(width / 2.0f, height / 2.0f);

    ScaleInfo scaleData = getScaleAndBounds(points, width, height);
    double sharedScaleX = scaleData.scaleX;
    double sharedScaleY = scaleData.scaleY;
    double visualScale = scaleData.visualScale;

    for (const auto& p : points) {
        if (p.getName().empty()) continue;

        sf::Vector2f pos = transformPoint(p, scaleData, sharedScaleX, sharedScaleY);
        pos += screenCenter;

        sf::Text labelText(globalFont);

        // Scale font size dynamically but clamp it so it doesn't get too small or too huge
        float size = std::max(std::min(static_cast<float>(FONT_SIZE * visualScale), static_cast<float>(FONT_SIZE)), FONT_SIZE / 3.0f);
        labelText.setString(p.getName());
        labelText.setCharacterSize(size);
        labelText.setFillColor(sf::Color::Black);

        // Calculate offset factor to distance text from the point center
        float offsetFactor = static_cast<float>(visualScale / 2.5);
        offsetFactor = std::min(offsetFactor, static_cast<float>(MAX_SCALE_ELEMENTS));

        labelText.setPosition(sf::Vector2f(pos.x + (LABEL_OFFSET_X * offsetFactor), pos.y - (LABEL_OFFSET_Y * offsetFactor)));

        window.draw(labelText);
    }
}

// Utility: Formats a double to a string with max 1 decimal place
std::string formatWeightString(double weight) {
    // Round to 1 decimal place
    double roundedWeight = std::round(weight * 10.0) / 10.0;
    std::string s = std::to_string(roundedWeight);

    // Trim trailing zeros after the decimal point
    size_t decimal_pos = s.find('.');
    if (decimal_pos != std::string::npos) {
        s = s.substr(0, decimal_pos + 2); // Keep only one digit after dot
    }

    return s;
}

// Renders numerical weights at the center of edges
void drawEdgeWeights(const std::vector<Line>& lines, const std::vector<Point>& points, sf::RenderWindow& window) {
    if (points.empty() || lines.empty()) return;

    if (!fontLoaded) {
        if (!globalFont.openFromFile("arial.ttf")) {
            std::cerr << "Error loading font for edge weights! The weights will not be displayed." << std::endl;
            return;
        }
        fontLoaded = true;
    }

    double width = static_cast<double>(window.getSize().x);
    double height = static_cast<double>(window.getSize().y);
    sf::Vector2f screenCenter(width / 2.0f, height / 2.0f);

    ScaleInfo scaleData = getScaleAndBounds(points, width, height);
    double sharedScaleX = scaleData.scaleX;
    double sharedScaleY = scaleData.scaleY;
    double visualScale = scaleData.visualScale;

    for (const auto& line : lines) {
        // Calculate start and end positions
        sf::Vector2f startPos = transformPoint(line.getStart(), scaleData, sharedScaleX, sharedScaleY);
        sf::Vector2f endPos = transformPoint(line.getEnd(), scaleData, sharedScaleX, sharedScaleY);

        startPos += screenCenter;
        endPos += screenCenter;

        // Calculate the midpoint of the line to place the text
        sf::Vector2f centerPos;
        centerPos.x = (startPos.x + endPos.x) / 2.0f;
        centerPos.y = (startPos.y + endPos.y) / 2.0f;

        sf::Text weightText(globalFont);

        // Determine dynamic font size
        float size = std::max(std::min(static_cast<float>(WEIGHT_FONT_SIZE * visualScale), static_cast<float>(WEIGHT_FONT_SIZE)), WEIGHT_FONT_SIZE / 3.0f);

        std::string s = formatWeightString(line.getWeight());
        weightText.setString(s);
        weightText.setCharacterSize(size);
        weightText.setFillColor(sf::Color::Black);
        weightText.setStyle(sf::Text::Italic);

        // Note: For perfect centering, we should ideally subtract half the text bounds width/height here
        weightText.setPosition(centerPos);

        window.draw(weightText);
    }
}