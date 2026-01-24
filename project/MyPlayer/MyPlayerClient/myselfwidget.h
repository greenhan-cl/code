#ifndef MYSELFWIDGET_H
#define MYSELFWIDGET_H

#include <QWidget>

namespace Ui {
class MyselfWidget;
}

class MyselfWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyselfWidget(QWidget *parent = nullptr);
    ~MyselfWidget();

private:
    // 初始化界⾯控件
    void initUI();
    //点击上传头像
    void uploadAvatarBtnClicked();
    // 点击设置按钮
    void settingBtnClicked();
    // 上传视频按钮槽函数
    void uploadViewBtnClicked();

signals:
    // 切换到上传视频⻚⾯
    void switchUploadVideoPage(int pageIndex);

private:
    Ui::MyselfWidget *ui;
};

#endif // MYSELFWIDGET_H
