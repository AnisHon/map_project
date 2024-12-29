//
// Created by happy on 24-12-29.
//

#include "MapAdjacencyList.h"

#include "coordinate_transformation.h"
#include "distance_utils.h"

bool MapAdjacencyList::is_connected(const QString &id1, const QString &id2) const {
    if (id1 == id2) {
        return true;
    }

    if (!(this->locations_.contains(id1) && this->locations_.contains(id2))) {
        return false;
    }


    const auto & [node1, weights1] = locations_[id1];
    const auto & [node2, weights2] = locations_[id2];

    bool result = weights1.contains(id2);
    result |= weights2.contains(id1);

    return result;


}

void MapAdjacencyList::add_location(const ScreenNode &node) {
    if (this->locations_.contains(node.id)) {
        return;
    }

    const Location location{node, QHash<QString, qreal>{}};
    locations_.insert(node.id, location);
}

void MapAdjacencyList::add_location(const std::initializer_list<ScreenNode> &list) {
    for (const auto& screen_node : list) {
        this->add_location(screen_node);
    }

}

void MapAdjacencyList::connect(const QString &id1, const QString &id2) {
    if (id1 == id2) {
        return;
    }
    if (!(this->locations_.contains(id1) && this->locations_.contains(id2))) {
        return;
    }

    auto & [node1, weights1] = locations_[id1];
    auto & [node2, weights2] = locations_[id2];

    const qreal distance = euclidean_distance(node1.position, node2.position);

    weights1.insert(node2.id, distance);
    weights2.insert(node1.id, distance);
}

qreal MapAdjacencyList::get_weight(const QString& id1, const QString& id2) const {
    if (is_connected(id1, id2)) {
        throw std::runtime_error(
            QString("node1(id: %1) and node2(id: %2) should be connected")
            .arg(id1, id2)
            .toUtf8()
            );
    }

    auto & [node, weights] = locations_[id1];

    return weights[id2];
}

void MapAdjacencyList::init_connect_nodes(const QVector<QString> &ids) {
    if (ids.size() <= 1) {
        return;
    }

    QString id = ids[0];

    for (int i = 1; i < ids.size(); i++) {
        const QString &connected_id = ids[i];
        this->connect(id, connected_id);
        id = connected_id;
    }
}

void MapAdjacencyList::init(MapReader& reader) {
    const auto &nodes = reader.getNodes();

    const Transformer &coordinate_transformation = this->transform_;


    // 像邻接表中添加元素，进行mercator坐标到平面坐标转换
    for (const auto &node : nodes) {
        const QPointF &point = node.getCoordinates();
        const QPointF &screen_point = coordinate_transformation.mercatorToScreen(point);

        add_location(ScreenNode{node.id, screen_point});
    }


    const QVector<WayNode> &way_nodes = reader.getWays();

    for (const auto &way_node : way_nodes) {
        this->init_connect_nodes(way_node.path_ids);
    }
}

MapAdjacencyList::MapAdjacencyList(const MapReader& reader, const Transformer &transformer):
    locations_{QHash<QString, Location>()},
    transform_{transformer} {
    this->init(const_cast<MapReader&>(reader));
}


