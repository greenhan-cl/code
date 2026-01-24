#include "volume.h"
#include "ui_volume.h"

Volume::Volume(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Volume)
{
    ui->setupUi(this);
    setWindowFlag(Qt::Popup); // 将⾳量调节窗⼝设置为弹出窗⼝
    // 安装拦截器
    ui->volumeBox->installEventFilter(this);
}

Volume::~Volume()
{
    delete ui;
}

bool Volume::eventFilter(QObject *watched, QEvent *event)
{
    if(ui->volumeBox == watched){
        // ⿏标按下事件
        if(event->type() == QEvent::MouseButtonPress){
            calcVolume();
        }else if(event->type() == QEvent::MouseButtonRelease){
            // ⿏标释放
            ;
        }else if(event->type() == QEvent::MouseMove){
            calcVolume();
        }
        return true;
    }
    return QObject::eventFilter(watched, event);
}

void Volume::calcVolume()
{
    int y = ui->volumeBox->mapFromGlobal(QCursor().pos()).y();
    // 验证y的正确性，y的范围在[5, 150]之间
    y = y < 5? 5 : y;
    y = y > 150? 150 : y;
    // 更新outLine的⾼度
    ui->outLine->setGeometry(ui->outLine->x(), y, ui->outLine->width(), 150 -
                                                                            y);
    // 更新volumeBox的位置
    int volumeBtnY = ui->outLine->y() - ui->volumeBtn->height()/2;
    // 为了防⽌当outLine的y坐标为150时，按钮的⼀半显⽰不出来的问题
    if(volumeBtnY > 140){
        volumeBtnY = 140;
    }
    ui->volumeBtn->move(ui->volumeBtn->x(), volumeBtnY);
}


