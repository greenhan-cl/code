#include "roletableitem.h"
#include "ui_RoleTableItem.h"
#include<edituserdialog.h>


RoleTableItem::RoleTableItem(QWidget *parent,int seqNumber) :
    QWidget(parent),
    ui(new Ui::RoleTableItem)
{
    ui->setupUi(this);

    updateUI(seqNumber);

    //绑定槽函数
    connect(ui->editBtn,&QPushButton::clicked,
            this,&RoleTableItem::onEditBtnClicked);
}

RoleTableItem::~RoleTableItem()
{
    delete ui;
}

//更新界面
void RoleTableItem::updateUI(int seqNumber)
{
    ui->idLabel->setText(QString::number(seqNumber));
}

void RoleTableItem::onEditBtnClicked()
{
    EditUserDialog* editUserDialog = new EditUserDialog(nullptr,"编辑后台用户");
    editUserDialog->exec();
    delete editUserDialog;
}

