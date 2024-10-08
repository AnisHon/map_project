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


QPointF mercatorProjection(double latitude, double longitude);



QPointF getCenter(const WayNode &wayNode);

class Transformer {
public:
    double scale = 1;
    QPointF center;
    QPointF mercatorToScreen(const QPointF &lonlat);
    Transformer(double scale_ = 1, QPointF center_ = {120, 30});
    QPointF operator()(const QPointF &lonlat);
};


#endif //MAP_PROJECT_COORDINATE_TRANSFORMATION_H
