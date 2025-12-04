#pragma once

#ifndef INTERFACE_H
#define INTERFACE_H

#include <vector>
#include "Point.h"
#include "Line.h"
#include <mutex>
#include <atomic>

using namespace std;

// ------------------------------------------------------------
// Global synchronization mutex for safe access to shared data
// Used by both the rendering thread and the console thread
// ------------------------------------------------------------
extern mutex dataMutex;

// ------------------------------------------------------------
// Animation delay (in milliseconds)
// Controls the speed of the visualization
// ------------------------------------------------------------
extern int ANIMATION_DELAY;

// ------------------------------------------------------------
// Atomic flag that indicates whether the application is running
// Console thread checks this to know when to stop
// ------------------------------------------------------------
extern atomic<bool> isRunning;

void MainInterface(vector<Point>& points, vector<Line>& lines);

#endif
