; Installs PKS Edit
; you must have a "x64" edition of Windows.

#define MyAppName "PKS Edit"
#define MyAppExeName "pksedit.exe"

[Setup]
AppName=PKS-Edit
AppVersion=1.9.0
WizardStyle=modern
DefaultDirName={autopf}\pksedit
UsePreviousAppDir=yes
LicenseFile=license
DisableDirPage=false
DefaultGroupName=PKS Edit
UninstallDisplayIcon={app}\{#MyAppExeName},1
Compression=lzma2
SolidCompression=yes
OutputDir=c:\temp
SetupIconFile=rc\pks2.ico
OutputBaseFilename=Setup PKS Edit
; "ArchitecturesAllowed=x64" specifies that Setup cannot run on
; anything but x64.
ArchitecturesAllowed=x64
; "ArchitecturesInstallIn64BitMode=x64" requests that the install be
; done in "64-bit mode" on x64, meaning it should use the native
; 64-bit Program Files directory and the 64-bit view of the registry.
ArchitecturesInstallIn64BitMode=x64
ChangesAssociations = yes

[Files]
Source: "x64\Release\{#MyAppExeName}"; DestDir: "{app}"; DestName: "{#MyAppExeName}"
Source: "pks_sys\pksedit.ini"; DestDir: "{app}\pks_sys"
Source: "pks_sys\pksconfig.json"; DestDir: "{app}\pks_sys"
Source: "pks_sys\pksedit.reg"; DestDir: "{app}\pks_sys"
Source: "pks_sys\pksedit.chm"; DestDir: "{app}\pks_sys"
Source: "readme.md"; DestDir: "{app}"; Flags: isreadme
Source: "doc\release_notes.md"; DestDir: "{app}\doc" 

[INI]
Filename: "win.ini"; Section: "PksEdit"; Key: "PKS_SYS"; String: "{app}"

[Icons]
Name: "{group}\PKS Edit"; Filename: "{app}\{#MyAppExeName}"

[Registry]
Root: HKCR; Subkey: ".txt"; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKCR; Subkey: ".jsp"; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKCR; Subkey: ".log"; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKCR; Subkey: "{#MyAppName}"; ValueData: "Program {#MyAppName}"; Flags: uninsdeletekey;   ValueType: string;  ValueName: ""
Root: HKCR; Subkey: "{#MyAppName}\DefaultIcon";  ValueData: "{app}\{#MyAppExeName},1"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKCR; Subkey: "{#MyAppName}\shell\open\command";  ValueData: """{app}\{#MyAppExeName}"" ""%1""";  Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
