@echo off

Rem 中心服务器
start .\CenterServer.exe   .\centerserver.ini

Rem 等待3秒 ping本机3次不使用默认的次数,nul是不显示ping信息:
@ping -n 3 127.0.0.1>nul

rem 登录服务器
start .\LogonServer.exe   .\logonserver.ini
@ping -n 3 127.0.0.1>nul

start .\ServiceLoader.exe   .\gameserver.ini
@ping -n 3 127.0.0.1>nul

echo 启动完成
pause


