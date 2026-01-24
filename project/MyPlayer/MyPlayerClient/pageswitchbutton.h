#ifndef PAGESWITCHBUTTON_H
#define PAGESWITCHBUTTON_H

#include <QWidget>
#include <QLabel>
#include <QString>

class PageSwitchButton : public QWidget
{
    Q_OBJECT
public:
    explicit PageSwitchButton(QWidget *parent = nullptr);

    //设置按钮上的照片和文本
    void setImageAndText(const QString &imagePath,const QString &text,int pageId);

    // 设置按钮上图标--按钮图标在选中和未选中间切换
    void setImage(const QString& imagePath);
    // 设置按钮上⽂本颜⾊，颜⾊在选中和未选中间切换
    void setTextColor(const QString& textColor = "#000000");
    // 获取当前按钮对应的⻚⾯id
    int getPageId()const;



    // 可选：用于外部设置选中态（切页按钮常用）
    void setSelected(bool selected);
    bool isSelected() const;

signals:
    void clicked();
    void switchPage(int pageId);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    QLabel* btnImage = nullptr;  // 图标
    QLabel* btnTitle= nullptr;  // 文本
    bool selected_ = false;
    int pageId;
};

#endif // PAGESWITCHBUTTON_H
