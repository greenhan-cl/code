#include "myplayer.h"
#include "ui_myplayer.h"
#include "util.h"
#include <QGraphicsDropShadowEffect>

MyPlayer::MyPlayer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyPlayer)
{
    ui->setupUi(this);

    initUI();
    //绑定信号槽
    connectSignalAndSlot();

    void onSwitchStackedWidgetPage(int pageId);
}

MyPlayer::~MyPlayer()
{
    delete ui;
}

//单例
MyPlayer *MyPlayer::getInstance()
{
    static MyPlayer* instance=nullptr;
    if(instance==nullptr){
        instance = new MyPlayer();
    }
    return instance;
}

void MyPlayer::initUI()
{
    //去除窗口边框
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    //设置窗口图标
    setWindowIcon(QIcon(":/images/MyPlayer.ico"));
    // QIcon windows_ico(":/images/bitPlayer.ico");
    // if (windows_ico.isNull() == true) {
    //     qDebug() << "Windows icon is nullptr";
    // }
    // this->setWindowIcon(windows_ico);
    // QApplication::setWindowIcon(windows_ico);

    //添加窗口阴影
    QGraphicsDropShadowEffect* dropShow =new QGraphicsDropShadowEffect(this);
    dropShow->setColor(Qt::black);//设置阴影颜色
    dropShow->setBlurRadius(5);//阴影的半径
    dropShow->setOffset(0,0);//阴影偏移量
    ui->MyPlayerBg->setGraphicsEffect(dropShow);//将阴影添加到主窗口

    // 设置bodyLeft中⾃定义按钮⽂本和图⽚
    ui->homePageBtn->setTextColor("#000000"); // 新增
    ui->homePageBtn->setImageAndText(":/images/homePage/shouyexuan.png", "⾸⻚", HomePage);
    ui->sysPageBtn->setTextColor("#999999"); // 新增
    ui->sysPageBtn->setImageAndText(":/images/homePage/admin.png", "系统管理",AdminPage);
    ui->myPageBtn->setTextColor("#999999"); // 新增
    ui->myPageBtn->setImageAndText(":/images/homePage/wode.png", "我的",MyselfPage);
    // 默认情况下，选中⾸⻚，系统管理⻚⾯隐藏
    ui->stackedWidget->setCurrentIndex(0);
}

void MyPlayer::connectSignalAndSlot()
{
    //最小化、关闭窗口
    connect(ui->minBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->quitBtn, &QPushButton::clicked, this, &QWidget::close);
    //切换界面
    connect(ui->homePageBtn, &PageSwitchButton::switchPage, this,
            &MyPlayer::onSwitchStackedWidgetPage);
    connect(ui->myPageBtn, &PageSwitchButton::switchPage, this,
            &MyPlayer::onSwitchStackedWidgetPage);
    connect(ui->sysPageBtn, &PageSwitchButton::switchPage, this,
            &MyPlayer::onSwitchStackedWidgetPage);

    // 我的⻚⾯切换到上传视频⻚⾯
    connect(ui->myPage, &MyselfWidget::switchUploadVideoPage, this, [=](int pageId){
        LOG()<<"切换到上传视频⻚⾯";
        onSwitchStackedWidgetPage(pageId);
    });
    // 视频上传⻚⾯切换到我的⻚⾯
    connect(ui->uploadVideo, &UploadVideoPage::switchMySelfPage, this, [=]
            (int pageId){
                MyPlayer::onSwitchStackedWidgetPage(pageId);
            });
}

void MyPlayer::onSwitchStackedWidgetPage(int pageId)
{
    // stackedWidget切换到pageId⻚⾯，切换后刷新下，保证⻚⾯能⽴⻢显⽰出来
    ui->stackedWidget->setCurrentIndex(pageId);
    repaint();
    // 切换按钮按下后，需要重置按钮上⽂本和图⽚的⾼亮样式
    resetSwitchBtnInfo(pageId);
}

void MyPlayer::resetSwitchBtnInfo(int pageId)
{
    // 重新设置左侧栏未选中按钮颜⾊
    QList<PageSwitchButton*> switchBtns = findChildren<PageSwitchButton*>();
    for(auto switchBtn : switchBtns){
        if(switchBtn->getPageId() != pageId){
            switchBtn->setTextColor("#999999");
        }
    }
    // 重新设置按钮上图⽚
    if(pageId == HomePage){
        ui->homePageBtn->setImage(":/images/homePage/shouyexuan.png");
        ui->myPageBtn->setImage(":/images/homePage/wode.png");
        ui->sysPageBtn->setImage(":/images/homePage/admin.png");
    }else if(pageId == AdminPage){
        ui->homePageBtn->setImage(":/images/homePage/shouye.png");
        ui->myPageBtn->setImage(":/images/homePage/wode.png");
        ui->sysPageBtn->setImage(":/images/homePage/adminxuan.png");
    }else if(pageId == MyselfPage){
        ui->homePageBtn->setImage(":/images/homePage/shouye.png");
        ui->myPageBtn->setImage(":/images/homePage/wodexuan.png");
        ui->sysPageBtn->setImage(":/images/homePage/admin.png");
    }else{
        qDebug()<<"暂不⽀持⻚⾯："<<pageId;
    }

}

void MyPlayer::mousePressEvent(QMouseEvent *event)
{
    // 获取⿏标按下时的位置
    QPoint point = event->position().toPoint();
    // 如果⿏标按下位置在标题栏，即head区域则进⾏拖拽
    if(ui->head->geometry().contains(point)){
        if(event->button() == Qt::LeftButton){
            // 记录⿏标按下位置和窗⼝左上⻆的相对距离
            // 当拖拽窗⼝时候，保证⿏标相对于窗⼝左上⻆的位置不变，即更新窗⼝左上⻆位置
            // 既可实现窗⼝拖拽
            dragPos = event->globalPosition().toPoint() - geometry().topLeft();
            return;
        }
    }
    QWidget::mousePressEvent(event);
}

void MyPlayer::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->position().toPoint();
    if(ui->head->geometry().contains(point)){
        if(event->buttons() == Qt::LeftButton){
            // 当⿏标移动时，保证⿏标位置相对于窗⼝左上⻆位置不变，则需要move窗⼝的位置
            move(event->globalPosition().toPoint() - dragPos);
            return;
        }
    }
    QWidget::mouseMoveEvent(event);
}
