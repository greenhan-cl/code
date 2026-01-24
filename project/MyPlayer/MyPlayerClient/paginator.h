#ifndef PAGINATOR_H
#define PAGINATOR_H

#include <QWidget>
#include <QLineEdit>
#include "pagebutton.h"

class Paginator : public QWidget
{
    Q_OBJECT
public:
    explicit Paginator(int count ,QWidget *parent = nullptr);

private:
    void setBtnStyle(QPushButton* btn);
    //初始化信号及槽
    void initSignalSlot();
    // 跳转到page⻚
    void jumpToPage(int page);
    // page <= 5 的情况
    void jumpToPageCase1(int page);
    // page >= pageCount - 4 的情况
    void jumpToPageCase2(int page);
    // page 在 5 到 pageCount - 4 之间
    void jumpToPageCase3(int page);

private slots:
    void prevPage();
    void nextPage();
    void clickPageBtn();

signals:
    // 跳转到指定⻚数的信号
    void pageChanged(int page);

private:
    int pageCount = 7; // 总⻚数
    int currentPage = 1; // 当前⻚数
    const int pageSize = 20; // 每⻚显⽰的条⽬数
    // 上⼀⻚和下⼀⻚按钮
    QPushButton* prevPageBtn;
    QPushButton* nextPageBtn;
    // ⻚⾯切换按钮集
    QList<PageButton*> pages;
    // 跳转值指定⻚
    QLineEdit* pageEdit;
};

#endif // PAGINATOR_H
