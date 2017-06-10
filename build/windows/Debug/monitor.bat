@echo off
pause

cd /d %~dp0
start "monitor --name=monitor --node_id=1 --server_ip=127.0.0.1 --server_ip=12580 --func=0" "%~dp0shyloo.exe" --name=monitor --node_id=1 --server_ip=127.0.0.1 --server_ip=12580 --func=0