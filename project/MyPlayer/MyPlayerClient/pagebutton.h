#ifndef PAGEBUTTON_H
#define PAGEBUTTON_H

#include <QWidget>
#include <QPushButton>

class PageButton : public QPushButton
{
    Q_OBJECT
public:
    explicit PageButton(int page ,QWidget *parent = nullptr);

    //设置按钮状态
    void setActive(bool active);
    //设置是否折叠
    void setFolded(bool folded);
    //设置页号
    void setPage(int page);
    //获取页号
    int getPage() const;
    // 检测按钮是否处于点击选中状态
    bool isActived() const;
    // 检测按钮是否为折叠按钮
    bool isFolded() const;

signals:

private:
    int page;   //页号
    //按钮是否被选中
    bool isActiveBtn = false;
    //是否为折叠按钮
    bool isFoldedBtn = false;
};

#endif // PAGEBUTTON_H
