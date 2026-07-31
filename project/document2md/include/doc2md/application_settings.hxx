#ifndef DOC2MD_APPLICATION_SETTINGS_HXX
#define DOC2MD_APPLICATION_SETTINGS_HXX

#include <QByteArray>
#include <QString>

namespace doc2md {

/**
 * @brief Python 运行环境的选择方式。
 */
enum class PythonMode {
    Bundled = 0,
    Automatic,
    Custom
};

/**
 * @brief 使用 QSettings 持久化用户级应用配置。
 */
class ApplicationSettings final {
public:
    [[nodiscard]] PythonMode pythonMode() const;
    void setPythonMode(PythonMode _mode) const;

    [[nodiscard]] QString pythonExecutable() const;
    void setPythonExecutable(const QString &_path) const;

    [[nodiscard]] QString lastOutputDirectory() const;
    void setLastOutputDirectory(const QString &_path) const;

    [[nodiscard]] QByteArray windowGeometry() const;
    void setWindowGeometry(const QByteArray &_geometry) const;

    [[nodiscard]] QByteArray splitterState() const;
    void setSplitterState(const QByteArray &_state) const;

    /** @brief 立即把尚未提交的设置写入持久化存储。 */
    void sync() const;
};

} // namespace doc2md

#endif // DOC2MD_APPLICATION_SETTINGS_HXX
