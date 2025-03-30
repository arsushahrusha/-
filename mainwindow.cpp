#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QResizeEvent>
#include <QDebug>
#include <QPainter>
#include <QDoubleValidator>
#include <QWheelEvent>
#include <QMouseEvent>
#include <cmath>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // 1. Создаем сцену
    scene = new QGraphicsScene(this);
    ui->setupUi(this);
    ui->drawingArea->setScene(scene);
    ui->drawingArea->setRenderHint(QPainter::Antialiasing);
    ui->drawingArea->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    ui->drawingArea->setDragMode(QGraphicsView::ScrollHandDrag);
    if (backgroundImage.load(":/back")) {

        backgroundItem = new QGraphicsPixmapItem(backgroundImage);
        scene->addItem(backgroundItem);

        QDoubleValidator *doubleValidator1 = new QDoubleValidator(0.0, 360.0, 2, this); //от 0.0 до 360.0, 2 знака после запятой
        ui->lineEdit->setValidator(doubleValidator1);
        //Настройка разделителя точкой
        doubleValidator1->setLocale(QLocale::English);

        QDoubleValidator *doubleValidator2 = new QDoubleValidator(0.0, 10.0, 2, this); //от 0.0 до 10.0, 2 знака после запятой
        ui->lineEdit_2->setValidator(doubleValidator2);
        //Настройка разделителя точкой
        doubleValidator2->setLocale(QLocale::English);

        QDoubleValidator *doubleValidator3 = new QDoubleValidator(0.0, 360.0, 2, this); // От 0.0 до 360.0, 2 знака после запятой
        ui->lineEdit_3->setValidator(doubleValidator3);
        //Настройка разделителя точкой
        doubleValidator3->setLocale(QLocale::English);

        QDoubleValidator *doubleValidator4 = new QDoubleValidator(0.0, 10.0, 2, this); // От 0.0 до 10.0, 2 знака после запятой
        ui->lineEdit_4->setValidator(doubleValidator4);
        //Настройка разделителя точкой
        doubleValidator4->setLocale(QLocale::English);

        pointsDrawn = false;
        connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
        connect(ui->pushButton2, &QPushButton::clicked, this, &MainWindow::on_pushButton2_clicked);
        connect(ui->pushButton3, &QPushButton::clicked, this, &MainWindow::on_pushButton3_clicked);

        scene->setSceneRect(0, 0, ui->drawingArea->width(), ui->drawingArea->height());

        resizeEvent(nullptr);

    } else {
                qDebug() << "Не удалось загрузить изображение!";
    }


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{

    QMainWindow::resizeEvent(event);


    int viewWidth = ui->drawingArea->width();
    int viewHeight = ui->drawingArea->height();

     QPixmap scaledPixmap = backgroundImage.scaled(viewWidth, viewHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    backgroundItem->setPixmap(scaledPixmap);

    backgroundItem->setPos(0, 0);

    scene->setSceneRect(0,0, viewWidth, viewHeight);
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    double zoomFactor = 1.05;
    if (event->angleDelta().y() > 0) {
        zoom(zoomFactor);
    } else {
        zoom(1.0 / zoomFactor);
    }
    event->accept();
}

void MainWindow::zoom(double factor) {
    qreal currentScale = ui->drawingArea->transform().m11();
    qreal minScale = 1.0;
    qreal maxScale = 5.0;

    if ((factor > 1.0 && currentScale < maxScale) || (factor < 1.0 && currentScale > minScale)) {
        ui->drawingArea->scale(factor, factor);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        lastMousePos = event->pos();
        dragging = true;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging) {
       QPointF delta = event->pos() - lastMousePos;

       QTransform currentTransform = ui->drawingArea->transform();

       QPointF newPos = currentTransform.map(delta);


       QRectF sceneRect = scene->sceneRect();
       QRectF viewportRect = ui->drawingArea->viewport()->rect();

       QRectF visibleSceneRect;
       visibleSceneRect.setTopLeft(currentTransform.inverted().map(viewportRect.topLeft()));
       visibleSceneRect.setBottomRight(currentTransform.inverted().map(viewportRect.bottomRight()));

       qreal dx = 0;
       qreal dy = 0;
       if (visibleSceneRect.left() + newPos.x()  > sceneRect.left()) {
                dx = sceneRect.left() - visibleSceneRect.left();
       }
       if (visibleSceneRect.right() + newPos.x() < sceneRect.right()) {
                dx = sceneRect.right() - visibleSceneRect.right();
       }

       if (visibleSceneRect.top() + newPos.y()  > sceneRect.top()) {
                dy = sceneRect.top() - visibleSceneRect.top();
       }
       if (visibleSceneRect.bottom() + newPos.y() < sceneRect.bottom()) {
           dy = sceneRect.bottom() - visibleSceneRect.bottom();
       }

       ui->drawingArea->translate(dx, dy);

       lastMousePos = event->pos();
   }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = false;
    }
}

void MainWindow::drawLine(QGraphicsEllipseItem* point1, QGraphicsEllipseItem* point2)
{
    int viewWidth = ui->drawingArea->width();
    int viewHeight = ui->drawingArea->height();

    qreal x1 = point1->rect().center().x();
    qreal y1 = point1->rect().center().y();
    qreal x2 = point2->rect().center().x();
    qreal y2 = point2->rect().center().y();
    qreal centerX = viewWidth / 2.0;
    qreal centerY = viewHeight / 2.0;
    qreal angle = atan2(y2 - y1, x2 - x1);
    qreal rayLength = qMax(viewWidth, viewHeight) * 2.0;
    qreal xEnd = x1 + rayLength * cos(angle);
    qreal yEnd = y1 + rayLength * sin(angle);
    qreal xEnd2 = centerX + rayLength * cos(angle);
    qreal yEnd2 = centerY + rayLength * sin(angle);
    ray1 = new QGraphicsLineItem(x1, y1, xEnd, yEnd);
    ray2 = new QGraphicsLineItem(centerX, centerY, xEnd2, yEnd2);
    ray3 = new QGraphicsLineItem(x1, y1, x2, y2);
    QPen pen;
    pen.setColor(QColor(255, 0, 0));
    pen.setWidthF(1);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    ray1->setPen(pen);
    scene->addItem(ray1);
    ray2->setPen(pen);
    scene->addItem(ray2);
    pen.setColor(QColor(0, 255, 0));
    pen.setWidthF(1.2);
    ray3->setPen(pen);
    scene->addItem(ray3);
}

QGraphicsEllipseItem* MainWindow::drawPoint(double angle, double dist)
{
    int viewWidth = ui->drawingArea->width();
    int viewHeight = ui->drawingArea->height();
    dist = 4.75*dist;
    double angleRad = ((angle-90) * M_PI) / 180.0;
    double centerX = viewWidth / 2;
    double centerY = viewHeight / 2;
    double radius = 2.0;
    double x = centerX + dist * cos(angleRad)-radius/2;
    double y = centerY + dist * sin(angleRad)-radius/2;
    QGraphicsEllipseItem *point = new QGraphicsEllipseItem(x, y, radius, radius);
    return point;
}

void MainWindow::on_pushButton_clicked()
{
    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidthF(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    if (point1&&point2) {
        if (ray1&&ray2&&ray3) {
            scene->removeItem(ray1);
            delete ray1;
            ray1 = nullptr;
            scene->removeItem(ray2);
            delete ray2;
            ray2 = nullptr;
            scene->removeItem(ray3);
            delete ray3;
            ray3 = nullptr;
        }
        scene->removeItem(point1);
        delete point1;
        point1 = nullptr;
        scene->removeItem(point2);
        delete point2;
        point2 = nullptr;
        QString text = ui->lineEdit->text();
        double angle = text.toDouble();
        QString text2 = ui->lineEdit_2->text();
        double dist = text2.toDouble();
        QString text3 = ui->lineEdit_3->text();
        double angle2 = text3.toDouble();
        QString text4 = ui->lineEdit_4->text();
        double dist2 = text4.toDouble();
        point1 = drawPoint(angle, dist);
        point2 = drawPoint(angle2, dist2);
        point1->setPen(pen);
        scene->addItem(point1);
        point2->setPen(pen);
        scene->addItem(point2);
    } else {
        QString text = ui->lineEdit->text();
        double angle = text.toDouble();
        QString text2 = ui->lineEdit_2->text();
        double dist = text2.toDouble();
        QString text3 = ui->lineEdit_3->text();
        double angle2 = text3.toDouble();
        QString text4 = ui->lineEdit_4->text();
        double dist2 = text4.toDouble();
        point1 = drawPoint(angle, dist);
        point2 = drawPoint(angle2, dist2);
        point1->setPen(pen);
        scene->addItem(point1);
        point2->setPen(pen);
        scene->addItem(point2);
    }

}

void MainWindow::on_pushButton2_clicked()
{
    if(point1 && point2){
        if (ray1 && ray2 && ray3) {
            scene->removeItem(ray1);
            delete ray1;
            ray1 = nullptr;
            scene->removeItem(ray2);
            delete ray2;
            ray2 = nullptr;
            scene->removeItem(ray3);
            delete ray3;
            ray3 = nullptr;
            drawLine(point1, point2);
            double x1 = point1->rect().center().x();
            double y1 = point1->rect().center().y();
            double x2 = point2->rect().center().x();
            double y2 = point2->rect().center().y();
            QString len_vp = QString::number((pow(pow(y2-y1, 2)+pow(x2-x1, 2), 0.5))/4.75);
            ui->lineEdit_8->setText(len_vp);
        }
        else {
            drawLine(point1, point2);
            double x1 = point1->rect().center().x();
            double y1 = point1->rect().center().y();
            double x2 = point2->rect().center().x();
            double y2 = point2->rect().center().y();
            QString len_vp = QString::number((pow(pow(x2-x1, 2)+pow(y2-y1, 2), 0.5))/4.75);
            QString angle_vp = QString::number((atan2(y2-y1, x2-x1)*180)/M_PI + 90);
            ui->lineEdit_8->setText(len_vp);
            ui->lineEdit_7->setText(angle_vp);
        }
    }
    else{
             qDebug() << "Недостаточно точек для отрисовки луча.";
    }
}

void MainWindow::on_pushButton3_clicked()
{
    QPen pen;
    pen.setColor(QColor(0, 0, 255));
    pen.setWidthF(1);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    if (ray0 && point3) {
        scene->removeItem(ray0);
        delete ray0;
        ray0 = nullptr;
        scene->removeItem(point3);
        delete point3;
        point3 = nullptr;
        int viewWidth = ui->drawingArea->width();
        int viewHeight = ui->drawingArea->height();
        qreal centerX = viewWidth / 2.0;
        qreal centerY = viewHeight / 2.0;
        QString text = ui->lineEdit_5->text();
        double angle = ((text.toDouble()-90) * M_PI) / 180.0;
        QString text2 = ui->lineEdit_6->text();
        double dist = text2.toDouble()*4.75;
        qreal xend = centerX + dist * cos(angle);
        qreal yend = centerY + dist * sin(angle);
        ray0 = new QGraphicsLineItem(centerX, centerY, xend, yend);
        ray0->setPen(pen);
        scene->addItem(ray0);
        double radius = 2.0;
        point3 = new QGraphicsEllipseItem(xend-radius/2, yend-radius/2, radius, radius);
        point3->setPen(pen);
        scene->addItem(point3);
    } else {
        int viewWidth = ui->drawingArea->width();
        int viewHeight = ui->drawingArea->height();
        qreal centerX = viewWidth / 2.0;
        qreal centerY = viewHeight / 2.0;
        QString text = ui->lineEdit_5->text();
        double angle = ((text.toDouble()-90) * M_PI) / 180.0;
        QString text2 = ui->lineEdit_6->text();
        double dist = text2.toDouble()*4.75;
        qreal xend = centerX + dist * cos(angle);
        qreal yend = centerY + dist * sin(angle);
        ray0 = new QGraphicsLineItem(centerX, centerY, xend, yend);
        ray0->setPen(pen);
        scene->addItem(ray0);
        double radius = 2.0;
        point3 = new QGraphicsEllipseItem(xend-radius/2, yend-radius/2, radius, radius);
        point3->setPen(pen);
        scene->addItem(point3);
    }

}
