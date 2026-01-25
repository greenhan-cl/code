#ifndef PLAYSLIDER_H
#define PLAYSLIDER_H

#include <QWidget>

namespace Ui {
class PlaySlider;
}

// 自定义进度条控件：上层用两条 QFrame 叠加表示总时长和当前播放进度
class PlaySlider : public QWidget
{
    Q_OBJECT

public:
    explicit PlaySlider(QWidget *parent = nullptr);
    ~PlaySlider();

    // ⿏标按下、移动、释放事件
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

    // 设置播放进度
    void setPlayStep(double stepRatio);

signals:
    void setPlayProgress(double playRatio); // 设置播放进度信号

private:
    void moveSlider(); // 改变outLine的geometry

private:
    Ui::PlaySlider *ui;
    int playGrogress; // 记录当前播放⻓度
};

#endif // PLAYSLIDER_H


