#include <QApplication>
#include <QFile>
#include <QIcon>
#include <QStyleFactory>

#include <doc2md/main_widget.hxx>

int main(int _argc, char *_argv[]) {
    QApplication _application(_argc, _argv);
    QApplication::setApplicationName(QStringLiteral("doc2md"));
    QApplication::setApplicationDisplayName(QStringLiteral("doc2md"));
    QApplication::setApplicationVersion(QStringLiteral(DOC2MD_VERSION));
    QApplication::setOrganizationName(QStringLiteral("doc2md"));
    QApplication::setWindowIcon(
        QIcon(QStringLiteral(":/doc2md/images/app.png")));
    QApplication::setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    QFile _style_file(QStringLiteral(":/doc2md/styles/light.qss"));
    if (_style_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        _application.setStyleSheet(QString::fromUtf8(_style_file.readAll()));
    }

    doc2md::MainWidget _main_widget;
    _main_widget.show();

    return _application.exec();
}
