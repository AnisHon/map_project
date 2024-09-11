#include "mainwindow.h"

#include <QApplication>
#include "mapreader.h"
#include "mapnode.h"
#include "coordinate_transformation.h"
int main(int argc, char *argv[])
{
    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    MapReader reader(":/road_vector/resource/tug_map.xml");
    for (const auto &item: reader.getWays()) {

    }
//    return 0;
    return a.exec();
}
