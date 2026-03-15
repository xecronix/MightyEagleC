@echo off
cd /d "%~dp0"
del /q output\*.obj 2>nul
del /q MightyEagleLib32.lib 2>nul
del /q MightyEagleTest32.exe 2>nul
del /q MightyEagleDemo32.exe 2>nul