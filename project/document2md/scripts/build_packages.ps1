[CmdletBinding()]
param(
    [string]$Version = "0.1.0",
    [string]$PackageRoot,
    [string]$OutputRoot,
    [string]$VisualStudioRoot = "D:\tools\VS",
    [string]$InnoCompiler
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$scriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = Split-Path -Parent $scriptRoot

if ([string]::IsNullOrWhiteSpace($PackageRoot)) {
    $PackageRoot = Join-Path $projectRoot "out\package\doc2md-$Version"
}

if ([string]::IsNullOrWhiteSpace($OutputRoot)) {
    $OutputRoot = Join-Path $projectRoot "out\release"
}

$PackageRoot = [System.IO.Path]::GetFullPath($PackageRoot)
$OutputRoot = [System.IO.Path]::GetFullPath($OutputRoot)
$applicationPath = Join-Path $PackageRoot "bin\doc2md.exe"
$redistInstallerPath = Join-Path $PackageRoot "bin\vc_redist.x64.exe"

if (-not (Test-Path -LiteralPath $applicationPath -PathType Leaf)) {
    throw "Invalid package directory. File not found: $applicationPath"
}

if (-not (Test-Path -LiteralPath $redistInstallerPath -PathType Leaf)) {
    throw "The installer package requires vc_redist.x64.exe: $redistInstallerPath"
}

New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null

function Find-MsvcCrtDirectory {
    param([string]$VsRoot)

    $redistRoot = Join-Path $VsRoot "VC\Redist\MSVC"
    if (-not (Test-Path -LiteralPath $redistRoot -PathType Container)) {
        throw "MSVC Redist directory not found: $redistRoot"
    }

    $crtDirectory = Get-ChildItem -LiteralPath $redistRoot -Directory |
        Sort-Object {
            try { [version]$_.Name } catch { [version]"0.0" }
        } -Descending |
        ForEach-Object {
            Get-ChildItem -LiteralPath (Join-Path $_.FullName "x64") -Directory -Filter "Microsoft.VC*.CRT" -ErrorAction SilentlyContinue |
                Select-Object -First 1
        } |
        Select-Object -First 1

    if ($null -eq $crtDirectory) {
        throw "No x64 Microsoft.VC*.CRT directory was found under: $redistRoot"
    }

    return $crtDirectory.FullName
}

function Find-InnoCompiler {
    param([string]$RequestedPath)

    if (-not [string]::IsNullOrWhiteSpace($RequestedPath)) {
        if (Test-Path -LiteralPath $RequestedPath -PathType Leaf) {
            return [System.IO.Path]::GetFullPath($RequestedPath)
        }
        throw "The requested Inno Setup compiler does not exist: $RequestedPath"
    }

    $command = Get-Command "ISCC.exe" -ErrorAction SilentlyContinue
    if ($null -ne $command) {
        return $command.Source
    }

    $candidates = @(
        (Join-Path $env:LOCALAPPDATA "Programs\Inno Setup 6\ISCC.exe"),
        "C:\Program Files (x86)\Inno Setup 6\ISCC.exe",
        "C:\Program Files\Inno Setup 6\ISCC.exe"
    )

    foreach ($candidate in $candidates) {
        if (Test-Path -LiteralPath $candidate -PathType Leaf) {
            return $candidate
        }
    }

    throw "Inno Setup 6 was not found. Install it or specify ISCC.exe with -InnoCompiler."
}

function New-PortableZip {
    param(
        [string]$SourceDirectory,
        [string]$DestinationPath
    )

    $tarCommand = Get-Command "tar.exe" -ErrorAction SilentlyContinue
    if ($null -ne $tarCommand) {
        $sourceParent = Split-Path -Parent $SourceDirectory
        $sourceName = Split-Path -Leaf $SourceDirectory
        & $tarCommand.Source -a -c -f $DestinationPath -C $sourceParent $sourceName
        if ($LASTEXITCODE -ne 0) {
            throw "tar.exe failed with exit code: $LASTEXITCODE"
        }
        return
    }

    Compress-Archive -LiteralPath $SourceDirectory -DestinationPath $DestinationPath -CompressionLevel Optimal
}

Write-Host "[1/2] Building portable ZIP..."
$portableName = "doc2md-$Version-windows-x64-portable"
$portableStage = Join-Path $OutputRoot $portableName
$portableZip = Join-Path $OutputRoot "$portableName.zip"

if (Test-Path -LiteralPath $portableStage) {
    Remove-Item -LiteralPath $portableStage -Recurse -Force
}
if (Test-Path -LiteralPath $portableZip) {
    Remove-Item -LiteralPath $portableZip -Force
}

New-Item -ItemType Directory -Path $portableStage -Force | Out-Null
Copy-Item -Path (Join-Path $PackageRoot "*") -Destination $portableStage -Recurse -Force

$portableRedistInstaller = Join-Path $portableStage "bin\vc_redist.x64.exe"
if (Test-Path -LiteralPath $portableRedistInstaller) {
    Remove-Item -LiteralPath $portableRedistInstaller -Force
}

$crtDirectory = Find-MsvcCrtDirectory -VsRoot $VisualStudioRoot
$crtDlls = @(Get-ChildItem -LiteralPath $crtDirectory -File -Filter "*.dll")
if ($crtDlls.Count -eq 0) {
    throw "No MSVC CRT DLL was found in: $crtDirectory"
}
Copy-Item -LiteralPath $crtDlls.FullName -Destination (Join-Path $portableStage "bin") -Force

New-PortableZip -SourceDirectory $portableStage -DestinationPath $portableZip
Remove-Item -LiteralPath $portableStage -Recurse -Force
Write-Host "Portable ZIP: $portableZip"

Write-Host "[2/2] Building installer..."
$isccPath = Find-InnoCompiler -RequestedPath $InnoCompiler
$installerScript = Join-Path $projectRoot "installer\doc2md.iss"
$packageDefine = "/DPackageRoot=$PackageRoot"
$outputDefine = "/DOutputDir=$OutputRoot"
$versionDefine = "/DAppVersion=$Version"

& $isccPath $versionDefine $packageDefine $outputDefine $installerScript
if ($LASTEXITCODE -ne 0) {
    throw "Inno Setup failed with exit code: $LASTEXITCODE"
}

$installerPath = Join-Path $OutputRoot "doc2md-$Version-windows-x64-setup.exe"
if (-not (Test-Path -LiteralPath $installerPath -PathType Leaf)) {
    throw "Inno Setup did not produce the expected file: $installerPath"
}

Write-Host "Installer: $installerPath"
Write-Host "Both release artifacts were created successfully."
