#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "coordinate_transformation.h"

#include <QPainterPath>
#include <QGraphicsItem>
#include <QEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    scale = 1;
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::RenderHint::Antialiasing, true);
    ui->graphicsView->installEventFilter(this);
    ui->graphicsView->viewport()->installEventFilter(this);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    transformer = Transformer(1, {117, 39});

    auto c = this->palette().base().color();
    color = QColor(255 - c.red(), 255 - c.green(), 255 - c.blue());
    init();


}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
//    if (watched != ui->graphicsView) {
//        return QObject::eventFilter(watched, event);
//    }




    switch (event->type()) {
        case QEvent::MouseButtonPress:

        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
            mouseEvent(event);
            break;
        case QEvent::Wheel:
            wheelEvent(event);
            break;
    }

    switch (event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::Wheel:
            event->accept();
            return true;
    }

    return QObject::eventFilter(watched, event);
}

QPointF mapToRelative(QPointF mapCoord, QPointF mapTopLeft, QPointF mapBottomRight) {
    QPointF relativeCoord;
    relativeCoord.setX((mapCoord.x() - mapTopLeft.x()) / (mapBottomRight.x() - mapTopLeft.x()));
    relativeCoord.setY((mapCoord.y() - mapTopLeft.y()) / (mapBottomRight.y() - mapTopLeft.y()));
    return relativeCoord;
}
void MainWindow::wheelEvent(QEvent *event) {
    auto e = dynamic_cast<QWheelEvent *>(event);
    if (e->angleDelta().y() < 0) {
        ui->zoomIn->click();
    } else {
        ui->zoomOut->click();
    }
}
void MainWindow::mouseEvent(QEvent *event) {
    static bool isPressed = false;
    static QPoint lastPressed;
    auto e = dynamic_cast<QMouseEvent *>(event);
    if (event->type() == QEvent::MouseButtonPress) {
        if (e->button() == Qt::LeftButton) {
            lastPressed = e->pos();
            isPressed = true;
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        if (e->button() == Qt::LeftButton) {
            isPressed = false;
        }
    } else if (event->type() == QEvent::MouseMove) {
        if (!isPressed) {
            return;
        }
        int dx = e->pos().x() - lastPressed.x();
        int dy = e->pos().y() - lastPressed.y();
        lastPressed = e->pos();
        auto h = ui->graphicsView->horizontalScrollBar();
        auto v = ui->graphicsView->verticalScrollBar();
        h->setValue(-dx + h->value());
        v->setValue(-dy + v->value());
    }
}


void MainWindow::init() {
    MapReader reader(":/road_vector/resource/tug_map.xml");
    ways = reader.getWays();
    for (const auto &item: ways) {
        if (item.contain("building")) {
            drawBuilding(item);
        } else if (item.contain("highway")) {
            drawRoad(item);
        } else {
            drawLack(item);
        }
        QPointF center = transformer(getCenter(item));

        const auto &c = item.tags.find("name");
        if (c != item.tags.end()) {
            QGraphicsItem *text = scene->addText(c.value());
            auto temp = center;
            center.setX(center.x() - text->boundingRect().size().width() / 2);
            center.setY(center.y());
            text->setPos(center);
            text->setZValue(3);
            temp.setY(temp.y());
            scene->addEllipse(temp.x(), temp.y(), 10, 10, color, QColor(123, 143, 4))->setZValue(2);
        }
//
//        QGraphicsItem *text = scene->addText(item.id);
//        center.setX(center.x() - text->boundingRect().size().width() / 2);
//        center.setY(center.y() - text->boundingRect().size().height() / 2);
//        text->setPos(center);

    }

}




void MainWindow::on_zoomIn_clicked() {
    ui->graphicsView->scale(0.9, 0.9);


}


void MainWindow::on_zoomOut_clicked() {

    ui->graphicsView->scale(1.1, 1.1);

}

void MainWindow::drawLack(const WayNode &node) {
    QPolygonF poly;
    std::transform(node.paths.begin(), node.paths.end(), std::back_inserter(poly), [this] (auto & a) {
        return transformer(a);
    });
    QColor c;
    if (node.contain("amenity")) {
        c = QColor(0, 255, 0);

    } else if (node.contain("water")) {
        c = QColor(0, 0, 255);
    } else {
        c = this->palette().base().color();
    }
    scene->addPolygon(poly, color, c);
}

void MainWindow::drawRoad(const WayNode &node) {
    QPainterPath path(transformer(node.paths[0]));
    for (int i = 1; i < node.paths.size(); ++i) {
        path.lineTo(transformer(node.paths[i]));
    }

    QPen p;
    p.setWidth(5);
    p.setColor(color);

    scene->addPath(path, p);
}

void MainWindow::drawBuilding(const WayNode &node) {
    QPolygonF poly;
    std::transform(node.paths.begin(), node.paths.end(), std::back_inserter(poly), [this] (auto & a) { return transformer(a);});
    scene->addPolygon(poly, color, QColor(255, 0, 0));

}

MainWindow::~MainWindow() {
    delete scene;
    delete ui;
}





