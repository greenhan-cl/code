#ifndef MPVPLAYER_H
#define MPVPLAYER_H

#include <QObject>
#include "./mpv/client.h"

class MpvPlayer : public QObject
{
    Q_OBJECT
public:
    explicit MpvPlayer(QObject *parent = nullptr ,QWidget* videoRenderWnd = nullptr);
    ~MpvPlayer();

    // 加载视频
    void startPlay(const QString& videoPath);
    // 播放
    void play();
    //暂停
    void pause();
    // 倍速播放
    void setPlaySpeed(double speed);
    //静音模式
    void setMute(bool isMuted);
    // 设置⾳量⼤⼩
    void setVolume(int64_t volume);
    // 设置视频播放位置
    void setCurrentPlayPosition(int64_t seconds);

public slots:
    void onMpvEvents();

signals:
    //当mpv触发事件时，在回调函数发射信号，由用户程序处理mpv的事件
    void mpvEvents();
    //当播放时间发生变化
    void playPositionChanged(int64_t seconds);

private:
    // 处理mpv的事件
    void handleMpvEvent(mpv_event* event);

private:
    mpv_handle* mpv=nullptr;
};

#endif // MPVPLAYER_H
