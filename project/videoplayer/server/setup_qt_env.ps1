# Qt 6.9.1 环境配置脚本
# 使用方法: .\setup_qt_env.ps1

Write-Host "配置 Qt 6.9.1 (MinGW 64-bit) 环境..." -ForegroundColor Green

# Qt 安装路径
$QtDir = "D:\Qt\6.9.1\mingw_64"
$QtBin = "$QtDir\bin"
$QtLib = "$QtDir\lib"
$QtInclude = "$QtDir\include"

# 检查Qt是否存在
if (-not (Test-Path "$QtBin\qmake.exe")) {
    Write-Host "错误: 找不到 Qt 安装目录: $QtBin" -ForegroundColor Red
    exit 1
}

# 设置环境变量（当前会话）
$env:QTDIR = $QtDir
$env:QT_PLUGIN_PATH = "$QtDir\plugins"
$env:QML2_IMPORT_PATH = "$QtDir\qml"

# 添加到PATH（当前会话）
if ($env:PATH -notlike "*$QtBin*") {
    $env:PATH = "$QtBin;$env:PATH"
}

# 设置CMake变量
$env:CMAKE_PREFIX_PATH = "$QtDir;$env:CMAKE_PREFIX_PATH"

Write-Host "Qt 环境变量已设置:" -ForegroundColor Yellow
Write-Host "  QTDIR = $env:QTDIR"
Write-Host "  PATH 已添加: $QtBin"
Write-Host "  CMAKE_PREFIX_PATH = $env:CMAKE_PREFIX_PATH"

# 验证qmake
Write-Host "`n验证 Qt 安装..." -ForegroundColor Green
& "$QtBin\qmake.exe" -v

Write-Host "`n配置完成！" -ForegroundColor Green
Write-Host "注意: 这些环境变量只在当前PowerShell会话中有效。" -ForegroundColor Yellow
Write-Host "要永久设置，请使用系统环境变量设置或每次运行此脚本。" -ForegroundColor Yellow


