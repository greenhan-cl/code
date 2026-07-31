# doc2md

doc2md 是一个基于 Qt 6 和 MarkItDown 的 Windows 桌面工具，用于将本地文档批量转换为 Markdown 文件。

当前版本：`0.1.0`

## 主要功能

- 拖拽或手动选择多个源文件。
- 批量转换文档并显示任务状态。
- 为每个输出 Markdown 文件单独命名。
- 支持源文件和输出文件的内部预览、默认应用打开及指定应用打开。
- 支持自定义 Python 路径，并通过 QSettings 持久化配置。
- 支持将 Qt、MarkItDown 和可选的 Python 运行环境部署到发布目录。

## 开发环境

- Windows 10/11 x64
- CMake 3.16+
- C++17
- MSVC x64
- Qt 6.5，使用 `Widgets`、`Pdf`、`PdfWidgets` 和 `Svg` 模块
- Python 3.10+

MarkItDown 源码位于 `3rdparty/markitdown`。

## 构建项目

项目提供了 `Qt-Debug` 和 `Qt-Release` 两个 CMake 配置预设。请先确认 `CMakeUserPresets.json` 中的 `QTDIR` 与本机 Qt 安装位置一致。

### Debug

```powershell
cmake --preset Qt-Debug
cmake --build out/build/debug
```

生成的程序位于：

```text
out/build/debug/apps/doc2md.exe
```

### Release

```powershell
cmake --preset Qt-Release
cmake --build out/build/release
```

生成的程序位于：

```text
out/build/release/apps/doc2md.exe
```

## 生成发布目录

执行安装命令时请使用绝对路径作为 `--prefix`：

```powershell
cmake --install out/build/release `
  --prefix "D:/path/to/document2md/out/package/doc2md-0.1.0"
```

CMake 会将程序、Qt 运行库、插件、MarkItDown 源码和第三方声明安装到发布目录。

如果需要同时携带准备好的 Python 运行环境，可在配置 Release 时启用：

```powershell
cmake --preset Qt-Release `
  -DDOC2MD_BUNDLE_PYTHON=ON `
  -DDOC2MD_PYTHON_RUNTIME_DIR=D:/path/to/python-runtime

cmake --build out/build/release
```

`DOC2MD_PYTHON_RUNTIME_DIR` 必须指向包含 `python.exe` 和所需 MarkItDown 依赖的完整 Python 运行目录。

## 使用方法

1. 启动 `doc2md.exe`。
2. 在设置中选择内置、自动检测或自定义 Python。
3. 添加需要转换的文档。
4. 选择 Markdown 输出目录，并按需修改输出文件名。
5. 点击“开始转换”。

任务列表支持右键预览、打开、重命名输出文件和移除任务。

## 目录说明

```text
apps/       程序入口与安装规则
forms/      Qt Designer UI 文件
include/    项目头文件
resources/  QSS、图标和图片资源
src/        主要业务实现
3rdparty/   第三方源码
docs/       设计与部署文档
```

更详细的发布说明请参阅 `docs/deployment.md`。

## 生成安装包和便携包

先按照上面的步骤生成包含 Qt、Python、MarkItDown 和
`vc_redist.x64.exe` 的发布目录，然后安装 Inno Setup 6 并执行：

```powershell
powershell -ExecutionPolicy Bypass -File scripts/build_packages.ps1
```

脚本会在 `out/release` 中生成：

```text
doc2md-0.1.0-windows-x64-setup.exe
doc2md-0.1.0-windows-x64-portable.zip
```

正式安装包会静默安装 Microsoft Visual C++ x64 运行库。便携包会将
MSVC x64 CRT DLL 放在 `doc2md.exe` 旁边，因此不要求用户另行安装该运行库。
