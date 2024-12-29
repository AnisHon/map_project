//
// Created by happy on 24-12-29.
//

#include "distance_utils.h"

qreal euclidean_distance(const QPointF& point1, const QPointF& point2) {
    const QPointF diff = point1 - point2; // 点的差值
    return std::sqrt(QPointF::dotProduct(diff, diff)); // 向量模长 2范数
}
