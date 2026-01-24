#include "startuppage.h"
#include<QLabel>
#include<QTimer>

startupPage::startupPage(QWidget *parent)
    : QDialog{parent}
{
    // Qt::FramelessWindowHint：去掉窗⼝的边框，即没有标题栏
    // Qt::Tool：将窗⼝设置为⼯具窗⼝，图标不在标题栏显⽰
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    setFixedSize(1450,860);
    setStyleSheet("background-color:#FFFFFF");

    QLabel* imageLabel=new QLabel(this);
    QPixmap pixmap(":/images/startupPage/MyPlayer.png");
    if (!pixmap.isNull()) {
        imageLabel->setPixmap(pixmap);
        imageLabel->adjustSize();  // 自动调整大小以适应图片
        imageLabel->move(524,373);
    }
}

void startupPage::startTimer()
{
    QTimer* timer=new QTimer();
    connect(timer,&QTimer::timeout,this,[=]{
        timer->stop();
        delete timer;
        close();
    });
    //定时两秒
    timer->start(2000);

    //自动登录 TODO

}
