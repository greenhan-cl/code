#ifndef STARTUPPAGE_H
#define STARTUPPAGE_H

#include <QDialog>

class startupPage : public QDialog
{
    Q_OBJECT
public:
    explicit startupPage(QWidget *parent = nullptr);
    void startTimer();

signals:
};

#endif // STARTUPPAGE_H
