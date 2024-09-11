#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QVector>
#include "waynode.h"
#include "mapreader.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_zoomIn_clicked();

    void on_zoomOut_clicked();

private:
    void init();

    void drawLack(const WayNode &node);

    void drawRoad(const WayNode &node);

    void drawBuilding(const WayNode &node);

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QVector<WayNode> ways;
    double scale;


};
#endif // MAINWINDOW_H
