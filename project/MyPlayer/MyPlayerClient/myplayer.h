#ifndef MYPLAYER_H
#define MYPLAYER_H

#include <QWidget>
#include <QMouseEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MyPlayer;
}
QT_END_NAMESPACE

enum QStackedWidgetPage{
    HomePage=0,
    MyselfPage=1,
    AdminPage=2,
    UploadPage = 3,
};

class MyPlayer : public QWidget
{
    Q_OBJECT

public:   
    ~MyPlayer();
    //单例
    static MyPlayer* getInstance();

private:
    MyPlayer(QWidget *parent = nullptr);
    void initUI();
    void connectSignalAndSlot();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void onSwitchStackedWidgetPage(int pageId);
    // 切换按钮点击后，重置按钮上⽂本和图⽚
    void resetSwitchBtnInfo(int pageId);


private:
    Ui::MyPlayer *ui;
    QPoint dragPos;
};
#endif // MYPLAYER_H
