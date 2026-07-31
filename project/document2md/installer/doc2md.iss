#ifndef AppVersion
  #define AppVersion "0.1.0"
#endif

#ifndef PackageRoot
  #define PackageRoot "..\out\package\doc2md-0.1.0"
#endif

#ifndef OutputDir
  #define OutputDir "..\out\release"
#endif

[Setup]
AppId={{EBD14075-E9ED-42F6-8731-D8956448E89D}
AppName=doc2md
AppVersion={#AppVersion}
AppPublisher=doc2md
DefaultDirName={autopf}\doc2md
DefaultGroupName=doc2md
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
OutputDir={#OutputDir}
OutputBaseFilename=doc2md-{#AppVersion}-windows-x64-setup
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern
DisableProgramGroupPage=yes
UninstallDisplayIcon={app}\bin\doc2md.exe
VersionInfoVersion={#AppVersion}
VersionInfoProductName=doc2md
VersionInfoProductVersion={#AppVersion}

[Tasks]
Name: "desktopicon"; Description: "创建桌面快捷方式"; GroupDescription: "附加任务："; Flags: unchecked

[Files]
Source: "{#PackageRoot}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\doc2md"; Filename: "{app}\bin\doc2md.exe"; WorkingDir: "{app}\bin"
Name: "{autodesktop}\doc2md"; Filename: "{app}\bin\doc2md.exe"; WorkingDir: "{app}\bin"; Tasks: desktopicon

[Run]
Filename: "{app}\bin\vc_redist.x64.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: "正在安装 Microsoft Visual C++ 运行库..."; Flags: runhidden waituntilterminated
Filename: "{app}\bin\doc2md.exe"; Description: "启动 doc2md"; WorkingDir: "{app}\bin"; Flags: nowait postinstall skipifsilent
