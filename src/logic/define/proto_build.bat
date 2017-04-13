@echo off
cd /d %~dp0
FOR /R ./ %%F IN (*.proto) DO .\\ProtoGen\\protoc --proto_path=%~dp0 --cpp_out=. "%%F"
FOR /R ./ %%F IN (*.proto) DO .\\ProtoGen\\protogen.exe -i:%%F -o:%%F.cs -p:detectMissing