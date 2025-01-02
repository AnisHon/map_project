#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "coordinate_transformation.h"

#include <QGraphicsItem>

#include <QScrollBar>
#include <QMessageBox>

#include <QGraphicsSceneMouseEvent>
#include <QGesture>

#include "distance_utils.h"

MainWindow::MainWindow(MapReader &map_reader, const Transformer &transform, QWidget *parent)
    : QMainWindow(parent)
    , transformer(transform)
    , map_reader{map_reader}
    , path{nullptr}
    , adjacency_list{map_reader, transform}
    , addOriginalEnabled(false)
    , addDestinationEnabled(false)
    , isTouch(false)
    , destination_item(nullptr)
    , original_item(nullptr)
    , kd_tree_(nullptr)
    , ui(new Ui::MainWindow) {


    ui->setupUi(this);
    this->scene = new QGraphicsScene(this);
    scale = 1;
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::RenderHint::Antialiasing, true);
    ui->graphicsView->installEventFilter(this);
    ui->graphicsView->viewport()->installEventFilter(this);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->grabGesture(Qt::PinchGesture);
    auto c = this->palette().base().color();
    color = QColor(255 - c.red(), 255 - c.green(), 255 - c.blue());
    init(map_reader);

    ui->originBtn->setDisabled(true);
    ui->destBtn->setDisabled(true);
    // ui->enableBox->setDisabled(true);

    // grabGesture(Qt::GestureType::PinchGesture);


    initKdTree();
}

void MainWindow::initKdTree() {

    const auto points = this->map_reader.getHighWays();
    std::vector<KDTree::Pair> treeNode;
    treeNode.reserve(points.size());
    for (const auto &high_way : points) {
        const auto point = this->adjacency_list.get_location(high_way);
        treeNode.push_back(KDTree::Pair{point.x(), point.y(), high_way});
    }

    this->kd_tree_ = new KDTree(treeNode);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
//    if (watched != ui->graphicsView) {
//        return QObject::eventFilter(watched, event);
//    }
    switch (event->type()) {
        case QEvent::Gesture:
            this->pinchGestureEvent(dynamic_cast<QGestureEvent*>(event));
            break;
        case QEvent::GraphicsSceneMousePress:
        case QEvent::MouseButtonPress:
            this->markPosition(dynamic_cast<QMouseEvent *>(event));
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
            mouseEvent(event);
            break;
        case QEvent::Wheel:
            wheelEvent(event);
            break;

        default: ;
    }
    // qDebug() << "pinchGestureEvent;

    switch (event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::Wheel:
            event->accept();
            return true;
        default: ;
    }

    return QObject::eventFilter(watched, event);
}

QPointF mapToRelative(const QPointF mapCoord, const QPointF mapTopLeft, QPointF mapBottomRight) {
    QPointF relativeCoord;
    relativeCoord.setX((mapCoord.x() - mapTopLeft.x()) / (mapBottomRight.x() - mapTopLeft.x()));
    relativeCoord.setY((mapCoord.y() - mapTopLeft.y()) / (mapBottomRight.y() - mapTopLeft.y()));
    return relativeCoord;
}
void MainWindow::wheelEvent(QEvent *event) const {
    auto e = dynamic_cast<QWheelEvent *>(event);

    const int delta = e->angleDelta().x();
    const int horizontalDelta = e->angleDelta().y();
    const auto h = ui->graphicsView->horizontalScrollBar();
    const auto v = ui->graphicsView->verticalScrollBar();

    if (isTouch) {

        // if (delta != 0) {
            // qDebug() << "delta";

        v->setValue(v->value() - horizontalDelta / 8);
        h->setValue(h->value() - delta / 8);
        // }
        // if (horizontalDelta != 0) {
            // qDebug() << "horizontalDelta";


        // }
    } else {
        if (e->angleDelta().y() < 0) {
            ui->zoomIn->click();
        } else {
            ui->zoomOut->click();
        }
    }

}
void MainWindow::mouseEvent(QEvent *event) const {
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
        const int dx = e->pos().x() - lastPressed.x();
        const int dy = e->pos().y() - lastPressed.y();
        lastPressed = e->pos();
        const auto h = ui->graphicsView->horizontalScrollBar();
        const auto v = ui->graphicsView->verticalScrollBar();
        h->setValue(-dx + h->value());
        v->setValue(-dy + v->value());
    }
}



void MainWindow::init(MapReader &map_reader) {
    MapReader &reader = map_reader;
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

            ui->destinationCombo->addItem(c.value(), QVariant::fromValue(temp));
            ui->originalCombo->addItem(c.value(), QVariant::fromValue(temp));
        }
        // for (auto path_id : item.path_ids) {
        //     ui->destinationCombo->addItem(path_id, QVariant::fromValue(this->adjacency_list.get_location(path_id)));
        //     ui->originalCombo->addItem(path_id, QVariant::fromValue(this->adjacency_list.get_location(path_id)));
        // }


        // ui->originalCombo->addItem(item.id, QVariant::fromValue(temp));
//
//        QGraphicsItem *text = scene->addText(item.id);
//        center.setX(center.x() - text->boundingRect().size().width() / 2);
//        center.setY(center.y() - text->boundingRect().size().height() / 2);
//        text->setPos(center);
    }

    // for (auto high_way : this->map_reader.getHighWays()) {
    //     auto point = this->adjacency_list.get_location(high_way);
    //     const auto graphics_ellipse_item = scene->addEllipse(point.x() - 10, point.y() - 10, 20, 20, color, QColor(255, 0, 0));
    //     graphics_ellipse_item->setZValue(2);
    //     graphics_ellipse_item->setToolTip(high_way);
    //     graphics_ellipse_item->setData(123, QVariant::fromValue(high_way));
    // }

}




void MainWindow::on_zoomIn_clicked() const {
    ui->graphicsView->scale(0.9, 0.9);
}


void MainWindow::on_zoomOut_clicked() const {

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

void MainWindow::drawRoad(const WayNode &node) const {
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
    delete kd_tree_;
}



QString MainWindow::closest_point(const QPointF &current) {
    const auto &highways = this->map_reader.getHighWays();
    return *std::min_element(highways.begin(), highways.end(), [&current, this] (auto &a, auto &b) {
        const QPointF a_point =  adjacency_list.get_location(a);
        const QPointF b_point =  adjacency_list.get_location(b);
        const qreal distance1 = euclidean_distance(current, a_point);
        const qreal distance2 = euclidean_distance(current, b_point);
        return distance1 < distance2;
    });
    // return kd_tree_->find_closest(current.x(), current.y());
}

QVector<QString> MainWindow::find_way(const QString& original_node, const QString& dest_node) {
    const int algorithm_index = ui->algorithmCombo->currentIndex();
    qDebug() << "algorithm: " << (algorithm_index == 0 ? "dijkstra" : "A*");
    if (algorithm_index == 0) {
        return this->adjacency_list.dijkstra(original_node, dest_node);
    } else {
        return this->adjacency_list.a_star(original_node, dest_node);
    }
}

void MainWindow::choose_mode() {
    const auto destination = ui->destinationCombo->currentData().value<QPointF>();
    const auto original = ui->originalCombo->currentData().value<QPointF>();
    if (destination == original) {
        QMessageBox::critical(this, QString("禁止原地TP"), "不允许选择同一个地点，请不要选择同一个地点");
        return;
    }

    const auto original_node = closest_point(original);
    const auto dest_node = closest_point(destination);


    // auto original_node = ui->destinationCombo->currentText();
    // auto dest_node = ui->originalCombo->currentText();



    const auto &path = find_way(original_node, dest_node);

    qDebug() << path;



    QPainterPath q_path(original);
    for (const auto &id : path) {
        q_path.lineTo(this->adjacency_list.get_location(id));
    }
    q_path.lineTo(destination);

    if (this->path != nullptr) {
        scene->removeItem(this->path);
    }

    this->path = scene->addPath(q_path, QPen(QColor(0, 255, 0), 4));
}


void MainWindow::click_mode() {
    if (!original_item || !destination_item) {
        QMessageBox::critical(this, "错误的起点终点", "您没有选起点或终点");
        return;
    }
    const QPointF &original = original_item->data(114514).toPointF();
    const QPointF &destination = destination_item->data(114514).toPointF();

    const auto original_node = closest_point(original);
    const auto dest_node = closest_point(destination);

    // auto original_node = ui->destinationCombo->currentText();
    // auto dest_node = ui->originalCombo->currentText();

    const auto &path = find_way(original_node, dest_node);

    qDebug() << path;



    QPainterPath q_path(original);
    for (const auto &id : path) {
        q_path.lineTo(this->adjacency_list.get_location(id));
    }
    q_path.lineTo(destination);

    if (this->path != nullptr) {
        scene->removeItem(this->path);
    }

    this->path = scene->addPath(q_path, QPen(QColor(0, 255, 0), 4));
}

void MainWindow::on_startBtn_clicked() {
    bool enable_click = ui->enableBox->isChecked();

    if (enable_click) {
        click_mode();
    } else {
        choose_mode();
    }

}


void MainWindow::on_enableBox_stateChanged(const int arg1) const {
    ui->originBtn->setDisabled(!arg1);
    ui->destBtn->setDisabled(!arg1);
    ui->originalCombo->setDisabled(arg1);
    ui->destinationCombo->setDisabled(arg1);
}


void MainWindow::on_clearBtn_clicked() {
    if (this->path != nullptr) {
        scene->removeItem(this->path);
    }
    if (this->original_item) {
        scene->removeItem(this->original_item);
    }

    if (this->destination_item) {
        scene->removeItem(this->destination_item);
    }

    this->path = nullptr;
    this->original_item = nullptr;
    this->destination_item = nullptr;

}

void MainWindow::pinchGestureEvent(const QGestureEvent *event) const {
    // 获取 Pinch 手势

    auto* pinch = dynamic_cast<QPinchGesture*>(event->gesture(Qt::PinchGesture));
    if (pinch) {
        // 获取缩放因子
        const qreal scaleFactor = pinch->scaleFactor();
        // 根据缩放因子进行缩放
        ui->graphicsView->scale(scaleFactor, scaleFactor);
    }
}

void MainWindow::markPosition(const QMouseEvent* event)  {
    const QPointF &point = ui->graphicsView->mapToScene(event->pos());

    if (addOriginalEnabled) {
        // 创建一个小圆点并将其添加到场景中
        if (this->original_item) {
            scene->removeItem(original_item);
        }
        this->original_item = this->scene->addEllipse(point.x() - 5, point.y() - 5, 10, 10, QPen(), Qt::red);
        this->original_item->setData(114514, QVariant::fromValue(point));
        this->original_item->setZValue(3);
    } else if (addDestinationEnabled) {
        // 创建一个小圆点并将其添加到场景中
        if (this->destination_item) {
            scene->removeItem(destination_item);
        }

        this->destination_item = this->scene->addEllipse(point.x() - 5, point.y() - 5, 10, 10, QPen(), Qt::blue);
        this->destination_item->setData(114514, QVariant::fromValue(point));
        this->destination_item->setZValue(3);
    }
    this->addDestinationEnabled = false;
    this->addOriginalEnabled = false;
}

void MainWindow::on_originBtn_clicked() {
    this->addOriginalEnabled = true;
}


void MainWindow::on_destBtn_clicked() {
    this->addDestinationEnabled = true;
}


void MainWindow::on_originalCombo_currentIndexChanged(int index) const {
    const auto &original = ui->originalCombo->currentData().value<QPointF>();
    ui->graphicsView->centerOn(original);
    on_zoomOut_clicked();

}


void MainWindow::on_destinationCombo_currentIndexChanged(int index) const {
    const auto &destination = ui->destinationCombo->currentData().value<QPointF>();
    ui->graphicsView->centerOn(destination);
    on_zoomOut_clicked();
}


void MainWindow::on_touchBox_stateChanged(const int arg1) {
    isTouch = arg1;
}

