#ifndef DOC2MD_MAIN_WIDGET_HXX
#define DOC2MD_MAIN_WIDGET_HXX

#include <QStringList>
#include <QVector>
#include <QWidget>

#include <doc2md/application_settings.hxx>

class QCloseEvent;
class QPoint;

namespace Ui {
class MainWidget;
}

namespace doc2md {

class ConversionRunner;
class PythonEnvironment;

/**
 * @brief 文档转换主界面与顺序任务调度器。
 */
class MainWidget final : public QWidget {
    Q_OBJECT

public:
    explicit MainWidget(QWidget *_parent = nullptr);
    ~MainWidget() override;

protected:
    /** @brief 持久化窗口和输出目录状态。 */
    void closeEvent(QCloseEvent *_event) override;

private:
    struct Task {
        QString m_input_path;
        QString m_output_name;
        QString m_output_path;
        QString m_status;
        QString m_message;
    };

    void setupUi();
    void setupConnections();
    void setupActionConnections();
    void setupRunnerConnections();
    void setupEnvironmentConnections();
    void restorePersistentState();
    void refreshEnvironmentStatus();
    void showAbout();
    void openSettings();
    void addFiles(const QStringList &_paths);
    void chooseFiles();
    void chooseOutputDirectory();
    void clearTasks();
    void removeSelectedTask();
    void removeTask(int _row);
    void showTaskContextMenu(const QPoint &_position);
    void openInternalPreview(const QString &_path);
    void openFileDefault(const QString &_path);
    void openFileWithApplication(const QString &_path);
    void renameTask(int _row);
    [[nodiscard]] QString taskFilePath(int _row, int _column) const;
    [[nodiscard]] bool validateBatchStart();
    [[nodiscard]] bool validateOutputPaths();
    void prepareBatchTasks();
    void startBatch();
    void startNextTask();
    void finishBatch();
    void setControlsEnabled(bool _enabled);
    void updateTaskRow(int _row);
    void updateOutputItem(int _row, const Task &_task);
    void updateStatusItem(int _row, const Task &_task);
    void updateTaskSummary();
    [[nodiscard]] QString defaultOutputName(const QString &_input_path) const;
    [[nodiscard]] QString normalizeOutputName(const QString &_name,
                                              const QString &_input_path) const;
    [[nodiscard]] bool validateOutputName(const QString &_name,
                                          QString *_error) const;
    [[nodiscard]] QString makeOutputPath(const QString &_input_path,
                                         const QString &_output_name) const;

    Ui::MainWidget *m_ui = nullptr;
    ApplicationSettings m_settings;
    QVector<Task> m_tasks;
    ConversionRunner *m_runner = nullptr;
    PythonEnvironment *m_python_environment = nullptr;
    int m_current_task_index = -1;
    int m_success_count = 0;
    int m_failure_count = 0;
    bool m_batch_running = false;
    bool m_environment_ready = false;
    QString m_environment_message;
};

} // namespace doc2md

#endif // DOC2MD_MAIN_WIDGET_HXX
