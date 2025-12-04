#pragma once

#ifndef IMPLEMENTATIONALGORITHM_H
#define IMPLEMENTATIONALGORITHM_H
#include <vector>
#include "Point.h"
#include "Line.h"

using namespace std;

pair<string, double> findShortestPath(const vector<Point>& points, const vector<Line>& lines);

#endif