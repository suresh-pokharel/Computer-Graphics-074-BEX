@echo on
set includes=/I ./include/ 
set srcFiles=./src/main.cpp ./src/common.cpp
set libFiles=glu32.lib opengl32.lib gdi32.lib kernel32.lib user32.lib 
set binDir=./bin
set optimize=/Zi
IF [%1]==[] (
  set optimize=/O2
)
@echo on
call cl %srcFiles% %includes% %libFiles% %optimize% /Fe%binDir%/app.exe /EHsc /link /SUBSYSTEM:CONSOLE
@echo off
del *.obj
@echo on
@echo Compilation Successfull


