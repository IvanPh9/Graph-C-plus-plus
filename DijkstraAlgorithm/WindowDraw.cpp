#include "WindowDraw.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>

sf::Font globalFont;
bool fontLoaded = false;

float LABEL_OFFSET_X = 3.0f;
float LABEL_OFFSET_Y = -3.0f;
const unsigned int FONT_SIZE = 12;

// --- Допоміжна Структура ---
struct ScaleInfo {
    double scaleX;
	double scaleY;
    double minX;
    double minY;
    double centerX; // Додаємо центр даних для зміщення
    double centerY;
};

// --- Оновлена Функція Обчислення Масштабу та Меж ---
ScaleInfo getScaleAndBounds(const std::vector<Point>& points, double windowSizeX, double windowSizeY) {
    if (points.empty()) return { 1.0, 0.0, 0.0, 0.0, 0.0 };

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


    // Якщо дані займають лише одну точку або занадто малий діапазон
	if (rangeX < 1e-6 && rangeY < 1e-6) {
        return { 1.0, minX, minY, centerX, centerY };
    }

    else if (rangeX < 1e-6) {
        rangeX = rangeY;
    }
	else if (rangeY < 1e-6) {
        rangeY = rangeX;
	}


    const double targetViewSize = std::min(windowSizeX, windowSizeY) / 2.0;
    const double paddingFactor = 0.9;

    // Розраховуємо, скільки пікселів SFML відповідає одній одиниці даних.
    double scaleX = ((windowSizeX / 2.0) * paddingFactor) / (rangeX / 2.0);
    double scaleY = ((windowSizeY / 2.0) * paddingFactor) / (rangeY / 2.0);
    if (scaleX > scaleY * 2) scaleX = scaleY * 2;
    else if (scaleY > scaleX * 2) scaleY = scaleX * 2;

    return { scaleX, scaleY, minX, minY, centerX, centerY };
}

sf::Vector2f transformPoint(const Point& p, const ScaleInfo& info, double scaleX, double scaleY) {
    // Масштабування та Центрування у логічній системі координат SFML

    // 1. Зміщення: Переносимо центр ваших даних (info.centerX, info.centerY)
    // у центр SFML вигляду (0, 0).
    double centeredX = p.getX() - info.centerX;
    double centeredY = p.getY() - info.centerY;

    // 2. Масштабування: Застосовуємо єдиний коефіцієнт масштабування (scaleX = scaleY)
    // Оскільки ми вже забезпечили збереження пропорцій у getScaleAndBounds,
    // тут використовується єдиний масштаб (info.scale, який ми передали як scaleX/scaleY).
    float x = static_cast<float>(centeredX * scaleX);
    float y = static_cast<float>(-centeredY * scaleY);

    // Y-координата: НЕ ПОТРІБНЕ ПЕРЕВЕРТАННЯ
    // Оскільки ми встановили view.setCenter(0.f, 0.f), SFML Y-вісь вже направлена вгору.

    return sf::Vector2f(x, y);
}

void drawPoints(const std::vector<Point>& points, sf::RenderWindow& window)
{
    if (points.empty()) return;

    double width = static_cast<double>(window.getSize().x);
    double height = static_cast<double>(window.getSize().y);

    // Отримуємо спільний масштаб та межі
    ScaleInfo scaleData = getScaleAndBounds(points, width, height);
    double sharedScaleX = scaleData.scaleX;
	double sharedScaleY = scaleData.scaleY;

    for (const auto& p : points) {

        float radius = std::max(static_cast<float>(p.getSize() * (std::min(sharedScaleX, sharedScaleY) / 2.5)), 3.0f);
        sf::CircleShape shape(radius);

        float outlineOffset = std::max(static_cast<float>(p.getOutlineSize() * (std::min(sharedScaleX, sharedScaleY) / 2.5)), 1.0f);
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

    for (const auto& p : points) {
        if (p.getName().empty()) {
            continue;
        }

        sf::Vector2f pos = transformPoint(p, scaleData, sharedScaleX, sharedScaleY);

        sf::Text labelText(globalFont);
        float size = std::max(static_cast<float>(FONT_SIZE * (std::min(sharedScaleX, sharedScaleY) / 2.5)), 12.f);
        labelText.setString(p.getName());
        labelText.setCharacterSize(size);
        labelText.setFillColor(sf::Color::Black);

        labelText.setPosition(sf::Vector2f(pos.x + (LABEL_OFFSET_X * (sharedScaleX/2.5)), pos.y - (LABEL_OFFSET_Y * (sharedScaleY / 2.5))));

        window.draw(labelText);
    }
}
