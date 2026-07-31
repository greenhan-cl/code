#include <doc2md/main_widget.hxx>

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QColor>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QHeaderView>
#include <QIcon>
#include <QInputDialog>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QProgressBar>
#include <QProcess>
#include <QPoint>
#include <QPushButton>
#include <QRegularExpression>
#include <QSet>
#include <QSignalBlocker>
#include <QSplitter>
#include <QStandardPaths>
#include <QStyle>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTimer>
#include <QUrl>

#include <doc2md/conversion_runner.hxx>
#include <doc2md/drop_area.hxx>
#include <doc2md/preview_widget.hxx>
#include <doc2md/python_environment.hxx>
#include <doc2md/settings_dialog.hxx>

#include "ui_main_widget.h"

namespace doc2md {

MainWidget::MainWidget(QWidget *_parent)
    : QWidget(_parent)
    , m_ui(new Ui::MainWidget)
    , m_runner(new ConversionRunner(this))
    , m_python_environment(new PythonEnvironment(this)) {
    setupUi();
    setupConnections();
    restorePersistentState();
    refreshEnvironmentStatus();
}

MainWidget::~MainWidget() {
    delete m_ui;
}

void MainWidget::closeEvent(QCloseEvent *_event) {
    m_settings.setWindowGeometry(saveGeometry());
    m_settings.setLastOutputDirectory(
        QDir::fromNativeSeparators(
            m_ui->m_output_directory_edit->text().trimmed()));
    m_settings.sync();
    QWidget::closeEvent(_event);
}

void MainWidget::setupUi() {
    m_ui->setupUi(this);
    setWindowTitle(tr("doc2md · 文档转 Markdown"));

    m_ui->m_about_button->setIcon(
        QIcon(QStringLiteral(":/doc2md/icons/about.svg")));
    m_ui->m_about_button->setIconSize(QSize(22, 22));
    m_ui->m_settings_button->setIcon(
        QIcon(QStringLiteral(":/doc2md/icons/settings.svg")));
    m_ui->m_settings_button->setIconSize(QSize(22, 22));
    m_ui->m_start_button->setIcon(
        QIcon(QStringLiteral(":/doc2md/icons/play.svg")));
    m_ui->m_start_button->setIconSize(QSize(18, 18));
    m_ui->m_empty_state_label->setText(
        tr("<div style='text-align:center'>"
           "<img src=':/doc2md/icons/empty-state.svg' width='96' height='80'/><br/>"
           "<span style='font-size:16px;font-weight:600;color:#334155'>"
           "还没有转换任务</span><br/>"
           "<span style='font-size:13px;color:#94a3b8'>"
           "拖入文档或点击上方区域选择文件</span>"
           "</div>"));
    m_ui->m_engine_status_label->setProperty(
        "engineState", QStringLiteral("checking"));

    QHeaderView *_header = m_ui->m_task_table->horizontalHeader();
    _header->setSectionResizeMode(0, QHeaderView::Stretch);
    _header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    _header->setSectionResizeMode(2, QHeaderView::Stretch);
    _header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    _header->setMinimumSectionSize(72);
    m_ui->m_task_table->verticalHeader()->setVisible(false);
    m_ui->m_task_table->verticalHeader()->setDefaultSectionSize(46);
    m_ui->m_task_table->setColumnWidth(1, 72);
    m_ui->m_task_table->setColumnWidth(3, 86);

    m_ui->m_content_splitter->setStretchFactor(0, 1);
    m_ui->m_content_splitter->setSizes({1100});
    m_ui->m_batch_progress->setVisible(false);
    m_ui->m_clear_button->setEnabled(false);
    m_ui->m_remove_selected_button->setEnabled(false);
    m_ui->m_start_button->setEnabled(false);
    m_ui->m_task_table->setContextMenuPolicy(Qt::CustomContextMenu);
}

void MainWidget::setupConnections() {
    setupActionConnections();
    setupRunnerConnections();
    setupEnvironmentConnections();
}

void MainWidget::setupActionConnections() {
    connect(m_ui->m_drop_area, &DropArea::filesDropped,
            this, &MainWidget::addFiles);
    connect(m_ui->m_drop_area, &DropArea::browseRequested,
            this, &MainWidget::chooseFiles);
    connect(m_ui->m_clear_button, &QPushButton::clicked,
            this, &MainWidget::clearTasks);
    connect(m_ui->m_remove_selected_button, &QPushButton::clicked,
            this, &MainWidget::removeSelectedTask);
    connect(m_ui->m_browse_output_button, &QPushButton::clicked,
            this, &MainWidget::chooseOutputDirectory);
    connect(m_ui->m_start_button, &QPushButton::clicked,
            this, &MainWidget::startBatch);
    connect(m_ui->m_about_button, &QPushButton::clicked,
            this, &MainWidget::showAbout);
    connect(m_ui->m_settings_button, &QPushButton::clicked,
            this, &MainWidget::openSettings);
    connect(m_ui->m_task_table, &QTableWidget::customContextMenuRequested,
            this, &MainWidget::showTaskContextMenu);
    connect(m_ui->m_task_table, &QTableWidget::itemSelectionChanged,
            this, [this]() {
                m_ui->m_remove_selected_button->setEnabled(
                    !m_batch_running
                    && m_ui->m_task_table->currentRow() >= 0);
            });
}

void MainWidget::setupRunnerConnections() {
    connect(m_runner, &ConversionRunner::finished, this,
            [this](bool _success, const QString &_message) {
                if (m_current_task_index < 0
                    || m_current_task_index >= m_tasks.size()) {
                    return;
                }

                Task &_task = m_tasks[m_current_task_index];
                if (_success && QFileInfo::exists(_task.m_output_path)) {
                    _task.m_status = tr("成功");
                    _task.m_message.clear();
                    ++m_success_count;
                } else {
                    _task.m_status = tr("失败");
                    _task.m_message = _success
                                          ? tr("转换进程结束，但没有生成输出文件。")
                                          : _message;
                    ++m_failure_count;
                }
                updateTaskRow(m_current_task_index);
                m_ui->m_batch_progress->setValue(m_current_task_index + 1);
                QTimer::singleShot(0, this, &MainWidget::startNextTask);
            });
}

void MainWidget::setupEnvironmentConnections() {
    connect(m_python_environment, &PythonEnvironment::probeStarted, this, [this]() {
        m_environment_ready = false;
        m_environment_message = tr("正在检测 Python 运行环境，请稍候。");
        m_ui->m_engine_status_label->setText(tr("● 正在检测环境"));
        m_ui->m_engine_status_label->setProperty(
            "engineState", QStringLiteral("checking"));
        m_ui->m_engine_status_label->style()->unpolish(
            m_ui->m_engine_status_label);
        m_ui->m_engine_status_label->style()->polish(
            m_ui->m_engine_status_label);
    });
    connect(m_python_environment, &PythonEnvironment::probeFinished,
            this,
            [this](bool _success,
                   const QString &,
                   const QString &_version,
                   const QString &,
                   const QString &_message) {
                m_environment_ready = _success;
                m_environment_message = _message;
                m_ui->m_engine_status_label->setText(
                    _success ? tr("● 引擎就绪 (%1)").arg(_version)
                             : tr("● 环境不可用"));
                m_ui->m_engine_status_label->setToolTip(_message);
                m_ui->m_engine_status_label->setProperty(
                    "engineState", _success ? QStringLiteral("ready")
                                            : QStringLiteral("error"));
                m_ui->m_engine_status_label->style()->unpolish(
                    m_ui->m_engine_status_label);
                m_ui->m_engine_status_label->style()->polish(
                    m_ui->m_engine_status_label);
                m_ui->m_engine_status_label->update();
            });
}

void MainWidget::restorePersistentState() {
    const QByteArray _geometry = m_settings.windowGeometry();
    if (_geometry.isEmpty()) {
        resize(1180, 760);
    } else {
        restoreGeometry(_geometry);
    }

    m_ui->m_output_directory_edit->setText(
        QDir::toNativeSeparators(m_settings.lastOutputDirectory()));
}

void MainWidget::refreshEnvironmentStatus() {
    m_python_environment->probe(PythonEnvironment::currentConfig());
}

void MainWidget::showAbout() {
    QMessageBox::about(
        this,
        tr("关于 doc2md"),
        tr("<h2>doc2md</h2>"
           "<p>版本 %1</p>"
           "<p>将本地文档批量转换为清晰、可编辑的 Markdown。</p>"
           "<p>基于 Qt %2</p>")
            .arg(QApplication::applicationVersion().toHtmlEscaped(),
                 QString::fromLatin1(qVersion())));
}

void MainWidget::openSettings() {
    SettingsDialog _dialog(this);
    connect(&_dialog, &SettingsDialog::settingsChanged,
            this, &MainWidget::refreshEnvironmentStatus);
    _dialog.exec();
}

void MainWidget::addFiles(const QStringList &_paths) {
    if (m_batch_running) {
        return;
    }

    QSet<QString> _known_paths;
    for (const Task &_task : m_tasks) {
        _known_paths.insert(QDir::cleanPath(_task.m_input_path).toLower());
    }

    int _ignored_count = 0;
    m_ui->m_task_table->setUpdatesEnabled(false);
    for (const QString &_path : _paths) {
        const QFileInfo _file_info(_path);
        const QString _absolute_path = _file_info.absoluteFilePath();
        const QString _key = QDir::cleanPath(_absolute_path).toLower();
        if (!_file_info.exists() || !_file_info.isFile()
            || _known_paths.contains(_key)) {
            ++_ignored_count;
            continue;
        }

        Task _task;
        _task.m_input_path = _absolute_path;
        _task.m_output_name = defaultOutputName(_absolute_path);
        _task.m_status = tr("等待");
        m_tasks.append(_task);
        _known_paths.insert(_key);
        m_ui->m_task_table->insertRow(m_ui->m_task_table->rowCount());
        updateTaskRow(m_tasks.size() - 1);
    }
    m_ui->m_task_table->setUpdatesEnabled(true);
    m_ui->m_task_table->viewport()->update();

    updateTaskSummary();
    if (_ignored_count > 0) {
        m_ui->m_summary_label->setText(
            tr("已添加 %1 个文件，忽略 %2 个无效或重复项目")
                .arg(m_tasks.size())
                .arg(_ignored_count));
    }
}

void MainWidget::chooseFiles() {
    const QStringList _paths = QFileDialog::getOpenFileNames(
        this, tr("选择需要转换的文档"));
    addFiles(_paths);
}

void MainWidget::chooseOutputDirectory() {
    QString _initial_directory =
        m_ui->m_output_directory_edit->text().trimmed();
    if (_initial_directory.isEmpty()) {
        _initial_directory = m_tasks.isEmpty()
                                 ? QStandardPaths::writableLocation(
                                       QStandardPaths::DocumentsLocation)
                                 : QFileInfo(m_tasks.constFirst().m_input_path)
                                       .absolutePath();
    }

    const QString _directory = QFileDialog::getExistingDirectory(
        this, tr("选择输出目录"), _initial_directory);
    if (!_directory.isEmpty()) {
        m_ui->m_output_directory_edit->setText(
            QDir::toNativeSeparators(_directory));
        m_settings.setLastOutputDirectory(_directory);
        m_settings.sync();
        for (int _row = 0; _row < m_tasks.size(); ++_row) {
            updateTaskRow(_row);
        }
    }
}

void MainWidget::clearTasks() {
    if (m_batch_running) {
        return;
    }
    m_tasks.clear();
    m_ui->m_task_table->setRowCount(0);
    m_ui->m_remove_selected_button->setEnabled(false);
    m_ui->m_batch_progress->setVisible(false);
    updateTaskSummary();
}

void MainWidget::removeSelectedTask() {
    removeTask(m_ui->m_task_table->currentRow());
}

void MainWidget::removeTask(int _row) {
    if (m_batch_running || _row < 0 || _row >= m_tasks.size()) {
        return;
    }
    m_tasks.removeAt(_row);
    m_ui->m_task_table->removeRow(_row);
    m_ui->m_remove_selected_button->setEnabled(
        m_ui->m_task_table->currentRow() >= 0);
    updateTaskSummary();
}

QString MainWidget::taskFilePath(int _row, int _column) const {
    if (_row < 0 || _row >= m_tasks.size()) {
        return {};
    }
    const Task &_task = m_tasks.at(_row);
    if (_column == 0) {
        return _task.m_input_path;
    }
    if (_column == 2 && _task.m_status == tr("成功")
        && QFileInfo::exists(_task.m_output_path)) {
        return _task.m_output_path;
    }
    return {};
}

void MainWidget::showTaskContextMenu(const QPoint &_position) {
    QTableWidgetItem *_item = m_ui->m_task_table->itemAt(_position);
    if (_item == nullptr || m_batch_running) {
        return;
    }
    const int _row = _item->row();
    const int _column = _item->column();
    m_ui->m_task_table->selectRow(_row);
    QMenu _menu(this);
    _menu.setObjectName(QStringLiteral("taskContextMenu"));
    _menu.setWindowFlags(_menu.windowFlags()
                         | Qt::FramelessWindowHint
                         | Qt::NoDropShadowWindowHint);
    _menu.setAttribute(Qt::WA_TranslucentBackground, true);
    if (_column == 0 || _column == 2) {
        const QString _path = taskFilePath(_row, _column);
        QAction *_internal_action = _menu.addAction(
            QIcon(QStringLiteral(":/doc2md/icons/preview.svg")),
            tr("内部预览"));
        QAction *_default_action = _menu.addAction(
            QIcon(QStringLiteral(":/doc2md/icons/open-default.svg")),
            tr("默认应用打开"));
        QAction *_choose_action = _menu.addAction(
            QIcon(QStringLiteral(":/doc2md/icons/open-with.svg")),
            tr("选择应用打开"));
        const bool _can_open = !_path.isEmpty();
        _internal_action->setEnabled(_can_open);
        _default_action->setEnabled(_can_open);
        _choose_action->setEnabled(_can_open);
        connect(_internal_action, &QAction::triggered,
                this, [this, _path]() { openInternalPreview(_path); });
        connect(_default_action, &QAction::triggered,
                this, [this, _path]() { openFileDefault(_path); });
        connect(_choose_action, &QAction::triggered,
                this, [this, _path]() { openFileWithApplication(_path); });
        if (_column == 2) {
            _menu.addSeparator();
            QAction *_rename_action = _menu.addAction(
                QIcon(QStringLiteral(":/doc2md/icons/rename.svg")),
                tr("重命名输出文件"));
            connect(_rename_action, &QAction::triggered,
                    this, [this, _row]() { renameTask(_row); });
        }
        _menu.addSeparator();
    }
    QAction *_remove_action = _menu.addAction(
        QIcon(QStringLiteral(":/doc2md/icons/remove.svg")),
        tr("从列表移除"));
    connect(_remove_action, &QAction::triggered,
            this, [this, _row]() { removeTask(_row); });
    _menu.exec(m_ui->m_task_table->viewport()->mapToGlobal(_position));
}

void MainWidget::openInternalPreview(const QString &_path) {
    auto *_preview_widget = new PreviewWidget(_path, this);
    _preview_widget->show();
    _preview_widget->raise();
    _preview_widget->activateWindow();
}

void MainWidget::openFileDefault(const QString &_path) {
    if (_path.isEmpty()
        || !QDesktopServices::openUrl(QUrl::fromLocalFile(_path))) {
        QMessageBox::warning(this, tr("无法打开文件"),
                             tr("系统默认应用无法打开所选文件。"));
    }
}

void MainWidget::openFileWithApplication(const QString &_path) {
    if (_path.isEmpty()) {
        return;
    }
    const QString _application = QFileDialog::getOpenFileName(
        this, tr("选择用于打开文件的应用程序"), QString(),
        tr("应用程序 (*.exe);;所有文件 (*.*)"));
    if (!_application.isEmpty()
        && !QProcess::startDetached(_application,
                                    {QDir::toNativeSeparators(_path)})) {
        QMessageBox::warning(this, tr("无法启动应用程序"),
                             tr("无法使用所选应用程序打开文件。"));
    }
}

void MainWidget::renameTask(int _row) {
    if (m_batch_running || _row < 0 || _row >= m_tasks.size()) {
        return;
    }
    Task &_task = m_tasks[_row];
    bool _accepted = false;
    const QString _name = QInputDialog::getText(
        this, tr("重命名输出文件"), tr("Markdown 文件名："),
        QLineEdit::Normal, _task.m_output_name, &_accepted);
    if (!_accepted) {
        return;
    }
    const QString _output_name = normalizeOutputName(
        _name, _task.m_input_path);
    QString _error;
    if (!validateOutputName(_output_name, &_error)) {
        QMessageBox::warning(this, tr("输出文件名无效"), _error);
        return;
    }
    if (_output_name.compare(_task.m_output_name, Qt::CaseInsensitive) == 0) {
        return;
    }
    _task.m_output_name = _output_name;
    _task.m_output_path.clear();
    _task.m_status = tr("等待");
    _task.m_message.clear();
    updateTaskRow(_row);
    updateTaskSummary();
}

bool MainWidget::validateBatchStart() {
    if (m_tasks.isEmpty()) {
        m_ui->m_summary_label->setText(tr("请先添加需要转换的文件"));
        return false;
    }

    if (m_python_environment->isProbing()) {
        m_ui->m_summary_label->setText(
            tr("正在检测 Python 运行环境，请稍候..."));
        return false;
    }
    if (!m_environment_ready) {
        QMessageBox::critical(
            this,
            tr("运行环境不可用"),
            m_environment_message.isEmpty()
                ? tr("请先在设置中选择并测试 Python 运行环境。")
                : m_environment_message);
        openSettings();
        return false;
    }

    const QString _runtime_error = m_runner->environmentError();
    if (!_runtime_error.isEmpty()) {
        QMessageBox::critical(this, tr("运行环境不可用"), _runtime_error);
        openSettings();
        return false;
    }

    const QString _configured_output =
        m_ui->m_output_directory_edit->text().trimmed();
    if (!_configured_output.isEmpty() && !QDir().mkpath(_configured_output)) {
        QMessageBox::critical(this, tr("输出目录不可用"),
                              tr("无法创建或访问输出目录：\n%1")
                                  .arg(_configured_output));
        return false;
    }
    return validateOutputPaths();
}

bool MainWidget::validateOutputPaths() {
    QSet<QString> _reserved_paths;
    for (int _row = 0; _row < m_tasks.size(); ++_row) {
        const Task &_task = m_tasks.at(_row);
        QString _error;
        if (!validateOutputName(_task.m_output_name, &_error)) {
            m_ui->m_task_table->selectRow(_row);
            QMessageBox::warning(this, tr("输出文件名无效"), _error);
            return false;
        }

        const QString _path = makeOutputPath(
            _task.m_input_path, _task.m_output_name);
        const QString _key = QDir::cleanPath(_path).toLower();
        if (_reserved_paths.contains(_key) || QFileInfo::exists(_path)) {
            m_ui->m_task_table->selectRow(_row);
            QMessageBox::warning(
                this, tr("输出文件重名"),
                tr("输出文件已存在或与其他任务重名：\n%1\n\n"
                   "请右键“输出文件”列修改名称。")
                    .arg(QDir::toNativeSeparators(_path)));
            return false;
        }
        _reserved_paths.insert(_key);
    }
    return true;
}

void MainWidget::prepareBatchTasks() {
    for (int _row = 0; _row < m_tasks.size(); ++_row) {
        Task &_task = m_tasks[_row];
        _task.m_output_path = makeOutputPath(
            _task.m_input_path, _task.m_output_name);
        _task.m_status = tr("等待");
        _task.m_message.clear();
        updateTaskRow(_row);
    }
}

void MainWidget::startBatch() {
    if (!validateBatchStart()) {
        return;
    }
    prepareBatchTasks();

    m_current_task_index = -1;
    m_success_count = 0;
    m_failure_count = 0;
    m_batch_running = true;
    m_ui->m_batch_progress->setRange(0, m_tasks.size());
    m_ui->m_batch_progress->setValue(0);
    m_ui->m_batch_progress->setVisible(true);
    m_ui->m_start_button->setText(tr("转换中..."));
    setControlsEnabled(false);
    startNextTask();
}

void MainWidget::startNextTask() {
    ++m_current_task_index;
    if (m_current_task_index >= m_tasks.size()) {
        finishBatch();
        return;
    }

    Task &_task = m_tasks[m_current_task_index];
    const QString _output_directory =
        QFileInfo(_task.m_output_path).absolutePath();
    if (!QDir().mkpath(_output_directory)) {
        _task.m_status = tr("失败");
        _task.m_message = tr("无法创建输出目录：%1").arg(_output_directory);
        ++m_failure_count;
        updateTaskRow(m_current_task_index);
        m_ui->m_batch_progress->setValue(m_current_task_index + 1);
        QTimer::singleShot(0, this, &MainWidget::startNextTask);
        return;
    }

    _task.m_status = tr("转换中");
    updateTaskRow(m_current_task_index);
    m_ui->m_task_table->selectRow(m_current_task_index);
    m_ui->m_summary_label->setText(
        tr("正在转换 %1/%2 · %3")
            .arg(m_current_task_index + 1)
            .arg(m_tasks.size())
            .arg(QFileInfo(_task.m_input_path).fileName()));

    if (!m_runner->start(_task.m_input_path, _task.m_output_path)) {
        _task.m_status = tr("失败");
        _task.m_message = m_runner->environmentError();
        if (_task.m_message.isEmpty()) {
            _task.m_message = tr("无法启动转换任务。");
        }
        ++m_failure_count;
        updateTaskRow(m_current_task_index);
        m_ui->m_batch_progress->setValue(m_current_task_index + 1);
        QTimer::singleShot(0, this, &MainWidget::startNextTask);
    }
}

void MainWidget::finishBatch() {
    m_batch_running = false;
    m_ui->m_batch_progress->setValue(m_tasks.size());
    m_ui->m_start_button->setText(tr("开始转换"));
    setControlsEnabled(true);
    m_ui->m_summary_label->setText(
        tr("转换完成 · 成功 %1 个，失败 %2 个")
            .arg(m_success_count)
            .arg(m_failure_count));
}

void MainWidget::setControlsEnabled(bool _enabled) {
    m_ui->m_drop_area->setEnabled(_enabled);
    m_ui->m_clear_button->setEnabled(_enabled && !m_tasks.isEmpty());
    m_ui->m_remove_selected_button->setEnabled(
        _enabled && m_ui->m_task_table->currentRow() >= 0);
    m_ui->m_start_button->setEnabled(_enabled && !m_tasks.isEmpty());
    m_ui->m_browse_output_button->setEnabled(_enabled);
    m_ui->m_output_directory_edit->setEnabled(_enabled);
    m_ui->m_settings_button->setEnabled(_enabled);
}

void MainWidget::updateTaskRow(int _row) {
    if (_row < 0 || _row >= m_tasks.size()) {
        return;
    }
    const QSignalBlocker _signal_blocker(m_ui->m_task_table);
    if (m_ui->m_task_table->item(_row, 0) == nullptr) {
        for (int _column = 0;
             _column < m_ui->m_task_table->columnCount();
             ++_column) {
            m_ui->m_task_table->setItem(_row, _column, new QTableWidgetItem);
        }
    }

    const Task &_task = m_tasks.at(_row);
    const QFileInfo _input_info(_task.m_input_path);
    m_ui->m_task_table->item(_row, 0)->setText(_input_info.fileName());
    m_ui->m_task_table->item(_row, 0)->setIcon(
        QIcon(QStringLiteral(":/doc2md/images/file.png")));
    m_ui->m_task_table->item(_row, 0)->setToolTip(
        tr("右键可预览或打开源文件\n%1")
            .arg(QDir::toNativeSeparators(_task.m_input_path)));
    m_ui->m_task_table->item(_row, 1)->setText(
        _input_info.suffix().isEmpty()
            ? QStringLiteral("-")
            : _input_info.suffix().toUpper());
    updateOutputItem(_row, _task);
    updateStatusItem(_row, _task);
}

void MainWidget::updateOutputItem(int _row, const Task &_task) {
    QTableWidgetItem *_output_item = m_ui->m_task_table->item(_row, 2);
    _output_item->setText(_task.m_output_name);
    _output_item->setToolTip(
        tr("右键可重命名或打开输出文件\n%1")
            .arg(QDir::toNativeSeparators(makeOutputPath(
                _task.m_input_path, _task.m_output_name))));

    for (const int _column : {0, 1, 2, 3}) {
        QTableWidgetItem *_item = m_ui->m_task_table->item(_row, _column);
        _item->setFlags(_item->flags() & ~Qt::ItemIsEditable);
    }
}

void MainWidget::updateStatusItem(int _row, const Task &_task) {
    QTableWidgetItem *_status_item = m_ui->m_task_table->item(_row, 3);
    _status_item->setText(_task.m_status);
    _status_item->setToolTip(_task.m_message);
    _status_item->setTextAlignment(Qt::AlignCenter);
    if (_task.m_status == tr("成功")) {
        _status_item->setForeground(QColor(QStringLiteral("#15803d")));
        _status_item->setBackground(QColor(QStringLiteral("#dcfce7")));
    } else if (_task.m_status == tr("失败")) {
        _status_item->setForeground(QColor(QStringLiteral("#b91c1c")));
        _status_item->setBackground(QColor(QStringLiteral("#fee2e2")));
    } else if (_task.m_status == tr("转换中")) {
        _status_item->setForeground(QColor(QStringLiteral("#1d4ed8")));
        _status_item->setBackground(QColor(QStringLiteral("#dbeafe")));
    } else {
        _status_item->setForeground(QColor(QStringLiteral("#64748b")));
        _status_item->setBackground(QColor(QStringLiteral("#f1f5f9")));
    }
}

void MainWidget::updateTaskSummary() {
    const int _task_count = m_tasks.size();
    m_ui->m_task_stack->setCurrentWidget(
        _task_count == 0 ? m_ui->m_empty_task_page
                         : m_ui->m_task_table_page);
    m_ui->m_task_count_label->setText(tr("%1 个文件").arg(_task_count));
    m_ui->m_clear_button->setEnabled(_task_count > 0 && !m_batch_running);
    m_ui->m_start_button->setEnabled(_task_count > 0 && !m_batch_running);
    m_ui->m_summary_label->setText(
        _task_count == 0 ? tr("准备就绪")
                         : tr("已准备 %1 个转换任务").arg(_task_count));
}

QString MainWidget::defaultOutputName(const QString &_input_path) const {
    const QFileInfo _input_info(_input_path);
    QString _base_name = _input_info.completeBaseName();
    if (_base_name.isEmpty()) {
        _base_name = _input_info.fileName();
    }
    return _base_name + QStringLiteral(".md");
}

QString MainWidget::normalizeOutputName(const QString &_name,
                                        const QString &_input_path) const {
    QString _output_name = _name.trimmed();
    if (_output_name.isEmpty()) {
        return defaultOutputName(_input_path);
    }
    if (!_output_name.endsWith(QStringLiteral(".md"), Qt::CaseInsensitive)) {
        _output_name.append(QStringLiteral(".md"));
    }
    return _output_name;
}

bool MainWidget::validateOutputName(const QString &_name,
                                    QString *_error) const {
    static const QRegularExpression _invalid_characters(
        QStringLiteral(R"([<>:"/\\|?*\x00-\x1F])"));
    const QString _stem = QFileInfo(_name).completeBaseName();
    const QString _device_name = QFileInfo(_name)
                                     .fileName()
                                     .section(QLatin1Char('.'), 0, 0)
                                     .toUpper();
    static const QSet<QString> _reserved_names = {
        QStringLiteral("CON"), QStringLiteral("PRN"), QStringLiteral("AUX"),
        QStringLiteral("NUL"), QStringLiteral("COM1"), QStringLiteral("COM2"),
        QStringLiteral("COM3"), QStringLiteral("COM4"), QStringLiteral("COM5"),
        QStringLiteral("COM6"), QStringLiteral("COM7"), QStringLiteral("COM8"),
        QStringLiteral("COM9"), QStringLiteral("LPT1"), QStringLiteral("LPT2"),
        QStringLiteral("LPT3"), QStringLiteral("LPT4"), QStringLiteral("LPT5"),
        QStringLiteral("LPT6"), QStringLiteral("LPT7"), QStringLiteral("LPT8"),
        QStringLiteral("LPT9")};
    if (_name.isEmpty() || _name.size() > 255
        || _stem.trimmed().isEmpty()
        || _invalid_characters.match(_name).hasMatch()
        || _stem.endsWith(QLatin1Char('.'))
        || _stem.endsWith(QLatin1Char(' '))
        || _name.endsWith(QLatin1Char('.'))
        || _name.endsWith(QLatin1Char(' '))
        || _reserved_names.contains(_device_name)) {
        if (_error != nullptr) {
            *_error = tr("文件名不能为空，不能包含 < > : \" / \\ | ? *，"
                         "不能超过 255 个字符，也不能使用 Windows 保留名称。");
        }
        return false;
    }
    return true;
}

QString MainWidget::makeOutputPath(const QString &_input_path,
                                   const QString &_output_name) const {
    const QFileInfo _input_info(_input_path);
    const QString _configured_directory =
        m_ui->m_output_directory_edit->text().trimmed();
    const QDir _output_directory(_configured_directory.isEmpty()
                                     ? _input_info.absolutePath()
                                     : _configured_directory);
    return QDir::cleanPath(
        _output_directory.absoluteFilePath(_output_name));
}

} // namespace doc2md
