#include "modifymyselfdialog.h"
#include "ui_modifymyselfdialog.h"
#include "NewPasswordDialog.h"
#include "util.h"

ModifyMyselfDialog::ModifyMyselfDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ModifyMyselfDialog)
{
    ui->setupUi(this);
    // 去掉窗⼝边框
    setWindowFlag(Qt::FramelessWindowHint);
    ui->passwordWidget->hide();
    //绑定槽
    connect(ui->submitBtn, &QPushButton::clicked, this,
            &ModifyMyselfDialog::onSubmitBtnClicked);
    connect(ui->cancelBtn, &QPushButton::clicked, this,
            &ModifyMyselfDialog::onCancelBtnClicked);
    connect(ui->passwordBtn, &QPushButton::clicked, this,
            &ModifyMyselfDialog::showPasswordDlg);
    connect(ui->changePasswordBtn, &QPushButton::clicked, this,
            &ModifyMyselfDialog::showPasswordDlg);
}

ModifyMyselfDialog::~ModifyMyselfDialog()
{
    delete ui;
}

void ModifyMyselfDialog::onSubmitBtnClicked()
{
    LOG()<<"提交";
}

void ModifyMyselfDialog::onCancelBtnClicked()
{
    close();
}

void ModifyMyselfDialog::showPasswordDlg()
{
    // 显⽰再次修改密码对话框, ⽤主窗⼝作为对话框的⽗窗⼝.
    NewPasswordDialog* dialog = new NewPasswordDialog();
    dialog->exec();
    QString currentPassword = dialog->getNewPassword();
    if (currentPassword.isEmpty()) {
        LOG() << "取消修改密码";
        return;
    }
    LOG() << "新密码已设置: " << currentPassword;
    // 隐藏修改密码按钮, 显⽰已修改
    ui->passwordBtn->hide();
    ui->passwordWidget->show();
    // 释放对话框
    delete dialog;
}

