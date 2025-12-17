# 在 Cursor 中开发 Qt 项目指南

## ✅ 已找到的 Qt 安装信息

```
Qt 版本: Qt 6.9.1
安装路径: D:\Qt-\6.9.1\mingw_64
编译器: MinGW 13.1.0 (D:\Qt-\Tools\mingw1310_64)
CMake: D:\Qt-\Tools\CMake_64
Qt Creator: D:\Qt-\Tools\QtCreator
```

---

## 📦 需要安装的 Cursor 插件

### 必装插件：

1. **C/C++** (Microsoft)
   - ID: `ms-vscode.cpptools`
   - 功能：C++ 代码高亮、智能补全、跳转

2. **CMake Tools** (Microsoft)
   - ID: `ms-vscode.cmake-tools`
   - 功能：CMake 项目管理和构建

### 推荐插件：

3. **Qt for Python** (Sean Wu)
   - ID: `seanwu.vscode-qt-for-python`
   - 功能：.ui 文件预览、Qt 代码片段

4. **Better C++ Syntax** (Jeff Hykin)
   - 功能：更好的 C++ 语法高亮

---

## 🚀 快速开始

### 方法1：使用 PowerShell 脚本（推荐）

```powershell
# 1. 配置 Qt 环境
.\setup-qt-env.ps1

# 2. 配置项目
cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=D:/Qt-/6.9.1/mingw_64 -B build

# 3. 编译项目
cmake --build build

# 4. 运行程序
.\build\BitPlayer.exe
```

### 方法2：使用 Cursor 的 CMake Tools

1. **按 F1** 或 **Ctrl+Shift+P**
2. 输入 **"CMake: Configure"**
3. 输入 **"CMake: Build"**
4. 按 **F5** 开始调试

---

## 🎨 编辑 .ui 文件

### 打开 Qt Designer：

```powershell
# 打开空白 Qt Designer
.\open-designer.ps1

# 打开指定 .ui 文件
.\open-designer.ps1 login.ui
```

或者直接运行：
```powershell
D:\Qt-\6.9.1\mingw_64\bin\designer.exe
```

---

## ⚙️ 已配置的文件

项目已自动创建以下配置文件：

```
.vscode/
├── settings.json          # Cursor/VSCode 设置
├── c_cpp_properties.json  # C++ IntelliSense 配置
├── tasks.json             # 构建任务配置
└── launch.json            # 调试配置
```

---

## 🔧 常用命令

### 环境配置
```powershell
# 配置 Qt 环境（每次打开新 PowerShell 时执行）
.\setup-qt-env.ps1
```

### 项目构建
```powershell
# 配置 CMake（首次或修改 CMakeLists.txt 后）
cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=D:/Qt-/6.9.1/mingw_64 -B build

# 编译（支持增量编译）
cmake --build build

# 清理构建
Remove-Item -Recurse -Force build
```

### Qt Designer
```powershell
# 打开 Qt Designer
.\open-designer.ps1

# 编辑指定 UI 文件
.\open-designer.ps1 bitplayer.ui
```

### 运行和调试
```powershell
# 直接运行
.\build\BitPlayer.exe

# 在 Cursor 中调试（按 F5）
```

---

## 🎯 Cursor 快捷键

| 功能 | 快捷键 |
|------|--------|
| 打开命令面板 | `Ctrl+Shift+P` 或 `F1` |
| CMake Configure | `F1` → "CMake: Configure" |
| CMake Build | `F1` → "CMake: Build" |
| 开始调试 | `F5` |
| 运行任务 | `Ctrl+Shift+B` |
| 打开终端 | ``Ctrl+` `` |

---

## 📝 注意事项

1. **环境变量**：每次打开新的 PowerShell 终端，需要运行 `.\setup-qt-env.ps1` 配置环境

2. **系统 PATH**：如果想永久添加 Qt 到 PATH，可以：
   - Win+X → 系统 → 高级系统设置 → 环境变量
   - 在系统变量 Path 中添加：
     ```
     D:\Qt-\6.9.1\mingw_64\bin
     D:\Qt-\Tools\mingw1310_64\bin
     D:\Qt-\Tools\CMake_64\bin
     ```

3. **MPV 库**：项目依赖 libmpv，确保 `mpv/dll/libmpv-2.dll` 存在

4. **编译器**：使用 MinGW 13.1.0，不要混用 MSVC 和 MinGW

---

## 🐛 常见问题

### Q: CMake 找不到 Qt？
```powershell
# 确保设置了 CMAKE_PREFIX_PATH
cmake -DCMAKE_PREFIX_PATH=D:/Qt-/6.9.1/mingw_64 -B build
```

### Q: 编译错误：找不到 g++？
```powershell
# 运行环境配置脚本
.\setup-qt-env.ps1
```

### Q: IntelliSense 不工作？
- 确保安装了 **C/C++** 和 **CMake Tools** 插件
- 检查 `.vscode/c_cpp_properties.json` 中的路径是否正确
- 按 F1 → "C/C++: Reload IntelliSense Database"

### Q: .ui 文件无法预览？
- 使用 Qt Designer 打开：`.\open-designer.ps1 xxx.ui`
- 或者直接查看 XML 内容（.ui 本质是 XML）

---

## 🎓 学习资源

- Qt 官方文档：https://doc.qt.io/qt-6/
- Qt 中文网：https://www.qt.io/zh-cn/
- CMake 文档：https://cmake.org/documentation/

---

## 🤝 需要帮助？

如有问题，可以：
1. 查看本指南的常见问题部分
2. 检查 Cursor 的 OUTPUT 面板（查看编译错误）
3. 运行 `.\setup-qt-env.ps1` 验证环境配置

祝开发顺利！🚀



















