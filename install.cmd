@Echo off

Setlocal EnableDelayedExpansion
Setlocal EnableExtensions

Color 0B

Rem Clear the screen and write a header.
Cls
Echo *****************************************************************************
Echo *                                                                           * 
Echo *               Summoning Wars installation script                          *
Echo *It suggests that you have already built sumwars.exe Release executable file*
Echo *            It doesn't support DEBUG version of the game                   *
Echo *****************************************************************************

Rem Get the confirmation to copy the data to current folder.
Set /p install= Do you really want to install SumWars in current directory? [ Y / N ] 
If /i not "%install%" == "Y" (
  Echo Ok, I will not do this.
  Echo If you want to install the game into another folder,
  Echo copy this script where you want and run it again from there.
  GoTo end
) Else (
  Echo Let's do it
)

If not Exist "plugins_win.cfg" (
  Call :AskSVN
) Else ( 
  Call :check
)

Echo Copy ALUT

If Exist "%ALUTDIR%\." (
  Copy "%ALUTDIR%\lib\alut.dll" alut.dll
) Else (
  Echo Cannot find env var [ALUTDIR]
  Echo Please define it so that the file "[ALUTDIR]\lib\alut.dll" can be found. 
)

Echo Copy CEGUI

If Exist "%CEGUIDIR%\." (
  Copy "%CEGUIDIR%\bin\CEGUIBase.dll" CEGUIBase.dll
  Copy "%CEGUIDIR%\bin\CEGUIExpatParser.dll" CEGUIExpatParser.dll
  Copy "%CEGUIDIR%\bin\CEGUIFalagardWRBase.dll" CEGUIFalagardWRBase.dll
  Copy "%CEGUIDIR%\bin\CEGUILuaScriptModule.dll" CEGUILuaScriptModule.dll
  Copy "%CEGUIDIR%\bin\CEGUIOgreRenderer.dll" CEGUIOgreRenderer.dll
  Copy "%CEGUIDIR%\bin\CEGUISILLYImageCodec.dll" CEGUISILLYImageCodec.dll
) Else (
  Echo Can't find CEGUI folder, ensure that environmant variable is set correctly
  Color 0C
  Call :fail
  GoTo end
)

Echo OGRE libraries

If Exist "%OGRE_HOME%" (
  Rem Trim trailing backslash from the name
  If "%OGRE_HOME:~-1%"=="\" (
    Set OGRE_HOME=%OGRE_HOME:~0,-1%
  )

  Copy "!OGRE_HOME!\bin\release\cg.dll" cg.dll
  Copy "!OGRE_HOME!\bin\release\OgreMain.dll" OgreMain.dll
  Copy "!OGRE_HOME!\bin\release\OgrePaging.dll" OgrePaging.dll
  Copy "!OGRE_HOME!\bin\release\OgreRTShaderSystem.dll" OgreRTShaderSystem.dll
  Copy "!OGRE_HOME!\bin\release\OgreTerrain.dll" OgreTerrain.dll
  Copy "!OGRE_HOME!\bin\release\OIS.dll" OIS.dll
  Copy "!OGRE_HOME!\bin\release\Plugin_CgProgramManager.dll" Plugin_CgProgramManager.dll
  Copy "!OGRE_HOME!\bin\release\Plugin_OctreeSceneManager.dll" Plugin_OctreeSceneManager.dll
  Copy "!OGRE_HOME!\bin\release\Plugin_OctreeZone.dll" Plugin_OctreeZone.dll
  Copy "!OGRE_HOME!\bin\release\Plugin_ParticleFX.dll" Plugin_ParticleFX.dll
  Copy "!OGRE_HOME!\bin\release\Plugin_PCZSceneManager.dll" Plugin_PCZSceneManager.dll
  Copy "!OGRE_HOME!\bin\release\RenderSystem_Direct3D9.dll" RenderSystem_Direct3D9.dll
  Copy "!OGRE_HOME!\bin\release\RenderSystem_GL.dll" RenderSystem_GL.dll
) Else (
  Echo Can't find OGRE folder, please ensure that the environment variable [OGRE_HOME] is set correctly ^!
  Echo Please define it so that the file "[OGRE_HOME]\bin\release\OgreMain.dll" can be found. 
  Color 0C
  Call :fail
  GoTo end
)

Echo Copy PhysFS

If Exist "%PHYSFSDIR%" (
  If Exist "%PHYSFSDIR%\lib\." (
    Copy "%PHYSFSDIR%\lib\physfs.dll" physfs.dll
  )
  
  If Exist "%PHYSFSDIR%\bin\." (
    Copy "%PHYSFSDIR%\bin\physfs.dll" physfs.dll
  )
  
  If Exist "%PHYSFSDIR%\Release\." (
    Copy "%PHYSFSDIR%\Release\physfs.dll" physfs.dll
  )
) Else (
  Echo Cannot find [PHYSFSDIR] env var.
  Echo Please define it so that the file "[PHYSFSDIR]\Release\physfs.dll" can be found.
  Echo Or "[PHYSFSDIR]\lib\physfs.dll" can be found; OR "[PHYSFSDIR]\bin\physfs.dll" can be found.
)

If Not Exist physfs.dll (
  Echo Cannot find PHYSFS folder, please ensure that environment variable is set correctly
  Color 0C
  Call :fail
  GoTo end
)

Echo Copy POCO

If Exist "%POCO_HOME%\." (
  Copy "%POCO_HOME%\bin\PocoFoundation.dll" PocoFoundation.dll
) Else (
  Echo Can't find POCO folder, please ensure that the environment variable [POCO_HOME] is set correctly
  Echo Please define it so that the file "[POCO_HOME]\bin\PocoFoundation.dll" can be found.
  Color 0C
  Call :fail
  GoTo end
)

Echo Copy Lua

If Exist "%LUA_DEV%\." (
  Rem Copy lua5.1.dll to lua51.dll because lua51.dll is only wrapper and we need full dll.
  Copy "%LUA_DEV%\lua5.1.dll" lua51.dll
) Else (
  Echo Can't find LUA folder, please ensure that the environment variable [LUA_DEV] is set correctly
  Echo Please define it so that the file "[LUA_DEV]\lua5.1.dll" can be found.
  Color 0C
  Call :fail
  GoTo end
)

Echo Copy Ogg

if EXIST "%OGGDIR%\Win32\Release\libogg.dll" (
  copy "%OGGDIR%\Win32\Release\libogg.dll" libogg.dll
)
if EXIST "%OGGDIR%\Release\libogg.dll" (
  copy "%OGGDIR%\Release\libogg.dll" libogg.dll
)
if EXIST "%OGGDIR%\libogg.dll" (
  copy "%OGGDIR%\libogg.dll" libogg.dll
)
if not EXIST "%OGGDIR%\." (
  Echo Can't find OGG folder, please ensure that the environment variable [OGGDIR] is set correctly
  Echo Please define it so that the file "[OGGDIR]\Win32\Release\libogg.dll" can be found.
  Echo OR "[OGGDIR]\Release\libogg.dll" can be found.
  Echo OR "[OGGDIR]\libogg.dll" can be found.
  Color 0C
  Call :fail
  GoTo end
)

Echo Copy Vorbis

If Exist "%VORBISDIR%\Win32\Release\libvorbis.dll" (
Copy "%VORBISDIR%\Win32\Release\libvorbis.dll" libvorbis.dll
Copy "%VORBISDIR%\Win32\Release\libvorbisfile.dll" libvorbisfile.dll
)
If Exist "%VORBISDIR%\Release\libvorbis.dll" (
Copy "%VORBISDIR%\Release\libvorbis.dll" libvorbis.dll
Copy "%VORBISDIR%\Release\libvorbisfile.dll" libvorbisfile.dll
)
If Exist "%VORBISDIR%\libvorbis.dll" (
Copy "%VORBISDIR%\libvorbis.dll" libvorbis.dll
Copy "%VORBISDIR%\libvorbisfile.dll" libvorbisfile.dll
)
If not Exist "%VORBISDIR%\." (
Echo Can't find Vorbis folder, enshure that environmant variable is set correctly
Color 0C
Call :fail
GoTo end
)

Echo Copy GetText libraries

If Exist "%GNUWINDIR%\." (
  Set _myFile=libgettextlib.dll
  If Exist "%GNUWINDIR%\bin\!_myFile!" Copy "%GNUWINDIR%\bin\!_myFile!" !_myFile!
  Set _myFile=libgettextpo.dll
  If Exist "%GNUWINDIR%\bin\!_myFile!" Copy "%GNUWINDIR%\bin\!_myFile!" !_myFile!
  Set _myFile=libgettextsrc.dll
  If Exist "%GNUWINDIR%\bin\!_myFile!" Copy "%GNUWINDIR%\bin\!_myFile!" !_myFile!
  Set _myFile=libiconv2.dll
  If Exist "%GNUWINDIR%\bin\!_myFile!" Copy "%GNUWINDIR%\bin\!_myFile!" !_myFile!
  Set _myFile=libintl3.dll
  If Exist "%GNUWINDIR%\bin\!_myFile!" Copy "%GNUWINDIR%\bin\!_myFile!" !_myFile!
)
If Exist "%programfiles%\GnuWin32\bin\." (
  Set _myFile=libgettextlib.dll
  If Exist "%programfiles%\GnuWin32\bin\!_myFile!" Copy "%programfiles%\GnuWin32\bin\!_myFile!" !_myFile!
  Set _myFile=libgettextpo.dll
  If Exist "%programfiles%\GnuWin32\bin\!_myFile!" Copy "%programfiles%\GnuWin32\bin\!_myFile!" !_myFile!
  Set _myFile=libgettextsrc.dll
  If Exist "%programfiles%\GnuWin32\bin\!_myFile!" Copy "%programfiles%\GnuWin32\bin\!_myFile!" !_myFile!
  Set _myFile=libiconv2.dll
  If Exist "%programfiles%\GnuWin32\bin\!_myFile!" Copy "%programfiles%\GnuWin32\bin\!_myFile!" !_myFile!
  Set _myFile=libintl3.dll
  If Exist "%programfiles%\GnuWin32\bin\!_myFile!" Copy "%programfiles%\GnuWin32\bin\!_myFile!" !_myFile!
)
If not Exist libgettextlib.dll (
  Echo Can't find GetText folder, enshure that environmant variable is set correctly
  Color 0C
  Call :fail
  GoTo end
  )
)

If ERRORLEVEL==1 (
  Color 0C
  Call :fail
) Else (
  Color 0A
  GoTo end
)

GoTo end

:check
Echo Is this your SumWars source folder?
Echo ^(Answer [No] if you are just installing the game to a second location^)
Set /p stry_svn=Your answer: [ Y / N ] 
   
If /i "%stry_svn%" == "Y" (
  Echo Copy sumwars.exe

  If Exist "Release\sumwars.exe" ( Copy "Release\sumwars.exe" sumwars.exe
  ) Else (
    Echo Looks like you didn't build sumwars.exe ^(Release^)
    GoTo end
  )
) Else (
  Call :AskSVN
)
GoTo :eof


:AskSVN
Echo Please provide where is your SumWars source folder
Set /p svn_repo= Don't add \ at the end of path 
If not Exist "%svn_repo%\plugins_win.cfg" (

  Echo You have choosen wrong directory
  Call :AskSVN
) Else (
   Call :AskDiff
   )
GoTo :eof


:AskDiff
 Set /p buildsame= Is your build folder the same as source folder? [ Y / N ]
 
     If /i not "%buildsame%" == "Y" ( Call :AskBuild
     ) Else (
	 set build=%svn_repo%
     Call :copyexe
	 Call :notSVNDir
     )
GoTo :eof

:AskBuild
Echo Please provide your SumWars build folder
Set /p build= Don't add \ at the end of path 
If not Exist "%build%\Release\sumwars.exe" (

Echo You have choosen wrong directory
Call :AskBuild
) Else (
Call :copyexe
)

Call :notSVNDir
GoTo :eof

:notSVNDir
Echo Copy misc files

Copy "%svn_repo%\Authors" Authors
Copy "%svn_repo%\authors.txt" authors.txt
Copy "%svn_repo%\gpl-3.0.txt" gpl-3.0.txt
Copy "%svn_repo%\Copying" Copying
Copy "%svn_repo%\Install" Install
Copy "%svn_repo%\Readme" Readme
Copy "%svn_repo%\resources.cfg" resources.cfg
Copy "%svn_repo%\plugins_win.cfg" plugins_win.cfg
xCopy "%svn_repo%\data\*" data /s /i
xCopy "%svn_repo%\resources\*" resources /s /i
xCopy "%svn_repo%\translation\*" translation /s /i
GoTo :eof

:copyexe
If Exist "%build%\Release\sumwars.exe" (
  Echo Copy sumwars.exe
  Copy "%build%\Release\sumwars.exe" sumwars.exe
) Else (
    Echo Looks like you didn't compile sumwars.exe
    Color 0C
    Call :fail
    GoTo end
)
GoTo :eof

:fail
Echo Some files were not copied, sorry. Most likely game will not work
GoTo :eof


:end
Echo Please delete "Plugin=Plugin_BSPSceneManager" line from plugins_win.cfg
Echo and change PluginFolder=lib to PluginFolder=.

Echo That's all

pause