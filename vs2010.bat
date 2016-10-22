@ECHO off
CALL "%VS100COMNTOOLS%vsvars32.bat"
CD "%~dp0vs2010"
CALL "%VS100COMNTOOLS%..\IDE\devenv.exe" vs2010.sln
EXIT