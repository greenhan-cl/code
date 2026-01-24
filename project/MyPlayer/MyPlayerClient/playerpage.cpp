#include "playerpage.h"
#include "ui_playerpage.h"
#include "toast.h"
#include <QMouseEvent>

PlayerPage::PlayerPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayerPage)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);
    //模态对话框
    setAttribute(Qt::WA_ShowModal);
    volume = new Volume(this);       //实例化音量调节窗口
    playSpeed = new PlaySpeed(this); // 实例化倍数播放窗⼝对象

    connect(ui->minBtn,&QPushButton::clicked,this,&QWidget::showMinimized);
    connect(ui->quitBtn,&QPushButton::clicked,this,&QWidget::close);
    // 显⽰⾳量调节窗⼝
    connect(ui->volumeBtn, &QPushButton::clicked, this,
            &PlayerPage::onVolumeBtnClicked);
    // 绑定倍数播放按钮信号槽
    connect(ui->speedBtn, &QPushButton::clicked, this,
            &PlayerPage::onSpeedBtnClicked);
    // 绑定点赞
    connect(ui->likeImageBtn, &QPushButton::clicked, this,
            &PlayerPage::onLkeImageBtnClcked);
}

PlayerPage::~PlayerPage()
{
    delete ui;
}

void PlayerPage::moveWindows(const QPoint &point)
{
    //音量调节窗口位置
    QPoint newPoint = point + QPoint(this->width() - volume->width() - 13,
                                     533);
    volume->move(newPoint);
    //倍速播放窗口位置
    newPoint = point + QPoint(this->width() - playSpeed->width()-72, 563);
    playSpeed->move(newPoint);
}

void PlayerPage::mousePressEvent(QMouseEvent *event)
{
    // 先要获取⿏标点击时的位置
    QPoint point = event->position().toPoint();
    // 检测该位置是否在head中 且 是⿏标左键单击时
    if(ui->playHead->geometry().contains(point)){
        if(event->button() == Qt::LeftButton){
            dragPos = event->globalPosition().toPoint() - geometry().topLeft();
            return;
        }
    }
    QWidget::mousePressEvent(event);
}

void PlayerPage::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->position().toPoint();
    if(ui->playHead->geometry().contains(point)){
        if(event->buttons() == Qt::LeftButton){
            move(event->globalPosition().toPoint() - dragPos);
            return;
        }
    }
    QWidget::mouseMoveEvent(event);

}

void PlayerPage::onVolumeBtnClicked()
{
    // mapToGlobal(QPoint(0, 0))将PlayerPage窗⼝左上⻆转换为全局坐标
    moveWindows(mapToGlobal(QPoint(0, 0)));
    // 弹出⾳量调节窗⼝
    volume->show();
}

void PlayerPage::onSpeedBtnClicked()
{
    moveWindows(mapToGlobal(QPoint(0, 0)));
    //弹出倍速窗口
    playSpeed->show();
}

//点赞槽函数
void PlayerPage::onLkeImageBtnClcked()
{
    // 检测⽤⼾是否登录，登录时才能点赞
    Login* login = new Login(this);
    login->show();
    Toast::showMessage("先登录，登录完成后才可以进行点赞功能",login);
}
