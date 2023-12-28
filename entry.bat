rmdir /s /q .git\hooks
mklink /J .git\hooks %~dp0git-hooks

set "tools_dir=%~dp0tools"
set "windows_dir=%tools_dir%\windows"

if not exist "%windows_dir%" (
    echo Extracting windows.zip to %windows_dir%
    powershell -Command "Expand-Archive -Path '%tools_dir%\windows.zip' -DestinationPath '%tools_dir%' -Force"
) else (
    echo %windows_dir% already exists.
)

set "path=%path%;%~dp0tools\windows\xpack-arm-none-eabi-gcc-13.2.1-1.1\bin\;%~dp0tools\windows\xpack-windows-build-tools-4.4.1-2\bin\;%~dp0tools\windows\xpack-openocd-0.12.0-2\bin\"
code .