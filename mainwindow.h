#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QVector>
#include "waynode.h"
#include "mapreader.h"
#include "coordinate_transformation.h"
#include "MapAdjacencyList.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(
            MapReader& map_reader,
            const Transformer &transform = Transformer(1, {117, 39}),
            QWidget *parent = nullptr
        );
    ~MainWindow();

    QString closest_point(QPointF current);

    QVector<QString> find_way(const QString& original_node, const QString& dest_node);

    void choose_mode();

    void click_mode();

private slots:
    void on_zoomIn_clicked() const;

    void on_zoomOut_clicked() const;

    void on_startBtn_clicked();

    void on_enableBox_stateChanged(int arg1) const;

    void on_clearBtn_clicked();

    void markPosition(QGraphicsSceneMouseEvent* event);

    void on_originBtn_clicked();

    void on_destBtn_clicked();

    void on_originalCombo_currentIndexChanged(int index) const;

    void on_destinationCombo_currentIndexChanged(int index) const;

private:
    void init(MapReader& map_reader);

    void drawLack(const WayNode &node);

    void drawRoad(const WayNode &node) const;

    void drawBuilding(const WayNode &node);

    bool eventFilter(QObject *watched, QEvent *event) override;

    void mouseEvent(QEvent *event) const;

    void wheelEvent(QEvent *event) const;

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QVector<WayNode> ways;
    double scale;
    QColor color;
    Transformer transformer;
    MapReader map_reader;
    MapAdjacencyList adjacency_list;
    QGraphicsPathItem *path;
    bool addOriginalEnabled;
    bool addDestinationEnabled;
    QGraphicsEllipseItem *original_item;
    QGraphicsEllipseItem *destination_item;

};
#endif // MAINWINDOW_H
