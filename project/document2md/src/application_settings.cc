#include <doc2md/application_settings.hxx>

#include <QSettings>
#include <QVariant>

namespace doc2md {

PythonMode ApplicationSettings::pythonMode() const {
    QSettings _settings;
    const int _value = _settings.value(QStringLiteral("python/mode"),
                                        static_cast<int>(PythonMode::Automatic))
                           .toInt();
    switch (static_cast<PythonMode>(_value)) {
    case PythonMode::Bundled:
    case PythonMode::Automatic:
    case PythonMode::Custom:
        return static_cast<PythonMode>(_value);
    }
    return PythonMode::Automatic;
}

void ApplicationSettings::setPythonMode(PythonMode _mode) const {
    QSettings _settings;
    _settings.setValue(QStringLiteral("python/mode"), static_cast<int>(_mode));
}

QString ApplicationSettings::pythonExecutable() const {
    QSettings _settings;
    return _settings.value(QStringLiteral("python/executable")).toString();
}

void ApplicationSettings::setPythonExecutable(const QString &_path) const {
    QSettings _settings;
    _settings.setValue(QStringLiteral("python/executable"), _path);
}

QString ApplicationSettings::lastOutputDirectory() const {
    QSettings _settings;
    return _settings.value(QStringLiteral("output/lastDirectory")).toString();
}

void ApplicationSettings::setLastOutputDirectory(const QString &_path) const {
    QSettings _settings;
    _settings.setValue(QStringLiteral("output/lastDirectory"), _path);
}

QByteArray ApplicationSettings::windowGeometry() const {
    QSettings _settings;
    return _settings.value(QStringLiteral("ui/windowGeometry")).toByteArray();
}

void ApplicationSettings::setWindowGeometry(const QByteArray &_geometry) const {
    QSettings _settings;
    _settings.setValue(QStringLiteral("ui/windowGeometry"), _geometry);
}

QByteArray ApplicationSettings::splitterState() const {
    QSettings _settings;
    return _settings.value(QStringLiteral("ui/splitterState")).toByteArray();
}

void ApplicationSettings::setSplitterState(const QByteArray &_state) const {
    QSettings _settings;
    _settings.setValue(QStringLiteral("ui/splitterState"), _state);
}

void ApplicationSettings::sync() const {
    QSettings _settings;
    _settings.sync();
}

} // namespace doc2md
