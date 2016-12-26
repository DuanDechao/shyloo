@echo off
taskkill /f /im shyloo.exe

pause

cd /d %~dp0
start "slave --name=slave --node_id=1" "%~dp0shyloo.exe" --name=slave --node_id=1