@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ============================================
echo  p-adic Converter - Windows Installation
echo ============================================
echo.

:: 语言选择
echo Please select your language:
echo.
echo   1. English
echo   2. 简体中文
echo   3. 繁體中文
echo   4. 日本語
echo   5. 한국어
echo   6. Français
echo   7. Русский
echo   8. Español
echo   9. Deutsch
echo.

:LANG_SELECT
set /p LANG_CHOICE="Enter number (1-9): "
if "%LANG_CHOICE%"=="1" set LANG_CODE=en & set LANG_NAME=English & goto LANG_DONE
if "%LANG_CHOICE%"=="2" set LANG_CODE=zh_CN & set LANG_NAME=简体中文 & goto LANG_DONE
if "%LANG_CHOICE%"=="3" set LANG_CODE=zh_TW & set LANG_NAME=繁體中文 & goto LANG_DONE
if "%LANG_CHOICE%"=="4" set LANG_CODE=ja & set LANG_NAME=日本語 & goto LANG_DONE
if "%LANG_CHOICE%"=="5" set LANG_CODE=ko & set LANG_NAME=한국어 & goto LANG_DONE
if "%LANG_CHOICE%"=="6" set LANG_CODE=fr & set LANG_NAME=Français & goto LANG_DONE
if "%LANG_CHOICE%"=="7" set LANG_CODE=ru & set LANG_NAME=Русский & goto LANG_DONE
if "%LANG_CHOICE%"=="8" set LANG_CODE=es & set LANG_NAME=Español & goto LANG_DONE
if "%LANG_CHOICE%"=="9" set LANG_CODE=de & set LANG_NAME=Deutsch & goto LANG_DONE
echo Invalid choice, please enter 1-9.
goto LANG_SELECT

:LANG_DONE
echo.
echo Selected: %LANG_NAME%
echo.

:: 检测 gcc
echo [1/4] Checking for gcc...
where gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo.
    echo [!] gcc not found.
    echo     Please install MinGW to get gcc:
    echo     https://www.mingw-w64.org/downloads/
    echo.
    echo     Steps:
    echo     1. Download and install MinGW
    echo     2. Add MinGW bin folder to PATH
    echo        e.g. C:\mingw64\bin
    echo     3. Re-run this installer
    echo.
    pause
    exit /b 1
)
echo     gcc found.

:: 检测 make
echo [2/4] Checking for make...
where make >nul 2>&1
if %errorlevel% neq 0 (
    where mingw32-make >nul 2>&1
    if !errorlevel! neq 0 (
        echo.
        echo [!] make not found.
        echo     Please install make via MinGW or use:
        echo     https://gnuwin32.sourceforge.net/packages/make.htm
        echo.
        pause
        exit /b 1
    ) else (
        echo     mingw32-make found, using as make.
        doskey make=mingw32-make $*
    )
) else (
    echo     make found.
)

:: 询问安装路径
echo.
echo [3/4] Installation path
echo     Default: C:\p-adic
echo.
set /p PATH_CHOICE="Use default path? (Y/N): "
if /i "%PATH_CHOICE%"=="Y" (
    set INSTALL_DIR=C:\p-adic
) else (
    set /p INSTALL_DIR="Enter installation path: "
)

echo.
echo     Installing to: %INSTALL_DIR%

:: 创建目录
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"
if %errorlevel% neq 0 (
    echo [!] Failed to create directory: %INSTALL_DIR%
    echo     Try running as Administrator.
    pause
    exit /b 1
)

:: 编译
echo.
echo [4/4] Compiling...
make clean >nul 2>&1
make
if %errorlevel% neq 0 (
    echo.
    echo [!] Compilation failed.
    pause
    exit /b 1
)
echo     Compilation successful.

:: 复制文件
copy /Y padic_converter.exe "%INSTALL_DIR%\padic_converter.exe" >nul 2>&1
if %errorlevel% neq 0 (
    :: 如果没有 .exe 后缀，尝试复制无后缀版本
    copy /Y padic_converter "%INSTALL_DIR%\padic_converter.exe" >nul 2>&1
)

:: 创建 smc-padic.bat 启动脚本
echo @echo off > "%INSTALL_DIR%\smc-padic.bat"
echo "%INSTALL_DIR%\padic_converter.exe" >> "%INSTALL_DIR%\smc-padic.bat"

:: 写入语言配置
echo LANG=%LANG_CODE%> "%USERPROFILE%\.padic_config"
echo     Language config saved.

:: 添加到 PATH
echo.
echo Adding to system PATH...
for /f "tokens=2*" %%A in ('reg query "HKCU\Environment" /v PATH 2^>nul') do set CURRENT_PATH=%%B
echo !CURRENT_PATH! | findstr /i "%INSTALL_DIR%" >nul
if %errorlevel% neq 0 (
    setx PATH "!CURRENT_PATH!;%INSTALL_DIR%" >nul
    echo     PATH updated. Please restart your terminal.
) else (
    echo     Already in PATH.
)

echo.
echo ============================================
echo  Installation complete!
echo  Run: smc-padic
echo  (Restart terminal first if PATH was updated)
echo ============================================
echo.
pause