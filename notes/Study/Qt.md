## **File**

* •
  严格的头源分离
* •

## **Base Using**

### **信号与槽**

### `connect(sender, signal, receiver, slot)`

* Qt 的事件通信机制，用来解耦对象之间的调用。
* •
  当 `sender` 发出 `signal` 时，`receiver` 的 `slot` 自动被调用。
* •
  GUI 中几乎所有交互（按钮、菜单、快捷键）都靠它。

  ### **Widget基础属性和行为**


  * •
    `move(x, y)`：移动控件左上角到父窗口坐标。
  * •
    `geometry()`：获取控件在**父窗口坐标系**下的位置和大小。
  * •
    `setGeometry(x, y, w, h / QRect)`：直接设置位置和尺寸。
  * •
    `frameGeometry()`：包含窗口边框的几何信息（顶层窗口常用）。
  * •
    `setFrameGeometry()`：通常用于精确控制窗口位置（例如居中）。

### **窗口相关**

#### 标题 / 文本

* •
  `setWindowTitle()`：设置窗口标题。
* •
  `setText()`：设置控件显示的文本（如 QLabel、Button）。

#### 启用 / 禁用

* •
  `isEnabled()`：判断控件是否可用。
* •
  `setEnabled(false)`：禁用控件（灰掉且不可交互）。

#### 透明度

* •
  `windowOpacity()`：获取窗口透明度（0~1）。
* •
  `setWindowOpacity()`：常用于淡入淡出效果。

### **鼠标/光标相关**

#### 光标

* •
  `cursor()`：获取当前控件光标样式。
* •
  `setCursor(QCursor)`：设置鼠标悬停样式（手型、图片等）。

#### 鼠标事件

* •
  `mousePressEvent / ReleaseEvent / MoveEvent / DoubleClickEvent`
* •
  需重写函数来响应鼠标操作。
* •
  `setMouseTracking(true)`：不按键也能触发 move 事件。

### **图像与图标**

#### `QPixmap`

* •
  用于加载和显示图片（适合 GUI）。
* •
  `scaled()`：返回缩放后的新图片，不改变原对象。

#### `QIcon`

* •
  窗口、按钮、菜单项的图标类。
* •
  `setIcon()` + `setIconSize()` 常一起使用。

### **字体与样式**

#### `QFont`

* •
  控制字体家族、大小、加粗、倾斜等。
* •
  常配合 `setFont()` 使用。

#### 样式表

* •
  `styleSheet / setStyleSheet()`：Qt 的 CSS。
* •
  用于快速美化界面，不影响逻辑。

### **提示 / 焦点 / 快捷键**

#### ToolTip

* •
  `setToolTip()`：鼠标悬停提示。
* •
  `setToolTipDuration()`：控制显示时间。

#### 焦点

* •
  `focusPolicy()` / `setFocusPolicy()`
* •
  `Qt::StrongFocus`：Tab + 鼠标都可获得焦点。

#### 快捷键

* •
  `setShortcut(QKeySequence)`
* •
  `Qt::Key_?`：键盘枚举值。

## **按钮类控件**

### RadioButton（单选）

* •
  `setChecked(true)`：默认选中。
* •
  通常用 `QButtonGroup` 实现互斥。

### CheckBox（复选）

* •
  可多选，状态改变常用 `toggled()` 信号。

### 槽触发类型

* •
  `clicked()`：点击完成
* •
  `pressed()`：按下瞬间
* •
  `released()`：释放时
* •
  `toggled()`：状态改变

## **标签与显示类控件**

### `QLabel`

* •
  显示文本或图片。
* •
  支持 Plain / Rich / Markdown 三种文本格式。
* •
  `setScaledContents(true)`：图片自适应。

### `QLCDNumber`

* •
  数字显示控件，类似计算器。
* •
  `display()` 设置显示内容。

### `QProgressBar`

* •
  显示进度（0~100）。
* •
  常配合定时器或任务进度使用。

## **输入类控件**

### `QLineEdit`

* •
  单行输入框。
* •
  支持占位文本、清空按钮、密码模式。

### 输入校验

* •
  `QRegularExpressionValidator`
* •
  注册后自动限制用户输入格式。

### `QTextEdit`

* •
  多行文本输入。
* •
  `toPlainText()` 获取纯文本内容。

### `QComboBox`

* •
  下拉选择框。
* •
  `addItem()` / `addItems()` 添加选项。

## **时间 / 数值控制**

### `QDateTime`

* •
  表示日期和时间。
* •
  `currentDateTime()` 获取当前时间。

### Slider

* •
  滑动条控件。
* •
  `setMinimum / setMaximum / setValue` 控制范围。

## **菜单 / 工具栏 / 状态栏**

### `QMenuBar`

* •
  窗口顶部菜单栏。
* •
  `addMenu()` 添加菜单。

### `QMenu`

* •
  菜单容器。
* •
  `addAction()` 添加菜单项。

### `QAction`

* •
  菜单和工具栏 **可复用的行为对象** 。
* •
  绑定图标、文本、快捷键和槽。

### `QToolBar`

* •
  工具栏，可直接添加 QAction。

### `QStatusBar`

* •
  底部状态栏。
* •
  `showMessage()` 显示临时信息。

## **停靠与对话框**

### `QDockWidget`

* •
  可拖拽、浮动的窗口组件。
* •
  常用于 IDE 风格界面。

### `QDialog`

* •
  模态 / 非模态对话框。
* •
  `Qt::WA_DeleteOnClose` 防止内存泄漏。

### `QMessageBox`

* •
  标准消息对话框（提示、警告）。
* •
  `exec()` 返回用户选择。

## **事件 / 定时器**

### 事件

* •
  `leaveEvent()`：鼠标离开控件。
* •
  `resizeEvent()`：窗口大小改变。

### 定时器

* •
  `startTimer()` / `killTimer()`
* •
  重写 `timerEvent()` 处理逻辑。

## **文件与系统**

### `QFile`

* •
  文件读写对象。
* •
  `readAll()` 一次性读取内容。

### `QFileInfo`

* •
  获取文件大小、路径、类型等属性。
