@echo off
taskkill /f /im shyloo.exe

pause

cd /d %~dp0
start "slave --name=slave --node_id=1" "%~dp0shyloo.exe" --name=slave --node_id=1
start "master --name=master --harbor=7700 --node_id=1 --agent=12580" "%~dp0shyloo.exe" --name=master --harbor=7700 --node_id=1 --agent=12580
