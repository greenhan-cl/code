#ifndef HOMEPAGEWIDGET_H
#define HOMEPAGEWIDGET_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class HomePageWidget;
}

class HomePageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HomePageWidget(QWidget *parent = nullptr);
    ~HomePageWidget();

private:
    // 初始化分类和标签处的按钮
    void initKindsAndTags();
    //初始化视频列表
    void initVideos();
    // 初始化刷新和置顶按钮
    void initRefreshAndTop();
    // 创建按钮函数
    QPushButton* buildSelectBtn(QWidget* parent, const QString &color,const QString &text);
    // 当点击分类后，需要显⽰该分类下的标签
    void resetTags(const QList<QString> &tags);
protected:
    // 分类按钮点击
    void onKindBtnClicked(QPushButton* clickedkindBtn);
    // 标签按钮点击
    void onTagBtnClicked(QPushButton* clickedLabelBtn);
    // 刷新按钮点击槽函数
    void onRefreshBtnClicked();
    // 置顶按钮点击槽函数
    void onTopBtnClicked();

private:
    Ui::HomePageWidget *ui;
    QHash<QString, QList<QString>> tags;
};

#endif // HOMEPAGEWIDGET_H
