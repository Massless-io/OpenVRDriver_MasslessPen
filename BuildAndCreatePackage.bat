call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

:: Deleting the main builds folder
RMDIR /s /q Builds

cd driver_massless

msbuild.exe driver_massless.vcxproj /p:Configuration=Release /t:Build

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"

cd ../Installer
msbuild.exe MasslessOpenVRDriverInstaller.sln /p:Configuration=Release /t:Build

cd ..

CreateOpenVRRelease.bat %1