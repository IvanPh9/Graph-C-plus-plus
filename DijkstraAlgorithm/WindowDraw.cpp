#include "WindowDraw.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>

// --- Допоміжна Структура ---
struct ScaleInfo {
    double scale;
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

    // Максимальний діапазон для збереження пропорцій
    double maxRange = std::max(rangeX, rangeY);

    // Якщо дані займають лише одну точку або занадто малий діапазон
    if (maxRange < 1e-6) {
        return { 1.0, minX, minY, centerX, centerY };
    }

    // Ми хочемо, щоб найбільший діапазон (maxRange) був відмасштабований
    // до 90% від розміру вікна. Але оскільки центр (0,0), робочий діапазон SFML
    // тепер [-(window/2), +(window/2)].

    // Тому ми масштабуємо від maxRange до (windowSize / 2.0),
    // що гарантує, що фігура впишеться.
    const double targetViewSize = std::min(windowSizeX, windowSizeY) / 2.0;
    const double paddingFactor = 0.9;

    // Розраховуємо, скільки пікселів SFML відповідає одній одиниці даних.
    double scale = (targetViewSize * paddingFactor) / (maxRange / 2.0);
    // Примітка: ділення maxRange на 2.0 потрібне, оскільки range від -Center до +Center.

    return { scale, minX, minY, centerX, centerY };
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
    double sharedScale = scaleData.scale; // Єдиний коефіцієнт

    for (const auto& p : points) {
        sf::CircleShape shape(5);
        shape.setFillColor(sf::Color::Red);

        // Передаємо єдиний коефіцієнт як scaleX та scaleY
        sf::Vector2f pos = transformPoint(p, scaleData, sharedScale, sharedScale);

        // Зверніть увагу: радіус 5 буде виглядати малим. Можливо, варто масштабувати радіус
        // або використовувати фіксований розмір, незалежний від масштабу даних.
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
    double sharedScale = scaleData.scale; // Єдиний коефіцієнт

    for (const auto& line : lines) {
        sf::Vertex vertices[2];

        // Перетворюємо точки, використовуючи спільний масштаб
        vertices[0].position = transformPoint(line.getStart(), scaleData, sharedScale, sharedScale);
        vertices[0].color = sf::Color::Black;

        vertices[1].position = transformPoint(line.getEnd(), scaleData, sharedScale, sharedScale);
        vertices[1].color = sf::Color::Black;

        window.draw(vertices, 2, sf::PrimitiveType::Lines);
    }
}
