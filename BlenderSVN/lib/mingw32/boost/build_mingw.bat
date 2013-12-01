@echo off

:: build
call bootstrap.bat
bjam -j4 --toolset=gcc --build-type=complete --variant=release --link=static --runtime-link=static --threading=multi --with-filesystem --with-thread --with-regex --with-system --with-date_time stage
bjam -j4 --toolset=gcc --build-type=complete --variant=debug --link=static --runtime-link=static --threading=multi --with-filesystem --with-thread --with-regex --with-system --with-date_time stage

:: copy files
xcopy /E /Y stage\lib  ..\boost\lib
xcopy /E /Y boost ..\boost\include\boost


