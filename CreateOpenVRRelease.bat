echo "Constructing a Massless OpenVR Driver release version: " %1

if not exist "%~dp0\..\..\Staging\MasslessOpenVRDriverV%1" mkdir "%~dp0\..\..\Staging\MasslessOpenVRDriverV%1"

:: xcopy /v /y /s /i "%~dp0\driver_massless\Output\driver\massless\*" "%~dp0\..\..\Staging\MasslessOpenVRDriverV%1\massless"
:: echo F|xcopy /v /y "%~dp0\Documentation\Setting up Massless in SteamVR.pdf" "%~dp0\..\..\Staging\MasslessOpenVRDriverV%1\Setting up Massless in SteamVR.pdf"

echo F|xcopy /v /y "%~dp0\ExtraFilesForZip\InstallScript.bat" "%~dp0\..\..\Staging\MasslessOpenVRDriverV%1\InstallScript.bat"
echo F|xcopy /v /y "%~dp0\Build\MasslessOpenVRDriverInstaller.msi" "%~dp0\..\..\Staging\MasslessOpenVRDriverV%1\MasslessOpenVRDriverInstaller.msi"
echo F|xcopy /v /y "%~dp0\ExtraFilesForZip\vcredist_2012_x64.exe" "%~dp0\..\..\Staging\MasslessOpenVRDriverV%1\vcredist_2012_x64.exe"
echo F|xcopy /v /y "%~dp0\ExtraFilesForZip\vcredist_2019_x64.exe" "%~dp0\..\..\Staging\MasslessOpenVRDriverV%1\vcredist_2019_x64.exe"

IF EXIST "%~dp0\..\..\Staging\MasslessOpenVRDriverV%1.zip" DEL /F "%~dp0\..\..\Staging\MasslessOpenVRDriverV%1.zip"

"C:\Program Files\7-Zip\7z.exe" a "%~dp0\..\..\Staging\MasslessOpenVRDriverV%1.zip" "%~dp0\..\..\Staging\MasslessOpenVRDriverV%1\"
