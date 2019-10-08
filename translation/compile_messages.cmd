@echo off
rem compiles *.po to *.mo binary without updating message-strings

rem New *.po files (resulting from new domains) need to be added by hand.

If EXIST "%programfiles%\GnuWin32\bin\msgfmt.exe" (
PATH=%PATH%;%programfiles%\GnuWin32\bin
) ELSE (

CALL :setpath

)

FOR /f "tokens=*" %%a in (domains) do (

set LINE=%%a
CALL :compile
)

echo ... Finished

goto end

:setpath
color 0C
echo Can't find msgfmt.exe
set /p gettext= Please specify your GetText bin folder 

set PATH=%PATH%;%gettext%

color 07
goto :eof

:compile

set LANGUAGE=de
echo +++ %LINE% - GERMAN +++
echo [ MO ]
msgfmt -c -v -o %LANGUAGE%\LC_MESSAGES\%LINE%.mo %LANGUAGE%\%LINE%.po

if ERRORLEVEL==1 COLOR 0C

set LANGUAGE=it
echo +++ %LINE% - ITALIAN +++
echo [ MO ]
msgfmt -c -v -o %LANGUAGE%\LC_MESSAGES\%LINE%.mo %LANGUAGE%\%LINE%.po

if ERRORLEVEL==1 COLOR 0C

set LANGUAGE=ru
echo +++ %LINE% - RUSSIAN +++
echo [ MO ]
msgfmt -c -v -o %LANGUAGE%\LC_MESSAGES\%LINE%.mo %LANGUAGE%\%LINE%.po

if ERRORLEVEL==1 COLOR 0C

set LANGUAGE=pt
echo +++ %LINE% - PORTUGUESE +++
echo [ MO ]
msgfmt -c -v -o %LANGUAGE%\LC_MESSAGES\%LINE%.mo %LANGUAGE%\%LINE%.po

if ERRORLEVEL==1 COLOR 0C

set LANGUAGE=uk
echo +++ %LINE% - UKRAINIAN +++
echo [ MO ]
msgfmt -c -v -o %LANGUAGE%\LC_MESSAGES\%LINE%.mo %LANGUAGE%\%LINE%.po

if ERRORLEVEL==1 COLOR 0C

goto :eof

:end

pause
