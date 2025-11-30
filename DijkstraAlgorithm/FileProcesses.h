#pragma once

#ifndef FILEPROCESSES_H
#define FILEPROCESSES_H

#include <string>
#include <vector>
#include "Point.h"
#include "Line.h"

bool loadPointsFromFile(const std::string& filename, std::vector<Point>& points);
bool loadLinesFromFile(const std::string& filename, std::vector<Line>& lines, std::vector<Point>& points);

#endif
