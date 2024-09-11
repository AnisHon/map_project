//
// Created by Anis Han on 24-9-11.
//

#ifndef MAP_PROJECT_COORDINATE_TRANSFORMATION_H
#define MAP_PROJECT_COORDINATE_TRANSFORMATION_H
#include <QPointF>
#include <iostream>
#include <cmath>
#include "waynode.h"
const double R = 6378137.0;
const double PI = 3.14159265358979323846;
const double SCALE = 1;

QPointF mercatorProjection(double latitude, double longitude);

QPointF mercatorToScreen(const QPointF &lonlat);

QPointF getCenter(const WayNode &wayNode);

#endif //MAP_PROJECT_COORDINATE_TRANSFORMATION_H
