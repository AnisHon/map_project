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

    struct DijkstraTable {
        QString id;
        bool is_known;
        qreal distance;
        QString prev;
    };


    void init(MapReader& reader);

    void init_connect_nodes(const QVector<QString>& ids);

    QHash<QString, DijkstraTable> build_dijkstra_table(const QString &original);

    static QString min_node(const QHash<QString, DijkstraTable> &table);

    static qreal heuristic_function(qreal weight, qreal heuristic);

public:
    explicit MapAdjacencyList(const MapReader &reader, const Transformer &transformer);


    bool is_connected(const QString &id1, const QString &id2) const;

    void add_location(const ScreenNode &node);

    void add_location(const std::initializer_list<ScreenNode> &list);

    void connect(const QString &id1, const QString &id2);

    QHash<QString, qreal> get_connected(const QString& id);

    qreal get_weight(const QString &id1, const QString &id2) const;


    QPointF get_location(const QString &id) const;

    QVector<QString> dijkstra(const QString &original, const QString &destination);

    QVector<QString> a_star(const QString &original, const QString &destination);


    void clear();
private:
    QHash<QString, Location> locations_;
    Transformer transform_;
    MapReader map_reader_;

};



#endif //MAPADJACENCYLIST_H
