#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include <QWidget>
#include <QLineEdit>

class SearchLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit SearchLineEdit(QWidget *parent = nullptr);

private:
    void searchBtnClicked();

signals:
};

#endif // SEARCHLINEEDIT_H
