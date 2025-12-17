@echo off
REM Qt 6.9.1 环境配置脚本 (批处理版本)
REM 使用方法: setup_qt_env.bat

echo 配置 Qt 6.9.1 (MinGW 64-bit) 环境...

REM Qt 安装路径
set QTDIR=D:\Qt\6.9.1\mingw_64
set PATH=%QTDIR%\bin;%PATH%
set CMAKE_PREFIX_PATH=%QTDIR%

REM 验证qmake
echo.
echo 验证 Qt 安装...
"%QTDIR%\bin\qmake.exe" -v

echo.
echo 配置完成！
echo 注意: 这些环境变量只在当前命令提示符窗口中有效。
echo 要永久设置，请在系统环境变量中添加：
echo   QTDIR = %QTDIR%
echo   在 PATH 中添加: %QTDIR%\bin
echo   CMAKE_PREFIX_PATH = %QTDIR%

REM 保持窗口打开（可选）
REM pause


