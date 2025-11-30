#pragma once

#ifndef INTERFACE_H
#define INTERFACE_H

#include <vector>
#include "Point.h"
#include "Line.h"
#include <mutex>
#include <atomic>

extern mutex dataMutex;
extern atomic<bool> isRunning;

void MainInterface(std::vector<Point>& points, std::vector<Line>& lines);

#endif
