#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QPixmap>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;


    void drawLine(QGraphicsEllipseItem* point1, QGraphicsEllipseItem* point2);
    void on_pushButton_clicked();
    void on_pushButton2_clicked();

    void on_pushButton3_clicked();

private:
    Ui::MainWindow *ui;
    QGraphicsView *drawingArea;
    QGraphicsScene *scene;
    QPixmap backgroundImage;
    QGraphicsPixmapItem* backgroundItem;

    QPoint lastMousePos;
    bool dragging;
    void setupScene();
    void drawSomeElements();
    void zoom(double factor);


    QList<QGraphicsEllipseItem*> points;
    QGraphicsEllipseItem* drawPoint(double angle, double dist);
    QGraphicsEllipseItem* point1 = nullptr;
    QGraphicsEllipseItem* point2 = nullptr;
    QGraphicsEllipseItem* point3 = nullptr;

    QGraphicsLineItem* ray0 = nullptr; // указатель на собственную скорость
    QGraphicsLineItem* ray1 = nullptr; // Указатель на первый луч
    QGraphicsLineItem* ray2 = nullptr; // Указатель на второй луч
    QGraphicsLineItem* ray3 = nullptr; // указатель на vp
    bool pointsDrawn=false;
    bool rayDrawn=false;
};


#endif // MAINWINDOW_H
