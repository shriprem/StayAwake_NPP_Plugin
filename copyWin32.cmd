@ECHO OFF

SET PLUGIN_NAME=StayAwake_NPP
SET BIN_DIR=bin\
SET NOTEPAD_DIR=C:\Portable\Notepad++ - x86\
SET PLUGINS_DIR=%NOTEPAD_DIR%\plugins\%PLUGIN_NAME%\

COPY LICENSE %BIN_DIR%
COPY %BIN_DIR%\%PLUGIN_NAME%.dll "%PLUGINS_DIR%"

START /D "%NOTEPAD_DIR%" notepa~1.exe
EXIT 0
