# 快速打开 Qt Designer
# 使用方法：.\open-designer.ps1 [ui文件名]

param(
    [string]$UiFile = ""
)

$DesignerPath = "D:\Qt-\6.9.1\mingw_64\bin\designer.exe"

if (-not (Test-Path $DesignerPath)) {
    Write-Host "❌ Qt Designer 未找到: $DesignerPath" -ForegroundColor Red
    exit 1
}

if ($UiFile -ne "" -and (Test-Path $UiFile)) {
    Write-Host "🎨 正在打开 Qt Designer 编辑: $UiFile" -ForegroundColor Green
    & $DesignerPath $UiFile
} else {
    Write-Host "🎨 正在打开 Qt Designer..." -ForegroundColor Green
    & $DesignerPath
}



















