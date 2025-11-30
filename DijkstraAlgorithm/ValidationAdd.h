#pragma once

#ifndef VALIDATIONADD_H
#define VALIDATIONADD_H

#include <string>
#include <vector>
#include "Point.h"
#include "Line.h"

bool isFirstCharAlpha(const std::string& name);
bool validateNewPoint(const Point& newPoint, const std::vector<Point>& existingPoints);
bool validateNewLine(const string& startName, const std::string& endName, double weight, const std::vector<Line>& existingLines, const std::vector<Point>& points);

#endif

