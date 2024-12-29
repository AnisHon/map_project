//
// Created by happy on 24-12-29.
//

#include "MapAdjacencyList.h"

#include "coordinate_transformation.h"
#include "distance_utils.h"
#include <limits>
#include <qstack.h>
#include <QDebug>
#include <queue>

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

void MapAdjacencyList::clear() {
    this->locations_.clear();
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
    const auto &highway = reader.getHighWays();

    const Transformer &coordinate_transformation = this->transform_;


    // 像邻接表中添加元素，进行mercator坐标到平面坐标转换
    for (const auto &way : highway) {
        const QPointF &point = nodes[way].getCoordinates();
        const QPointF &screen_point = coordinate_transformation.mercatorToScreen(point);

        add_location(ScreenNode{way, screen_point});
    }


    const QVector<WayNode> &way_nodes = reader.getWays();

    for (const auto &way_node : way_nodes) {
        this->init_connect_nodes(way_node.path_ids);
    }
}

MapAdjacencyList::MapAdjacencyList(const MapReader& reader, const Transformer &transformer):
    locations_{QHash<QString, Location>()},
    transform_{transformer},
    map_reader_(reader) {
    this->init(const_cast<MapReader&>(reader));
}

QPointF MapAdjacencyList::get_location(const QString& id) const {
    return this->locations_[id].node.position;
}

QHash<QString, qreal> MapAdjacencyList::get_connected(const QString& id) {
    return this->locations_[id].weights;
}


QHash<QString, MapAdjacencyList::DijkstraTable> MapAdjacencyList::build_dijkstra_table(const QString &original) {
    QHash<QString, DijkstraTable> result;
    constexpr qreal max = std::numeric_limits<qreal>::max();

    for (const auto &high_way : map_reader_.getHighWays()) {
        result[high_way] = DijkstraTable{high_way, false, max, ""};
    }

    // result[original].is_known = true;
    result[original].distance = 0;

    // for (auto [id, is_known, distance, prev] : result) {
    //     qDebug() << id.toStdString() << " " << " " << is_known  << " " << prev.toStdString() << "\t\t" << distance;
    // }
    // qDebug() << "=============================";
    return result;
}

QString MapAdjacencyList::min_node(const QHash<QString, DijkstraTable> &table) {
    if (table.empty()) {
        return "";
    }

    qreal min = std::numeric_limits<qreal>::max();
    QString result;

    for (const auto &node : table) {
        if (node.is_known) {
            continue;
        }

        if (min > node.distance) {
            min = node.distance;
            result = node.id;
        }
    }
    // const auto row = table[result];
    // qDebug() << result;
    // qDebug() << row.id << "\t" << row.is_known << "\t" << row.distance << "\t" << row.prev;


    if (result.isEmpty()) {
        qDebug() << "MapAdjacencyList::min_node: No node found";
    }
    return result;

}


QVector<QString> MapAdjacencyList::dijkstra(const QString& original, const QString& destination) {
    auto table = build_dijkstra_table(original);
    qDebug() << "original" << original << " destination" << destination;
    // 获取最小元素
    QString min_id;
    while (!(min_id = min_node(table)).isEmpty()) {

        // 遍历节点，connected_nodes是一个存储权重的Hash表
        auto connected_nodes = this->get_connected(min_id);

        // 当前节点的表行
        DijkstraTable &curr_row = table[min_id];
        curr_row.is_known = true;

        if (min_id == destination) {
            break;
        }

        for (const auto &id : connected_nodes.keys()) {

            // 邻接节点的表行
            DijkstraTable &row = table[id];

            // qDebug() << row.id << "\t" << row.is_known << "\t" << row.distance << "\t" << row.prev;
            if (row.is_known) {
                continue;
            }

            // distance_ = distance + weight
            const qreal distance = curr_row.distance + connected_nodes[id];

            if (distance < row.distance) {
                row.distance = distance;
                row.prev = min_id;
            }

        }

        // for (auto [id, is_known, distance, prev] : table) {
        //
        //     // qDebug() << id.toStdString() << " " << " " << is_known  << " " << prev.toStdString() << "\t\t" << distance;
        // }


    }
    // for (auto row : table) {
    //     if (!row.is_known) {
    //         continue;
    //     }
    //     qDebug() << row.id << " " << " " << row.is_known << row.distance << " " << row.prev;
    // }

    QStack<QString> stack{};
    QVector<QString> path{};


    if (!table[destination].is_known) {
        qDebug() << "Destination not known";
        return path;
    }

    stack.push(destination);
    while (stack.top() != original) {
        stack.push(table[stack.top()].prev);
    }



    while (!stack.isEmpty()) {

        path.emplace_back(stack.top());
        stack.pop();
    }

    return path;
}

qreal MapAdjacencyList::heuristic_function(const qreal weight, const qreal heuristic) {
    if (weight == std::numeric_limits<qreal>::max()) {
        return std::numeric_limits<qreal>::max();
    }
    return weight + heuristic;
}

QVector<QString> MapAdjacencyList::a_star(const QString& original, const QString& destination) {

    // 初始化启发函数和权重
    QHash<QString, qreal> weights{}, heuristic{};
    QHash<QString, QString> path;
    QSet<QString> close{};

    const auto destination_point = this->get_location(destination);
    for (auto id : this->map_reader_.getHighWays()) {
        weights[id] = std::numeric_limits<qreal>::max();
        heuristic[id] = euclidean_distance(get_location(id), destination_point);
    }
    weights[original] = 0;

    // 优先队列，按照启发 函数 + 权重排序（小根堆）
    const auto compare = [this, &weights, &heuristic] (const QString &id1, const QString &id2) -> bool {
        const qreal heuristic1 = weights[id1] + heuristic[id1];
        const qreal heuristic2 = weights[id2] + heuristic[id2];
        return heuristic1 > heuristic2;
    };
    std::priority_queue<QString, std::vector<QString>, decltype(compare)> queue(compare);

    queue.push(original);

    while (!queue.empty()) {
        const QString id = queue.top();
        queue.pop();

        if (close.contains(id)) {
            continue;
        }
        close.insert(id);

        if (id == destination) {
            break;
        }


        const auto &connected = this->get_connected(id);
        for (auto key : connected.keys()) {
            // 总权重
            const qreal weight = connected[key] + weights[id];

            // 启发式函数计算
            const qreal new_f = heuristic_function(weight, heuristic[key]);
            const qreal old_f = heuristic_function(weights[key], heuristic[key]);

            if (new_f > old_f) {
                continue;
            }
            weights[key] = weight;
            queue.push(key);
            path[key] = id;
        }
    }

    QStack<QString> stack;
    if (path.contains(destination)) {
        QString current = destination;
        while (current != original) {
            stack.push_back(current);
            current = path[current];
        }
    }
    stack.append(original);

    QVector<QString> result;

    while (!stack.isEmpty()) {
        const QString id = stack.top();
        stack.pop();
        result.push_back(id);
    }


    return result;
}









