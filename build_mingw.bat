@echo off

setlocal enabledelayedexpansion

set "BTC_CMAKE_PATH=E:\vsci\bin"
set "MINGW_PATH=C:\mingw64\11.3\bin"
set "INCLUDE=c:\mingw64\11.3\bin\..\lib\gcc\x86_64-w64-mingw32\11.3.0\include;c:\mingw64\11.3\bin\..\lib\gcc\x86_64-w64-mingw32\11.3.0\..\..\..\..\include;c:\mingw64\11.3\bin\..\lib\gcc\x86_64-w64-mingw32\11.3.0\include-fixed;c:\mingw64\11.3\bin\..\lib\gcc\x86_64-w64-mingw32\11.3.0\..\..\..\..\x86_64-w64-mingw32\include"
set "PATH=%BTC_CMAKE_PATH%;%MINGW_PATH%;%PATH%"

REM Seat Heating Controller Build Script for MinGW/GCC
REM Uses MinGW Makefiles generator with GCC compiler

REM Default options
set BUILD_TYPE=Debug
set CLEAN=true
set RUN_TESTS=false
set ENABLE_COVERAGE=false
set BUILD_TESTING=ON
set VERBOSE=false
set GENERATOR=MinGW Makefiles
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
if /i "%~1"=="--coverage" (
    set ENABLE_COVERAGE=true
    shift
    goto parse_args
)
if /i "%~1"=="-v" (
    set VERBOSE=true
    shift
    goto parse_args
)
if /i "%~1"=="--verbose" (
    set VERBOSE=true
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
if /i "%~1"=="-j" (
    set JOBS=%~2
    shift
    shift
    goto parse_args
)
if /i "%~1"=="--jobs" (
    set JOBS=%~2
    shift
    shift
    goto parse_args
)
echo Unknown option: %~1
goto show_help

:end_parse

REM Check if MinGW is available
gcc --version >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [ERROR] GCC not found in PATH!
    echo Please install MinGW or adjust MINGW_PATH in this script.
    exit /b 1
)

REM Check if CMake is available
cmake --version >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake not found in PATH!
    echo Please install CMake or adjust CMAKE_PATH in this script.
    exit /b 1
)

REM Display compiler version
echo [INFO] Checking GCC version...
gcc --version 2>nul | findstr /C:"gcc"
echo.

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

REM Enable verbose makefiles if requested
if "%VERBOSE%"=="true" (
    set CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_VERBOSE_MAKEFILE=ON
)

REM Add coverage flags if requested
if "%ENABLE_COVERAGE%"=="true" (
    set CMAKE_ARGS=%CMAKE_ARGS% -DENABLE_COVERAGE=ON
)

REM Explicitly set compilers to avoid conflicts
set CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++

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

REM Generate coverage report if requested
if "%ENABLE_COVERAGE%"=="true" (
    if "%RUN_TESTS%"=="true" (
        echo [INFO] Generating coverage report...
        gcovr --version >nul 2>nul
        if %ERRORLEVEL% equ 0 (
            cmake --build . --target coverage
            echo [SUCCESS] Coverage report generated in %BUILD_DIR%\coverage\index.html
        ) else (
            echo [WARNING] gcovr not found. Install with: pip install gcovr
        )
    ) else (
        echo [WARNING] Coverage requires tests to be run. Use -t or --test flag.
    )
)

REM Summary
echo ==========================================
echo [SUCCESS] Build Summary
echo ==========================================
echo Build type: %BUILD_TYPE%
echo Library: %BUILD_DIR%\libseat_heating.a
if "%BUILD_TESTING%"=="ON" (
    echo Tests: %BUILD_DIR%\seat_heating_tests.exe
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
        echo   seat_heating_tests.exe
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
echo     -g, --generator G   Specify CMake generator (default: "MinGW Makefiles")
echo     -v, --verbose       Show all compiler commands (verbose build)
echo     -j, --jobs N        Number of parallel jobs (default: %NUMBER_OF_PROCESSORS%)
echo     --no-tests          Disable building tests
echo     --coverage          Enable code coverage analysis (requires gcovr)
echo.
echo Examples:
echo     %~nx0                           # Build in Debug mode
echo     %~nx0 -r -t                     # Build Release and run tests
echo     %~nx0 -v                        # Build with verbose output
echo     %~nx0 -c --coverage -t          # Clean build with coverage
echo.
exit /b 0