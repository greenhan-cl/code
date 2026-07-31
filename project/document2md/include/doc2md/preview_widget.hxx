#ifndef DOC2MD_PREVIEW_WIDGET_HXX
#define DOC2MD_PREVIEW_WIDGET_HXX

#include <QString>
#include <QWidget>

class QLabel;
class QPdfDocument;
class QPdfView;
class QPlainTextEdit;
class QScrollArea;
class QStackedWidget;
class QTabWidget;
class QTextBrowser;

namespace doc2md {

class ConversionRunner;

/**
 * @brief 在独立 QWidget 窗口中预览源文件或 Markdown 输出文件。
 */
class PreviewWidget final : public QWidget {
    Q_OBJECT

public:
    explicit PreviewWidget(const QString &_path, QWidget *_parent = nullptr);
    ~PreviewWidget() override;

private:
    void setupUi();
    void setupMessagePage();
    void setupTextPage();
    void setupImagePage();
    void setupPdfPage();
    void loadFile(const QString &_path);
    [[nodiscard]] bool previewDirectly(const QString &_path);
    [[nodiscard]] bool previewText(const QString &_path,
                                   bool _render_markdown,
                                   bool _render_html);
    [[nodiscard]] bool previewImage(const QString &_path);
    [[nodiscard]] bool previewPdf(const QString &_path);
    void previewWithMarkItDown(const QString &_path);
    void finishGeneratedPreview(bool _success, const QString &_message);
    [[nodiscard]] QString makePreviewOutputPath(const QString &_path) const;
    void setMessage(const QString &_message);
    void setTextContent(const QString &_content, bool _render_markdown);

    ConversionRunner *m_runner = nullptr;
    QStackedWidget *m_preview_stack = nullptr;
    QWidget *m_message_page = nullptr;
    QLabel *m_message_label = nullptr;
    QTabWidget *m_text_tabs = nullptr;
    QTextBrowser *m_rendered_preview = nullptr;
    QPlainTextEdit *m_source_preview = nullptr;
    QScrollArea *m_image_scroll_area = nullptr;
    QWidget *m_image_page = nullptr;
    QLabel *m_image_label = nullptr;
    QWidget *m_pdf_page = nullptr;
    QPdfDocument *m_pdf_document = nullptr;
    QPdfView *m_pdf_view = nullptr;
    QString m_preview_output_path;
};

} // namespace doc2md

#endif // DOC2MD_PREVIEW_WIDGET_HXX
