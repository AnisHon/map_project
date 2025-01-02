#include "mapreader.h"

#include <utility>

#include <QString>
#include <QtXml>

MapReader::MapReader(QString path): path_(std::move(path)) {
    init();
}

QMap<QString, MapNode> MapReader::getNodes() {

    return nodes_;
}

void MapReader::init() {
    auto file = QFile(path_);
    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("File not open");
    }
    QDomDocument dom;
    if (!dom.setContent(&file)) {
        throw std::runtime_error("Fail to load xml");
    }
    readNode(dom.elementsByTagName("node"));
    readWay(dom.elementsByTagName("way"));
    filterHighWay();
}

void MapReader::readNode(const QDomNodeList &nodes) {
    for (int i = 0; i < nodes.length(); ++i) {
        const auto & node = nodes.at(i);
        const auto id = node.attributes().namedItem("id").toAttr().value();
        const auto lat = node.attributes().namedItem("lat").toAttr().value();
        const auto lon = node.attributes().namedItem("lon").toAttr().value();
        nodes_.insert(id, MapNode(id, lon, lat));
        if (node.hasChildNodes()) {
            WayNode wayNode(id);
            wayNode.addPath(MapNode(id, lon, lat).getCoordinates());
            for (int j = 0; j < node.childNodes().length(); ++j) {
                const auto &n = node.childNodes().at(j);
                wayNode.addTag(n.toElement().attribute("k"), n.toElement().attribute("v"));
//                 n.toElement().attribute("k") << n.toElement().attribute("v");
            }
            ways_.emplaceBack(wayNode);
        }
    }
}

void MapReader::readWay(const QDomNodeList &list) {

    for (int i = 0; i < list.length(); ++i) {
        const auto &node = list.at(i);
        auto nodeDom = node.firstChildElement("nd");

        WayNode wayNode(node.attributes().namedItem("id").nodeValue());

        while (!nodeDom.isNull()) {
            auto map_node = nodes_[nodeDom.attribute("ref")];
            wayNode.path_ids.append(map_node.id);
            wayNode.addPath(map_node.getCoordinates());
            nodeDom = nodeDom.nextSiblingElement("nd");
        }
        nodeDom = node.firstChildElement("tag");
        while (!nodeDom.isNull()) {
            wayNode.addTag(nodeDom.attribute("k"), nodeDom.attribute("v"));
            nodeDom = nodeDom.nextSiblingElement("tag");
        }
        ways_.emplaceBack(wayNode);
    }
}

void MapReader::filterHighWay() {
    for (const WayNode &way : this->ways_) {
        if (!way.contain("highway")) {
            continue;
        }
        if (way.tags["highway"] != "service") {
            continue;
        }
        for (const auto &path_id : way.path_ids) {
            this->highways_.push_back(path_id);
        }
    }

}

QVector<WayNode> MapReader::getWays() {
    return ways_;
}

QVector<QString> MapReader::getHighWays() {
    
    return this->highways_;
}


