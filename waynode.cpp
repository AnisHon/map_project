#include "waynode.h"

#include <utility>

WayNode::WayNode(QString id_): id(std::move(id_)) {

}

void WayNode::addPath(QPointF point) {
    paths.emplaceBack(point);
}

void WayNode::addTag(const QString &k, const QString &v) {
    tags.insert(k, v);
}
