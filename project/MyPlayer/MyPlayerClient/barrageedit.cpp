#include "barrageedit.h"
#include "util.h"

BarrageEdit::BarrageEdit(QWidget *parent)
    : QLineEdit{parent}
{
    // 初始化编辑框
    this->setFixedSize(1030, 32);
    this->setPlaceholderText("这个视频在说的是什么...");
    this->setMaxLength(30); // 最多输⼊30个字符
    this->setTextMargins(12, 6, 0, 7); // 左 上 右 下
    // 初始化发送按钮
    sendBSBtn = new QPushButton(this);
    sendBSBtn->setFixedSize(58, 28);
    sendBSBtn->setText("发送");
    sendBSBtn->setCursor(QCursor(Qt::ArrowCursor));
    sendBSBtn->setStyleSheet("background-color : #3ECEFE;"
                             "border-radius : 4px;"
                             "color : #FFFFFF;"
                             "font-size : 14px;"
                             "font-family : 微软雅⿊;");
    sendBSBtn->move(width()-sendBSBtn->width() - 2, 2);

    connect(sendBSBtn,&QPushButton::clicked,this,&BarrageEdit::onSendBSBtnClicked);
}

void BarrageEdit::onSendBSBtnClicked()
{
    LOG()<<"发送d弹幕: "<<text();
}
