#pragma once

#ifndef INTERFACE_H
#define INTERFACE_H

#include <vector>
#include "Point.h"
#include "Line.h"
#include <mutex>
#include <atomic>

using namespace std;

extern mutex dataMutex;
extern int ANIMATION_DELAY;
extern atomic<bool> isRunning;

void MainInterface(vector<Point>& points, vector<Line>& lines);

#endif
