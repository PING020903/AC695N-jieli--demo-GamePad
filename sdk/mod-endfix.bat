@echo off
for /r %%a in (*.ex_) do ren "%%a" "%%~na.exe"

for /r %%a in (*.ba_) do ren "%%a" "%%~na.bat"