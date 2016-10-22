@ECHO off
CALL "%VS120COMNTOOLS%vsvars32.bat"
CD "%~dp0vs2013"
CALL "%VS120COMNTOOLS%..\IDE\devenv.exe" vs2013.sln
EXIT