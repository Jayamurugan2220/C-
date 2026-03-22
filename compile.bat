@echo off
echo Compiling Space Battle Game...

REM Setup Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

REM Compile with SFML 2.6.1
set SFML_PATH=C:\SFML-2.6.1-windows-vc17-64-bit\SFML-2.6.1

echo.
echo Compiling source files...
cl /EHsc /I"%SFML_PATH%\include" /I"include" ^
   main.cpp ^
   src/Game.cpp ^
   src/Player.cpp ^
   src/Enemy.cpp ^
   src/Bullet.cpp ^
   src/Collision.cpp ^
   src/PowerUp.cpp ^
   /link /LIBPATH:"%SFML_PATH%\lib" ^
   sfml-graphics.lib ^
   sfml-window.lib ^
   sfml-system.lib ^
   /out:SpaceBattle.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ✅ Compilation successful!
    echo 🚀 Run SpaceBattle.exe to play the game!
    echo.
    
    echo Copying SFML DLLs...
    copy "%SFML_PATH%\bin\sfml-graphics-2.dll" . >nul 2>&1
    copy "%SFML_PATH%\bin\sfml-window-2.dll" . >nul 2>&1
    copy "%SFML_PATH%\bin\sfml-system-2.dll" . >nul 2>&1
    
    echo 🎮 Game Controls:
    echo   Arrow Keys - Move spaceship
    echo   Left/Right CTRL - Shoot bullets
    echo   Space - Also shoot bullets
    echo   ESC - Exit game
    echo.
    echo ✅ GAME READY!
) else (
    echo.
    echo ❌ Compilation failed!
)

pause
