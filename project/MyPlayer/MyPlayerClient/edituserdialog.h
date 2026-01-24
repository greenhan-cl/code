#ifndef EDITUSERDIALOG_H
#define EDITUSERDIALOG_H

#include <QDialog>

namespace Ui {
class EditUserDialog;
}

class EditUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditUserDialog(QWidget *parent = nullptr ,const QString& text = "");
    ~EditUserDialog();

private slots:
    //取消按钮点击
    void onCancelBtnClicked();

private:
    Ui::EditUserDialog *ui;
};

#endif // EDITUSERDIALOG_H

