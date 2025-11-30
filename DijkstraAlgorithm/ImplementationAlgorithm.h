#pragma once

#ifndef IMPLEMENTATIONALGORITHM_H
#define IMPLEMENTATIONALGORITHM_H
#include <vector>
#include "Point.h"
#include "Line.h"

std::vector<Line> findShortestPath(const Point& startPoint, const Point& endPoint, const std::vector<Point>& points, const std::vector<Line>& lines);

#endif