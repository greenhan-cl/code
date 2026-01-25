#ifndef UPLOADVIDEOPAGE_H
#define UPLOADVIDEOPAGE_H

#include <QWidget>

namespace Ui {
class UploadVideoPage;
}

class UploadVideoPage : public QWidget
{
    Q_OBJECT

public:
    explicit UploadVideoPage(QWidget *parent = nullptr);
    ~UploadVideoPage();

private:
    //提交按钮槽函数
    void onCommitBtnClicked();
    // 标题QLineEdit::textChanged信号对应槽函数
    void onLineEditTextChanged(const QString& text);
    // 简介QPlainTextEdit::textChanged信号对应槽函数
    void onPlainEditTextChanged();
    // 更改视频封⾯图按钮槽函数
    void onChangeBtnClicked();
    // QComoBox中分类选择改变槽函数
    void onUpdateTags(const QString &kind);
    // 将kind下标签以按钮形式展⽰在界⾯上
    void addTagsByKind(const QString& kind);

signals:
    //切换到我的页面信号
    void switchMySelfPage(int pageId);

private:
    Ui::UploadVideoPage *ui;
};

#endif // UPLOADVIDEOPAGE_H

