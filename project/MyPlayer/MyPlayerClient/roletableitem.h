#ifndef ROLETABLEITEM_H
#define ROLETABLEITEM_H

#include <QWidget>

namespace Ui {
class RoleTableItem;
}

class RoleTableItem : public QWidget
{
    Q_OBJECT

public:
    explicit RoleTableItem(QWidget *parent = nullptr ,int seqNumber = 1);
    ~RoleTableItem();

    // 更新界⾯元素
    void updateUI(int seqNumber);

private slots:
    //编辑按钮
    void onEditBtnClicked();

private:
    Ui::RoleTableItem *ui;
    //model::AdminInfo adminInfo;
};

#endif // ROLETABLEITEM_H

