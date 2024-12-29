#ifndef MAPREADER_H
#define MAPREADER_H
#include "mapnode.h"
#include "waynode.h"

#include <QMap>
#include <QString>
#include <QVector>
#include <QXmlStreamReader>
#include <QtXml/QDomDocument>

class MapReader
{


public:
    explicit MapReader(QString path);

    QMap<QString, MapNode> getNodes();

    QVector<WayNode> getWays();

    QVector<QString> getHighWays();


private:
    void init();

    void readNode(const QDomNodeList &node);

    void filterHighWay();


    QString path_;
    QMap<QString, MapNode> nodes_;
    QVector<WayNode> ways_;
    QVector<QString> highways_;

    void readWay(const QDomNodeList &list);
};

#endif // MAPREADER_H
