#include "WindowDraw.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>

#define M_PI 3.14159265358979323846

const double MAX_SCALE_ELEMENTS = 3.0;
const double paddingFactor = 0.9;

sf::Font globalFont;
bool fontLoaded = false;

float LABEL_OFFSET_X = 5.0f;
float LABEL_OFFSET_Y = -5.0f;

const unsigned int FONT_SIZE = 24;
const unsigned int WEIGHT_FONT_SIZE = 20;

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
        rangeX = 1;
        rangeY = 1;
    }
    else if (rangeX < 1e-6 || rangeY < 1e-6) {

        rangeX = std::max(rangeX, 1.);
        rangeY = std::max(rangeY, 1.);
    }

    rangeX = std::max(rangeX, 1.);
    rangeY = std::max(rangeY, 1.);

    double scaleX = ((windowSizeX / 2.0) * paddingFactor) / (rangeX / 2.0);
    double scaleY = ((windowSizeY / 2.0) * paddingFactor) / (rangeY / 2.0);

    if (scaleX > scaleY * 2) scaleX = scaleY * 2;
    else if (scaleY > scaleX * 2) scaleY = scaleX * 2;


    double windowFactor = std::min(windowSizeX, windowSizeY) / 100.0;
    double visualScale = 0.2 * windowFactor;

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
    double visualScale = scaleData.visualScale * 0.7;

    for (const auto& p : points) {

        // Обчислюємо радіус
        float calculatedRadius = static_cast<float>(p.getSize() * visualScale);

        float radius = std::max(std::min(calculatedRadius, static_cast<float>(p.getSize())), 3.0f);

        sf::CircleShape shape(radius);

        float calculatedOutlineOffset = static_cast<float>(p.getOutlineSize() * visualScale);
        float outlineOffset = std::max(std::min(calculatedOutlineOffset, static_cast<float>(p.getOutlineSize())), 1.0f);
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
        sf::Vector2f startPos = transformPoint(line.getStart(), scaleData, sharedScaleX, sharedScaleY);
        sf::Vector2f endPos = transformPoint(line.getEnd(), scaleData, sharedScaleX, sharedScaleY);

        // 1. Обчислити довжину лінії
        sf::Vector2f direction = endPos - startPos;
        float lineLength = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        // 2. Створити прямокутник, який буде лінією
		float calculatedBoldness = static_cast<float>(line.getBoldness() * visualScale);
		float boldness = std::max(std::min(calculatedBoldness, static_cast<float>(line.getBoldness())), 1.0f);
        sf::RectangleShape rect(sf::Vector2f(lineLength, boldness)); // line.getBoldness() як товщина

        // 3. Встановити колір
        rect.setFillColor(line.getColor());

        // 4. Встановити позицію та обертання
        rect.setPosition(startPos);

        // Обчислити кут обертання
        sf::Angle angle = sf::degrees(std::atan2(direction.y, direction.x) * 180.f / static_cast<float>(M_PI));
        rect.setRotation(angle);

        // Зсув, щоб лінія була центрована по товщині (за замовчуванням обертання відбувається навколо (0,0))
        rect.setOrigin(sf::Vector2f(0.0f, line.getBoldness() / 2.0f));

        // 5. Намалювати
        window.draw(rect);
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
        float size = std::max(std::min(static_cast<float>(FONT_SIZE * visualScale), static_cast<float>(FONT_SIZE)), FONT_SIZE / 3.0f);
        labelText.setString(p.getName());
        labelText.setCharacterSize(size);
        labelText.setFillColor(sf::Color::Black);

        float offsetFactor = static_cast<float>(visualScale / 2.5);
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
        float size = std::max(std::min(static_cast<float>(WEIGHT_FONT_SIZE * visualScale), static_cast<float>(WEIGHT_FONT_SIZE)), WEIGHT_FONT_SIZE / 3.0f);

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