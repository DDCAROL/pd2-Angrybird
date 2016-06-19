#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QList>
#include <QDesktopWidget>
#include <QTimer>
#include <Box2D/Box2D.h>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <iostream>
#include <QSize>
#include <QPushButton>
#include <QBrush>
#include <QImage>
#include <math.h>
#include <QGraphicsPixmapItem>

#include <gameitem.h>
#include <land.h>
#include <bird.h>
#include <wood.h>
#include <pig.h>
#include <edge.h>
#include <yellowbird.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void showEvent(QShowEvent *);
    bool eventFilter(QObject *,QEvent *event);
    void closeEvent(QCloseEvent *);

    void restartGame();

    void initialGame();
    void destoryTtem();
    void hit();
    void hidePixmap();


signals:
    // Signal for closing the game
    void quitGame();

private slots:
    void tick();
    // For debug slot
    void QUITSLOT();
    void createBird();
    void restart();
private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    b2World *world;
    QList<GameItem *> itemList;
    QTimer timer;
    QList<Bird *> birdList;
    QTimer *createTimer;
    QTimer *pixmapTimer;
    QGraphicsPixmapItem * shooter;
    int keep;
    float mousePress_X,mousePress_Y,mouseRelease_X,mouseRelease_Y;
    QMouseEvent * mouseEvent;
    float originalPos_X,originalPos_Y;
    int status;
    int Ability;
    int BirdNum;
};

#endif // MAINWINDOW_H
