@echo off
title Space Battle Game Launcher
color 0A

:beginning
echo.
echo  ╔══════════════════════════════════════════════════════════════╗
echo  ║                    🚀 SPACE BATTLE GAME 🚀                      ║
echo  ║                                                              ║
echo  ║  Choose your gaming platform:                                  ║
echo  ║                                                              ║
echo  ║  1. 🖥️  Desktop Version (C++ - High Performance)              ║
echo  ║  2. 🌐  Web Browser Version (Play Online)                     ║
echo  ║  3. 📖  View Documentation                                    ║
echo  ║  4. ❌  Exit                                                   ║
echo  ║                                                              ║
echo  ╚══════════════════════════════════════════════════════════════╝
echo.
set /p choice="Enter your choice (1-4): "

if "%choice%"=="1" goto desktop
if "%choice%"=="2" goto web
if "%choice%"=="3" goto docs
if "%choice%"=="4" goto exit
goto invalid

:desktop
echo.
echo 🎮 Launching Desktop Version...
echo.
if exist SpaceBattle.exe (
    start SpaceBattle.exe
    echo ✅ Desktop game launched!
) else (
    echo ❌ SpaceBattle.exe not found. Compiling...
    call compile.bat
    if exist SpaceBattle.exe (
        start SpaceBattle.exe
        echo ✅ Desktop game compiled and launched!
    ) else (
        echo ❌ Compilation failed!
    )
)
echo.
pause
goto beginning

:web
echo.
echo 🌐 Launching Web Version...
echo.
if exist web\index.html (
    start web\index.html
    echo ✅ Web game opened in browser!
) else (
    echo ❌ Web version not found!
)
echo.
pause
goto beginning

:docs
echo.
echo 📖 Opening Documentation...
echo.
if exist GAME_PACKAGE.md (
    start GAME_PACKAGE.md
    echo ✅ Documentation opened!
) else (
    echo ❌ Documentation not found!
)
echo.
pause
goto beginning

:invalid
echo.
echo ❌ Invalid choice! Please select 1-4.
echo.
pause
goto beginning

:exit
echo.
echo 🚀 Thanks for playing Space Battle Game!
echo.
timeout /t 2 >nul
exit
