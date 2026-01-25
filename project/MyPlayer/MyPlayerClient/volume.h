#ifndef VOLUME_H
#define VOLUME_H

#include <QWidget>

namespace Ui {
class Volume;
}

class Volume : public QWidget
{
    Q_OBJECT

public:
    explicit Volume(QWidget *parent = nullptr);
    ~Volume();

    // 拦截⿏标按下、移动、释放事件
    bool eventFilter(QObject *watched, QEvent *event);
    // 更新圆球按钮位置 以及 outLine的尺⼨并更新
    void calcVolume();
signals:
    void setVolume(int volumeRatio); // 该信号通知PlayerPage⻚⾯，完成⾳量调节设置

private:
    Ui::Volume *ui;
    int volumeRatio; // 保存⾳量数值⼤⼩
};

#endif // VOLUME_H


