@set PATH=C:\msys64\mingw64\bin;C:\msys64\usr\local\bin;C:\msys64\usr\bin;C:\msys64\bin;%PATH%
set MSYSTEM=MINGW64

cd /d %~dp0

for %%i in (
    win_console_input win_console_output
) do (
    gcc -g -O2 -Wall -Wextra -o %%i.exe %%i.c
)

pause
