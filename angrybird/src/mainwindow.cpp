#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Enable the event Filter
    qApp->installEventFilter(this);

    //
    originalPos_X = 6.0f;
    originalPos_Y = 9.0f;

    keep = 0 ;
    status = 0 ;
    BirdNum = 0;
    createTimer = new QTimer();
    connect(createTimer,SIGNAL(timeout()),this,SLOT(createBird()));
    pixmapTimer = new QTimer();
    shooter= new QGraphicsPixmapItem();
    shooter->setPixmap(QPixmap(":/shooter.png"));
    shooter->setPos(140,300);
    //
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent *)
{
    // Setting the QGraphicsScene
    scene = new QGraphicsScene(0,0,width(),ui->graphicsView->height());
    ui->graphicsView->setScene(scene);
    //ui->graphicsView->setFixedHeight(600);
    //ui->graphicsView->setFixedWidth(800);

//
    QPushButton *restart =  new QPushButton();
    restart->setGeometry(QRect(QPoint(680,0),QSize(55,55)));
    QPixmap pixmap(":/restart.png");
    QIcon ButtonIcon(pixmap);
    restart->setIcon(ButtonIcon);
    restart->setIconSize(pixmap.rect().size());
    scene->addWidget(restart);
    connect(restart,SIGNAL(clicked(bool)),this,SLOT(restart()));
//

    world = new b2World(b2Vec2(0.0f, -9.8f));
    // Create world

    GameItem::setGlobalSize(QSizeF(32,18),QSize(800,600));

    scene->addItem(shooter);
    shooter->show();

    initialGame();

    // Timer
    connect(&timer,SIGNAL(timeout()),this,SLOT(tick()));
    connect(this,SIGNAL(quitGame()),this,SLOT(QUITSLOT()));
    timer.start(100/6);
}

bool MainWindow::eventFilter(QObject *, QEvent *event)
{
    mouseEvent = (QMouseEvent *) event ;

    if(event->type() == QEvent::MouseButtonPress && BirdNum != 2)
    {
        if(status == 0){
        keep = 1 ;
        mousePress_X =  (mouseEvent->pos().x()-23) /800.0*32.0;
        mousePress_Y = 18.0-((mouseEvent->pos().y()-46) *18 / 600.0);
        status = 1;
        }
        else if(status == 3 && Ability == 1){
            Ability = birdList[BirdNum]->Ability();
            status = 4 ;
        }
    }
    if(event->type() == QEvent::MouseMove && keep >= 1 && BirdNum != 2)
    {
       if(status == 1 || status == 2){
            float pos_X = (mouseEvent->pos().x()-23) /800.0*32.0;
            float pos_Y = 18.0-((mouseEvent->pos().y()-46) *18 / 600.0);

            if( (originalPos_X - pos_X)* (originalPos_X - pos_X) + (originalPos_Y - pos_Y)* (originalPos_Y - pos_Y) <= 25)
                birdList[BirdNum]->getBody().SetTransform(b2Vec2(pos_X,pos_Y),0);

            status = 2;
       }

    }

    if(event->type() == QEvent::MouseButtonRelease && BirdNum != 2)
    {
        if(status == 2){
            keep = 0;
            mouseRelease_X =  (mouseEvent->pos().x()-23) /800.0*32.0;
            mouseRelease_Y = 18.0-((mouseEvent->pos().y()-46) *18 / 600.0);
            float positionNow_X = birdList[BirdNum]->getBody().GetPosition().x;
            float positionNow_Y = birdList[BirdNum]->getBody().GetPosition().y;

            birdList[BirdNum]->getBody().SetLinearVelocity(b2Vec2((positionNow_X- originalPos_X)*-5,(positionNow_Y- originalPos_Y)*-5 ));
            birdList[BirdNum]->getBody().SetGravityScale(1);
            birdList[BirdNum]->getBirdType();
            if(birdList[BirdNum] == 0){
               birdList[BirdNum]->Ability();
               createTimer->start(500);
               status = 0 ;
            }
            else{
                Ability = 1;
                status = 3;
            }
        }
        else if(status == 4 ){
            createTimer->start(10);
            status = 0 ;
        }
    }
    return false;
}

void MainWindow::closeEvent(QCloseEvent *)
{
    // Close event
    emit quitGame();
}

//
void MainWindow::initialGame()
{
    //wood
    itemList.push_back(new Wood(20.f,5.0f,0.03*10,0.03*100,&timer,QPixmap(":/thinwood1.png"),world,scene));
    itemList.push_back(new Wood(29.f,5.0f,0.03*10,0.03*100,&timer,QPixmap(":/thinwood1.png"),world,scene));
    itemList.push_back(new Wood(24.f,5.0f,0.03*10,0.03*100,&timer,QPixmap(":/thinwood1.png"),world,scene));
    itemList.push_back(new Wood(28.f,5.0f,0.03*10,0.03*100,&timer,QPixmap(":/thinwood1.png"),world,scene));
    itemList.push_back(new Wood(23.f,5.0f,0.03*10,0.03*100,&timer,QPixmap(":/thinwood1.png"),world,scene));
    //ground
    itemList.push_back(new Land(16,1.5,32,3,QPixmap(":/ground.png").scaled(width(),height()/6.0),world,scene));
    //pig
    Pig *pig = new Pig(25.0f,5.0f,0.27f,&timer,QPixmap(":/pig.png").scaled(height()/9.0,height()/9.0),world,scene);
    itemList.push_back(pig);
    //bird
    Bird *bird1 = new Bird(6.0f,9.0f,0.27f,&timer,QPixmap(":/bird.png").scaled(height()/9.0,height()/9.0),world,scene);
    birdList.push_back(bird1);
    itemList.push_back(bird1);

    YellowBird *bird2 = new YellowBird(100.0f,5.0f,0.27f,&timer,QPixmap(":/yellow.png").scaled(height()/9.0,height()/9.0),world,scene);
    birdList.push_back(bird2);
    itemList.push_back(bird2);
    scene->update();
}
//
//
void MainWindow::destoryTtem()
{
    foreach(GameItem *i ,itemList){
        if (i->getHP() <=0){
            itemList.removeOne(i);
            delete i ;
        }
    }
}
//
//
void MainWindow::hit()
{
    for(int i = 5 ; i < itemList.size();i++){
        for (b2ContactEdge* edge = itemList[i]->getBody().GetContactList(); edge; edge = edge->next){
            itemList[i]->hurt(edge->other->GetLinearVelocity().Length());
            itemList[i]->ifHit = 1;
        }
    }
}
//

void MainWindow::tick()
{
    destoryTtem();
    world->Step(1.0/60.0,6,2);
    hit();
    scene->update();
}

void MainWindow::QUITSLOT()
{
    // For debug
    std::cout << "Quit Game Signal receive !" << std::endl ;
}
//
void MainWindow::createBird()
{
    BirdNum = BirdNum + 1 ;
    if(BirdNum <= 1){
        birdList[BirdNum]->getBody().SetTransform(b2Vec2(6.0f,9.0f),0);
    }
    else{
        BirdNum = 2;
    }
    createTimer->stop();

}
//
//
void MainWindow::restart()
{
    createTimer->stop();
    timer.stop();
    foreach(Bird * bird ,birdList){
        birdList.removeOne(bird);
    }
    foreach(GameItem * item ,itemList){
        itemList.removeOne(item);
        delete item;
    }

    keep = 0;
    status = 0;
    Ability = 0;
    BirdNum =0;

    initialGame();
    world->Step(1.0/60.0,6,2);
    scene->update();
    timer.start();

}
//
