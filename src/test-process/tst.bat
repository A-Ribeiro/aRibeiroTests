@echo off
echo begin
ping 192.0.2.0 -n 1 -w 5000 > nul
echo %1%
ping 192.0.2.0 -n 1 -w 5000 > nul
echo end
