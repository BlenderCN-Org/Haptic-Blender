REM To generate the python libs for MinGW, the libraries from the lib windows folder were used. You need gendef to generate the def files for MinGW

cd c:\blenderdev\lib\windows\python\lib
c:\MinGW\bin\dlltool.exe --dllname python33_d.dll --input-def python33.def --output-lib python33mw_d.lib
c:\MinGW\bin\dlltool.exe --dllname python33.dll --input-def python33.def --output-lib python33mw.lib
@ECHO OFF
SET USRINPUT=

