#ifndef PLAYERPAGE_H
#define PLAYERPAGE_H

#include <QWidget>
#include <volume.h>
#include <playspeed.h>
#include "login.h"
#include "mpv/mpvplayer.h"

namespace Ui {
class PlayerPage;
}

class PlayerPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerPage(QWidget *parent = nullptr);
    ~PlayerPage();
    //移动窗口
    void moveWindows(const QPoint& point);
    //加载ship
    void startPlaying(const QString &videoFilePath);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event) override;

private:
    // 将秒转换为：xx:xx:xx 格式时间
    QString secondToTime(int64_t second);


private slots:
    void onVolumeBtnClicked(); // ⾳量调节
    void onSpeedBtnClicked(); // 倍速播放
    void onLkeImageBtnClcked(); // 点赞
    void onplayBtnClicked(); // 播放视频
    void onPlaySpeedChanged(double speed); // 倍数播放
    void setVolume(int volumeRatio);    //音量调节
    void onPlayPositionChanged(int64_t playTime); // 播放位置改变
    void setPlayProgress(double playRatio); // 设置播放进度

private:
    Ui::PlayerPage *ui;
    QPoint dragPos;
    // ⾳量调节窗⼝指针
    Volume* volume;
    PlaySpeed* playSpeed; // 倍数播放窗⼝
    MpvPlayer* mpvPlayer = nullptr; // 封装mpv库，控制播放视频
    bool isPlay=false;         //默认暂停播放
    int64_t playTime = 0; // 当前播放时⻓
    QString videoFilePath; //视频位置
};

#endif // PLAYERPAGE_H
