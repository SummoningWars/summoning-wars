/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "application.h"
#include "config.h"
#include "tooltipmanager.h"
#include "itemwindow.h"
#include "templateloader.h"
#include "music.h"
#include "sumwarshelper.h"

#ifdef BUILD_TOOLS
#include "benchmarktab.h"
#include "debugpanel.h"
#include "iconeditortab.h"
#include "guidebugtab.h"
#include "debugcameratab.h"
#include "luascripttab.h"
#include "textfileeditwindow.h"
//#include "reloadtab.h"
//content editor
#include "contenteditor.h"
#include "guiTabs.h"
#endif
//#include "CEGUI/ScriptingModules/LuaScriptModule/CEGUILua.h"

#include "OgreConfigFile.h"
#include <OgreParticleSystemManager.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "../winicon/resource.h"
#endif

// Add the material manager to allow setting shadow techniques
#include <OgreMaterialManager.h>

// Clipboard singleton created here.
#include "clipboard.h"


/**
	Application constructor. Will call the init function.
*/
Application::Application(char *argv)
{
	// Initialize application variables
	m_main_window = 0;
	m_document = 0;
	bool ret = false;
	m_running = false;
	m_shutdown = false;

	// Call the specialized initialization function.
	try
	{
		ret = init(argv);
	}
	catch (std::exception &e)
	{
		ERRORMSG("Error message: %s",e.what());
	}

	if (ret == false)
	{
		// ERRORMSG("Initialisation failed, stop");
		m_shutdown = true;
	}
}



bool Application::init(char *argv)
{
	// Initialise the PHYSFS library
	if (PHYSFS_init (argv) == 0)
	{
		printf("PHYSFS_init failed: %s\n", PHYSFS_getLastError ());
		return false;
	}

	// Create the sumwars helper singleton
	new SumwarsHelper ();

	// Before adding the user directory to the path, keep in mind that for portable installs, the exe folder should
	// be added to the path, NOT the user folder.

	if (SumwarsHelper::getSingletonPtr ()->hasFeature ("allows-preload"))
	{
		Ogre::ConfigFile cf;
		std::string preloadFileName (SumwarsHelper::getSingletonPtr ()->getPreloadFileName ());
		try
		{
			cf.load (preloadFileName.c_str ());

			if (SumwarsHelper::getSingletonPtr ()->hasFeature ("portable-mode"))
			{
				Ogre::String portableOption = cf.getSetting ("portable", "", "");
				bool isPortable = false;
				if (portableOption == "true" || portableOption == "True" || portableOption == "TRUE" || portableOption == "1")
				{
					isPortable = true;
				}

				SumwarsHelper::getSingletonPtr ()->setPortable (isPortable);
			}
		}
		catch (std::exception&)
		{
			// Leave it untreated. It's because we're missing the OPTIONAL file for pre-load.
		}
	}

	std::string storagePath (SumwarsHelper::getSingletonPtr ()->getStorageBasePath ());

	// Add the user directory
	if (PHYSFS_addToSearchPath 	 (storagePath.c_str (), 1) == 0)
	{
		printf("PHYSFS_addToSearchPath 	 failed: %s\n", PHYSFS_getLastError ());
		return false;
	}

	// Set the user directory as the default location to write to.
	if (PHYSFS_setWriteDir (storagePath.c_str ()) == 0)
	{
		printf("PHYSFS_setWriteDir failed: %s\n", PHYSFS_getLastError ());
		return false;
	}

#ifdef __APPLE__
    if (!PHYSFS_exists("Library/Application Support/Sumwars"))
    {
        if (PHYSFS_mkdir("Library/Application Support/Sumwars") == 0)
        {
            printf("mkdir failed: %s\n", PHYSFS_getLastError());
            return false;
        }
        if (PHYSFS_mkdir("Library/Application Support/Sumwars/save") == 0)
        {
            printf("mkdir failed: %s\n", PHYSFS_getLastError());
            return false;
        }
    }
    if (!PHYSFS_exists("Library/Application Support/Sumwars/save"))
    {
        if (PHYSFS_mkdir("Library/Application Support/Sumwars/save") == 0)
        {
            printf("mkdir failed: %s\n", PHYSFS_getLastError());
            return false;
        }
    }
#elif _WIN32

	if (!PHYSFS_exists(".sumwars"))
	{
		if (PHYSFS_mkdir(".sumwars") == 0)
		{
			printf("mkdir failed: %s\n", PHYSFS_getLastError());
			return false;
		}
	}

	if (!PHYSFS_exists(".sumwars/save"))
	{
		if (PHYSFS_mkdir(".sumwars/save") == 0)
		{
			printf("mkdir failed: %s\n", PHYSFS_getLastError());
			return false;
		}
	}

	if(!PHYSFS_exists(".sumwars/plugins.cfg"))
	{
		TCHAR szPath[MAX_PATH];

		if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
		{
			printf("Cannot retrieve module path (%d)\n", GetLastError());
			return false;
		}

		std::string path(szPath);
		path.erase(path.size() - 11, path.size());

		std::string pluginFile (".sumwars/plugins.cfg");

		PHYSFS_file* pluginsFile = PHYSFS_openWrite(pluginFile.c_str ());
		if (0 != pluginsFile)
		{
			std::string str = "# Defines plugins to load\n"
								"# Define plugin folder\n"
								"PluginFolder=";
			str.append(path).append("\n\n");
			str.append(		  "# Define plugins\n"
								"Plugin=RenderSystem_Direct3D9\n"
								"Plugin=RenderSystem_GL\n"
								"Plugin=Plugin_ParticleFX\n"
								"Plugin=Plugin_OctreeSceneManager\n"
								"Plugin=Plugin_CgProgramManager\n");

			PHYSFS_write(pluginsFile, str.c_str(), sizeof(char), str.size());
			PHYSFS_close(pluginsFile);
		}
		else
		{
			printf("Could not write file %s; error : %s\n", pluginFile.c_str (), PHYSFS_getLastError());
		}
	}



#elif defined (__unix__)
	// creating 'save' directory
	std::string saveDir = "/.sumwars/save";
	if (! PHYSFS_exists(saveDir.c_str()))
	{
		printf("Save dir doesn't exist, making it: %s%s\n", PHYSFS_getWriteDir(), saveDir.c_str());
		int result = PHYSFS_mkdir(saveDir.c_str());
		if (result == 0)
		{
			printf("PHYSFS_mkdir failed: %s\n", PHYSFS_getLastError());
			return false;
		}
	}

	// creating local {ogre,plugins}.cfg files, if system-wide ones not
	// found, the local ones relative to PHYSFS_writeDir
	std::string ogreCfgSystem = (SumwarsHelper::gameDataPath() + "/ogre.cfg").c_str();
	std::string ogreCfgUser = "/.sumwars/ogre.cfg";
	std::string pluginsCfgSystem = (SumwarsHelper::gameDataPath() + "/plugins.cfg").c_str();
	std::string pluginsCfgUser = "/.sumwars/plugins.cfg";

	if (! PHYSFS_exists(ogreCfgSystem.c_str())) {
		std::string str =
			"Render System=OpenGL Rendering Subsystem\n"
			"\n"
			"[OpenGL Rendering Subsystem]\n"
			"FSAA=0\n"
			"Full Screen=No\n"
			"RTT Preferred Mode=PBuffer\n"
			"Video Mode=940 x 705\n";
		PHYSFS_file* ogreFile = PHYSFS_openWrite(ogreCfgUser.c_str());
		int count = PHYSFS_write(ogreFile,
					 str.c_str(), sizeof(char), str.size());

		if (count < str.size()) {
			printf("A global '%s' file could not be found, and attempting to write a local one failed: PHYSFS_write('%s') failed: %s\n",
				 ogreCfgSystem.c_str(),
				 ogreCfgUser.c_str(),
				 PHYSFS_getLastError());
			return false;
		} else {
			printf("Created '%s%s'\n", PHYSFS_getWriteDir(), ogreCfgUser.c_str());
		}
	}

	if (! PHYSFS_exists(pluginsCfgSystem.c_str())) {
		std::string str =
			"# Defines plugins to load\n"
			"\n"
			"# Define plugin folder\n"
			"PluginFolder=/usr/lib/OGRE\n"
			"\n"
			"# Define OpenGL rendering implementation plugin\n"
			"Plugin=RenderSystem_GL.so\n"
			"Plugin=Plugin_ParticleFX.so\n"
			"Plugin=Plugin_BSPSceneManager.so\n"
			"Plugin=Plugin_OctreeSceneManager.so\n";
		PHYSFS_file* pluginsFile = PHYSFS_openWrite(pluginsCfgUser.c_str());
		int count = PHYSFS_write(pluginsFile,
					 str.c_str(), sizeof(char), str.size());

		if (count < str.size()) {
			printf("A global '%s' file could not be found, and attempting to write a local one failed: PHYSFS_write('%s') failed: %s\n",
				 pluginsCfgSystem.c_str(),
				 pluginsCfgUser.c_str(),
				 PHYSFS_getLastError());
			return false;
		} else {
			printf("Created '%s%s'\n", PHYSFS_getWriteDir(), pluginsCfgUser.c_str());
		}
	}
#endif
    
	// Get the path to use for storing data.
	// This will be relative to the user directory on the user OS.
	Ogre::String operationalPath = SumwarsHelper::userPath ();

	// Initialize the loggers.
	LogManager::instance ().addLog ("stdout",new StdOutLog (Log::LOGLEVEL_DEBUG));
	LogManager::instance ().addLog ("logfile",new FileLog (operationalPath + "/sumwars.log", Log::LOGLEVEL_DEBUG));

	DEBUG ("Initialized logging. Level: %d", Log::LOGLEVEL_DEBUG);
	Timer tm;
	m_timer.start();

	// Ogre Root erzeugen
	// pluginfile: plugins.cfg
	// configfile: keines
#if defined (_WIN32)
	m_ogre_root = new Ogre::Root (operationalPath + "/plugins.cfg", operationalPath + "/ogre.cfg", operationalPath + "/ogre.log");
#elif defined (__APPLE__)
	Ogre::String plugins = 
	m_ogre_root = new Ogre::Root(SumwarsHelper::macPath() + "/plugins_mac.cfg",
				     SumwarsHelper::macPath() + "/ogre.cfg",
				     operationalPath + "/ogre.log");
#elif defined (__unix__)
	// ogreCfgSystem, ogreCfgUser, pluginsCfgSystem and pluginsCfgUser
	// declared above
	if (PHYSFS_exists(pluginsCfgSystem.c_str()) && PHYSFS_exists(pluginsCfgSystem.c_str())) {
		m_ogre_root = new Ogre::Root(pluginsCfgSystem,
					     ogreCfgSystem,
					     SumwarsHelper::userPath() + "/Ogre.log");
	} else {
		m_ogre_root = new Ogre::Root(PHYSFS_getWriteDir() + pluginsCfgUser,
					     PHYSFS_getWriteDir() + ogreCfgUser,
					     SumwarsHelper::userPath() + "/Ogre.log");
	}
#endif

	if (m_ogre_root == 0)
		return false;

	// Augustin Preda, 2011.11.15: trial code: load the options.xml file; it's required
	// TODO: don't load it again later. Currently it's done via m_document->loadSettings();
	Options::getInstance()->readFromFile(SumwarsHelper::userPath() + "/options.xml");

	bool ret = true;

	// Ogre configurieren
	ret = configureOgre();
	if (ret==false)
	{
		ERRORMSG("Configuring Ogre failed");
		return false;
	}

	// Ogre initialisieren
	ret = initOgre();
	if (ret==false)
	{
		ERRORMSG("Initialisation of Ogre failed");
		return false;
	}

	// Ressourcen festlegen
	ret = setupResources();
	if (ret==false)
	{
		ERRORMSG("Setting up Resources failed");
		return false;
	}

	//Gettext initialisieren
	ret = initGettext();
	if (ret==false)
	{
		ERRORMSG("Initialisation of Gettext failed");
		return false;
	}

	tm.start();
	//CEGUI initialisieren
	ret = initCEGUI();
	if (ret==false)
	{
		ERRORMSG("Initialisation of CEGUI failed");
		return false;
	}

	// Document anlegen
	ret = createDocument();
	if (ret==false)
	{
		ERRORMSG("cant create document");
		return false;
	}
	// View anlegen
	ret = createView();
	if (ret==false)
	{
		ERRORMSG("cant create view");
		return false;
	}

	ret = initOpenAL();
	if (ret == false)
	{
		ERRORMSG("Sound initialisation failed");
	}

	DEBUG("time to start %f",tm.getTime());
	// Ressourcen laden
	ret = loadResources();
	if (ret == false)
	{
		ERRORMSG("could not load ressources");
	}

	DEBUG ("Application initialized. Storing data in folder [%s]\n\n", operationalPath.c_str ());
	// debugging
	//MyFrameListener* mfl = new MyFrameListener(m_main_window,m_document);
	//m_ogre_root->addFrameListener(mfl);

	return true;
}


Application::~Application()
{
	// Release (deallocate) dynamically allocated objects in reverse order. (reverse to their creation)

	printf("deleting application\n");
	if (m_main_window)
	{
		delete m_main_window;
	}
	if (m_document)
	{
		delete m_document;
	}
	try
	{
		CEGUI::OgreRenderer::destroySystem(); // deletes everything
	}
	catch(std::exception& e)
	{
		// this happens, if the program dies before the renderer was initialized
		ERRORMSG ("Caught exception: [%s]", e.what ());
	}
	if (m_ogre_root)
	{
		delete m_ogre_root;
	}
	
	
	ObjectFactory::cleanup();
	ItemFactory::cleanup();
	SoundSystem::cleanup();
	LogManager::cleanup();
    
    PHYSFS_deinit();

	// Free the singletons? - Should be cleaned up automatically along with the rest of the application memory space.
	// delete SumwarsHelper::getSingletonPtr ();
	// delete SWUtil::Clipboard::getSingletonPtr ();
}

void Application::run()
{
	// this happens, if initialization failed
	if (m_shutdown)
	{
		return;
	}
	
	m_running = true;
	
	Ogre::Timer timer;
	float time[7]={0,0,0,0,0,0,0},t;
	float frametime;
	int count=0;
	Ogre::Timer timer2;

	int nr = 100;
	timer.reset ();

	while (m_document->getState() != Document::END_GAME)
	{
		frametime =timer.getMicroseconds ()/1000.0;
		// set cap_fps to no to disable fps reduction
		if (Options::getInstance()->getDebugOption("cap_fps","true") == "true")
		{
			// cap framerate at 50 fps
			if (frametime < 20.0)
			{
				#ifdef WIN32
					Sleep(20-frametime);
				#else
					usleep(1000*(20-frametime));
				#endif
				frametime =timer.getMicroseconds ()/1000.0;
			}
		}
		time[0] += frametime;
		timer.reset();


		count ++;
		if (count >=nr)
		{
			// set print_fps to true to get some data
			if (Options::getInstance()->getDebugOption("print_fps","false") == "true")
			{
				count =0;
				
				DEBUG("average stats over %i frames",nr);
				DEBUG("frame time: %f (%f fps)",time[0]/nr, 1000/(time[0]/nr));
				DEBUG("app update time: %f",time[1]/nr);
				DEBUG("message pump time: %f",time[2]/nr);
				DEBUG("world update time: %f",time[3]/nr);
				DEBUG("scene update time: %f",time[4]/nr);
				DEBUG("gui update time: %f",time[5]/nr);
				DEBUG("ogre  time: %f \n",time[6]/nr);


				for (int i=0; i<7; i++)
				time[i]=0;
			}

		}

		timer2.reset();

		update();

		t =timer2.getMicroseconds ()/1000.0;
		time[1] += t;
		if (t > 20)
		{
			DEBUG("update time was %f",t);
		}


		timer2.reset();
		// run the message pump
		Ogre::WindowEventUtilities::messagePump();

		t =timer2.getMicroseconds ()/1000.0;
		time[2] += t;
		if (t>20)
		{
			DEBUGX("message pump time was %f",t);
		}

		timer2.reset();

		bool inactive_on_focus = m_window->isDeactivatedOnFocusChange();
		if (m_document->isSinglePlayer() != inactive_on_focus)
		{
			m_window->setDeactivateOnFocusChange( m_document->isSinglePlayer() );
		}
		
		// Document aktualisieren
		bool active = m_window->isActive() || ! (m_document->isSinglePlayer());
		if (active)
		{
			m_document->update(frametime);
		}

		t =timer2.getMicroseconds ()/1000.0;
		time[3] += t;
		if (t> 20)
		{
			DEBUGX("document update time was %f",t);
		}

		try
		{
			timer2.reset();

			// Update the view
			DEBUGX("main window update");
			m_main_window->update(frametime);

			t =timer2.getMicroseconds ()/1000.0;
			time[4] += t;
			if (t > 20)
			{
				DEBUGX("view update time was %f",t);
			}
		}
		catch (CEGUI::Exception e)
		{
			ERRORMSG("Error message: %s",e.getMessage().c_str());
			return;
		}



		timer2.reset();

		m_cegui_system->injectTimePulse(frametime/1000.0);

		t =timer2.getMicroseconds ()/1000.0;
		time[5] += t;

		if (t> 20)
		{
			DEBUGX("cegui update time was %f",t);
		}

		// rendern
		timer2.reset();

		if (active)
		{
			m_ogre_root->renderOneFrame();
		}

		t =timer2.getMicroseconds ()/1000.0;
		time[6] += t;
		if (t > 200)
		{
			DEBUGX("ogre frame time was %f",t);
		}

		// Musik aktualisieren
		MusicManager::instance().update();
	}

	DEBUG ("Application: run function has run its course");


}



void Application::setApplicationIcon ()
{
	//
	// Platform specific code - set the application icon.
	// On Windows, it's required to set the icon of the application. This will be visible in the taskbar + alt-tab operations.
	// Also, if the application is started in windowed mode, the icon is set to the titlebar.
	//
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	{
		HWND hwnd;
		m_window->getCustomAttribute ("WINDOW", &hwnd);
		HINSTANCE hinstance;
		hinstance = GetModuleHandle (0); 
		// If you get an error similar to [error C2065: 'IDI_SUMWARS_APP_ICON' : undeclared identifier]
		// Make sure you add the resource files [Sumwars.rc] and [resource.h] to the project.
		HICON icon = LoadIcon (hinstance, MAKEINTRESOURCE (IDI_SUMWARS_APP_ICON));
		SendMessage (hwnd, WM_SETICON, ICON_BIG, LPARAM (icon));
		SendMessage (hwnd, WM_SETICON, ICON_SMALL, LPARAM (icon)); 
	}
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	// TODO: add platform specific code.
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
	// TODO: add platform specific code.
#endif
}


/**
	Retrieves (via output parameters) the settings requested by the user for the video mode size
	\param videoModeWidth The width specified in the video mode.
	\param videoModeHeight The height specified in the video mode.
	\return The full string containing the video mode specification, from the driver. It may include color depth.
*/
std::string Application::retrieveRenderSystemWindowSize (int& videoModeWidth, int& videoModeHeight)
{
	std::string returnValue ("");

	// If the user selected a Windowed mode, but with the size of the entire screen, do additional preparations.
	Ogre::RenderSystem* myRenderSys = m_ogre_root->getRenderSystem ();
	if (myRenderSys != 0)
	{
		Ogre::ConfigOptionMap myCfgMap = myRenderSys->getConfigOptions ();
		for (Ogre::ConfigOptionMap::iterator it = myCfgMap.begin ();
				it != myCfgMap.end (); 
				++ it)
		{
			DEBUG ("option name=[%s], val=[%s]", it->second.name.c_str (), it->second.currentValue.c_str ());
		}

		Ogre::ConfigOptionMap::iterator opt_it = myCfgMap.find ("Video Mode");
		if (opt_it != myCfgMap.end ())
		{
			DEBUG ("Currently selected video mode: %s",  opt_it->second.currentValue.c_str ());
			returnValue = opt_it->second.currentValue;
			std::string sLeft, sRight;
			int nPos = returnValue.find (" ");
			if (nPos == std::string::npos)
			{
				// some error...
				ERRORMSG ("Unable to find space in [%s]", returnValue.c_str ());
				return "";
			}

			sLeft = returnValue.substr (0, nPos);
			sRight= returnValue.substr (nPos + 3);// + 3 for " x "
			nPos = sRight.find (" ");
			std::string sAux;

			if (nPos == std::string::npos)
			{
				// we don't have a colour depth.
				sAux = sRight;
			}
			else
			{
				sAux = sRight.substr(0, nPos);
			}

			videoModeWidth = atoi (sLeft.c_str ());
			videoModeHeight = atoi (sAux.c_str ());
		}
	}
	else
	{
		ERRORMSG ("Called retrieveRenderSystemWindowSize, but no render system is available!");
	}

	return returnValue;
}



/**
	Handle Ogre specific initializations.

*/
bool Application::initOgre()
{
	DEBUG("init ogre");

	// Create window.
	// Here, we have 2 options:
	// 1. Let OGRE create the window automatically, but have less control over it
	// 2. Create the window ourselves, longer code, but have more control

	// Initialize Ogre with the automatic creation of a window.
	m_window = m_ogre_root->initialise (true, "Summoning Wars"); // TODO: define constant for name of obtain from config file.

	if (m_window->isFullScreen ())
	{
		Options::getInstance()->setUsedDisplayMode (FULLSCREEN_EX);
	}
	else
	{
		Options::getInstance()->setUsedDisplayMode (WINDOWED_WITH_BORDER);
	}

	const Ogre::RenderSystemList rsList = m_ogre_root->getAvailableRenderers ();
	for (Ogre::RenderSystemList::const_iterator it = rsList.begin (); it != rsList.end (); ++ it)
	{
		Ogre::String rsName = (*it)->getName ();
		Options::getInstance ()->getEditableAvailableVideoDrivers ().push_back (rsName);
		Ogre::ConfigOptionMap optionsMap = (*it)->getConfigOptions ();

		for (Ogre::ConfigOptionMap::iterator optit = optionsMap.begin(); optit != optionsMap.end(); ++ optit)
		{
			if (optit->second.name == "Video Mode")
			{
				Ogre::StringVector possibleValues = optit->second.possibleValues;
				Options::getInstance ()->getEditableResolutionsMapping()[rsName].clear ();
				
				for (Ogre::StringVector::const_iterator subit = possibleValues.begin ();
					subit != possibleValues.end (); ++ subit)
				{
					Options::getInstance ()->getEditableResolutionsMapping()[rsName].push_back (*subit);
				}
			}
		}
	}

	Options::getInstance ()->setUsedVideoDriver (m_ogre_root->getRenderSystem()->getName ());

	setApplicationIcon ();

	int videoModeWidth = 0;
	int videoModeHeight = 0;
	std::string usedResolution ("");
	usedResolution = retrieveRenderSystemWindowSize (videoModeWidth, videoModeHeight);

	Options::getInstance ()->setUsedResolution (usedResolution);
	DEBUG ("Stored used resolution into global options [%s]", usedResolution.c_str ());
	DEBUG ("Size should be %d x %d", videoModeWidth, videoModeHeight);

	//
	// Platform specific code: 
	// On Windows, if the user specifies Windowed mode and a Width and Height equal to the screen size, 
	// remove the window borders.
	//
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	Options::getInstance()->setWindowedFullscreenModeSupported (true);

	if (! m_window->isFullScreen ())
	{
		// -------------------------------- beginning of platform dependent code --------------------------
		int desktopWidth = (int)GetSystemMetrics (SM_CXSCREEN);
		int desktopHeight = (int)GetSystemMetrics (SM_CYSCREEN);

		if (desktopWidth == videoModeWidth && desktopHeight == videoModeHeight)
		{
			DEBUG ("Windowed (Fullscreen) mode selected; desktop at %d x %d.", desktopWidth, desktopHeight);
			DEBUG ("Correcting settings.");

			// Retrieve the window handle
			HWND hwnd; // handle of window
			m_window->getCustomAttribute ("WINDOW", &hwnd);

			// Also hide the cursor. This may make the debugging sometimes more difficult.
			ShowCursor(false); // TODO: if you hide the cursor, make sure you react to the focus lost events, to show the cursor for other apps.

			SetWindowLong (hwnd, GWL_STYLE, WS_VISIBLE);
			//SetWindowLong (hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW);
			SetWindowPos (hwnd, HWND_TOP, 0, 0, desktopWidth, desktopHeight, SWP_SHOWWINDOW);

			SendMessage (hwnd, WM_STYLECHANGING, 0, WS_OVERLAPPED);
			UpdateWindow (hwnd);

			Options::getInstance()->setUsedDisplayMode (WINDOWED_FULLSCREEN);
		}
		// ----------------------------------- end of platform dependent code -----------------------------
	}
#endif //OGRE_PLATFORM_WIN32


	// Set the texture filtering.
	// Possible values:
	// 0 - TFO_NONE
	// 1 - TFO_BILINEAR
	// 2 - TFO_TRILINEAR
	// 3 - TFO_ANISOTROPIC : http://en.wikipedia.org/wiki/Anisotropic_filtering
	
	int textureFilteringMode = 3; // TODO: remove hardcoding; add to XML options.
	// Change the default texture filtering mode.
	Ogre::MaterialManager::getSingleton ().setDefaultTextureFiltering 
		(static_cast<Ogre::TextureFilterOptions> (textureFilteringMode));

	// Create the scene manager
	m_scene_manager = m_ogre_root->createSceneManager (Ogre::ST_GENERIC, "DefaultSceneManager");

#ifndef DONT_USE_SHADOWS
	// TODO: the order in which the settings are loaded is wrong. Fix it! The options file is loaded after OGRE is initialized, 
	// but data is needed NOW, when we're doing the initialization.

	int shadowMode = Options::getInstance ()->getShadowMode ();

	if (shadowMode > 0)
	{
		switch (shadowMode)
		{
		case 2:
			m_scene_manager->setShadowTechnique (Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
			break;
		case 3:
			m_scene_manager->setShadowTechnique (Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);
			break;
		case 4:
			m_scene_manager->setShadowTechnique (Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
			break;
		case 1:
		default:
			m_scene_manager->setShadowTechnique (Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
			break;
		}
	}
	else
	{
		m_scene_manager->setShadowTechnique (Ogre::SHADOWTYPE_NONE);
	}

	m_scene_manager->setShadowFarDistance (45);
	m_scene_manager->setShadowColour( Ogre::ColourValue(0.4, 0.4, 0.4) );

    /*// set Shadows enabled before any mesh is loaded
	m_scene_manager->setShadowTextureSelfShadow(false);
	m_scene_manager->setShadowTextureConfig(0,2048,2048,Ogre::PF_X8R8G8B8);
*/

	// If we can use texture sizes BIGGER than the window size, specify the settings...
	// This difference may come up most often due to the D3D/OpenGL support.
	if (m_ogre_root->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_HWRENDER_TO_TEXTURE))
    {
        // In D3D, use a 1024x1024 shadow texture
		m_scene_manager->setShadowTextureSettings(1024, 2);
    }
    else
    {
        // Use 256x256 texture in GL since we can't go higher than the window res
		// Make sure you use a resolution higher than this!
        m_scene_manager->setShadowTextureSettings(256, 2);
    }
#endif // DONT_USE_SHADOWS

	// Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener (m_window, this);

	Ogre::LogManager::getSingleton ().createLog (SumwarsHelper::userPath () + "/BenchLog.log");
	return true;
}



bool Application::configureOgre()
{
	DEBUG("configure ogre");

	// Use the default logging level.
	// Possible options:
	// LL_LOW - errors
	// LL_NORMAL - general initializations and errors
	// LL_BOREME - high debug information - NOT recommended.
	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_NORMAL);
	
	// Restore config if ogre.cfg is show, otherwise show the config dialog
	if(!(m_ogre_root->restoreConfig() || m_ogre_root->showConfigDialog()))
	{
		delete m_ogre_root;
		m_ogre_root = 0;
		return false;
	}

	return true;
}



bool Application::setupResources()
{
	DEBUG("initalizing resources");
#ifdef NOMIPMAPS
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(0);
#endif

	Ogre::String path;
	Ogre::ConfigFile cf;
#if defined (__unix__)
	Ogre::String resourcesCfgUser = "/.sumwars/resources.cfg";
	Ogre::String resourcesCfgSystem = SumwarsHelper::gameDataPath() + "/resources.cfg";
	if (PHYSFS_exists(resourcesCfgUser.c_str())) {
		cf.load(std::string(PHYSFS_getWriteDir()) + resourcesCfgUser);
	} else {
		cf.load(resourcesCfgSystem);
		if (cf.getSetting("FileSystem", "monsters", "notfound") == "notfound") {
			ERRORMSG("Error message: resource.cfg file not found, searched in '%s%s' and '%s'",
				 PHYSFS_getWriteDir(), resourcesCfgUser.c_str(),
				 resourcesCfgSystem.c_str());
			return false;
		}
	}
#elif defined (__APPLE__)
	path = SumwarsHelper::macPath() + "/";
	cf.load(path + "/resources.cfg");
#elif defined (_WIN32)
	cf.load(path + "resources.cfg");
#endif

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
 
	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + archName, typeName, secName);
		}
	}
	
	Ogre::String savePath = SumwarsHelper::userPath() + "/save";
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(savePath, "FileSystem", "Savegame");

#if defined(WIN32)
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("c:\\windows\\fonts", "FileSystem", "GUI");
#endif

	// Gruppen initialisieren
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("General");
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Particles");
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Savegame");
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("GUI");


	// Debugging: Meshes direkt anlegen

	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);

	Ogre::MeshManager::getSingleton().createPlane("square44", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,200,200,1,1,true,1,1,1,Ogre::Vector3::UNIT_X);


	Ogre::MeshManager::getSingleton().createPlane("rect81", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,400,50,1,1,true,1,1,1,Ogre::Vector3::UNIT_X);

	return true;
}

bool Application::initGettext()
{
	DEBUG("initializing internationalisation");
	Ogre::StringVectorPtr path = Ogre::ResourceGroupManager::getSingleton().findResourceLocation("translation", "*");
	Gettext::init("", path.get()->at(0));
	return true;
}

bool Application::initCEGUI()
{
	DEBUG("init CEGUI\n");
    
	// Log level
	new CEGUI::DefaultLogger();	
	CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);
	CEGUI::DefaultLogger::getSingleton().setLogFilename(SumwarsHelper::userPath() + "/CEGUI.log");
	
	// Bootstrap the CEGUI System
	CEGUI::OgreRenderer::bootstrapSystem();
    
	// Scheme laden
	CEGUI::SchemeManager::getSingleton().create((CEGUI::utf8*)"TaharezLook.scheme", (CEGUI::utf8*)"GUI");

	// Imagesets laden
	CEGUI::ImagesetManager::getSingleton().create("skills.imageset");

	CEGUI::Texture &startScreenTex = CEGUI::System::getSingleton().getRenderer()->createTexture("startscreen.png", (CEGUI::utf8*)"GUI");
    
	try
	{
		CEGUI::ImagesetManager::getSingleton().createFromImageFile("startscreen.png", "startscreen.png");
		CEGUI::ImagesetManager::getSingleton().createFromImageFile("worldMap.png","worldMap.png",(CEGUI::utf8*)"GUI");
	}
	catch (CEGUI::Exception& e)
	{
		DEBUG("CEGUI exception %s",e.getMessage().c_str());
	}
    m_cegui_system = CEGUI::System::getSingletonPtr();
	
	/*CEGUI::LuaScriptModule &scriptm(CEGUI::LuaScriptModule::create());
	m_cegui_system->setScriptingModule(&scriptm);*/

	// Mauscursor setzen (evtl eher in View auslagern ? )
	m_cegui_system->setDefaultMouseCursor((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");
    m_cegui_system->setDefaultTooltip((CEGUI::utf8*)"TaharezLook/Tooltip");

	// Update the fade delay?
	if (m_cegui_system->getDefaultTooltip ())
	{
		float myFadeTime = m_cegui_system->getDefaultTooltip ()->getFadeTime ();
		// TODO: add option to specify tool-tip delay.
		//DEBUG ("Tooltip fade time was %f", myFadeTime);
		//myFadeTime /= 4;
		m_cegui_system->getDefaultTooltip ()->setFadeTime (myFadeTime);
		//DEBUG ("New tooltip fade time set to %f", myFadeTime);
	}
    
	// Font setzen
	CEGUI::FontManager::getSingleton().create("DejaVuSerif-8.font", (CEGUI::utf8*)"GUI");
	CEGUI::FontManager::getSingleton().create("DejaVuSerif-10.font", (CEGUI::utf8*)"GUI");
	CEGUI::FontManager::getSingleton().create("DejaVuSerif-12.font", (CEGUI::utf8*)"GUI");
	CEGUI::FontManager::getSingleton().create("DejaVuSerif-16.font", (CEGUI::utf8*)"GUI");
	CEGUI::FontManager::getSingleton().create("DejaVuSans-10.font", (CEGUI::utf8*)"GUI");
	m_cegui_system->setDefaultFont((CEGUI::utf8*)"DejaVuSerif-8");

	// eigene Factorys einfuegen
    CEGUI::WindowFactoryManager::getSingleton().addFalagardWindowMapping ("SumwarsTooltip", "DefaultWindow", "TaharezLook/CustomTooltip", "Falagard/Default");

#ifdef BUILD_TOOLS
	CEGUI::WindowFactoryManager::getSingleton().addFactory< CEGUI::TplWindowFactory<DebugCameraTab> >();
	CEGUI::WindowFactoryManager::getSingleton().addFactory< CEGUI::TplWindowFactory<GuiDebugTab> >();
	CEGUI::WindowFactoryManager::getSingleton().addFactory< CEGUI::TplWindowFactory<IconEditorTab> >();
	CEGUI::WindowFactoryManager::getSingleton().addFactory< CEGUI::TplWindowFactory<BenchmarkTab> >();
	CEGUI::WindowFactoryManager::getSingleton().addFactory< CEGUI::TplWindowFactory<LuaScriptTab> >();
	//CEGUI::WindowFactoryManager::getSingleton().addFactory< CEGUI::TplWindowFactory<ReloadTab> >();
	CEGUI::WindowFactoryManager::getSingleton().addFactory< CEGUI::TplWindowFactory<TextFileEditWindow> >();
	//CONTENTEDITOR
#endif

	return true;
}

bool Application::initOpenAL()
{
	SoundSystem::init();
	bool err = SoundSystem::checkErrors();
	return !err;
}

bool Application::createDocument()
{
	DEBUG("create document\n");
	m_document = new Document();

	/*
	m_document->installShortkey(OIS::KC_ESCAPE,CLOSE_ALL);
	m_document->installShortkey(OIS::KC_I,SHOW_INVENTORY);
	m_document->installShortkey(OIS::KC_C,SHOW_CHARINFO);
	m_document->installShortkey(OIS::KC_T,SHOW_SKILLTREE);
	m_document->installShortkey(OIS::KC_P,SHOW_PARTYMENU);
	m_document->installShortkey(OIS::KC_M,SHOW_CHATBOX);
	m_document->installShortkey(OIS::KC_Q,SHOW_QUESTINFO);
	m_document->installShortkey(OIS::KC_TAB,SHOW_MINIMAP);
	m_document->installShortkey(OIS::KC_F10,SHOW_OPTIONS);
	m_document->installShortkey(OIS::KC_W,SWAP_EQUIP);
	m_document->installShortkey(OIS::KC_LMENU,SHOW_ITEMLABELS);

	m_document->installShortkey(OIS::KC_RETURN,SHOW_CHATBOX_NO_TOGGLE,false);
	m_document->installShortkey(OIS::KC_1,USE_POTION,false);
	m_document->installShortkey(OIS::KC_2,(ShortkeyDestination) (USE_POTION+1),false);
	m_document->installShortkey(OIS::KC_3,(ShortkeyDestination) (USE_POTION+2),false);
	m_document->installShortkey(OIS::KC_4,(ShortkeyDestination) (USE_POTION+3),false);
	m_document->installShortkey(OIS::KC_5,(ShortkeyDestination) (USE_POTION+4),false);
	m_document->installShortkey(OIS::KC_6,(ShortkeyDestination) (USE_POTION+5),false);
	m_document->installShortkey(OIS::KC_7,(ShortkeyDestination) (USE_POTION+6),false);
	m_document->installShortkey(OIS::KC_8,(ShortkeyDestination) (USE_POTION+7),false);
	m_document->installShortkey(OIS::KC_9,(ShortkeyDestination) (USE_POTION+8),false);
	m_document->installShortkey(OIS::KC_0,(ShortkeyDestination) (USE_POTION+9),false);

	m_document->installSpecialKey(OIS::KC_ESCAPE);
	m_document->installSpecialKey(OIS::KC_LSHIFT);
//	m_document->installSpecialKey(OIS::KC_LCONTROL);
//	m_document->installSpecialKey(OIS::KC_RCONTROL);
//	m_document->installSpecialKey(OIS::KC_RMENU);
//	m_document->installSpecialKey(OIS::KC_LMENU);
	m_document->installSpecialKey(OIS::KC_TAB);
	m_document->installSpecialKey(OIS::KC_RETURN);
	m_document->installSpecialKey(OIS::KC_BACK);
	m_document->installSpecialKey(OIS::KC_UP);
	m_document->installSpecialKey(OIS::KC_DOWN);
	m_document->installSpecialKey(OIS::KC_LEFT);
	m_document->installSpecialKey(OIS::KC_RIGHT);
	m_document->installSpecialKey(OIS::KC_CAPITAL);
	// CHEATS
	m_document->installShortkey(OIS::KC_L,(ShortkeyDestination) (CHEAT+0));
*/
	m_document->loadSettings();

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	if (! Options::getInstance ()->getShowConsoleWindow ())
	{
		// should hide the console window
		HWND hWnd = GetConsoleWindow();
		ShowWindow( hWnd, SW_HIDE );
	}
#endif //OGRE_PLATFORM == OGRE_PLATFORM_WIN32

	return true;
}

bool Application::createView()
{
	DEBUG("create view\n");
	m_main_window = new MainWindow(m_ogre_root, m_cegui_system,m_window,m_document);

#ifdef BUILD_TOOLS
	new DebugPanel();
	DebugPanel::getSingleton().init(false);
	new ContentEditor();
	ContentEditor::getSingleton().init(false);
#endif

	TooltipManager *mgr = new TooltipManager();
	mgr->setFadeInTime(200.0f);
	mgr->setFadeOutTime(200.0f);
	mgr->setVisibleTime(0.0f);

	// Create the clipboard singleton
	new SWUtil::Clipboard ();

	return true;
}

bool Application::loadResources(int datagroups)
{
	TiXmlBase::SetCondenseWhiteSpace(false);

	Ogre::FileInfoListPtr files;
	Ogre::FileInfoList::iterator it;
	std::string file;

	if (datagroups & World::DATA_IMAGES)
	{
		DEBUG("Loading images.");
		files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("itempictures","*.png");
		for (it = files->begin(); it != files->end(); ++it)
		{

			file = it->filename;

			CEGUI::ImagesetManager::getSingleton().createFromImageFile(file,file,(CEGUI::utf8*)"itempictures");

			updateStartScreen(0.1);
		}

		// Imagesets laden
		files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("itempictures","*.imageset");
		for (it = files->begin(); it != files->end(); ++it)
		{

			file = it->filename;

			CEGUI::ImagesetManager::getSingleton().create(file);

			updateStartScreen(0.2);
		}
	

		files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("emotionsets","*.imageset");
		for (it = files->begin(); it != files->end(); ++it)
		{

			file = it->filename;

			CEGUI::ImagesetManager::getSingleton().create(file);

			updateStartScreen(0.3);
		}
	
	}

	if (datagroups & World::DATA_MODELS)
	{
		DEBUG("Loading models.");
		Ogre::ResourceGroupManager::getSingleton().loadResourceGroup("General");
	}
	if (datagroups & World::DATA_PARTICLESYSTEMS)
	{
		DEBUG("Loading particlesystems.");
		
		Ogre::ResourceGroupManager::getSingleton().loadResourceGroup("Particles");
		if (m_running)
		{
			files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("Particles","*.particle");
			for (it = files->begin(); it != files->end(); ++it)
			{
				try
				{
					file = it->filename;
					DEBUGX("loading script file %s",file.c_str());
					Ogre::DataStreamPtr filehandle;
					filehandle = Ogre::ResourceGroupManager::getSingleton().openResource(file);
					Ogre::ParticleSystemManager::getSingleton().parseScript(filehandle,"Particles" ); 
				}
				catch (Ogre::Exception& e)
				{
					DEBUG("failed with exception %s",e.what());
				}
			}
		}
		
	}
	updateStartScreen(0.4);
	Ogre::ResourceGroupManager::getSingleton().loadResourceGroup("Savegame");
	updateStartScreen(0.5);
	if (datagroups & World::DATA_GUI)
	{
		Ogre::ResourceGroupManager::getSingleton().loadResourceGroup("GUI");
	}

	// only load abilities if world is not present
	// normally, loading abilities is job of the World
	// this is needed to display the correct animation on the preview screen
	if (World::getWorld() == 0)
	{
		if (datagroups & World::DATA_ABILITIES)
		{
			EventSystem::init();
			DEBUG("Loading ability data.");
			Action::init();

			files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("abilities","*.xml");
			for (it = files->begin(); it != files->end(); ++it)
			{
				file = it->archive->getName();
				file += "/";
				file += it->filename;

				Action::loadAbilityData(file.c_str());

			}
		}
	}


	// Spielerklassen initialisieren
	if (datagroups & World::DATA_PLAYERCLASSES)
	{
		DEBUG("Loading playerclasses.");
		files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("playerclasses","*.xml");
		for (it = files->begin(); it != files->end(); ++it)
		{
			file = it->archive->getName();
			file += "/";
			file += it->filename;

			ObjectLoader::loadPlayerData(file.c_str());

			updateStartScreen(0.55);
		}
	}
	
	if (datagroups & World::DATA_ITEMS)
	{
		DEBUG("Loading items.");
		// Items initialisieren
		ItemFactory::init();
		files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("items","*.xml");
		for (it = files->begin(); it != files->end(); ++it)
		{
			file = it->archive->getName();
			file += "/";
			file += it->filename;

			ItemLoader::loadItemData(file.c_str());

			updateStartScreen(0.6);
		}
		
#ifdef BUILD_TOOLS
		ContentEditor::getSingleton().fullUpdateComponent("ItemEditor");
		
#endif
	}

	if (datagroups & World::DATA_OBJECTS)
	{
		// Objekt Templates
		files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("obj_templates","*.xml");
		for (it = files->begin(); it != files->end(); ++it)
		{
			file = it->archive->getName();
			file += "/";
			file += it->filename;

			TemplateLoader::loadObjectTemplateData(file.c_str());

			updateStartScreen(0.65);
		}
		
		// Objekt Gruppen Templates
		files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("object_groups","*.xml");
		for (it = files->begin(); it != files->end(); ++it)
		{
			file = it->archive->getName();
			file += "/";
			file += it->filename;
			TemplateLoader::loadObjectGroupData(file.c_str());

			updateStartScreen(0.7);
		}
	}

	if (datagroups & World::DATA_RENDERINFO)
	{
		DEBUG("Loading renderinfo.");
		
		// Render Infos
		files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("renderinfo","*.xml");
		for (it = files->begin(); it != files->end(); ++it)
		{
			file = it->archive->getName();
			file += "/";
			file += it->filename;
			GraphicManager::loadRenderInfoData(file.c_str());

			updateStartScreen(0.8);
		}
	}

	if (datagroups & World::DATA_SOUND)
	{
		DEBUG("Loading sound.");
		
		// Sounds
		files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("sounddata","*.xml");
		for (it = files->begin(); it != files->end(); ++it)
		{
			file = it->archive->getName();
			file += "/";
			file += it->filename;

			SoundSystem::loadSoundData(file.c_str());

			updateStartScreen(0.9);
		}
	}
	
	if (!m_running)
	{
		Ogre::MeshManager& mesh_mgr = Ogre::MeshManager::getSingleton();
		Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
		mesh_mgr.createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 200, 200, 5, 5, true, 1,1,1,Ogre::Vector3::UNIT_X);

		updateStartScreen(1.0);
	
	
		m_main_window->setReadyToStart(true);
	}

	return true;
}

void Application::cleanup(int datagroups)
{
	if (datagroups & World::DATA_IMAGES)
	{
		// TODO
	}
	
	if (datagroups & World::DATA_MODELS)
	{
		// only unload ressources that can be reloaded later
		Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup("General", true);
		GraphicManager::invalidateGraphicObjects();
	}
	
	if (datagroups & World::DATA_PARTICLESYSTEMS)
	{
		//Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup("Particles", true);
		// delete all the templates from the ParticleSystemManager
		Ogre::ParticleSystemManager& pmgr = Ogre::ParticleSystemManager::getSingleton();
		pmgr.removeAllTemplates();
		
		GraphicManager::clearParticlePool();
		DEBUGX("deleting particlesystems");
	}
	
	if (datagroups & World::DATA_GUI)
	{
		// only unload ressources that can be reloaded later
		Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup("GUI", true);
	}
	
	ObjectFactory::cleanup(datagroups);
	
	if (datagroups & World::DATA_ITEMS)
	{
		ItemFactory::cleanup();
	}
	
	if (datagroups & World::DATA_RENDERINFO)
	{
		GraphicManager::clearRenderInfos();
	}
	
	if (datagroups & World::DATA_ABILITIES)
	{
		Action::cleanup();
	}
	
	if (datagroups & World::DATA_LUAENVIRONMENT)
	{
		EventSystem::cleanup();
	}
	
	if (datagroups & World::DATA_SOUND)
	{
		SoundSystem::cleanupBuffers();
	}
	
	if (datagroups & World::DATA_MUSIC)
	{
		// just stop the music to release the buffers
		MusicManager::instance().stop();
	}
}

void  Application::update()
{
	if (World::getWorld() != 0)
	{
		World* world = World::getWorld();
		if (world->getDataReloadRequests() != World::DATA_NONE)
		{
			DEBUG("reload request %x",world->getDataReloadRequests());
			
			int datagroups = world->getDataReloadRequests();
			
			cleanup(datagroups);
			Application::loadResources(datagroups);
			
			world->loadGameData();
		}
	}
}

void Application::updateStartScreen(float percent)
{
	// this occurs when ressources are loaded while running
	if (m_running)
		return;
	
	if (m_timer.getTime() < 20)
	{
		return;
	}

	DEBUGX("update time %f  perc: %f",m_timer.getTime(), percent);
	m_main_window->update(m_timer.getTime()/1000);
	m_main_window->setRessourceLoadingBar(percent);
	m_cegui_system->injectTimePulse(m_timer.getTime()/1000);

	Ogre::WindowEventUtilities::messagePump();

	m_ogre_root->renderOneFrame();
	//MusicManager::instance().update();
	m_timer.start();
}



void Application::windowResized (Ogre::RenderWindow* rw)
{
	// Only act for own renderwindow. Can it happen that we receive something else here? I don't know... doesn't hurt to check.
	if (rw && rw == m_window)
	{
		// Currently this is a placeholder function.
	}

}


bool Application::windowClosing (Ogre::RenderWindow* rw)
{
	// Only close for own renderwindow. Can it happen that we receive something else here? I don't know... doesn't hurt to check.

	if (rw && rw == m_window)
	{
		DEBUG ("Application got a window closing event.");

		if (m_document != 0)
		{
			m_document->setState (Document::END_GAME);
		}
	}

	// The state was set internally, so the game will know it needs to shutdown. But it needs time to do this.
	// Return false so that we don't allow the closing to take place yet!
	return false;
}


void Application::windowFocusChange (Ogre::RenderWindow* rw)
{
	// Only act for own renderwindow. Can it happen that we receive something else here? I don't know... doesn't hurt to check.
	if (rw && rw == m_window)
	{
		// Currently this is a placeholder function.
		// There is already a function reacting to the application losing focus. Maybe it should be moved here.
		// Still under investigation. (Augustin Preda, 2011.10.26).
	}
}
