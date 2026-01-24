#include "pageswitchbutton.h"

#include <QMouseEvent>
#include <QPixmap>
#include <QVBoxLayout>

PageSwitchButton::PageSwitchButton(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(QSize(52, 45));
    setCursor(Qt::PointingHandCursor);

    btnImage = new QLabel(this);
    btnImage->setFixedSize(24, 24);
    btnImage->setAlignment(Qt::AlignCenter);

    btnTitle = new QLabel(this);
    btnTitle->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    layout->addWidget(btnImage, 0, Qt::AlignHCenter);
    layout->addWidget(btnTitle, 0, Qt::AlignHCenter);

    setSelected(false);
}

void PageSwitchButton::setImageAndText(const QString &imagePath, const QString &text,int pageId)
{
    btnTitle->setText(text);
    btnTitle->move((48-text.size()*12)/2, 30); // ⽂字在界⾯中居中
    setImage(imagePath);
    this->pageId = pageId;
}


void PageSwitchButton::setSelected(bool selected)
{
    selected_ = selected;
    // 简单高亮：选中态文字更深
    if (btnTitle) {
        btnTitle->setStyleSheet(selected_ ? "color:#111111;" : "color:#666666;");
    }
}

bool PageSwitchButton::isSelected() const
{
    return selected_;
}

void PageSwitchButton::mousePressEvent(QMouseEvent *event)
{
    (void)event;
    setTextColor("#000000");
    emit switchPage(pageId);
    qDebug()<<btnTitle->text();
}



void PageSwitchButton::setImage(const QString &imagePath)
{
    btnImage->setPixmap(QPixmap(imagePath));
}

void PageSwitchButton::setTextColor(const QString &textColor)
{
    btnTitle->setStyleSheet("font-family: 微软雅⿊;"
                             "font-size: 12px;"
                             "font-style: normal;"
                             "font-weight:bold;"
                             "color: " + textColor + ";");
}

int PageSwitchButton::getPageId() const
{
    return pageId;
}

