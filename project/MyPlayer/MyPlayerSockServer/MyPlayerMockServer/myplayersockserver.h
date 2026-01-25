#ifndef MYPLAYERSOCKSERVER_H
#define MYPLAYERSOCKSERVER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MyPlayerSockServer;
}
QT_END_NAMESPACE

class MyPlayerSockServer : public QWidget
{
    Q_OBJECT

public:
    MyPlayerSockServer(QWidget *parent = nullptr);
    ~MyPlayerSockServer();

private:
    Ui::MyPlayerSockServer *ui;
};
#endif // MYPLAYERSOCKSERVER_H
