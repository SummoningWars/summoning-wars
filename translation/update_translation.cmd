@echo off
rem updates *.po using template *.pot files

rem New *.po files (resulting from new domains) need to be added by hand.

If EXIST "%programfiles%\GnuWin32\bin\msgmerge.exe" (
PATH=%PATH%;%programfiles%\GnuWin32\bin
) ELSE (

CALL :setpath

)

FOR /f "tokens=*" %%a in (domains) do (

set LINE=%%a
CALL :update
)

echo ... Finished

goto end

:setpath
color 0C
echo Can't find msgmerge .exe
set /p gettext= Please specify your GetText bin folder 

set PATH=%PATH%;%gettext%

color 07
goto :eof

:update

set LANGUAGE=de
echo +++ %LINE% - GERMAN +++
echo [ PO ]
msgmerge -v -U %LANGUAGE%\%LINE%.po en\%LINE%.pot

if ERRORLEVEL==1 COLOR 0C

set LANGUAGE=it
echo +++ %LINE% - ITALIAN +++
echo [ PO ]
msgmerge -v -U %LANGUAGE%\%LINE%.po en\%LINE%.pot

if ERRORLEVEL==1 COLOR 0C

set LANGUAGE=ru
echo +++ %LINE% - RUSSIAN +++
echo [ PO ]
msgmerge -v -U %LANGUAGE%\%LINE%.po en\%LINE%.pot

if ERRORLEVEL==1 COLOR 0C

set LANGUAGE=pt
echo +++ %LINE% - PORTUGUESE +++
echo [ PO ]
msgmerge -v -U %LANGUAGE%\%LINE%.po en\%LINE%.pot

if ERRORLEVEL==1 COLOR 0C

set LANGUAGE=uk
echo +++ %LINE% - UKRAINIAN +++
echo [ PO ]
msgmerge -v -U %LANGUAGE%\%LINE%.po en\%LINE%.pot

if ERRORLEVEL==1 COLOR 0C

goto :eof

:end

pause
