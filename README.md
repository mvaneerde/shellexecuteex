# shellexecuteex.exe
Command-line utility which takes the arguments for the [ShellExecuteEx](https://learn.microsoft.com/en-us/windows/win32/shell/launch) API on the command line, calls the API, and exits.

## Install

1. Go to the [latest release](https://github.com/mvaneerde/shellexecuteex/releases/latest)
1. Download the ShellExecuteEx-x.y.z.zip asset
1. Extract the ShellExecuteEx/x64/ShellExecuteEx.exe or ShellExecuteEx/arm64/ShellExecuteEx.exe file, as appropriate to your local system architecture
1. Place it somewhere in your local file system, perhaps in your PATH
1. The executable is self-signed. Windows might complain that the certificate is not trusted. You can ease things by adding the signing certificate to your trusted certificate store

## Example usage

Get a full usage prompt

```
shellexecuteex.exe --help
```

Open Calculator
```
shellexecuteex.exe `
    --file calc.exe
```

Open https://example.com
```
shellexecuteex.exe `
    --file "https://example.com"
```

Open PowerShell.exe elevated
```
shellexecuteex.exe `
    --file powershell.exe `
    --verb runas
```

Run a command line minimized, wait for it to complete, and return the exit code

```
shellexecuteex.exe `
    --file cmd.exe `
    --parameters "/c timeout /t 10 /nobreak & exit 12345" `
    --show SW_SHOWMINIMIZED `
    --no-close-process `
    --relay-exit-code
Write-Host $LASTEXITCODE
```

Open the Search dialog box in the home directory

```
shellexecuteex.exe `
    --file "%userprofile%" `
    --verb "find" `
    --do-environment-substitution
```
