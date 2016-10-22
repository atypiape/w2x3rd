@ECHO off
CALL "%VS140COMNTOOLS%vsvars32.bat"
CD "%~dp0vs2015"
CALL "%VS140COMNTOOLS%..\IDE\devenv.exe" vs2015.sln
EXIT