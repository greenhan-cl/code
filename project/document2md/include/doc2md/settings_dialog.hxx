#ifndef DOC2MD_SETTINGS_DIALOG_HXX
#define DOC2MD_SETTINGS_DIALOG_HXX

#include <QDialog>

#include <doc2md/application_settings.hxx>

class QComboBox;
class QDialogButtonBox;
class QFrame;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;

namespace doc2md {

class PythonEnvironment;
struct PythonRuntimeConfig;

/**
 * @brief 配置并检测 Python 运行环境。
 */
class SettingsDialog final : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *_parent = nullptr);
    ~SettingsDialog() override;

signals:
    void settingsChanged();

private:
    void setupUi();
    [[nodiscard]] QFrame *createSettingsCard();
    [[nodiscard]] QFrame *createStatusCard();
    [[nodiscard]] QHBoxLayout *createButtonLayout();
    void setupConnections();
    void loadSettings();
    void updateCustomPathControls();
    void browsePythonExecutable();
    void testEnvironment();
    void saveSettings();
    void updateStatus(bool _success,
                      const QString &_title,
                      const QString &_message);
    [[nodiscard]] PythonRuntimeConfig selectedConfig() const;

    ApplicationSettings m_settings;
    PythonEnvironment *m_python_environment = nullptr;
    QComboBox *m_mode_combo = nullptr;
    QLineEdit *m_python_path_edit = nullptr;
    QPushButton *m_browse_button = nullptr;
    QPushButton *m_test_button = nullptr;
    QPushButton *m_save_button = nullptr;
    QDialogButtonBox *m_button_box = nullptr;
    QFrame *m_status_frame = nullptr;
    QLabel *m_status_title_label = nullptr;
    QLabel *m_status_message_label = nullptr;
};

} // namespace doc2md

#endif // DOC2MD_SETTINGS_DIALOG_HXX
