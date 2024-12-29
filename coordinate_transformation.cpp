//
// Created by Anis Han on 24-9-11.
//

#include "coordinate_transformation.h"
QPointF mercatorProjection(const double latitude, const double longitude) {
    QPointF mercatorCoord;
    mercatorCoord.setX(R * (longitude * PI / 180.0));
    mercatorCoord.setY(R * std::log(std::tan(PI / 4.0 + (latitude * PI / 180.0) / 2.0)));
    return mercatorCoord;
}


QPointF getCenter(const WayNode &wayNode) {
    QPointF center(0, 0);
    for (const auto &item: wayNode.paths) {
        center += item;
    }
    center /= wayNode.paths.size();
    return center;
}

QPointF Transformer::mercatorToScreen(const QPointF &lonlat) const {
    const QPointF transformation = mercatorProjection(lonlat.y() - center.y(), lonlat.x() - center.x());
    return transformation * scale;
}

Transformer::Transformer(const double scale_, const QPointF center_): scale(scale_), center(center_) {}

QPointF Transformer::operator()(const QPointF &lonlat) const {
    return mercatorToScreen(lonlat);
}
