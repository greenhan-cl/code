# Qt 6.9.1 永久环境配置脚本（需要管理员权限）
# 使用方法: 以管理员身份运行 PowerShell，然后执行: .\setup_qt_env_permanent.ps1

Write-Host "配置 Qt 6.9.1 (MinGW 64-bit) 到系统环境变量..." -ForegroundColor Green

# 检查管理员权限
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Host "错误: 此脚本需要管理员权限！" -ForegroundColor Red
    Write-Host "请以管理员身份运行 PowerShell，然后重新执行此脚本。" -ForegroundColor Yellow
    Write-Host "`n或者使用以下方法手动添加:" -ForegroundColor Yellow
    Write-Host "1. 右键'此电脑' -> 属性 -> 高级系统设置" -ForegroundColor Cyan
    Write-Host "2. 点击'环境变量'" -ForegroundColor Cyan
    Write-Host "3. 在'系统变量'或'用户变量'中添加:" -ForegroundColor Cyan
    Write-Host "   - QTDIR = D:\Qt\6.9.1\mingw_64" -ForegroundColor Cyan
    Write-Host "   - 在 PATH 中添加: D:\Qt\6.9.1\mingw_64\bin" -ForegroundColor Cyan
    Write-Host "   - CMAKE_PREFIX_PATH = D:\Qt\6.9.1\mingw_64" -ForegroundColor Cyan
    exit 1
}

# Qt 安装路径
$QtDir = "D:\Qt\6.9.1\mingw_64"
$QtBin = "$QtDir\bin"

# 检查Qt是否存在
if (-not (Test-Path "$QtBin\qmake.exe")) {
    Write-Host "错误: 找不到 Qt 安装目录: $QtBin" -ForegroundColor Red
    exit 1
}

# 获取当前PATH
$currentPath = [Environment]::GetEnvironmentVariable("PATH", "User")
if ([string]::IsNullOrEmpty($currentPath)) {
    $currentPath = [Environment]::GetEnvironmentVariable("PATH", "Machine")
}

# 检查是否已存在
if ($currentPath -like "*$QtBin*") {
    Write-Host "Qt bin 目录已在 PATH 中" -ForegroundColor Yellow
} else {
    # 添加到用户级PATH（推荐，不需要管理员权限）
    $newPath = "$QtBin;$currentPath"
    [Environment]::SetEnvironmentVariable("PATH", $newPath, "User")
    Write-Host "已添加 Qt bin 目录到用户 PATH: $QtBin" -ForegroundColor Green
}

# 设置QTDIR
[Environment]::SetEnvironmentVariable("QTDIR", $QtDir, "User")
Write-Host "已设置 QTDIR = $QtDir" -ForegroundColor Green

# 设置CMAKE_PREFIX_PATH
$currentCmakePath = [Environment]::GetEnvironmentVariable("CMAKE_PREFIX_PATH", "User")
if ([string]::IsNullOrEmpty($currentCmakePath)) {
    [Environment]::SetEnvironmentVariable("CMAKE_PREFIX_PATH", $QtDir, "User")
    Write-Host "已设置 CMAKE_PREFIX_PATH = $QtDir" -ForegroundColor Green
} elseif ($currentCmakePath -notlike "*$QtDir*") {
    $newCmakePath = "$QtDir;$currentCmakePath"
    [Environment]::SetEnvironmentVariable("CMAKE_PREFIX_PATH", $newCmakePath, "User")
    Write-Host "已更新 CMAKE_PREFIX_PATH" -ForegroundColor Green
} else {
    Write-Host "CMAKE_PREFIX_PATH 已包含 Qt 路径" -ForegroundColor Yellow
}

Write-Host "`n配置完成！" -ForegroundColor Green
Write-Host "注意: 需要重新打开命令行窗口或重启IDE才能使环境变量生效。" -ForegroundColor Yellow


