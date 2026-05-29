#define MyAppName "MathBoard"
#define MyAppVersion "0.1.0"
#define MyAppExeName "mathboard.exe"

[Setup]
AppId={{A3B86E2B-2F79-4E9A-8C7A-7C7A3F7A5A8F}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputDir=dist
OutputBaseFilename=MathBoard-Setup
Compression=lzma
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64
WizardStyle=modern
DisableProgramGroupPage=yes
; --- ADDED: This sets the icon for the actual MathBoard-Setup.exe file ---
SetupIconFile=logo.ico

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "build\Release\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
; --- ADDED: This copies the icon file to the install directory so shortcuts can use it ---
Source: "logo.ico"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
; --- MODIFIED: Added IconFilename to explicitly use your custom icon for the shortcuts ---
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; IconFilename: "{app}\logo.ico"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; IconFilename: "{app}\logo.ico"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; Flags: unchecked

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "Launch {#MyAppName}"; Flags: nowait postinstall skipifsilent