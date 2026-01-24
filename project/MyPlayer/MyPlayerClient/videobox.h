#ifndef VIDEOBOX_H
#define VIDEOBOX_H

#include <QWidget>
#include <playerpage.h>

namespace Ui {
class VideoBox;
}

class VideoBox : public QWidget
{
    Q_OBJECT

public:
    explicit VideoBox(QWidget *parent = nullptr);
    ~VideoBox();

    bool eventFilter(QObject *watched, QEvent *event);
private:
    void onPlayCliecked();

private:
    Ui::VideoBox *ui;

    PlayerPage* playerPage;
};

#endif // VIDEOBOX_H
