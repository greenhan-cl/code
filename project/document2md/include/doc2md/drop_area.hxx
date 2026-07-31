#ifndef DOC2MD_DROP_AREA_HXX
#define DOC2MD_DROP_AREA_HXX

#include <QLabel>
#include <QStringList>

class QMouseEvent;

namespace doc2md {

/**
 * @brief 接收本地文件拖放并把文件路径发送给主界面。
 */
class DropArea final : public QLabel {
    Q_OBJECT

public:
    explicit DropArea(QWidget *_parent = nullptr);

signals:
    void filesDropped(const QStringList &_paths);
    void browseRequested();

protected:
    /** @brief 接受包含本地文件 URL 的拖入事件。 */
    void dragEnterEvent(QDragEnterEvent *_event) override;

    /** @brief 恢复拖放区域的普通样式。 */
    void dragLeaveEvent(QDragLeaveEvent *_event) override;

    /** @brief 提取本地文件路径并发送 filesDropped 信号。 */
    void dropEvent(QDropEvent *_event) override;

    /** @brief 点击拖放区域时请求打开文件选择窗口。 */
    void mousePressEvent(QMouseEvent *_event) override;

private:
    /** @brief 切换拖放区域高亮样式。 */
    void setHighlighted(bool _highlighted);
};

} // namespace doc2md

#endif // DOC2MD_DROP_AREA_HXX
