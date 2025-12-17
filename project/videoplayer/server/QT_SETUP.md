# Qt 6.9.1 开发环境配置指南

## 已检测到的Qt安装

- **Qt版本**: 6.9.1
- **编译器**: MinGW 64-bit
- **安装路径**: `D:\Qt\6.9.1\mingw_64`

## ⚠️ 当前状态

**Qt尚未永久添加到系统环境变量中**

- ✅ Qt已安装并可用
- ❌ 系统PATH中未包含Qt（需要每次手动设置）
- ❌ QTDIR环境变量未设置
- ⚠️ 当前PowerShell会话中已临时设置（关闭后失效）

**要永久配置，请使用下面的"永久配置"方法。**

## 快速配置

### 方法1: 使用批处理脚本（推荐，最简单）

双击运行或在命令提示符中运行：
```cmd
setup_qt_env.bat
```

### 方法2: 使用PowerShell脚本

如果遇到执行策略限制，先运行：
```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

然后在PowerShell中运行：
```powershell
.\setup_qt_env.ps1
```

### 方法3: 直接在当前会话中设置（临时）

在PowerShell中运行：
```powershell
$env:QTDIR = "D:\Qt\6.9.1\mingw_64"
$env:PATH = "D:\Qt\6.9.1\mingw_64\bin;$env:PATH"
$env:CMAKE_PREFIX_PATH = "D:\Qt\6.9.1\mingw_64"
```

这些脚本会：
- 设置 `QTDIR` 环境变量
- 将Qt的bin目录添加到PATH
- 设置 `CMAKE_PREFIX_PATH` 以便CMake找到Qt

### 方法4: 永久配置到系统环境变量（推荐）

#### 方式A: 使用批处理脚本（最简单）

**右键以管理员身份运行**：
```cmd
setup_qt_env_permanent.bat
```

#### 方式B: 使用PowerShell脚本

**以管理员身份运行PowerShell**，然后执行：
```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
.\setup_qt_env_permanent.ps1
```

#### 方式C: 手动设置（图形界面）

1. 右键"此电脑" -> "属性" -> "高级系统设置"
2. 点击"环境变量"
3. 在"用户变量"（推荐）或"系统变量"中添加：
   - **新建变量** `QTDIR` = `D:\Qt\6.9.1\mingw_64`
   - **编辑** `PATH`，添加：`D:\Qt\6.9.1\mingw_64\bin`
   - **新建变量** `CMAKE_PREFIX_PATH` = `D:\Qt\6.9.1\mingw_64`
4. 点击"确定"保存
5. **重要**: 需要重新打开命令行窗口或重启IDE才能生效

**注意**: 永久配置后，所有新的命令行窗口和IDE都会自动识别Qt。

## 验证安装

运行以下命令验证Qt是否正确配置：

```powershell
# 检查qmake
& "D:\Qt\6.9.1\mingw_64\bin\qmake.exe" -v

# 检查Qt版本
& "D:\Qt\6.9.1\mingw_64\bin\qmake.exe" -query QT_VERSION
```

## 在CMake项目中使用Qt

### 基本配置

在你的 `CMakeLists.txt` 中添加：

```cmake
# 设置Qt路径（如果CMake找不到）
set(CMAKE_PREFIX_PATH "D:/Qt/6.9.1/mingw_64")

# 查找Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# 启用自动MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 链接Qt库
target_link_libraries(your_target
    Qt6::Core
    Qt6::Widgets
)
```

### 常用Qt模块

根据你的需求，可以添加以下模块：

- `Qt6::Core` - 核心功能
- `Qt6::Widgets` - GUI应用程序
- `Qt6::Quick` - QML/Quick应用
- `Qt6::Network` - 网络功能
- `Qt6::Sql` - 数据库
- `Qt6::Multimedia` - 多媒体
- `Qt6::Charts` - 图表

## 使用Qt Creator

如果你使用Qt Creator IDE：

1. 打开Qt Creator
2. 进入 "工具" -> "选项" -> "Kits"
3. 确保Qt Creator能检测到你的Qt 6.9.1安装
4. 如果没有，手动添加：
   - Qt版本路径: `D:\Qt\6.9.1\mingw_64`
   - 编译器: MinGW 64-bit

## 常见问题

### CMake找不到Qt

解决方案：
```cmake
set(CMAKE_PREFIX_PATH "D:/Qt/6.9.1/mingw_64")
```

或者在运行cmake时指定：
```powershell
cmake -DCMAKE_PREFIX_PATH="D:/Qt/6.9.1/mingw_64" ..
```

### 运行时找不到Qt DLL

确保Qt的bin目录在PATH中，或者将Qt DLL复制到可执行文件目录。

## 参考文件

- `setup_qt_env.ps1` - 环境配置脚本
- `qt_example_CMakeLists.txt` - Qt项目CMake配置示例

