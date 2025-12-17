# Qt 环境配置脚本
# 使用方法：在 PowerShell 中运行 .\setup-qt-env.ps1

Write-Host "正在配置 Qt 开发环境..." -ForegroundColor Green

# 添加 Qt 相关路径到 PATH
$QtPath = "D:\Qt-\6.9.1\mingw_64\bin"
$MinGWPath = "D:\Qt-\Tools\mingw1310_64\bin"
$CMakePath = "D:\Qt-\Tools\CMake_64\bin"
$NinjaPath = "D:\Qt-\Tools\Ninja"

$env:Path = "$QtPath;$MinGWPath;$CMakePath;$NinjaPath;" + $env:Path

# 设置 Qt 相关环境变量
$env:CMAKE_PREFIX_PATH = "D:\Qt-\6.9.1\mingw_64"
$env:Qt6_DIR = "D:\Qt-\6.9.1\mingw_64"

Write-Host "✅ Qt 环境配置完成！" -ForegroundColor Green
Write-Host ""
Write-Host "当前环境变量：" -ForegroundColor Cyan
Write-Host "  Qt 路径: $QtPath"
Write-Host "  MinGW 路径: $MinGWPath"
Write-Host "  CMake 路径: $CMakePath"
Write-Host ""

# 验证工具是否可用
Write-Host "验证工具..." -ForegroundColor Yellow
qmake --version
cmake --version
g++ --version | Select-Object -First 1

Write-Host ""
Write-Host "🎯 快速命令：" -ForegroundColor Cyan
Write-Host "  打开 Qt Designer: designer"
Write-Host "  配置项目: cmake -G 'MinGW Makefiles' -DCMAKE_PREFIX_PATH=D:/Qt-/6.9.1/mingw_64 -B build"
Write-Host "  编译项目: cmake --build build"
Write-Host "  运行程序: .\build\BitPlayer.exe"



















