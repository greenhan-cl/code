#ifndef MYPLAYER_H
#define MYPLAYER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MyPlayer;
}
QT_END_NAMESPACE

class MyPlayer : public QWidget
{
    Q_OBJECT

public:
    MyPlayer(QWidget *parent = nullptr);
    ~MyPlayer();

private:
    Ui::MyPlayer *ui;
};
#endif // MYPLAYER_H
