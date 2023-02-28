@echo off
rem Creating CMake files
CD ..\..\
call thirdparty\premake\premake5.exe gmake
pause