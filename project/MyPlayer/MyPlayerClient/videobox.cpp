#include "videobox.h"
#include "ui_videobox.h"
#include"util.h"
#include <QDir>


VideoBox::VideoBox(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VideoBox)
{
    ui->setupUi(this);
    // 默认隐藏删除按钮
    ui->delVideoBtn->hide();

    playerPage=new PlayerPage();

    //安装事件拦截器
    ui->imageBox->installEventFilter(this);
    ui->videoTitle->installEventFilter(this);

}

VideoBox::~VideoBox()
{
    delete ui;
}

//拦截鼠标点击信号
bool VideoBox::eventFilter(QObject *watched, QEvent *event)
{
    // 拦截视频封⾯和视频标题的⿏标点击事件
    if(ui->imageBox == watched || ui->videoTitle == watched){
        if(QEvent::MouseButtonPress == event->type()){
            onPlayCliecked();
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}

//show
void VideoBox::onPlayCliecked()
{
    //显示播放窗口
    playerPage->show();
    //mpv test
    QDir dir = QDir::current(); // qtcreate中拿到的是exe所在⽬录
    dir.cdUp();
    dir.cdUp();
    QString videoPath = dir.absolutePath();
    videoPath += "/videos/111.mp4";
    playerPage->startPlaying(videoPath);
}
