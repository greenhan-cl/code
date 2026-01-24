#ifndef PLAYERPAGE_H
#define PLAYERPAGE_H

#include <QWidget>
#include <volume.h>
#include <playspeed.h>
#include "login.h"

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

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private slots:
    void onVolumeBtnClicked(); // ⾳量调节
    void onSpeedBtnClicked(); // 倍速播放
    void onLkeImageBtnClcked(); // 点赞

private:
    Ui::PlayerPage *ui;
    QPoint dragPos;
    // ⾳量调节窗⼝指针
    Volume* volume;
    PlaySpeed* playSpeed; // 倍数播放窗⼝
};

#endif // PLAYERPAGE_H
