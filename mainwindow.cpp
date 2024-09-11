#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QPainterPath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    QPainterPath p(QPointF(0, 0));
    p.lineTo(1000, 800);
    p.lineTo(3000, 400);
    p.lineTo(4000, 200);
    p.lineTo(2000, 100);
    qDebug() << "";
    scene->addPath(p);
}

MainWindow::~MainWindow()
{
    delete ui;
}
