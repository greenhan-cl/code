#include <doc2md/conversion_runner.hxx>

#include <QDir>
#include <QFileInfo>
#include <QProcess>

#include <doc2md/python_environment.hxx>

namespace doc2md {

ConversionRunner::ConversionRunner(QObject *_parent)
    : QObject(_parent) {
}

ConversionRunner::~ConversionRunner() {
    cancel();
}

bool ConversionRunner::isBusy() const {
    return m_process != nullptr;
}

QString ConversionRunner::findPythonExecutable() const {
    return PythonEnvironment::resolveExecutable(PythonEnvironment::currentConfig());
}

QString ConversionRunner::findEngineSourcePath() const {
    return PythonEnvironment::findEngineSourcePath();
}

QString ConversionRunner::environmentError() const {
    if (findPythonExecutable().isEmpty()) {
        return tr("未找到 Python 3.10 及以上版本，请先安装或配置 Python 环境。");
    }
    if (findEngineSourcePath().isEmpty()) {
        return tr("未找到本地 MarkItDown 引擎，请确认 3rdparty/markitdown/src 目录存在。");
    }
    return {};
}

void ConversionRunner::connectProcessSignals(QProcess *_process) {
    connect(_process, &QProcess::started, this, &ConversionRunner::started);
    connect(_process, &QProcess::readyReadStandardError, this, [this, _process]() {
        m_standard_error.append(QString::fromUtf8(_process->readAllStandardError()));
    });
    connect(_process, &QProcess::errorOccurred, this,
            [this, _process](QProcess::ProcessError _error) {
                if (_process == m_process && _error == QProcess::FailedToStart) {
                    complete(false,
                             tr("转换进程启动失败：%1").arg(_process->errorString()));
                }
            });
    connect(_process,
            qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
            this,
            [this, _process](int _exit_code, QProcess::ExitStatus _exit_status) {
                if (_process != m_process) {
                    return;
                }
                m_standard_error.append(
                    QString::fromUtf8(_process->readAllStandardError()));
                if (_exit_status == QProcess::NormalExit && _exit_code == 0) {
                    complete(true, {});
                    return;
                }

                QString _message = m_standard_error.trimmed();
                if (_message.isEmpty()) {
                    _message = _exit_status == QProcess::CrashExit
                                   ? tr("转换进程异常退出。")
                                   : tr("转换失败，进程退出码为 %1。").arg(_exit_code);
                }
                complete(false, _message);
            });
}

void ConversionRunner::configureProcess(QProcess *_process,
                                        const QString &_engine_source_path,
                                        const QString &_input_path,
                                        const QString &_output_path) {
    _process->setProcessEnvironment(
        PythonEnvironment::processEnvironment(_engine_source_path));
    _process->setProgram(findPythonExecutable());
    _process->setArguments({QStringLiteral("-m"),
                            QStringLiteral("markitdown"),
                            QStringLiteral("-o"),
                            QDir::toNativeSeparators(_output_path),
                            QDir::toNativeSeparators(_input_path)});
}

bool ConversionRunner::start(const QString &_input_path, const QString &_output_path) {
    if (isBusy()) {
        return false;
    }
    const QString _engine_source_path = findEngineSourcePath();
    if (findPythonExecutable().isEmpty() || _engine_source_path.isEmpty()) {
        return false;
    }

    m_standard_error.clear();
    m_process = new QProcess(this);
    connectProcessSignals(m_process);
    configureProcess(m_process, _engine_source_path, _input_path, _output_path);
    m_process->start();
    return true;
}

void ConversionRunner::cancel() {
    if (m_process == nullptr) {
        return;
    }

    QProcess *_cancelled_process = m_process;
    m_process = nullptr;
    _cancelled_process->disconnect(this);
    _cancelled_process->kill();
    if (_cancelled_process->state() != QProcess::NotRunning) {
        (void)_cancelled_process->waitForFinished(1000);
    }
    _cancelled_process->deleteLater();
    m_standard_error.clear();
}

void ConversionRunner::complete(bool _success, const QString &_message) {
    if (m_process == nullptr) {
        return;
    }
    QProcess *_completed_process = m_process;
    m_process = nullptr;
    _completed_process->deleteLater();
    emit finished(_success, _message);
}

} // namespace doc2md
