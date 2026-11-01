@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Build-Windows.ps1" %*
if errorlevel 1 pause
