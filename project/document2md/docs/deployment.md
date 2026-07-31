# doc2md Windows 发布说明

## 1. 准备 Python 运行环境

先准备一个完整的 Python 3.10 以上 x64 运行目录。该目录必须包含 `python.exe`，并提前安装 MarkItDown 所需依赖。

开发仓库中的 MarkItDown 源码会由 CMake 安装到 `runtime/markitdown/src`，因此准备的 Python 运行环境主要负责标准库和第三方依赖。

## 2. 配置项目

```powershell
cmake -S . -B build-release -G Ninja `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_PREFIX_PATH=D:/tools/Qt/6.5.3/msvc2019_64 `
  -DDOC2MD_BUNDLE_PYTHON=ON `
  -DDOC2MD_PYTHON_RUNTIME_DIR=D:/path/to/prepared-python
```

`DOC2MD_PYTHON_RUNTIME_DIR` 应指向包含 `python.exe` 的目录，不是单个可执行文件。

## 3. 编译与安装

```powershell
cmake --build build-release
cmake --install build-release --prefix package
```

安装后主要目录如下：

```text
package/bin/
├── doc2md.exe
├── runtime/
│   ├── python/
│   └── markitdown/src/markitdown/
└── Qt 运行库与平台插件
```

CMake 会通过 Qt 部署脚本一并收集 Widgets、Pdf、PdfWidgets 和 Svg 的运行库；发布前需在干净环境确认 PDF 预览及 SVG 图标可正常加载。

## 4. 不携带 Python 的发布方式

如果将 `DOC2MD_BUNDLE_PYTHON` 保持为 `OFF`，发布包不会复制 Python。用户可以在应用设置中选择系统 Python、虚拟环境或其他自定义 `python.exe`。

## 5. 发布前验证

- 在没有安装系统 Python 的干净 Windows 环境中测试内置模式。
- 测试 Python 路径包含中文和空格的情况。
- 分别验证有效路径、无效路径、低于 Python 3.10 和缺少依赖的环境。
- 验证 PDF、Word、Excel、PowerPoint 和纯文本等计划支持的格式。
- 确认安装目录包含 MarkItDown 的第三方声明文件，并补齐内置 Python 运行环境中各依赖的许可证文件。

## 6. 生成正式安装包和便携包

安装 Inno Setup 6 后，在项目根目录执行：

```powershell
powershell -ExecutionPolicy Bypass -File scripts/build_packages.ps1
```

脚本会在 `out/release` 中生成正式安装包和便携 ZIP。正式安装包会递归安装
`vc_redist.x64.exe`；便携 ZIP 会将 MSVC x64 CRT DLL 部署到应用程序本地目录。
