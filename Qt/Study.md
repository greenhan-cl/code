# Qt 学习路径指南

本仓库用于学习 Qt 正式项目中的信号传递和业务分层。

## 推荐学习顺序

### 第一阶段：信号与槽基础 → ToolsMainWindow

**目的**：理解 Qt 原生信号槽机制

**项目路径**：`Tools/ToolsMainWindow`

**学习重点**：
```
1. 父子窗口通信
   - 主窗口 → 对话框：通过 lambda 表达式连接
   - 对话框 → 主窗口：通过自定义信号 SigExit

2. 按钮与窗口联动
   connect(ui->m_minBtn, &QPushButton::clicked, [=](){this->showMinimized();});
   connect(ui->m_closeBtn, &QPushButton::clicked,[=](){m_exitDlg.exec();});

3. 自定义信号的定义与触发
   // WarningDlg.h
   signals:
       void SigExit();

   // warningdlg.cpp
   void WarningDlg::Exit()
   {
       emit SigExit();
   }
```

**关键文件**：
- `src/UI/MainWindow/mainwindow.cpp` - 信号连接
- `src/UI/WarningDlg/warningdlg.h/cpp` - 自定义信号

---

### 第二阶段：接口模式解耦 → AppTranslation

**目的**：学习通过接口实现解耦，避免直接依赖

**项目路径**：`Project/AppTranslation`

**学习重点**：
```
1. 定义通用接口
   class TranslationSupport
   {
   public:
       virtual void retranslate() = 0;
   };

2. 多个类实现同一接口
   class Widget1 : public QWidget, public TranslationSupport
   class Widget2 : public QWidget, public TranslationSupport

3. 父窗口广播通知所有实现者
   for (auto pIterWidget : QApplication::allWidgets())
   {
       if (auto pWidget = dynamic_cast<TranslationSupport*>(pIterWidget))
       {
           pWidget->retranslate();  // 统一调用
       }
   }
```

**核心思想**：父窗口不关心具体子类，只通过接口通信

---

### 第三阶段：业务与UI分离 → QtEmail

**目的**：学习将网络协议、数据结构与界面完全分离

**项目路径**：`Tools/QtEmail`

**架构分层**：
```
┌─────────────────────────────────────────┐
│  UI Layer (main.cpp 调用)               │
├─────────────────────────────────────────┤
│  Business Layer (SMTP Client)          │
│  - smtpclient.h/cpp                     │
│  - 只管发送不管界面                      │
├─────────────────────────────────────────┤
│  Data Layer (MIME 消息构建)              │
│  - MimeMessage (邮件结构)                │
│  - MimeText/MimeHtml/MimeAttachment     │
│  - MimeMultiPart (组合)                  │
├─────────────────────────────────────────┤
│  Shared Layer (publicdefine.h)          │
│  - SmtpParams, EmailAddress 等数据结构  │
└─────────────────────────────────────────┘
```

**信号传递流程**：
```
用户调用 → SmtpClient → QSocket信号 → 触发smtpError/smtpSuccess信号
                                    ↓
                              UI接收信号显示结果
```

**关键文件**：
- `src/smtpclient.h/cpp` - 业务层，纯网络操作
- `src/mimemessage.h/cpp` - 数据层，邮件结构
- `src/mimepart.h/cpp` - MIME基类，组合模式

---

### 第四阶段：C++与QML双向通信

#### 4.1 QML调用C++ → QtAppCallQml

**项目路径**：`Project/QtAppCallQml`

**学习重点**：
```
1. C++对象暴露给QML
   qmlRegisterType<MyType>("MyLib", 1, 0, "MyType");

2. QML调用C++方法
   QMetaObject::invokeMethod(page1, "setColor", Q_ARG(QVariant, "lightblue"));

3. QML直接访问C++属性
   pRedPage->setProperty("color", "lightblue");
```

#### 4.2 C++调用QML → VSQtUseQmlUi

**项目路径**：`Project/VSQtUseQmlUi`

**学习重点**：
```
1. QML发送信号给C++
   // QML中
   signal signalClicked(string msg)

   // C++中接收
   connect(pRedPage, SIGNAL(signalClicked(const QString&)),
           this, SLOT(onQmlClicked(const QString&)));

2. 页面切换信号
   connect(pSwipeView, SIGNAL(signalPageChanged(int)),
           this, SLOT(onPageChanged(int)));
```

---

### 第五阶段：Web混合开发 → WebTest

**目的**：学习Qt与JavaScript的双向通信

**项目路径**：`Project/WebTest`

**核心机制 QWebChannel**：
```
┌─────────────┐    registerObject    ┌─────────────┐
│   Qt C++    │ ←──────────────────→ │ JavaScript  │
│             │                      │             │
│  暴露对象    │   runJavaScript      │  调用Qt方法  │
│  接收JS消息  │   invokeMethod       │  接收Qt信号  │
└─────────────┘                      └─────────────┘
```

**关键代码**：
```cpp
// C++ 暴露对象给 JS
m_pWebChannel->registerObject(QString("qtui"), this);

// C++ 调用 JS
m_pWebView->page()->runJavaScript(jsStr);

// C++ 槽函数接收 JS 消息
public slots:
    void recieveJsMessage(const QString& jsMsg);
```

---

### 第六阶段：模型视图架构 → QmlUseCppModel

**目的**：学习 QAbstractListModel 自定义模型

**项目路径**：`Project/QmlUseCppModel`

**核心思想**：数据与视图分离，通过信号通知视图更新
```
CustomModel (数据) ←→ QML ListView (视图)
        ↓
   数据变化时发出 dataChanged() 信号
   视图自动更新
```

---

## 学习路径总结

```
┌─────────────────────────────────────────────────────────────┐
│  信号槽基础     → ToolsMainWindow                           │
│       ↓                                                    │
│  接口解耦       → AppTranslation                            │
│       ↓                                                    │
│  业务分层       → QtEmail                                   │
│       ↓                                                    │
│  C++/QML通信    → QtAppCallQml, VSQtUseQmlUi                │
│       ↓                                                    │
│  Web混合开发    → WebTest                                   │
│       ↓                                                    │
│  模型视图       → QmlUseCppModel                            │
└─────────────────────────────────────────────────────────────┘
```

## 核心知识点提炼

| 场景 | 解决方案 | 示例 |
|------|----------|------|
| 父子窗口通信 | 自定义信号 | WarningDlg::SigExit |
| 广播通知 | 接口模式 | TranslationSupport |
| 业务与UI分离 | 分层架构 | QtEmail (SMTP/MIME/UI) |
| QML调用C++ | qmlRegisterType + invokeMethod | QtAppCallQml |
| C++调用QML | QML信号连接C++槽 | VSQtUseQmlUi |
| JS与Qt通信 | QWebChannel | WebTest |
| 数据与视图分离 | QAbstractListModel | QmlUseCppModel |
