#ifndef MAPNODE_H
#define MAPNODE_H

#include <QString>
#include <QPointF>
#include <utility>
#include <ostream>

class MapNode {
public:
    MapNode();


public:
    QString id;
    bool visible;
    QString version;
    QString changeset;
    QString timestamp;
    QString user;
    QString uid;
    QString lat; // 纬度 赤道平行
    QString lon; // 经度

    MapNode(QString id_, QString lon_, QString lat_, bool visible_ = true, QString uid_ = "",
            QString version_ = "", QString changeset_ = "", QString timestamp_ = "", QString user_ = ""):
            id(std::move(id_)), lon(std::move(lon_)), lat(std::move(lat_)), visible(visible_),
            version(std::move(version_)), changeset(std::move(changeset_)),
            timestamp(std::move(timestamp_)), user(std::move(user_)), uid(std::move(uid_)) {}

    /*
     * get lon and lat (float type) QPointF(lon, lat)
     */
    QPointF getCoordinates() const;

    friend QDebug &operator<<(QDebug &os, const MapNode &nodes);


};

#endif // MAPNODE_H
