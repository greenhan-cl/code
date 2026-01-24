#include "playslider.h"
#include "ui_playslider.h"
#include "util.h"
#include <QMouseEvent>

PlaySlider::PlaySlider(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlaySlider)
{
    ui->setupUi(this);
    playGrogress = 0;
    moveSlider();
}

PlaySlider::~PlaySlider()
{
    delete ui;
}

//鼠标按下
void PlaySlider::mousePressEvent(QMouseEvent *event)
{
    // 当⿏标左键按下时，记录当前⿏标的x坐标
    // 以该x坐标为outLine的宽度，更新outLine的geometry
    if(event->button() == Qt::LeftButton){
        playGrogress = event->pos().x();
        moveSlider();
        return;
    }
    QWidget::mousePressEvent(event);
}

//鼠标按下
void PlaySlider::mouseReleaseEvent(QMouseEvent *event)
{
    // 当⿏标按下时，记录当前⿏标的x坐标
    // 以该x坐标为outLine的宽度，更新outLine的geometry
    if(event->button() == Qt::LeftButton){
        playGrogress = event->pos().x();
        moveSlider();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void PlaySlider::mouseMoveEvent(QMouseEvent *event)
{
    // ⿏标在播放进度窗⼝内拖动时
    // 检测⿏标位置是否在播放进度框中
    QRect playSliderRect = QRect(0, 0, this->width(), this->height());
    if(!playSliderRect.contains(event->pos())){
        LOG()<<playSliderRect<<"<-->"<<event->pos();
        return;
    }
    if(event->buttons() == Qt::LeftButton){
        playGrogress = event->pos().x();
        // 播放进度不能为负数
        if(playGrogress < 0){
            playGrogress = 0;
        }
        // 播放进度不能超过outLine的最⼤宽度
        int maxWidth = this->width();
        if(playGrogress > maxWidth){
            playGrogress = maxWidth;
        }
        moveSlider();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void PlaySlider::moveSlider()
{
    // 根据playGrogress或⿏标的x坐标位置设置outLine的geometry，以突出播放进度
    ui->outLine->setGeometry(ui->outLine->x(), ui->outLine->y(),
                             playGrogress, ui->outLine->height());
}


