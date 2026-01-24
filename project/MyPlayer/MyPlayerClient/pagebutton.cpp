#include "pagebutton.h"

PageButton::PageButton(int page ,QWidget *parent)
    : QPushButton{parent}
{
    this->page=page;
    // 设置按钮的图标尺⼨ 和 按钮⼤⼩
    this->setIconSize(QSize(12, 12));
    this->setFixedSize(QSize(32, 32));
    // 设置按钮上显⽰⽂本
    if (this->isFoldedBtn) {
        this->setText("...");
    } else {
        this->setText(QString::number(page));
    }
    // 设置按钮是否为选中状态
    setActive(isActiveBtn);
}

//设置选选中状态
void PageButton::setActive(bool active)
{
    this->isActiveBtn = active;
    if (active) {
        this->setStyleSheet("QPushButton {"
                            "color: #FFFFFF; "
                            "background-color: #3ECEFE; "
                            "border: 1px solid #3ECEFE; "
                            "border-radius: 2px;}");
    } else {
        this->setStyleSheet("QPushButton {"
                            "color: #000000; "
                            "background-color: #FFFFFF; "
                            "border: 1px solid #D9D9D9; "
                            "border-radius: 2px;}");
    }
}

//设置折叠标
void PageButton::setFolded(bool folded)
{
    this->isFoldedBtn = folded;
    if (folded) {
        this->setText("...");
    } else {
        this->setText(QString::number(page));
    }
}

//设置页数
void PageButton::setPage(int page)
{
    this->page = page;
    this->setText(QString::number(page));
}

//获取页数
int PageButton::getPage() const
{
    return page;
}

//是否被选中
bool PageButton::isActived() const
{
    return isActiveBtn;
}

//是否为折叠图标
bool PageButton::isFolded() const
{
    return isFoldedBtn;
}
