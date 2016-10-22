@ECHO off
CALL "%VS110COMNTOOLS%vsvars32.bat"
CD "%~dp0vs2012"
CALL "%VS110COMNTOOLS%..\IDE\devenv.exe" vs2012.sln
EXIT