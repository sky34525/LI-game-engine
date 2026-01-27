@echo off
vendor\bin\premake5\premake5.exe vs2026
IF %ERRORLEVEL% NEQ 0 (
  PAUSE
)