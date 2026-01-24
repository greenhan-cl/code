#ifndef TOAST_H
#define TOAST_H

#include <QDialog>

class Toast : public QDialog
{
    Q_OBJECT
public:
    Toast(const QString& text);
    
    Toast(const QString& text, QWidget *pWidget);

    // 并不需要⼿动来 new 这个对象, ⽽是通过 showMessage 来弹出窗⼝
    static void showMessage(const QString& text);
    static void showMessage(const QString& text, QWidget *pWidget);

private:
    void initUI(const QString& text);

signals:

};

#endif // TOAST_H
