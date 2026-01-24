#include "videobox.h"
#include "ui_videobox.h"
#include"util.h"

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

void VideoBox::onPlayCliecked()
{
    //显示播放窗口
    playerPage->show();
}
