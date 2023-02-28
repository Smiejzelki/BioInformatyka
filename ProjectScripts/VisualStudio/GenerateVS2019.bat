@echo off
rem Creating Visual studio 2019 project files via premake 
CD ..\..\
call thirdparty\premake\premake5.exe vs2019
pause