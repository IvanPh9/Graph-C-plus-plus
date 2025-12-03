#pragma once

#ifndef IMPLEMENTATIONALGORITHM_H
#define IMPLEMENTATIONALGORITHM_H
#include <vector>
#include "Point.h"
#include "Line.h"

std::pair<std::string, double> findShortestPath(const std::vector<Point>& points, const std::vector<Line>& lines);

#endif