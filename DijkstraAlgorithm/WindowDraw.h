#pragma once

#ifndef DRAWELEMENTS_H
#define DRAWELEMENTS_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "Point.h"
#include "Line.h"

extern bool LABEL_SHOW;
extern bool WEIGHT_SHOW;

void drawPoints(const std::vector<Point>& points, sf::RenderWindow& window);
void drawLines(const std::vector<Line>& lines, const std::vector<Point>& points, sf::RenderWindow& window);
void drawLabels(const std::vector<Point>& points, sf::RenderWindow& window);
void drawEdgeWeights(const std::vector<Line>& lines, const std::vector<Point>& points, sf::RenderWindow& window);

#endif
