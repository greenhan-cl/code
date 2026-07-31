#ifndef DOC2MD_PYTHON_ENVIRONMENT_HXX
#define DOC2MD_PYTHON_ENVIRONMENT_HXX

#include <QObject>
#include <QProcessEnvironment>
#include <QString>

#include <doc2md/application_settings.hxx>

class QProcess;
class QTimer;

namespace doc2md {

/**
 * @brief 一次 Python 运行环境选择。
 */
struct PythonRuntimeConfig {
    PythonMode m_mode = PythonMode::Automatic;
    QString m_custom_executable;
};

/**
 * @brief 解析 Python 路径并异步检查版本与 MarkItDown 可用性。
 */
class PythonEnvironment final : public QObject {
    Q_OBJECT

public:
    explicit PythonEnvironment(QObject *_parent = nullptr);

    [[nodiscard]] static PythonRuntimeConfig currentConfig();
    [[nodiscard]] static QString resolveExecutable(const PythonRuntimeConfig &_config);
    [[nodiscard]] static QString findEngineSourcePath();
    [[nodiscard]] static QProcessEnvironment processEnvironment(
        const QString &_engine_source_path);

    [[nodiscard]] bool isProbing() const;
    void probe(const PythonRuntimeConfig &_config);
    void cancelProbe();

signals:
    void probeStarted();
    void probeFinished(bool _success,
                       const QString &_executable,
                       const QString &_version,
                       const QString &_architecture,
                       const QString &_message);

private:
    void startProbeProcess(const QString &_executable);
    void handleProbeProcessFinished(QProcess *_probe_process,
                                    bool _normal_exit);
    void completeProbe(bool _success,
                       const QString &_executable,
                       const QString &_version,
                       const QString &_architecture,
                       const QString &_message);

    QProcess *m_process = nullptr;
    QTimer *m_timeout_timer = nullptr;
};

} // namespace doc2md

#endif // DOC2MD_PYTHON_ENVIRONMENT_HXX
