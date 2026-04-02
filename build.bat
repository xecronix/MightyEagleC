@echo off
setlocal EnableExtensions

set "ROOT=%~dp0"
set "MODE=%~1"
if "%MODE%"=="" set "MODE=Release"

if /I "%MODE%"=="clean" goto :clean

if /I not "%MODE%"=="Release" if /I not "%MODE%"=="Debug" (
    echo Usage: build.bat [Release^|Debug^|clean]
    exit /b 1
)

set "PELLESC_BIN=C:\Program Files\PellesC\Bin"
set "POVARS=%PELLESC_BIN%\povars32.bat"

if not exist "%POVARS%" (
    echo ERROR: Could not find "%POVARS%"
    exit /b 1
)

call "%POVARS%"
if errorlevel 1 (
    echo ERROR: Failed to initialize Pelles C environment.
    exit /b 1
)

call :build_here "%ROOT%" "MightyEagleLib32.ppj" "library"
if errorlevel 1 exit /b %errorlevel%

call :build_here "%ROOT%test" "MightyEagleTest32.ppj" "tests"
if errorlevel 1 exit /b %errorlevel%

call :build_here "%ROOT%demo" "MightyEagleDemo32.ppj" "demo"
if errorlevel 1 exit /b %errorlevel%

echo.
echo Build succeeded.
exit /b 0

:clean
echo.
echo Cleaning build artifacts...

rem Root project artifacts
call :delete_file "%ROOT%MightyEagleLib32.lib"
call :delete_file "%ROOT%MightyEagleTest32.exe"
call :delete_file "%ROOT%MightyEagleDemo32.exe"

rem Root folders
call :reset_dir "%ROOT%output"

rem Test project artifacts
call :delete_file "%ROOT%test\MightyEagleTest32.exe"
call :reset_dir  "%ROOT%test\output"

rem Demo project artifacts
call :delete_file "%ROOT%demo\MightyEagleDemo32.exe"
call :reset_dir  "%ROOT%demo\output"

echo.
echo Clean succeeded.
exit /b 0

:build_here
echo.
echo Building %~3...
pushd "%~1" || (
    echo ERROR: Could not change to "%~1"
    exit /b 1
)
pomake.exe -f "%~2" POC_PROJECT_MODE=%MODE%
set "ERR=%errorlevel%"
popd
exit /b %ERR%

:delete_file
if exist "%~1" (
    del /f /q "%~1"
    if errorlevel 1 (
        echo ERROR: Failed to delete file "%~1"
        exit /b 1
    )
    echo Deleted file: %~1
)
exit /b 0

:reset_dir
if exist "%~1" (
    rd /s /q "%~1"
    if errorlevel 1 (
        echo ERROR: Failed to remove directory "%~1"
        exit /b 1
    )
)
mkdir "%~1"
if errorlevel 1 (
    echo ERROR: Failed to create directory "%~1"
    exit /b 1
)
echo Reset directory: %~1
exit /b 0