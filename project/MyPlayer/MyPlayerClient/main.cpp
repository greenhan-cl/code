#include "myplayer.h"
#include"startuppage.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::Floor);
    QApplication a(argc, argv);

    //创建启动页
    startupPage startupPage;
    startupPage.show();
    startupPage.startTimer();
    startupPage.exec();
    //主界面
    MyPlayer* myPlayer=MyPlayer::getInstance();
    myPlayer->show();
    return a.exec();
}
