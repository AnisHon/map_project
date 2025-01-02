#include "mainwindow.h"

#include <QApplication>
#include "mapreader.h"
#include "mapnode.h"
#include "coordinate_transformation.h"
#include "MapAdjacencyList.h"
#include <QDebug>

#include "KDNode2D.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);







    MapReader reader(":/road_vector/resource/tug_map.xml");
    const auto& transform = Transformer(1, {117, 39});
    MapAdjacencyList mdl{reader, transform};









    MainWindow w(reader, transform);
    w.show();

    return a.exec();
}
