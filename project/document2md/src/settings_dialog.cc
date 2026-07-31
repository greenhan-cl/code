#include <doc2md/settings_dialog.hxx>

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStyle>
#include <QVBoxLayout>

#include <doc2md/python_environment.hxx>

namespace doc2md {

SettingsDialog::SettingsDialog(QWidget *_parent)
    : QDialog(_parent)
    , m_python_environment(new PythonEnvironment(this)) {
    setupUi();
    setupConnections();
    loadSettings();
}

SettingsDialog::~SettingsDialog() {
    m_python_environment->cancelProbe();
}

void SettingsDialog::setupUi() {
    setObjectName(QStringLiteral("SettingsDialog"));
    setWindowTitle(tr("设置"));
    setModal(true);
    setMinimumWidth(620);

    auto *_main_layout = new QVBoxLayout(this);
    _main_layout->setContentsMargins(24, 24, 24, 24);
    _main_layout->setSpacing(16);

    auto *_title_label = new QLabel(tr("Python 运行环境"), this);
    _title_label->setObjectName(QStringLiteral("settingsTitle"));
    _main_layout->addWidget(_title_label);

    auto *_description_label = new QLabel(
        tr("选择转换任务使用的 Python。版本由程序自动检测，无需手工填写。"),
        this);
    _description_label->setObjectName(QStringLiteral("settingsDescription"));
    _description_label->setWordWrap(true);
    _main_layout->addWidget(_description_label);

    _main_layout->addWidget(createSettingsCard());
    _main_layout->addWidget(createStatusCard());
    _main_layout->addLayout(createButtonLayout());
}

QFrame *SettingsDialog::createSettingsCard() {
    auto *_settings_card = new QFrame(this);
    _settings_card->setObjectName(QStringLiteral("settingsCard"));
    auto *_form_layout = new QFormLayout(_settings_card);
    _form_layout->setContentsMargins(18, 18, 18, 18);
    _form_layout->setHorizontalSpacing(14);
    _form_layout->setVerticalSpacing(14);
    _form_layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    m_mode_combo = new QComboBox(_settings_card);
    m_mode_combo->addItem(tr("使用内置 Python（发布版推荐）"),
                          static_cast<int>(PythonMode::Bundled));
    m_mode_combo->addItem(tr("自动检测内置或系统 Python"),
                          static_cast<int>(PythonMode::Automatic));
    m_mode_combo->addItem(tr("使用指定的 Python"),
                          static_cast<int>(PythonMode::Custom));
    _form_layout->addRow(tr("运行模式"), m_mode_combo);

    auto *_path_layout = new QHBoxLayout;
    _path_layout->setContentsMargins(0, 0, 0, 0);
    _path_layout->setSpacing(8);
    m_python_path_edit = new QLineEdit(_settings_card);
    m_python_path_edit->setPlaceholderText(tr("选择 python.exe 或虚拟环境中的 Python"));
    m_browse_button = new QPushButton(tr("浏览..."), _settings_card);
    _path_layout->addWidget(m_python_path_edit, 1);
    _path_layout->addWidget(m_browse_button);
    _form_layout->addRow(tr("Python 路径"), _path_layout);

    auto *_hint_label = new QLabel(
        tr("自定义模式支持系统 Python 和虚拟环境。发布给其他用户时，建议随程序携带受控的内置 Python。"),
        _settings_card);
    _hint_label->setObjectName(QStringLiteral("settingsHint"));
    _hint_label->setWordWrap(true);
    _form_layout->addRow(QString(), _hint_label);
    return _settings_card;
}

QFrame *SettingsDialog::createStatusCard() {
    m_status_frame = new QFrame(this);
    m_status_frame->setObjectName(QStringLiteral("environmentStatusCard"));
    auto *_status_layout = new QVBoxLayout(m_status_frame);
    _status_layout->setContentsMargins(16, 14, 16, 14);
    _status_layout->setSpacing(5);
    m_status_title_label = new QLabel(tr("尚未检测"), m_status_frame);
    m_status_title_label->setObjectName(QStringLiteral("environmentStatusTitle"));
    m_status_message_label = new QLabel(
        tr("点击“测试环境”检查 Python 版本与 MarkItDown。"),
        m_status_frame);
    m_status_message_label->setObjectName(QStringLiteral("environmentStatusMessage"));
    m_status_message_label->setWordWrap(true);
    _status_layout->addWidget(m_status_title_label);
    _status_layout->addWidget(m_status_message_label);
    return m_status_frame;
}

QHBoxLayout *SettingsDialog::createButtonLayout() {
    auto *_button_layout = new QHBoxLayout;
    _button_layout->setContentsMargins(0, 0, 0, 0);
    m_test_button = new QPushButton(tr("测试环境"), this);
    m_test_button->setObjectName(QStringLiteral("testEnvironmentButton"));
    m_button_box = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    m_save_button = m_button_box->button(QDialogButtonBox::Save);
    m_save_button->setObjectName(QStringLiteral("saveSettingsButton"));
    m_save_button->setText(tr("保存"));
    m_button_box->button(QDialogButtonBox::Cancel)->setText(tr("取消"));
    _button_layout->addWidget(m_test_button);
    _button_layout->addStretch(1);
    _button_layout->addWidget(m_button_box);
    return _button_layout;
}

void SettingsDialog::setupConnections() {
    connect(m_mode_combo, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::updateCustomPathControls);
    connect(m_browse_button, &QPushButton::clicked,
            this, &SettingsDialog::browsePythonExecutable);
    connect(m_test_button, &QPushButton::clicked,
            this, &SettingsDialog::testEnvironment);
    connect(m_button_box, &QDialogButtonBox::accepted,
            this, &SettingsDialog::saveSettings);
    connect(m_button_box, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
    connect(m_python_environment, &PythonEnvironment::probeStarted, this, [this]() {
        m_test_button->setEnabled(false);
        m_save_button->setEnabled(false);
        m_status_frame->setProperty("environmentState", QStringLiteral("checking"));
        m_status_title_label->setText(tr("正在检测..."));
        m_status_message_label->setText(tr("正在启动所选 Python 并导入 MarkItDown。"));
        m_status_frame->style()->unpolish(m_status_frame);
        m_status_frame->style()->polish(m_status_frame);
    });
    connect(m_python_environment, &PythonEnvironment::probeFinished,
            this,
            [this](bool _success,
                   const QString &_executable,
                   const QString &_version,
                   const QString &_architecture,
                   const QString &_message) {
                m_test_button->setEnabled(true);
                m_save_button->setEnabled(true);
                QString _title = _success ? tr("环境可用") : tr("环境不可用");
                if (_success) {
                    _title = tr("Python %1 · %2").arg(_version, _architecture);
                }
                QString _detail = _message;
                if (_success && !_executable.isEmpty()) {
                    _detail += tr("\n%1").arg(QDir::toNativeSeparators(_executable));
                }
                updateStatus(_success, _title, _detail);
            });
}

void SettingsDialog::loadSettings() {
    const int _mode_value = static_cast<int>(m_settings.pythonMode());
    const int _mode_index = m_mode_combo->findData(_mode_value);
    m_mode_combo->setCurrentIndex(_mode_index < 0 ? 1 : _mode_index);
    m_python_path_edit->setText(
        QDir::toNativeSeparators(m_settings.pythonExecutable()));
    updateCustomPathControls();
}

void SettingsDialog::updateCustomPathControls() {
    const bool _custom_mode = selectedConfig().m_mode == PythonMode::Custom;
    m_python_path_edit->setEnabled(_custom_mode);
    m_browse_button->setEnabled(_custom_mode);
}

void SettingsDialog::browsePythonExecutable() {
#ifdef Q_OS_WIN
    const QString _filter = tr("Python 可执行文件 (python.exe);;可执行文件 (*.exe)");
#else
    const QString _filter = tr("所有文件 (*)");
#endif
    const QString _current_path = m_python_path_edit->text().trimmed();
    const QString _initial_path = _current_path.isEmpty()
                                      ? QDir::homePath()
                                      : QFileInfo(_current_path).absolutePath();
    const QString _path = QFileDialog::getOpenFileName(
        this, tr("选择 Python 可执行程序"), _initial_path, _filter);
    if (!_path.isEmpty()) {
        m_python_path_edit->setText(QDir::toNativeSeparators(_path));
    }
}

void SettingsDialog::testEnvironment() {
    m_python_environment->probe(selectedConfig());
}

void SettingsDialog::saveSettings() {
    const PythonRuntimeConfig _config = selectedConfig();
    if (_config.m_mode == PythonMode::Custom) {
        const QFileInfo _file_info(_config.m_custom_executable);
        if (!_file_info.exists() || !_file_info.isFile()
            || !_file_info.isExecutable()) {
            updateStatus(false,
                         tr("路径无效"),
                         tr("请选择存在的 Python 可执行文件。"));
            return;
        }
    }

    m_settings.setPythonMode(_config.m_mode);
    const QString _custom_executable =
        _config.m_custom_executable.isEmpty()
            ? QString()
            : QDir::cleanPath(
                  QFileInfo(_config.m_custom_executable).absoluteFilePath());
    m_settings.setPythonExecutable(_custom_executable);
    m_settings.sync();
    emit settingsChanged();
    accept();
}

void SettingsDialog::updateStatus(bool _success,
                                  const QString &_title,
                                  const QString &_message) {
    m_status_frame->setProperty(
        "environmentState", _success ? QStringLiteral("ready")
                                      : QStringLiteral("error"));
    m_status_title_label->setText(_title);
    m_status_message_label->setText(_message);
    m_status_frame->style()->unpolish(m_status_frame);
    m_status_frame->style()->polish(m_status_frame);
    m_status_frame->update();
}

PythonRuntimeConfig SettingsDialog::selectedConfig() const {
    PythonRuntimeConfig _config;
    _config.m_mode = static_cast<PythonMode>(m_mode_combo->currentData().toInt());
    _config.m_custom_executable =
        QDir::fromNativeSeparators(m_python_path_edit->text().trimmed());
    return _config;
}

} // namespace doc2md
