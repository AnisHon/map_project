#include "mapnode.h"
#include "QString"
#include "QDebug"
MapNode::MapNode() {}

QPointF MapNode::getCoordinates() const {
    return {lon.toDouble(), lat.toDouble()};
}

QDebug &operator<<(QDebug &os, const MapNode &node) {
    os << "<node " << "id=" << node.id << " visible=" << node.visible << " version=" << node.version << " changeset="
       << node.changeset << " timestamp=" << node.timestamp << " user=" << node.user << " uid=" << node.uid
       << " lat=" << node.lat << " lon=" << node.lon << "/>";
    return os;
}





