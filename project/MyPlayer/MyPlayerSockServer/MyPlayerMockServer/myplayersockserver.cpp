#include "myplayersockserver.h"
#include "ui_myplayersockserver.h"

MyPlayerSockServer::MyPlayerSockServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyPlayerSockServer)
{
    ui->setupUi(this);
}

MyPlayerSockServer::~MyPlayerSockServer()
{
    delete ui;
}
