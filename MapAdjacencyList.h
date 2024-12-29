//
// Created by happy on 24-12-29.
//

#ifndef MAPADJACENCYLIST_H
#define MAPADJACENCYLIST_H
#include <QHash>
#include <QMap>

#include "coordinate_transformation.h"
#include "mapnode.h"
#include "mapreader.h"


/**
 * 屏幕坐标节点，MapNode用的是经纬坐标
 */
struct ScreenNode {
    QString id;
    QPointF position;
};

/**
 * 邻接表节点，表示当前节点和相邻节点关系，通过map存储id + distance（权重）表示关系
 */
struct Location {
    ScreenNode node;
    QHash<QString, qreal> weights;

};

class MapAdjacencyList {

private:
    void init(MapReader& reader);

public:
    explicit MapAdjacencyList(const MapReader &reader, const Transformer &transformer);

    bool is_connected(const QString &id1, const QString &id2) const;

    void add_location(const ScreenNode &node);

    void add_location(const std::initializer_list<ScreenNode> &list);

    void connect(const QString &id1, const QString &id2);

    qreal get_weight(const QString &id1, const QString &id2) const;

    void init_connect_nodes(const QVector<QString>& ids);

private:
    QHash<QString, Location> locations_;
    Transformer transform_;

};



#endif //MAPADJACENCYLIST_H
