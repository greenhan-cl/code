#ifndef ROLETABLE_H
#define ROLETABLE_H

#include <QWidget>
#include <QMap>
#include "paginator.h"

namespace Ui {
class RoleTable;
}

class RoleTable : public QWidget
{
    Q_OBJECT

public:
    explicit RoleTable(QWidget *parent = nullptr);
    ~RoleTable();

    void
    onResetBtnClicked();
    // 查询按钮点击
    void onQueryBtnClicked();

private:
    // 初始化样式表
    void initStyleSheet();

private slots:
    // 将RoleTableItem添加到表格
    void updateRoleTable();
    //新建按钮
    void onInsertBtnClicked();

private:
    Ui::RoleTable *ui;
    // 保存按钮样式
    QMap<QString, QString> styleSheet;
    Paginator* paginator=nullptr;
};

#endif // ROLETABLE_H

