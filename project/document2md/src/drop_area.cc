#include <doc2md/drop_area.hxx>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QStyle>
#include <QUrl>

namespace doc2md {

DropArea::DropArea(QWidget *_parent)
    : QLabel(_parent) {
    setAcceptDrops(true);
    setAlignment(Qt::AlignCenter);
    setCursor(Qt::PointingHandCursor);
    setMinimumHeight(138);
    setTextFormat(Qt::RichText);
    setText(tr("<div style='text-align:center'>"
               "<img src=':/doc2md/icons/upload.svg' width='58' height='58'/><br/>"
               "<span style='font-size:17px;font-weight:600;color:#1e3a8a'>"
               "将文档拖到这里</span><br/>"
               "<span style='font-size:13px;color:#64748b'>"
               "或点击选择文件，支持批量添加</span>"
               "</div>"));
    setHighlighted(false);
}

void DropArea::dragEnterEvent(QDragEnterEvent *_event) {
    if (_event->mimeData()->hasUrls()) {
        _event->acceptProposedAction();
        setHighlighted(true);
    }
}

void DropArea::dragLeaveEvent(QDragLeaveEvent *_event) {
    setHighlighted(false);
    QLabel::dragLeaveEvent(_event);
}

void DropArea::dropEvent(QDropEvent *_event) {
    QStringList _paths;
    for (const QUrl &_url : _event->mimeData()->urls()) {
        if (_url.isLocalFile()) {
            _paths.append(_url.toLocalFile());
        }
    }

    setHighlighted(false);
    if (!_paths.isEmpty()) {
        _event->acceptProposedAction();
        emit filesDropped(_paths);
    }
}

void DropArea::mousePressEvent(QMouseEvent *_event) {
    if (_event->button() == Qt::LeftButton) {
        emit browseRequested();
        _event->accept();
        return;
    }
    QLabel::mousePressEvent(_event);
}

void DropArea::setHighlighted(bool _highlighted) {
    setProperty("dragActive", _highlighted);
    style()->unpolish(this);
    style()->polish(this);
    update();
}

} // namespace doc2md
