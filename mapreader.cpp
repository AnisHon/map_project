#include "mapreader.h"

#include <utility>

#include <QFile>
#include <QDebug>
#include <QStringView>
#include <QString>
#include <QtXml>
#include <QtXml/QDomDocument>
#include <QDomNodeList>
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

}

void MapReader::readNode(const QDomNodeList &nodes) {
    for (int i = 0; i < nodes.length(); ++i) {
        const auto & node = nodes.at(i);
        auto id = node.attributes().namedItem("id").toAttr().value();
        auto lat = node.attributes().namedItem("lat").toAttr().value();
        auto lon = node.attributes().namedItem("lon").toAttr().value();
        nodes_.insert(id, MapNode(id, lon, lat));
    }
}

void MapReader::readWay(const QDomNodeList &list) {

    for (int i = 0; i < list.length(); ++i) {
        const auto &node = list.at(i);
        auto nodeDom = node.firstChildElement("nd");

        WayNode wayNode(node.attributes().namedItem("id").nodeValue());

        while (!nodeDom.isNull()) {
            wayNode.addPath(nodes_[nodeDom.attribute("ref")].getCoordinates());
            nodeDom = nodeDom.nextSiblingElement("nd");
        }
        nodeDom = node.firstChildElement("tag");
        while (!nodeDom.isNull()) {
            wayNode.addTag(nodeDom.attribute("k"), nodeDom.attribute("v"));
            nodeDom = nodeDom.nextSiblingElement("nd");
        }
        ways_.emplaceBack(wayNode);
    }
}

QVector<WayNode> MapReader::getWays() {
    return ways_;
}

