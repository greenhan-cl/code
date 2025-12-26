#include "myplayer.h"
#include "ui_myplayer.h"

MyPlayer::MyPlayer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyPlayer)
{
    ui->setupUi(this);
}

MyPlayer::~MyPlayer()
{
    delete ui;
}
