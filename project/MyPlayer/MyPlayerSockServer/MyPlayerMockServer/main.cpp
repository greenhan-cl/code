#include "myplayersockserver.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyPlayerSockServer w;
    w.show();
    return a.exec();
}
