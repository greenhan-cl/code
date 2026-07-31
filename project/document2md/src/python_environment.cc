#include <doc2md/python_environment.hxx>

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QStandardPaths>
#include <QStringList>
#include <QTimer>

namespace doc2md {
namespace {

QStringList bundledPythonCandidates() {
    const QString _application_directory = QCoreApplication::applicationDirPath();
#ifdef Q_OS_WIN
    return {
        _application_directory + QStringLiteral("/runtime/python/python.exe"),
        _application_directory + QStringLiteral("/python/python.exe")
    };
#else
    return {
        _application_directory + QStringLiteral("/runtime/python/bin/python3"),
        _application_directory + QStringLiteral("/python/bin/python3")
    };
#endif
}

QString findBundledPython() {
    for (const QString &_candidate : bundledPythonCandidates()) {
        const QFileInfo _file_info(_candidate);
        if (_file_info.exists() && _file_info.isFile()
            && _file_info.isExecutable()) {
            return QDir::cleanPath(_file_info.absoluteFilePath());
        }
    }
    return {};
}

QString findSystemPython() {
#ifdef Q_OS_WIN
    const QStringList _names = {
        QStringLiteral("python.exe"), QStringLiteral("python3.exe")
    };
#else
    const QStringList _names = {
        QStringLiteral("python3"), QStringLiteral("python")
    };
#endif
    for (const QString &_name : _names) {
        const QString _executable = QStandardPaths::findExecutable(_name);
        if (!_executable.isEmpty()) {
            return QDir::cleanPath(_executable);
        }
    }
    return {};
}

QString probeScript() {
    return QStringLiteral(
        "import json, platform, struct, sys\n"
        "result = {\"ok\": False, \"version\": platform.python_version(), "
        "\"architecture\": str(struct.calcsize('P') * 8) + '-bit', "
        "\"executable\": sys.executable, \"message\": ''}\n"
        "try:\n"
        "    if sys.version_info[:2] < (3, 10):\n"
        "        raise RuntimeError('Python 版本必须不低于 3.10')\n"
        "    if struct.calcsize('P') * 8 != 64:\n"
        "        raise RuntimeError('必须使用 64 位 Python')\n"
        "    import markitdown\n"
        "    result['ok'] = True\n"
        "except Exception as error:\n"
        "    result['message'] = str(error)\n"
        "print(json.dumps(result, ensure_ascii=False))\n"
        "sys.exit(0 if result['ok'] else 2)\n");
}

} // namespace

PythonEnvironment::PythonEnvironment(QObject *_parent)
    : QObject(_parent)
    , m_timeout_timer(new QTimer(this)) {
    m_timeout_timer->setSingleShot(true);
    m_timeout_timer->setInterval(5000);
    connect(m_timeout_timer, &QTimer::timeout, this, [this]() {
        if (m_process == nullptr) {
            return;
        }
        QProcess *_timed_out_process = m_process;
        completeProbe(false,
                      _timed_out_process->program(),
                      {},
                      {},
                      tr("Python 环境检测超时，请检查所选程序是否可以正常启动。"));
        _timed_out_process->kill();
    });
}

PythonRuntimeConfig PythonEnvironment::currentConfig() {
    const ApplicationSettings _settings;
    PythonRuntimeConfig _config;
    _config.m_mode = _settings.pythonMode();
    _config.m_custom_executable = _settings.pythonExecutable();
    return _config;
}

QString PythonEnvironment::resolveExecutable(const PythonRuntimeConfig &_config) {
    switch (_config.m_mode) {
    case PythonMode::Bundled:
        return findBundledPython();
    case PythonMode::Custom: {
        const QFileInfo _file_info(_config.m_custom_executable.trimmed());
        if (_file_info.exists() && _file_info.isFile()
            && _file_info.isExecutable()) {
            return QDir::cleanPath(_file_info.absoluteFilePath());
        }
        return {};
    }
    case PythonMode::Automatic: {
        const QString _bundled_python = findBundledPython();
        return _bundled_python.isEmpty() ? findSystemPython() : _bundled_python;
    }
    }
    return {};
}

QString PythonEnvironment::findEngineSourcePath() {
    const QString _application_directory = QCoreApplication::applicationDirPath();
    const QStringList _candidates = {
        _application_directory + QStringLiteral("/runtime/markitdown/src"),
        _application_directory + QStringLiteral("/3rdparty/markitdown/src"),
        _application_directory + QStringLiteral("/../3rdparty/markitdown/src"),
        _application_directory + QStringLiteral("/../../3rdparty/markitdown/src"),
        _application_directory + QStringLiteral("/../../../3rdparty/markitdown/src"),
        QStringLiteral(DOC2MD_MARKITDOWN_SOURCE_DIR)
    };
    for (const QString &_candidate : _candidates) {
        const QString _path = QDir::cleanPath(_candidate);
        if (QFileInfo::exists(_path + QStringLiteral("/markitdown/__main__.py"))) {
            return _path;
        }
    }
    return {};
}

QProcessEnvironment PythonEnvironment::processEnvironment(
    const QString &_engine_source_path) {
    QProcessEnvironment _environment = QProcessEnvironment::systemEnvironment();
    if (!_engine_source_path.isEmpty()) {
        QString _python_path = _engine_source_path;
        const QString _old_python_path =
            _environment.value(QStringLiteral("PYTHONPATH"));
        if (!_old_python_path.isEmpty()) {
            _python_path += QDir::listSeparator() + _old_python_path;
        }
        _environment.insert(QStringLiteral("PYTHONPATH"), _python_path);
    }
    _environment.insert(QStringLiteral("PYTHONUTF8"), QStringLiteral("1"));
    _environment.insert(QStringLiteral("PYTHONIOENCODING"), QStringLiteral("utf-8"));
    return _environment;
}

bool PythonEnvironment::isProbing() const {
    return m_process != nullptr;
}

void PythonEnvironment::probe(const PythonRuntimeConfig &_config) {
    cancelProbe();
    emit probeStarted();

    const QString _executable = resolveExecutable(_config);
    if (_executable.isEmpty()) {
        QTimer::singleShot(0, this, [this, _config]() {
            const QString _message = _config.m_mode == PythonMode::Custom
                                         ? tr("指定的 Python 路径无效，请重新选择 python.exe。")
                                         : tr("没有找到可用的 Python，请配置运行环境。");
            emit probeFinished(false, {}, {}, {}, _message);
        });
        return;
    }

    startProbeProcess(_executable);
}

void PythonEnvironment::startProbeProcess(const QString &_executable) {
    m_process = new QProcess(this);
    QProcess *_probe_process = m_process;
    _probe_process->setProgram(_executable);
    _probe_process->setArguments({QStringLiteral("-c"), probeScript()});
    _probe_process->setProcessEnvironment(
        processEnvironment(findEngineSourcePath()));

    connect(_probe_process, &QProcess::errorOccurred, this,
            [this, _probe_process](QProcess::ProcessError _error) {
                if (_probe_process != m_process
                    || _error != QProcess::FailedToStart) {
                    return;
                }
                completeProbe(false,
                              _probe_process->program(),
                              {},
                              {},
                              tr("无法启动 Python：%1")
                                  .arg(_probe_process->errorString()));
            });
    connect(_probe_process,
            qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
            this,
            [this, _probe_process](int, QProcess::ExitStatus _exit_status) {
                if (_probe_process != m_process) {
                    return;
                }
                handleProbeProcessFinished(
                    _probe_process, _exit_status == QProcess::NormalExit);
            });

    m_timeout_timer->start();
    _probe_process->start();
}

void PythonEnvironment::handleProbeProcessFinished(
    QProcess *_probe_process, bool _normal_exit) {
    const QByteArray _all_output =
        _probe_process->readAllStandardOutput().trimmed();
    const qsizetype _last_line_start = _all_output.lastIndexOf('\n');
    const QByteArray _standard_output =
        (_last_line_start < 0 ? _all_output
                              : _all_output.mid(_last_line_start + 1))
            .trimmed();
    QJsonParseError _parse_error;
    const QJsonDocument _document =
        QJsonDocument::fromJson(_standard_output, &_parse_error);
    if (!_normal_exit || _parse_error.error != QJsonParseError::NoError
        || !_document.isObject()) {
        QString _error =
            QString::fromUtf8(_probe_process->readAllStandardError()).trimmed();
        if (_error.isEmpty()) {
            _error = tr("Python 环境返回了无法识别的检测结果。");
        }
        completeProbe(false, _probe_process->program(), {}, {}, _error);
        return;
    }

    const QJsonObject _result = _document.object();
    const bool _success = _result.value(QStringLiteral("ok")).toBool();
    const QString _executable =
        _result.value(QStringLiteral("executable")).toString();
    const QString _version = _result.value(QStringLiteral("version")).toString();
    const QString _architecture =
        _result.value(QStringLiteral("architecture")).toString();
    QString _message = _result.value(QStringLiteral("message")).toString();
    if (_success) {
        _message = tr("Python %1（%2），MarkItDown 可用。")
                       .arg(_version, _architecture);
    } else if (_message.isEmpty()) {
        _message = tr("当前 Python 无法加载 MarkItDown。");
    }
    completeProbe(_success,
                  _executable,
                  _version,
                  _architecture,
                  _message);
}

void PythonEnvironment::cancelProbe() {
    if (m_process == nullptr) {
        return;
    }
    QProcess *_cancelled_process = m_process;
    m_process = nullptr;
    m_timeout_timer->stop();
    _cancelled_process->disconnect(this);
    _cancelled_process->kill();
    _cancelled_process->deleteLater();
}

void PythonEnvironment::completeProbe(bool _success,
                                      const QString &_executable,
                                      const QString &_version,
                                      const QString &_architecture,
                                      const QString &_message) {
    if (m_process == nullptr) {
        return;
    }
    QProcess *_completed_process = m_process;
    m_process = nullptr;
    m_timeout_timer->stop();
    _completed_process->disconnect(this);
    _completed_process->deleteLater();
    emit probeFinished(_success,
                       _executable,
                       _version,
                       _architecture,
                       _message);
}

} // namespace doc2md
