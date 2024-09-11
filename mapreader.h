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


private:
    void init();

    void readNode(const QDomNodeList &node);

private:
    QString path_;
    QMap<QString, MapNode> nodes_;
    QVector<WayNode> ways_;

    void readWay(const QDomNodeList &list);
};

#endif // MAPREADER_H
