#include "WindowDraw.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>

const double MAX_SCALE_ELEMENTS = 3.0;
const double MIN_RANGE = 15;
const float MAX_POINT_RADIUS = 15.0f;
const float MIN_POINT_RADIUS = 3.0f;
const double paddingFactor = 0.9;

sf::Font globalFont;
bool fontLoaded = false;

float LABEL_OFFSET_X = 5.0f;
float LABEL_OFFSET_Y = -5.0f;

const unsigned int FONT_SIZE = 12;
const unsigned int WEIGHT_FONT_SIZE = 10;

bool LABEL_SHOW = true;
bool WEIGHT_SHOW = true;

struct ScaleInfo {
    double scaleX;
	double scaleY;
    double minX;
    double minY;
    double centerX;
    double centerY;
    double visualScale;
};

ScaleInfo getScaleAndBounds(const std::vector<Point>& points, double windowSizeX, double windowSizeY) {


    // Якщо points порожній, повертаємо коректно ініціалізовану ScaleInfo (7 полів)
    if (points.empty()) {
        return { 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
    }

    double minX = points[0].getX(), maxX = points[0].getX();
    double minY = points[0].getY(), maxY = points[0].getY();

    for (const auto& p : points) {
        if (p.getX() < minX) minX = p.getX();
        if (p.getX() > maxX) maxX = p.getX();
        if (p.getY() < minY) minY = p.getY();
        if (p.getY() > maxY) maxY = p.getY();
    }

    double rangeX = maxX - minX;
    double rangeY = maxY - minY;

    // Центр даних
    double centerX = (minX + maxX) / 2.0;
    double centerY = (minY + maxY) / 2.0;


    if (rangeX < 1e-6 && rangeY < 1e-6) {
        rangeX = MIN_RANGE;
        rangeY = MIN_RANGE;
    }
    else if (rangeX < 1e-6 || rangeY < 1e-6) {

        rangeX = std::max(rangeX, MIN_RANGE);
        rangeY = std::max(rangeY, MIN_RANGE);
    }

    rangeX = std::max(rangeX, MIN_RANGE);
    rangeY = std::max(rangeY, MIN_RANGE);

    double targetViewSize = std::min(windowSizeX, windowSizeY) / 2.0;

    double scaleX = ((windowSizeX / 2.0) * paddingFactor) / (rangeX / 2.0);
    double scaleY = ((windowSizeY / 2.0) * paddingFactor) / (rangeY / 2.0);

    if (scaleX > scaleY * 2) scaleX = scaleY * 2;
    else if (scaleY > scaleX * 2) scaleY = scaleX * 2;

    double visualScale = std::min(std::min(scaleX, scaleY) / 2.7, MAX_SCALE_ELEMENTS);

    return { scaleX, scaleY, minX, minY, centerX, centerY, visualScale };
}

sf::Vector2f transformPoint(const Point& p, const ScaleInfo& info, double scaleX, double scaleY) {
    double centeredX = p.getX() - info.centerX;
    double centeredY = p.getY() - info.centerY;


    float x = static_cast<float>(centeredX * scaleX);
    float y = static_cast<float>(-centeredY * scaleY);

    return sf::Vector2f(x, y);
}

void drawPoints(const std::vector<Point>& points, sf::RenderWindow& window)
{
    if (points.empty()) return;

    double width = static_cast<double>(window.getSize().x);
    double height = static_cast<double>(window.getSize().y);

    ScaleInfo scaleData = getScaleAndBounds(points, width, height);
    double sharedScaleX = scaleData.scaleX;
	double sharedScaleY = scaleData.scaleY;
    double visualScale = scaleData.visualScale;

    for (const auto& p : points) {

        // Обчислюємо радіус
        float calculatedRadius = static_cast<float>(p.getSize() * visualScale);

        float radius = std::max(std::min(calculatedRadius, MAX_POINT_RADIUS), MIN_POINT_RADIUS);

        sf::CircleShape shape(radius);

        float calculatedOutlineOffset = static_cast<float>(p.getOutlineSize() * visualScale);
        float outlineOffset = std::max(std::min(calculatedOutlineOffset, MAX_POINT_RADIUS / 4.0f), 1.0f);
        shape.setOutlineThickness(outlineOffset);

        shape.setFillColor(p.getColor());
		shape.setOutlineColor(p.getOutlineColor());

        sf::Vector2f pos = transformPoint(p, scaleData, sharedScaleX, sharedScaleY);

        shape.setPosition(sf::Vector2f(pos.x - shape.getRadius(), pos.y - shape.getRadius()));
        window.draw(shape);
    }
}

void drawLines(const std::vector<Line>& lines, const std::vector<Point>& points, sf::RenderWindow& window)
{
    if (points.empty() || lines.empty()) return;

    double width = static_cast<double>(window.getSize().x);
    double height = static_cast<double>(window.getSize().y);

    // Отримуємо спільний масштаб та межі
    ScaleInfo scaleData = getScaleAndBounds(points, width, height);
    double sharedScaleX = scaleData.scaleX;
	double sharedScaleY = scaleData.scaleY;
	double visualScale = scaleData.visualScale;

    for (const auto& line : lines) {
        sf::Vertex vertices[2];

        // Перетворюємо точки, використовуючи спільний масштаб
        vertices[0].position = transformPoint(line.getStart(), scaleData, sharedScaleX, sharedScaleY);
        vertices[0].color = line.getColor();

        vertices[1].position = transformPoint(line.getEnd(), scaleData, sharedScaleX, sharedScaleY);
        vertices[1].color = line.getColor();

        window.draw(vertices, line.getBoldness(), sf::PrimitiveType::Lines);
    }
}

void drawLabels(const std::vector<Point>& points, sf::RenderWindow& window) {
    if (points.empty()) return;

    if (!fontLoaded) {  
        if (!globalFont.openFromFile("arial.ttf")) {
            std::cerr << "Error loading font! The labels will not be displayed." << std::endl;
            return;
        }
        fontLoaded = true;
    }

    double width = static_cast<double>(window.getSize().x);
    double height = static_cast<double>(window.getSize().y);

    ScaleInfo scaleData = getScaleAndBounds(points, width, height);
    double sharedScaleX = scaleData.scaleX;
    double sharedScaleY = scaleData.scaleY;
	double visualScale = scaleData.visualScale;

    for (const auto& p : points) {
        if (p.getName().empty()) {
            continue;
        }

        sf::Vector2f pos = transformPoint(p, scaleData, sharedScaleX, sharedScaleY);

        sf::Text labelText(globalFont);
        float size = std::max(static_cast<float>(FONT_SIZE * visualScale), static_cast<float>(FONT_SIZE));
        labelText.setString(p.getName());
        labelText.setCharacterSize(size);
        labelText.setFillColor(sf::Color::Black);

        float offsetFactor = static_cast<float>(std::min(sharedScaleX, sharedScaleY) / 2.5);
        offsetFactor = std::min(offsetFactor, static_cast<float>(MAX_SCALE_ELEMENTS));

        labelText.setPosition(sf::Vector2f(pos.x + (LABEL_OFFSET_X * offsetFactor), pos.y - (LABEL_OFFSET_Y * offsetFactor)));

        window.draw(labelText);
    }
}

std::string formatWeightString(double weight) {
    double roundedWeight = std::round(weight * 10.0) / 10.0;
    std::string s = std::to_string(roundedWeight);

    size_t decimal_pos = s.find('.');
    if (decimal_pos != std::string::npos) {
        s = s.substr(0, decimal_pos + 2);
    }

    return s;
}

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

    ScaleInfo scaleData = getScaleAndBounds(points, width, height);
    double sharedScaleX = scaleData.scaleX;
    double sharedScaleY = scaleData.scaleY;
	double visualScale = scaleData.visualScale;

    for (const auto& line : lines) {
        // 1. Знаходимо центр лінії в трансформованих координатах
        sf::Vector2f startPos = transformPoint(line.getStart(), scaleData, sharedScaleX, sharedScaleY);
        sf::Vector2f endPos = transformPoint(line.getEnd(), scaleData, sharedScaleX, sharedScaleY);

        sf::Vector2f centerPos;
        centerPos.x = (startPos.x + endPos.x) / 2.0f;
        centerPos.y = (startPos.y + endPos.y) / 2.0f;

        // 3. Створюємо об'єкт тексту
        sf::Text weightText(globalFont);

        // Масштабуємо розмір шрифту відповідно до загального масштабу графу
        float size = std::max(static_cast<float>(WEIGHT_FONT_SIZE * visualScale), static_cast<float>(WEIGHT_FONT_SIZE));

        std::string s = formatWeightString(line.getWeight());
		weightText.setString(s);
        weightText.setCharacterSize(size);
        weightText.setFillColor(sf::Color::Black);
        weightText.setStyle(sf::Text::Italic);
        weightText.setPosition(centerPos);

        // 5. Малюємо текст
        window.draw(weightText);
    }
}