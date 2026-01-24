#ifndef BARRAGEEDIT_H
#define BARRAGEEDIT_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class BarrageEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit BarrageEdit(QWidget *parent = nullptr);

signals:

private slots:
    void onSendBSBtnClicked();

private:
    QPushButton* sendBSBtn;
};

#endif // BARRAGEEDIT_H
