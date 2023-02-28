@echo off
rem Creating Visual studio 2022 project files via premake 
CD ..\..\
call thirdparty\premake\premake5.exe vs2022
pause