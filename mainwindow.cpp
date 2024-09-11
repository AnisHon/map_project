#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "coordinate_transformation.h"

#include <QPainterPath>
#include <QGraphicsItem>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    MapReader reader(":/road_vector/resource/tug_map.xml");
    ways = reader.getWays();
    init();
//    ui->graphicsView->scale(0.8, 0.8);

}

QPointF mapToRelative(QPointF mapCoord, QPointF mapTopLeft, QPointF mapBottomRight) {
    QPointF relativeCoord;
    relativeCoord.setX((mapCoord.x() - mapTopLeft.x()) / (mapBottomRight.x() - mapTopLeft.x()));
    relativeCoord.setY((mapCoord.y() - mapTopLeft.y()) / (mapBottomRight.y() - mapTopLeft.y()));
    return relativeCoord;
}

void MainWindow::init() {

    for (const auto &item: ways) {
        if (item.tags.find("building") != item.tags.end()) {
            drawBuilding(item);
        } else if (item.tags.find("water") != item.tags.end()) {
            drawLack(item);
        } else {
            drawRoad(item);
        }

        QPointF center = getCenter(item);
        const auto &c = item.tags.find("name");
        if (c != item.tags.end()) {
            QGraphicsItem *text = scene->addText(c.value());
            text->setPos(mercatorToScreen(center));
        }

    }

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_zoomIn_clicked() {
    scale /= 1.1;
    ui->graphicsView->scale(scale, scale);
}


void MainWindow::on_zoomOut_clicked() {
    scale *= 1.1;
    ui->graphicsView->scale(scale, scale);
}

void MainWindow::drawLack(const WayNode &node) {
    QPolygonF poly;
    std::transform(node.paths.begin(), node.paths.end(), std::back_inserter(poly), [] (auto & a) {
        return mercatorToScreen(a);
    });
    scene->addPolygon(poly, QColor(0, 0, 255), QColor(0, 0, 255));
}

void MainWindow::drawRoad(const WayNode &node) {
    QPainterPath path(mercatorToScreen(node.paths[0]));
    for (int i = 1; i < node.paths.size(); ++i) {
        path.lineTo(mercatorToScreen(node.paths[i]));
    }

    QPen p;

    p.setColor(QColor(255, 255, 255));

    scene->addPath(path, p);
}

void MainWindow::drawBuilding(const WayNode &node) {
    QPolygonF poly;
    std::transform(node.paths.begin(), node.paths.end(), std::back_inserter(poly), [] (auto & a) {
        return mercatorToScreen(a);
    });
    scene->addPolygon(poly, QColor(255, 255, 255), QColor(255, 0, 0));
}

