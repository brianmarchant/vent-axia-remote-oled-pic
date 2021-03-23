C:
cd "C:\Program Files (x86)\Microchip\MPLABX\v5.40\mplab_platform\mplab_ipe"
set dev=%1
rem strip off the first three characters
set dev=%dev:~3%
set hexf=%2
rem change .hex to .ihex
set hexf=%hexf:.hex=.ihex%
rem PK3CMD.exe /F%hexf% /ME /P%dev% /V5.000
PK3CMD.exe /P%dev% /GF"TempFile.hex" /R%hexf% /V5.000
rem Add /V5.000 to the line above to apply 5v from the programmer.
rem pause