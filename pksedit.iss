; Installs PKS Edit
; you must have a "x64" edition of Windows.

#define MyAppName "PKSEdit.1"
#define MyAppBaseName "pksedit"
#define MyAppExeName  "pksedit.exe"

[Setup]
AppName=PKS-Edit
AppVersion=2.1.0
WizardStyle=modern
DefaultDirName={autopf}\pksedit
UsePreviousAppDir=yes
LicenseFile=license
DisableDirPage=false
DefaultGroupName=PKS Edit
UninstallDisplayIcon={app}\pks_sys\pks.ico
Compression=lzma2
SolidCompression=yes
OutputDir=c:\temp
LanguageDetectionMethod=locale
SetupIconFile=rc\pks.ico
OutputBaseFilename=Setup PKS Edit
; "ArchitecturesAllowed=x64" specifies that Setup cannot run on
; anything but x64.
ArchitecturesAllowed=x64
; "ArchitecturesInstallIn64BitMode=x64" requests that the install be
; done in "64-bit mode" on x64, meaning it should use the native
; 64-bit Program Files directory and the 64-bit view of the registry.
ArchitecturesInstallIn64BitMode=x64
ChangesAssociations = yes

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "de"; MessagesFile: "compiler:Languages\German.isl"
Name: "nl"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "fr"; MessagesFile: "compiler:Languages\French.isl"
Name: "it"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl"

[CustomMessages]
en.MsgOpenWith=Open with %1
de.MsgOpenWith=Öffnen mit %1
it.MsgOpenWith=Aperta con
fr.MsgOpenWith=Ouvrir avec
nl.MsgOpenWith=Openen met
ru.MsgOpenWith=Открыть с

[Run]
Filename: "{app}\{#MyAppExeName}"; Parameters: "readme.md /w"; Flags: postinstall nowait

[Files]
Source: "x64\Release\{#MyAppExeName}"; DestDir: "{app}"; DestName: "{#MyAppExeName}"; Flags: ignoreversion
Source: "x64\Release\{#MyAppBaseName}.en-US.dll"; DestDir: "{app}"; DestName: "{#MyAppBaseName}.en-US.dll"; Flags: ignoreversion
Source: "pks_sys\pksedit.ini"; DestDir: "{app}\pks_sys"
Source: "pks_sys\batch.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\cpp.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\dart.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\typescript.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\default.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\markdown.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\ps.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\java.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\javascript.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\json.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\pascal.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\pksgrep.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\resource.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\makefile.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\python.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\fitnesse.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\integrity.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\pksedit.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\css.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\sql.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\logfile.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\proto.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\xml.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\text.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\shell.grammar.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\themeconfig.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\pkseditconfig.json"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\pksedit.mac"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "pks_sys\pksedit.chm"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "rc\pks.ico"; DestDir: "{app}\pks_sys"; Flags: ignoreversion
Source: "readme.md"; DestDir: "{app}"; Flags: ignoreversion 
Source: "doc\release_notes.md"; DestDir: "{app}\doc"; Flags: ignoreversion 
Source: "doc\markdown.md"; DestDir: "{app}\doc"; Flags: ignoreversion 
Source: "doc\cat.bmp"; DestDir: "{app}\doc"; Flags: ignoreversion 
Source: "doc\grammar.md"; DestDir: "{app}\doc"; Flags: ignoreversion 

[INI]
Filename: "win.ini"; Section: "PksEdit"; Key: "PKS_SYS"; String: "{app}\PKS_SYS"

[Icons]
Name: "{group}\PKS Edit"; Filename: "{app}\{#MyAppExeName}"

[Registry]
Root: HKCR; Subkey: "*\shell\{cm:MsgOpenWith,PKS Edit}\command"; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKCR; Subkey: "*\shell\{cm:MsgOpenWith,PKS Edit}"; ValueData: "{app}\pks_sys\pks.ico"; Flags: uninsdeletevalue; ValueType: string;  ValueName: "icon"
Root: HKCR; Subkey: "*\shell\{cm:MsgOpenWith,PKS Edit}\ddeexec";  ValueData: """%1""";  Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKCR; Subkey: "*\shell\{cm:MsgOpenWith,PKS Edit}\ddeexec\Application";  ValueData: "{#MyAppName}";  Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKCR; Subkey: "*\shell\{cm:MsgOpenWith,PKS Edit}\ddeexec\topic";  ValueData: "commandline";  Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\.txt"; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\.c"; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\.md"; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\.json"; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\.js"; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\.ts"; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\.ddl"; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\.sql"; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\.java"; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\.css"; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\.xml"; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\.log"; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\.pas"; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\.pkc"; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\{#MyAppName}"; ValueData: "{#MyAppName} Application"; Flags: uninsdeletekey;   ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\{#MyAppName}\DefaultIcon";  ValueData: "{app}\pks_sys\pks.ico"; Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\{#MyAppName}\shell\open\command";  ValueData: """{app}\{#MyAppExeName}"" ""%1""";  Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\{#MyAppName}\shell\open\ddeexec";  ValueData: """%1""";  Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\{#MyAppName}\shell\open\ddeexec\Application";  ValueData: "{#MyAppName}";  Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKLM; Subkey: "Software\Classes\{#MyAppName}\shell\open\ddeexec\topic";  ValueData: "commandline";  Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
