//
// Created by Anis Han on 24-9-11.
//

#include "coordinate_transformation.h"
QPointF mercatorProjection(double latitude, double longitude) {
    QPointF mercatorCoord;
    mercatorCoord.setX(R * (longitude * PI / 180.0));
    mercatorCoord.setY(R * std::log(std::tan(PI / 4.0 + (latitude * PI / 180.0) / 2.0)));
    return mercatorCoord;
}

QPointF mercatorToScreen(const QPointF &lonlat) {
    QPointF transformation = mercatorProjection(lonlat.y(), lonlat.x());
    return transformation * SCALE;
}

QPointF getCenter(const WayNode &wayNode) {
    QPointF center(0, 0);
    for (const auto &item: wayNode.paths) {
        center += item;
    }
    center /= wayNode.paths.size();
    return center;
}