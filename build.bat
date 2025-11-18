@echo off

setlocal enabledelayedexpansion

set BTC_CMAKE_PATH=E:\vsci\bin
set PATH=%BTC_CMAKE_PATH%;%PATH%

REM Seat Heating Controller Build Script for Windows
REM Supports Visual Studio, MinGW, and other Windows compilers

REM Default options
set BUILD_TYPE=Debug
set CLEAN=true
set RUN_TESTS=false
set ENABLE_COVERAGE=false
set BUILD_TESTING=ON
set GENERATOR=
set JOBS=%NUMBER_OF_PROCESSORS%

REM Parse command line arguments
:parse_args
if "%~1"=="" goto end_parse
if /i "%~1"=="-h" goto show_help
if /i "%~1"=="--help" goto show_help
if /i "%~1"=="-c" (
    set CLEAN=true
    shift
    goto parse_args
)
if /i "%~1"=="--clean" (
    set CLEAN=true
    shift
    goto parse_args
)
if /i "%~1"=="-r" (
    set BUILD_TYPE=Release
    shift
    goto parse_args
)
if /i "%~1"=="--release" (
    set BUILD_TYPE=Release
    shift
    goto parse_args
)
if /i "%~1"=="-t" (
    set RUN_TESTS=true
    shift
    goto parse_args
)
if /i "%~1"=="--test" (
    set RUN_TESTS=true
    shift
    goto parse_args
)
if /i "%~1"=="--no-tests" (
    set BUILD_TESTING=OFF
    shift
    goto parse_args
)
if /i "%~1"=="-g" (
    set GENERATOR=%~2
    shift
    shift
    goto parse_args
)
if /i "%~1"=="--generator" (
    set GENERATOR=%~2
    shift
    shift
    goto parse_args
)
echo Unknown option: %~1
goto show_help

:end_parse

REM Project directories

set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%
if "%PROJECT_ROOT:~-1%"=="\" set "PROJECT_ROOT=%PROJECT_ROOT:~0,-1%"

set BUILD_DIR=%PROJECT_ROOT%\build

echo ==========================================
echo Seat Heating Controller Build Script
echo ==========================================
echo Build type: %BUILD_TYPE%
echo Build directory: %BUILD_DIR%
echo Build testing: %BUILD_TESTING%
echo ==========================================

REM Clean build directory if requested
if "%CLEAN%"=="true" (
    echo [INFO] Cleaning build directory...
    if exist "%BUILD_DIR%" (
        rmdir /s /q "%BUILD_DIR%"
        echo [SUCCESS] Build directory cleaned
    )
)

REM Create build directory
if not exist "%BUILD_DIR%" (
    echo [INFO] Creating build directory...
    mkdir "%BUILD_DIR%"
)

cd /d "%BUILD_DIR%"

REM Configure with CMake
echo [INFO] Configuring project with CMake...

set CMAKE_ARGS=-DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DBUILD_TESTING=%BUILD_TESTING%

if not "%GENERATOR%"=="" (
    set CMAKE_ARGS=%CMAKE_ARGS% -G "%GENERATOR%"
)

cmake %CMAKE_ARGS% "%PROJECT_ROOT%"

if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake configuration failed!
    exit /b 1
)

echo [SUCCESS] Configuration completed

REM Build
echo [INFO] Building project...
cmake --build . --config %BUILD_TYPE% -j %JOBS%

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed!
    exit /b 1
)

echo [SUCCESS] Build completed successfully

REM Run tests if requested
if "%RUN_TESTS%"=="true" (
    if "%BUILD_TESTING%"=="ON" (
        echo [INFO] Running tests...
        ctest -C %BUILD_TYPE% --output-on-failure
        
        if %ERRORLEVEL% equ 0 (
            echo [SUCCESS] All tests passed!
        ) else (
            echo [ERROR] Some tests failed!
            exit /b 1
        )
    )
)

REM Summary
echo ==========================================
echo [SUCCESS] Build Summary
echo ==========================================
echo Build type: %BUILD_TYPE%
echo Library: %BUILD_DIR%\%BUILD_TYPE%\seat_heating.lib
if "%BUILD_TESTING%"=="ON" (
    echo Tests: %BUILD_DIR%\%BUILD_TYPE%\seat_heating_tests.exe
)
echo ==========================================
echo [SUCCESS] Build script completed successfully!

if "%RUN_TESTS%"=="false" (
    if "%BUILD_TESTING%"=="ON" (
        echo.
        echo To run tests manually:
        echo   cd %BUILD_DIR%
        echo   ctest -C %BUILD_TYPE% --output-on-failure
        echo Or:
        echo   %BUILD_TYPE%\seat_heating_tests.exe
    )
)

goto :eof

:show_help
echo Usage: %~nx0 [OPTIONS]
echo.
echo Options:
echo     -h, --help          Show this help message
echo     -c, --clean         Clean build directory before building
echo     -r, --release       Build in Release mode (default: Debug)
echo     -t, --test          Run tests after building
echo     -g, --generator G   Specify CMake generator (e.g., "Visual Studio 17 2022")
echo     --no-tests          Disable building tests
echo.
echo Examples:
echo     %~nx0                           # Build in Debug mode
echo     %~nx0 -r -t                     # Build Release and run tests
echo     %~nx0 -c --generator "MinGW Makefiles"  # Clean build with MinGW
echo.
exit /b 0
