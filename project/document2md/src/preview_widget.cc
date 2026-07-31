#include <doc2md/preview_widget.hxx>

#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLabel>
#include <QPdfDocument>
#include <QPdfView>
#include <QPixmap>
#include <QPlainTextEdit>
#include <QScrollArea>
#include <QSet>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QTabWidget>
#include <QTextBrowser>
#include <QVBoxLayout>

#include <doc2md/conversion_runner.hxx>

namespace doc2md {

PreviewWidget::PreviewWidget(const QString &_path, QWidget *_parent)
    : QWidget(_parent, Qt::Window)
    , m_runner(new ConversionRunner(this)) {
    setAttribute(Qt::WA_DeleteOnClose);
    setupUi();
    connect(m_runner, &ConversionRunner::finished,
            this, &PreviewWidget::finishGeneratedPreview);
    loadFile(_path);
}

PreviewWidget::~PreviewWidget() {
    m_runner->cancel();
}

void PreviewWidget::setupUi() {
    setObjectName(QStringLiteral("PreviewWidget"));
    resize(900, 680);
    auto *_root_layout = new QVBoxLayout(this);
    _root_layout->setContentsMargins(18, 18, 18, 18);

    m_preview_stack = new QStackedWidget(this);
    setupMessagePage();
    setupTextPage();
    setupImagePage();
    setupPdfPage();
    _root_layout->addWidget(m_preview_stack);
}

void PreviewWidget::setupMessagePage() {
    m_message_page = new QWidget(m_preview_stack);
    auto *_message_layout = new QVBoxLayout(m_message_page);
    m_message_label = new QLabel(m_message_page);
    m_message_label->setObjectName(QStringLiteral("previewWindowMessage"));
    m_message_label->setAlignment(Qt::AlignCenter);
    m_message_label->setWordWrap(true);
    _message_layout->addWidget(m_message_label);
    m_preview_stack->addWidget(m_message_page);
}

void PreviewWidget::setupTextPage() {
    m_text_tabs = new QTabWidget(m_preview_stack);
    m_rendered_preview = new QTextBrowser(m_text_tabs);
    m_source_preview = new QPlainTextEdit(m_text_tabs);
    m_source_preview->setReadOnly(true);
    m_text_tabs->addTab(m_rendered_preview, tr("预览"));
    m_text_tabs->addTab(m_source_preview, tr("源码"));
    m_preview_stack->addWidget(m_text_tabs);
}

void PreviewWidget::setupImagePage() {
    m_image_scroll_area = new QScrollArea(m_preview_stack);
    m_image_scroll_area->setWidgetResizable(true);
    m_image_page = new QWidget(m_image_scroll_area);
    auto *_image_layout = new QVBoxLayout(m_image_page);
    m_image_label = new QLabel(m_image_page);
    m_image_label->setAlignment(Qt::AlignCenter);
    _image_layout->addWidget(m_image_label);
    m_image_scroll_area->setWidget(m_image_page);
    m_preview_stack->addWidget(m_image_scroll_area);
}

void PreviewWidget::setupPdfPage() {
    m_pdf_page = new QWidget(m_preview_stack);
    auto *_pdf_layout = new QVBoxLayout(m_pdf_page);
    _pdf_layout->setContentsMargins(0, 0, 0, 0);
    m_pdf_document = new QPdfDocument(this);
    m_pdf_view = new QPdfView(m_pdf_page);
    m_pdf_view->setDocument(m_pdf_document);
    m_pdf_view->setPageMode(QPdfView::PageMode::MultiPage);
    m_pdf_view->setZoomMode(QPdfView::ZoomMode::FitToWidth);
    _pdf_layout->addWidget(m_pdf_view);
    m_preview_stack->addWidget(m_pdf_page);
}

void PreviewWidget::loadFile(const QString &_path) {
    const QFileInfo _file_info(_path);
    setWindowTitle(tr("预览 · %1").arg(_file_info.fileName()));
    if (!_file_info.exists() || !_file_info.isFile()) {
        setMessage(tr("文件不存在或无法访问：\n%1").arg(_path));
        return;
    }
    if (!previewDirectly(_path)) {
        previewWithMarkItDown(_path);
    }
}

bool PreviewWidget::previewDirectly(const QString &_path) {
    const QString _suffix = QFileInfo(_path).suffix().toLower();
    const QSet<QString> _image_types = {
        QStringLiteral("bmp"), QStringLiteral("gif"), QStringLiteral("jpg"),
        QStringLiteral("jpeg"), QStringLiteral("png"), QStringLiteral("webp")};
    const QSet<QString> _text_types = {
        QStringLiteral("c"), QStringLiteral("cc"), QStringLiteral("cpp"),
        QStringLiteral("csv"), QStringLiteral("h"), QStringLiteral("hpp"),
        QStringLiteral("hxx"), QStringLiteral("ini"), QStringLiteral("json"),
        QStringLiteral("log"), QStringLiteral("md"), QStringLiteral("py"),
        QStringLiteral("qss"), QStringLiteral("txt"), QStringLiteral("xml"),
        QStringLiteral("yaml"), QStringLiteral("yml")};
    if (_image_types.contains(_suffix)) {
        return previewImage(_path);
    }
    if (_suffix == QStringLiteral("pdf")) {
        return previewPdf(_path);
    }
    if (_suffix == QStringLiteral("htm") || _suffix == QStringLiteral("html")) {
        return previewText(_path, false, true);
    }
    if (_text_types.contains(_suffix)) {
        return previewText(_path, _suffix == QStringLiteral("md"), false);
    }
    return false;
}

bool PreviewWidget::previewText(const QString &_path,
                                bool _render_markdown,
                                bool _render_html) {
    QFile _file(_path);
    if (!_file.open(QIODevice::ReadOnly)) {
        setMessage(tr("无法读取文件：%1").arg(_file.errorString()));
        return true;
    }

    constexpr qint64 _preview_limit = 512 * 1024;
    QString _content = QString::fromUtf8(_file.read(_preview_limit));
    if (!_file.atEnd()) {
        _content.append(tr("\n\n--- 预览内容已截断 ---"));
    }
    setTextContent(_content, _render_markdown);
    if (_render_html) {
        m_rendered_preview->setHtml(_content);
    }
    return true;
}

bool PreviewWidget::previewImage(const QString &_path) {
    const QPixmap _image(_path);
    if (_image.isNull()) {
        setMessage(tr("无法读取图片文件。"));
        return true;
    }
    m_image_label->setPixmap(_image);
    m_image_label->setMinimumSize(_image.size());
    m_preview_stack->setCurrentWidget(m_image_scroll_area);
    return true;
}

bool PreviewWidget::previewPdf(const QString &_path) {
    const QPdfDocument::Error _error = m_pdf_document->load(_path);
    if (_error != QPdfDocument::Error::None) {
        setMessage(tr("无法加载 PDF 文件，错误代码：%1。")
                       .arg(static_cast<int>(_error)));
        return true;
    }
    m_preview_stack->setCurrentWidget(m_pdf_page);
    return true;
}

QString PreviewWidget::makePreviewOutputPath(const QString &_path) const {
    QString _cache_path = QStandardPaths::writableLocation(
        QStandardPaths::CacheLocation);
    if (_cache_path.isEmpty()) {
        _cache_path = QStandardPaths::writableLocation(
            QStandardPaths::TempLocation);
    }
    _cache_path = QDir(_cache_path).absoluteFilePath(QStringLiteral("previews"));
    if (!QDir().mkpath(_cache_path)) {
        return {};
    }
    const QFileInfo _file_info(_path);
    const QByteArray _key = QStringLiteral("%1|%2|%3")
                                .arg(_file_info.absoluteFilePath())
                                .arg(_file_info.size())
                                .arg(_file_info.lastModified().toMSecsSinceEpoch())
                                .toUtf8();
    const QString _name = QString::fromLatin1(
        QCryptographicHash::hash(_key, QCryptographicHash::Sha256).toHex());
    return QDir(_cache_path).absoluteFilePath(_name + QStringLiteral(".md"));
}

void PreviewWidget::previewWithMarkItDown(const QString &_path) {
    m_preview_output_path = makePreviewOutputPath(_path);
    if (m_preview_output_path.isEmpty()) {
        setMessage(tr("无法创建预览缓存目录。"));
        return;
    }
    if (QFileInfo::exists(m_preview_output_path)) {
        (void)previewText(m_preview_output_path, true, false);
        return;
    }
    setMessage(tr("正在生成临时预览…"));
    if (!m_runner->start(_path, m_preview_output_path)) {
        QString _error = m_runner->environmentError();
        setMessage(_error.isEmpty() ? tr("无法启动预览进程。") : _error);
    }
}

void PreviewWidget::finishGeneratedPreview(bool _success,
                                           const QString &_message) {
    if (!_success || !QFileInfo::exists(m_preview_output_path)) {
        QFile::remove(m_preview_output_path);
        setMessage(_message.isEmpty()
                       ? tr("预览未生成有效内容。")
                       : tr("预览生成失败：\n%1").arg(_message));
        return;
    }
    (void)previewText(m_preview_output_path, true, false);
}

void PreviewWidget::setMessage(const QString &_message) {
    m_message_label->setText(_message);
    m_preview_stack->setCurrentWidget(m_message_page);
}

void PreviewWidget::setTextContent(const QString &_content,
                                   bool _render_markdown) {
    m_source_preview->setPlainText(_content);
    if (_render_markdown) {
        m_rendered_preview->setMarkdown(_content);
    } else {
        m_rendered_preview->setPlainText(_content);
    }
    m_preview_stack->setCurrentWidget(m_text_tabs);
}

} // namespace doc2md
