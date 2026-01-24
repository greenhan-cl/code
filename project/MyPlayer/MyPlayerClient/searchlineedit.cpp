#include "searchlineedit.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include "util.h"

SearchLineEdit::SearchLineEdit(QWidget *parent)
    : QLineEdit{parent}
{
    // 搜索框图标
    QLabel* searchImg = new QLabel(this);
    searchImg->setFixedSize(16, 16);
    searchImg->setPixmap(QPixmap(":/images/homePage/sousuo.png"));
    // 搜索框按钮
    QPushButton* searchBtn = new QPushButton(this);
    searchBtn->setText("搜索");
    searchBtn->setFixedSize(62, 32);
    searchBtn->setCursor(QCursor(Qt::ArrowCursor));
    searchBtn->setStyleSheet("background-color : #3ECEFE;"
                             "border-radius : 16px;"
                             "font-family : 微软雅⿊;"
                             "font-size : 14px;"
                             "color : #FFFFFF;"
                             "font-style : normal;");
    this->setPlaceholderText("请搜索你感兴趣的视频");
    this->setFixedWidth(320);
    this->setFixedHeight(36);
    setObjectName("search");
    this->setStyleSheet("#search{background-color : #FFFFFF;"
                        "border-radius : 18px;"
                        "border : 1px solid #3ECEFE;"
                        "font-family : 微软雅⿊;"
                        "font-size : 14px;"
                        "font-style : normal;}");
    this->setTextMargins(33, 0, 0, 0); // 左 上 右 下
    QHBoxLayout* hLayout = new QHBoxLayout(this);
    hLayout->addWidget(searchImg);
    hLayout->addStretch();
    hLayout->addWidget(searchBtn);
    hLayout->setContentsMargins(11, 0, 2, 0); // 左 上 右 下
    connect(searchBtn, &QPushButton::clicked, this,
            &SearchLineEdit::searchBtnClicked);
    connect(this, &QLineEdit::returnPressed, this,
            &SearchLineEdit::searchBtnClicked);
}

void SearchLineEdit::searchBtnClicked()
{
    LOG()<<"搜索按钮点击搜索视频";
}
