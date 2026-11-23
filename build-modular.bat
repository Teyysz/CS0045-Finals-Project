@echo off
setlocal

set "VSDEVCMD=C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat"
if not exist "%VSDEVCMD%" (
    echo ERROR: Visual Studio Build Tools were not found.
    echo Update VSDEVCMD in this file if Visual Studio is installed elsewhere.
    exit /b 1
)

call "%VSDEVCMD%" -arch=x64
if errorlevel 1 exit /b 1

set "PROJECT_DIR=%~dp0"
set "VCPKG_ROOT=%PROJECT_DIR%..\vcpkg_installed"
if not exist "%VCPKG_ROOT%\x64-windows\include" set "VCPKG_ROOT=%PROJECT_DIR%..\..\vcpkg_installed"

if not exist "%VCPKG_ROOT%\x64-windows\include" (
    echo ERROR: Could not find vcpkg_installed\x64-windows.
    echo Update VCPKG_ROOT in build-modular.bat.
    exit /b 1
)

cl /nologo /Zi /EHsc /std:c++17 ^
    "%PROJECT_DIR%App.cpp" ^
    "%PROJECT_DIR%World.cpp" ^
    "%PROJECT_DIR%Character.cpp" ^
    "%PROJECT_DIR%Interface.cpp" ^
    /I "%VCPKG_ROOT%\x64-windows\include" ^
    /Fe:"%PROJECT_DIR%Campus_Drip.exe" ^
    /link /LIBPATH:"%VCPKG_ROOT%\x64-windows\lib" ^
    glew32.lib freeglut.lib opengl32.lib glu32.lib

if errorlevel 1 exit /b 1

copy /Y "%VCPKG_ROOT%\x64-windows\bin\glew32.dll" "%PROJECT_DIR%" >nul
copy /Y "%VCPKG_ROOT%\x64-windows\bin\freeglut.dll" "%PROJECT_DIR%" >nul

echo.
echo Build completed successfully:
echo %PROJECT_DIR%Campus_Drip.exe
endlocal
