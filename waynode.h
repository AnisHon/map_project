#ifndef WAYNODE_H
#define WAYNODE_H

#include <QString>
#include <QVector>
#include <QMap>
#include "mapnode.h"

class WayNode
{

public:
    explicit WayNode(QString id_);

    void addPath(QPointF point);

    void addTag(const QString &k, const QString &v);

public:
    QString id;
    QVector<QPointF> paths;
    QMap<QString, QString> tags;


};

#endif // WAYNODE_H
