@echo off
REM Qt 6.9.1 永久环境配置脚本（需要管理员权限）
REM 使用方法: 右键以管理员身份运行此批处理文件

echo 配置 Qt 6.9.1 (MinGW 64-bit) 到系统环境变量...
echo.

REM 检查管理员权限
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo 错误: 此脚本需要管理员权限！
    echo 请右键此文件，选择"以管理员身份运行"
    echo.
    echo 或者使用以下方法手动添加:
    echo 1. 右键"此电脑" -^> 属性 -^> 高级系统设置
    echo 2. 点击"环境变量"
    echo 3. 在"用户变量"中添加:
    echo    - QTDIR = D:\Qt\6.9.1\mingw_64
    echo    - 在 PATH 中添加: D:\Qt\6.9.1\mingw_64\bin
    echo    - CMAKE_PREFIX_PATH = D:\Qt\6.9.1\mingw_64
    pause
    exit /b 1
)

REM Qt 安装路径
set QTDIR=D:\Qt\6.9.1\mingw_64
set QTBIN=%QTDIR%\bin

REM 检查Qt是否存在
if not exist "%QTBIN%\qmake.exe" (
    echo 错误: 找不到 Qt 安装目录: %QTBIN%
    pause
    exit /b 1
)

REM 添加到用户PATH
for /f "tokens=2*" %%A in ('reg query "HKCU\Environment" /v PATH 2^>nul') do set "CURRENT_PATH=%%B"
if defined CURRENT_PATH (
    echo %CURRENT_PATH% | findstr /i "%QTBIN%" >nul
    if errorlevel 1 (
        setx PATH "%QTBIN%;%CURRENT_PATH%"
        echo 已添加 Qt bin 目录到用户 PATH: %QTBIN%
    ) else (
        echo Qt bin 目录已在 PATH 中
    )
) else (
    setx PATH "%QTBIN%"
    echo 已设置用户 PATH: %QTBIN%
)

REM 设置QTDIR
setx QTDIR "%QTDIR%"
echo 已设置 QTDIR = %QTDIR%

REM 设置CMAKE_PREFIX_PATH
for /f "tokens=2*" %%A in ('reg query "HKCU\Environment" /v CMAKE_PREFIX_PATH 2^>nul') do set "CURRENT_CMAKE=%%B"
if defined CURRENT_CMAKE (
    echo %CURRENT_CMAKE% | findstr /i "%QTDIR%" >nul
    if errorlevel 1 (
        setx CMAKE_PREFIX_PATH "%QTDIR%;%CURRENT_CMAKE%"
        echo 已更新 CMAKE_PREFIX_PATH
    ) else (
        echo CMAKE_PREFIX_PATH 已包含 Qt 路径
    )
) else (
    setx CMAKE_PREFIX_PATH "%QTDIR%"
    echo 已设置 CMAKE_PREFIX_PATH = %QTDIR%
)

echo.
echo 配置完成！
echo 注意: 需要重新打开命令行窗口或重启IDE才能使环境变量生效。
echo.
pause


