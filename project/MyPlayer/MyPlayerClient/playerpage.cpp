#include "playerpage.h"
#include "ui_playerpage.h"
#include "toast.h"
#include "util.h"
#include <QMouseEvent>
#include <QCloseEvent>
#include <QShortcut>

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
    //绑定播放
    connect(ui->playBtn, &QPushButton::clicked, this,
            &PlayerPage::onplayBtnClicked);
    // 设置倍数播放
    connect(playSpeed, &PlaySpeed::setPlaySpeed, this,
            &PlayerPage::onPlaySpeedChanged);
    // 设置⾳量
    connect(volume, &Volume::setVolume,
            this, &PlayerPage::setVolume);
    // 设置⾳量
    connect(volume, &Volume::setVolume,
            this, &PlayerPage::setVolume);
    // 点击进度条，设置播放进度
    connect(ui->videoSlider, &PlaySlider::setPlayProgress,
            this,&PlayerPage::setPlayProgress);

    // 播放按钮绑定空格快捷键
    QShortcut* shortcur = new QShortcut(ui->playBtn);
    QKeySequence keySequence(" ");
    shortcur->setKey(keySequence);
    connect(shortcur, &QShortcut::activated, this, [=](){
        ui->playBtn->animateClick();
    });
}

PlayerPage::~PlayerPage()
{
    // 清理 mpvPlayer 资源
    if(mpvPlayer){
        delete mpvPlayer;
        mpvPlayer = nullptr;
    }
    delete ui;
}

//移动窗口位置
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

//加载视频
void PlayerPage::startPlaying(const QString &videoFilePath)
{
    // 如果已经存在 mpvPlayer，先清理
    if(mpvPlayer){
        delete mpvPlayer;
        mpvPlayer = nullptr;
    }
    
    // 重置播放时间和进度条
    playTime = 0;
    ui->videoSlider->setPlayStep(0.0);
    ui->videoDuration->setText("00:00/00:10");
    
    mpvPlayer = new MpvPlayer(this, ui->screen);
    
    // 连接 mpv 事件信号，处理 mpv 内部事件
    connect(mpvPlayer, &MpvPlayer::mpvEvents, mpvPlayer, &MpvPlayer::onMpvEvents);
    // 连接播放位置改变信号，更新进度条
    connect(mpvPlayer, &MpvPlayer::playPositionChanged, this, &PlayerPage::onPlayPositionChanged);
    
    mpvPlayer->startPlay(videoFilePath);

    // 视频加载成功后会⽴⻢播放，初始先设置为暂停，让⽤⼾点击播放按钮去播放
    mpvPlayer->pause();
    // 保存当前播放视频路径，播放结束时点击再次播放时需要⽤到
    this->videoFilePath = videoFilePath;
}

//
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

//
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

void PlayerPage::onplayBtnClicked()
{
    isPlay = !isPlay;
    if(isPlay){
        // 播放
        ui->playBtn->setStyleSheet("border-image :url(:/images/PlayPage/bofang.png)");
        mpvPlayer->play();
    }else{
        // 暂停
        ui->playBtn->setStyleSheet("border-image :url(:/images/PlayPage/zanting.png)");
        mpvPlayer->pause();
    }
    // 播放完毕，再⼀次点击播放按钮时，重新开始播放
    if(playTime == 10 && isPlay){
        // 播放位置修改到起始为⽌，⽤⼾点击播放按钮可以重新播放
        this->playTime = 0;
        startPlaying(videoFilePath);
        mpvPlayer->play();
    }
}

//倍速播放
void PlayerPage::onPlaySpeedChanged(double speed)
{
    mpvPlayer->setPlaySpeed(speed);
}

//调节音量
void PlayerPage::setVolume(int volumeRatio)
{
    mpvPlayer->setVolume(volumeRatio);
}

void PlayerPage::onPlayPositionChanged(int64_t playTime)
{
    this->playTime = playTime;
    QString curPlayTime = secondToTime(this->playTime);
    QString totalTime = secondToTime(10);
    ui->videoDuration->setText(curPlayTime + "/" + totalTime);
    // 修改进度条
    ui->videoSlider->setPlayStep((double)this->playTime/10);
    if(this->playTime == 10){
        // 视频播放完毕，更新播放按钮图标
        // 此时播放按钮应该变为暂停
        isPlay = false;
        ui->playBtn->setStyleSheet("border-image :url(:/images/PlayPage/zanting.png)");
    }
}

void PlayerPage::setPlayProgress(double playRatio)
{
    playTime = 10*playRatio;
    mpvPlayer->setCurrentPlayPosition(playTime);
}

//清理资源
void PlayerPage::closeEvent(QCloseEvent *event)
{
    // 关闭时清理 mpvPlayer 资源
    if(mpvPlayer){
        delete mpvPlayer;
        mpvPlayer = nullptr;
    }
    // 接受关闭事件
    event->accept();
}

//转化为00：00
QString PlayerPage::secondToTime(int64_t second)
{
    QString time;
    // ⼩时存在时才显⽰
    if(second/60/60){
        time += QString::asprintf("%02lld:", second/60/60);
    }
    // 拼接上分和秒
    time += QString::asprintf("%02lld:%02lld",second/60,second%60);
    return time;
}


