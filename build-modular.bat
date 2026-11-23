@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64
if errorlevel 1 goto failed

cd /d "%~dp0"

if not exist "..\vcpkg_installed\x64-windows\include\GL\glew.h" goto missing

cl /nologo /Zi /EHsc /std:c++17 "App\App.cpp" "World\World.cpp" "Character\Character.cpp" "Interface\Interface.cpp" /I"App" /I"World" /I"Character" /I"Interface" /I"..\vcpkg_installed\x64-windows\include" /Fe:"Campus_Drip.exe" /link /LIBPATH:"..\vcpkg_installed\x64-windows\lib" glew32.lib freeglut.lib opengl32.lib glu32.lib

if errorlevel 1 goto failed

copy /Y "..\vcpkg_installed\x64-windows\bin\glew32.dll" "." >nul
copy /Y "..\vcpkg_installed\x64-windows\bin\freeglut.dll" "." >nul

echo.
echo Build completed successfully.
exit /b 0

:missing
echo.
echo ERROR: GLEW was not found in:
echo ..\vcpkg_installed\x64-windows\include\GL\glew.h
exit /b 1

:failed
echo.
echo Build failed.
exit /b 1