#include "mpvplayer.h"
#include "util.h"
#include <QWidget>

//callback
static void wakeup(void *ctx) {
    MpvPlayer *mvpPlayer = (MpvPlayer *)ctx;
    emit mvpPlayer->mpvEvents();
}

MpvPlayer::MpvPlayer(QObject *parent ,QWidget* videoRenderWnd)
    : QObject{parent}
{
    // 设置程序区域
    // 区域设置决定了程序在运⾏时如何处理各种本地化相关操作，⽐如：⽇期格式、数字格式、货币符号等
        // LC_NUMERIC: 设置数字格式(包括⼩数点和千位分隔符)
        // C：表⽰使⽤C标准的默认区域设置
        std::setlocale(LC_NUMERIC, "C");
    // 创建mpv实例
    mpv = mpv_create();
    if(nullptr == mpv){
        LOG()<<"创建mpv实例失败";
        throw std::runtime_error("can't create mpv instance!!!");
    }
    // 设置视频渲染窗⼝
    int64_t wid = videoRenderWnd->winId();
    mpv_set_option(mpv, "wid", MPV_FORMAT_INT64, &wid);
    //注册需要监控时间
    mpv_observe_property(mpv,0,"time-pos",MPV_FORMAT_INT64);


    // 设置mpv内部事件触发时的回到函数wakeup
    // 通过应⽤程序的主事件循环处理mpv事件
    mpv_set_wakeup_callback(mpv, wakeup, this);
    // 初始化mpv实例
    if(mpv_initialize(mpv) < 0){
        LOG()<<"初始化mpv失败";
        mpv_destroy(mpv);
        throw std::runtime_error("init mpv instance failed!!!");
    }
}

MpvPlayer::~MpvPlayer()
{
    // 释放mpv实例
    if(mpv){
        mpv_terminate_destroy(mpv);
    }
}

void MpvPlayer::startPlay(const QString &videoPath)
{
    // 与mpv_command相同，但异步运⾏命令来避免阻塞，直到进程终⽌
    const QByteArray c_filename = videoPath.toUtf8();
    const char *args[] = {"loadfile", c_filename.data(), NULL};
    mpv_command_async(mpv, 0, args);
}

//播放
void MpvPlayer::play()
{
    // pause：设置播放器的暂停状态，可以设置为暂停或播放
    int s = 0;
    mpv_set_property_async(mpv, 0, "pause", MPV_FORMAT_FLAG, &s);
}

//暂停
void MpvPlayer::pause()
{
    int s = 1;
    mpv_set_property_async(mpv, 0, "pause", MPV_FORMAT_FLAG, &s);
}

//倍速
void MpvPlayer::setPlaySpeed(double speed)
{
    mpv_set_property_async(mpv, 0, "speed", MPV_FORMAT_DOUBLE, &speed);
}

//静音
void MpvPlayer::setMute(bool isMuted)
{
    int flag = isMuted? 1 : 0;
    mpv_set_property_async(mpv, 0, "mute", MPV_FORMAT_FLAG, &flag);
}

//调节音量
void MpvPlayer::setVolume(int64_t volume)
{
    mpv_set_property_async(mpv, 0, "volume", MPV_FORMAT_INT64, &volume);
}

void MpvPlayer::setCurrentPlayPosition(int64_t seconds)
{
    mpv_set_property_async(mpv, 0, "time-pos", MPV_FORMAT_INT64, &seconds);
}

//主事件循环
void MpvPlayer::onMpvEvents()
{
    while(mpv){
        mpv_event* event = mpv_wait_event(mpv, 0);
        if(event->event_id == MPV_EVENT_NONE){
            break;
        }
        // 处理特定事件
        handleMpvEvent(event);
    }
}

//初始mpv事件
void MpvPlayer::handleMpvEvent(mpv_event *event)
{
    switch (event->event_id) {
    //播放时间发生改变
    case MPV_EVENT_PROPERTY_CHANGE:
    {
        mpv_event_property* eventPropery = (mpv_event_property*)event->data;
        if(nullptr == eventPropery->data){
            break;
        }
        if(strcmp(eventPropery->name, "time-pos") == 0){
            // 播放进度发⽣改变，发出信号，让界⾯更新进度条和时间
            int64_t seconds = *(int64_t*)(eventPropery->data);
            emit playPositionChanged(seconds);
        }
        break;
    }
    case MPV_EVENT_SHUTDOWN:
    {
        mpv_terminate_destroy(mpv);
        mpv = nullptr;
    }
    default:
        break;
    }
}
