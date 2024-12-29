#include "mainwindow.h"

#include <QApplication>
#include "mapreader.h"
#include "mapnode.h"
#include "coordinate_transformation.h"


struct A {
    int a;
public:
    A operator=(const A &b) {
        qDebug() << "q34234";
        return b;
    }
};

A c() {
    return A();
}

int main(int argc, char *argv[])
{
    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


//    MapReader reader(":/road_vector/resource/tug_map.xml");
//    for (const auto &item: reader.getWays()) {
//
//    }

    c() = A();

    return a.exec();
}
