#include "playspeed.h"
#include "ui_playspeed.h"
#include "util.h"

PlaySpeed::PlaySpeed(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlaySpeed)
{
    ui->setupUi(this);
    // Qt::NoDropShadowWindowHint : 去掉窗⼝阴影
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint |
                   Qt::NoDropShadowWindowHint);
    // 设置窗⼝背景透明，主要需要设置Qt::FramelessWindowHint标志，否则窗⼝背景是⿊⾊的
    setAttribute(Qt::WA_TranslucentBackground);
    connect(ui->speed05, &QPushButton::clicked, this,
            &PlaySpeed::onPlay05Speed);
    connect(ui->speed10, &QPushButton::clicked, this,
            &PlaySpeed::onPlay10Speed);
    connect(ui->speed15, &QPushButton::clicked, this,
            &PlaySpeed::onPlay15Speed);
    connect(ui->speed20, &QPushButton::clicked, this,
            &PlaySpeed::onPlay20Speed);
}

PlaySpeed::~PlaySpeed()
{
    delete ui;
}

void PlaySpeed::onPlay05Speed()
{
    emit setPlaySpeed(0.5);
}

void PlaySpeed::onPlay10Speed()
{
    emit setPlaySpeed(1.0);
}

void PlaySpeed::onPlay15Speed()
{
    emit setPlaySpeed(1.5);
}

void PlaySpeed::onPlay20Speed()
{
    emit setPlaySpeed(2.0);
}


