#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include "mapreader.h"
#include "mapnode.h"
#include "coordinate_transformation.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QPointF p = mercatorToScreen({116.4074, 39.9042});
    qDebug() << p;

    MapReader reader(":/road_vector/resource/tug_map.xml");
    for (const auto &item: reader.getWays()) {
        for (const auto &item2: item.tags.keys()) {
            qDebug() << item2 << " " << item.tags[item2];
        }
        qDebug() << "\n\n";
    }
//    return 0;
    return a.exec();
}
