@rem @set PATH=C:\msys64\mingw64\bin;C:\msys64\usr\local\bin;C:\msys64\usr\bin;C:\msys64\bin;%PATH%
@rem set MSYSTEM=MINGW64
set PROG1=win_console_output

cd /d %~dp0

@rem gdb %PROG1%.exe

@rem %PROG1%.exe

wt.exe %~dp0%PROG1%.exe

pause
