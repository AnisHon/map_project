#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QVector>
#include "waynode.h"
#include "mapreader.h"
#include "coordinate_transformation.h"
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
            MapReader& mapReader,
            const Transformer &transform = Transformer(1, {117, 39}),
            QWidget *parent = nullptr
        );
    ~MainWindow();

private slots:
    void on_zoomIn_clicked() const;

    void on_zoomOut_clicked() const;

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

};
#endif // MAINWINDOW_H
