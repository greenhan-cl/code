#ifndef NEWPASSWORDDIALOG_H
#define NEWPASSWORDDIALOG_H

#include<QWidget>
#include <QDialog>

namespace Ui {
class NewPasswordDialog;
}

class NewPasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewPasswordDialog(QWidget *parent = nullptr);
    ~NewPasswordDialog();
    // 获取修改后的新密码
    QString getNewPassword() const;
private slots:
    // 提交按钮槽函数
    void onSubmitBtnClicked();
    // QLineEdit编辑结束信号槽函数
    void onEditingFinished();
private:
    bool checkPasswordEdit();
    // 验证成功, 返回 "", 失败返回失败原因.
    QString passwordValid(const QString& password);

private:
    Ui::NewPasswordDialog *ui;
    // 保存⽤⼾修改后密码
    QString newPassword;
};

#endif // NEWPASSWORDDIALOG_H
