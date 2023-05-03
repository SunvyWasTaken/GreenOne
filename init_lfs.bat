@echo off
SETLOCAL EnableDelayedExpansion
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
  set "DEL=%%a"
)
echo.
set line="#" 
for /l %%x in (1, 1, 20) do ( 
	set line="#" 
	call :ColorText 05 "%line%"
)
echo.
call :ColorText 0a  " #     ####  ####"
echo.
call :ColorText 0a  " #     #     #   "
echo.
call :ColorText 0a  " #     ###   ####"
echo.
call :ColorText 0a  " #     #        #"
echo.
call :ColorText 0a  " ####  #     ####"
echo.
set line="#" 
for /l %%x in (1, 1, 20) do ( 
	set line="#" 
	call :ColorText 05 "%line%"
)
timeout 3
call :ColorText 0b "Init lfs" 
echo.
git lfs install
echo.
call :ColorText 0b "Pull lfs files"
echo.
git lfs push --all origin
echo.
call :ColorText 06 "That good ;)" 
echo.
pause
goto :eof

:ColorText
echo off
<nul set /p ".=%DEL%" > "%~2"
findstr /v /a:%1 /R "^$" "%~2" nul
del "%~2" > nul 2>&1
goto :eof
