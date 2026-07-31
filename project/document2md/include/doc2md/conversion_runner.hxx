#ifndef DOC2MD_CONVERSION_RUNNER_HXX
#define DOC2MD_CONVERSION_RUNNER_HXX

#include <QObject>
#include <QString>

class QProcess;

namespace doc2md {

/**
 * @brief 使用独立 Python 进程调用本地 MarkItDown 引擎。
 */
class ConversionRunner final : public QObject {
    Q_OBJECT

public:
    explicit ConversionRunner(QObject *_parent = nullptr);
    ~ConversionRunner() override;

    /** @brief 返回当前是否有转换进程正在运行。 */
    [[nodiscard]] bool isBusy() const;

    /** @brief 检查 Python 与 MarkItDown 源码目录是否可用。 */
    [[nodiscard]] QString environmentError() const;

    /** @brief 启动一次异步转换。 */
    bool start(const QString &_input_path, const QString &_output_path);

    /** @brief 终止当前进程，不发送 finished 信号。 */
    void cancel();

signals:
    void started();
    void finished(bool _success, const QString &_message);

private:
    /** @brief 查找随程序发布或系统 PATH 中的 Python。 */
    [[nodiscard]] QString findPythonExecutable() const;

    /** @brief 查找项目内的 MarkItDown Python 包源码目录。 */
    [[nodiscard]] QString findEngineSourcePath() const;

    /** @brief 连接转换进程的状态和输出信号。 */
    void connectProcessSignals(QProcess *_process);

    /** @brief 配置转换进程环境与命令参数。 */
    void configureProcess(QProcess *_process,
                          const QString &_engine_source_path,
                          const QString &_input_path,
                          const QString &_output_path);

    /** @brief 释放当前进程并发送最终结果。 */
    void complete(bool _success, const QString &_message);

    QProcess *m_process = nullptr;
    QString m_standard_error;
};

} // namespace doc2md

#endif // DOC2MD_CONVERSION_RUNNER_HXX
