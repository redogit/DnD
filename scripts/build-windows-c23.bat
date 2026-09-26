@echo off
setlocal
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build-windows-c23.ps1" %*
exit /b %ERRORLEVEL%
