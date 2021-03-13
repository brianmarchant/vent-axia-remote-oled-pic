C:
cd "C:\Program Files (x86)\Microchip\MPLABX\v5.40\mplab_platform\mplab_ipe"
set dev=%1
rem strip off the first three characters
set dev=%dev:~3%
PK3CMD.exe /I /L /MP /MC /V5 /Y /P%dev% /F%2 /V5.000
IF errorlevel 1 PAUSE
rem Add /V5.000 to the line above to apply 5v from the programmer.