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
//#include "music.h"
#include "sumwarshelper.h"

#ifdef SUMWARS_BUILD_TOOLS
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
#include "guitabs.h"
#endif
//#include "CEGUI/ScriptingModules/LuaScriptModule/CEGUILua.h"

#ifdef SUMWARS_BUILD_WITH_ONLINE_SERVICES
#include "onlineservicesmanager.h"
#endif


#include "OgreConfigFile.h"
#include <OgreParticleSystemManager.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "../winicon/resource.h"
#endif

// Add the material manager to allow setting shadow techniques
#include <OgreMaterialManager.h>

// Clipboard singleton created here.
#include "clipboard.h"

// Utilities for handling CEGUI
#include "ceguiutility.h"

// Sound and music management classes.
#include "gussound.h"
#include "gopenal.h"

// Helper for sound operations
#include "soundhelper.h"


//locally defined listener.
class SWOgreResListener : public Ogre::ResourceGroupListener
{
protected:
public:
	SWOgreResListener () {}
	void resourceGroupScriptingStarted (const Ogre::String& groupName, size_t scriptCount)
	{
		SoundHelper::signalSoundManager ();
	}
	void scriptParseStarted (const Ogre::String& scriptName, bool &skipThisScript)
	{
		SoundHelper::signalSoundManager ();
	}
	void scriptParseEnded (const Ogre::String& scriptName, bool skipped) {}
	void resourceGroupScriptingEnded (const Ogre::String& groupName) {}
	void resourceGroupLoadStarted (const Ogre::String& groupName, size_t resourceCount)
	{
		SoundHelper::signalSoundManager ();
	}
	void resourceLoadStarted (const Ogre::ResourcePtr& resource)
	{
		SoundHelper::signalSoundManager ();
	}
	void resourceLoadEnded (void) {}
	void worldGeometryStageStarted (const Ogre::String& description)
	{
		SoundHelper::signalSoundManager ();
	}
	void worldGeometryStageEnded (void) {}
	void resourceGroupLoadEnded (const Ogre::String& groupName) {}
};


/**
	Application constructor. Will call the init function.
*/
Application::Application()
{
	// Initialize application variables
	m_main_window = 0;
	m_document = 0;
	bool ret = false;
	m_running = false;
	m_shutdown = false;

	// Call the specialized initialization function.
	//try
	{
		ret = init();
	}
	//catch (std::exception &e)
	//{
	//	ERRORMSG("Error message: %s",e.what());
	//}

	if (ret == false)
	{
		// ERRORMSG("Initialisation failed, stop");
		m_shutdown = true;
	}
}

bool Application::init()
{
	// Before adding the user directory to the path, keep in mind that for portable installs, the exe folder should
	// be added to the path, NOT the user folder.

	if (SumwarsHelper::getSingletonPtr ()->hasFeature ("allows-preload"))
	{
		Ogre::ConfigFile cf;
		std::string preloadFileName (SumwarsHelper::getSingletonPtr ()->getPreloadFileName ());
		try
		{
			cf.load (preloadFileName.c_str ());

			// NB: portable mode was here, moved to configure step
		}
		catch (std::exception&)
		{
			// Leave it untreated. It's because we're missing the OPTIONAL file for pre-load.
		}
	}

	// Get the path to use for storing data.
	// This will be relative to the user directory on the user OS.
	Ogre::String operationalPath = SumwarsHelper::getStorageBasePath() + "/" + SumwarsHelper::userPath();

	// Initialize the loggers.
	LogManager::instance ().addLog ("stdout", new StdOutLog(Log::LOGLEVEL_DEBUG));
	LogManager::instance ().addLog ("logfile", new FileLog (operationalPath + "/sumwars.log", Log::LOGLEVEL_DEBUG));

	SW_DEBUG ("Initialized logging. Level: %d", Log::LOGLEVEL_DEBUG);
	Timer tm;
	m_timer.start();

	// Ogre Root erzeugen
	// pluginfile: plugins.cfg
	// configfile: keines
#ifdef __APPLE__
	Ogre::String plugins = 
	m_ogre_root = new Ogre::Root(SumwarsHelper::macPath() + "/plugins_mac.cfg",
				     SumwarsHelper::macPath() + "/ogre.cfg",
				     operationalPath + "/ogre.log");
#else

	Ogre::String pluginsFileName;
	Ogre::String configFileName;
	Ogre::String logFileName;

#ifdef _DEBUG
	pluginsFileName = operationalPath + "/pluginsdbg.cfg";
	configFileName = operationalPath + "/ogre.cfg";
	logFileName = operationalPath + "/ogre.log";
#else
	pluginsFileName = operationalPath + "/plugins.cfg";
	configFileName = operationalPath + "/ogre.cfg";
	logFileName = operationalPath + "/ogre.log";
#endif

	m_ogre_root = new Ogre::Root (pluginsFileName, configFileName, logFileName);
#endif

	if (m_ogre_root == 0)
		return false;

	bool ret = true;

	ret = initOpenAL();
	if (ret == false)
	{
		ERRORMSG("Sound initialisation failed");
	}

	SW_DEBUG ("Reading options file (options.xml)...");
	// Augustin Preda, 2011.11.15: trial code: load the options.xml file; it's required
	// TODO: don't load it again later. Currently it's done via m_document->loadSettings();
	Options::getInstance()->readFromFile(operationalPath + "/options.xml");

#ifdef SUMWARS_BUILD_WITH_ONLINE_SERVICES
    new OnlineServicesManager(operationalPath);
#endif

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

	// Load menu playlist and start music play.
	loadAndPlayMenuMusic ();

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

	SW_DEBUG("time to start %f",tm.getTime());
	// Ressourcen laden
	ret = loadResources();
	if (ret == false)
	{
		ERRORMSG("could not load ressources");
	}

	SW_DEBUG ("Application initialized. Storing data in folder [%s]\n\n", operationalPath.c_str ());
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
	
	//
	// Destroy the singletons
	//
	
	ObjectFactory::cleanup();
	ItemFactory::cleanup();
	//SoundSystem::cleanup();

	// Destroy the sound system
    SoundManager::destroy ();

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
				
				SW_DEBUG("average stats over %i frames",nr);
				SW_DEBUG("frame time: %f (%f fps)",time[0]/nr, 1000/(time[0]/nr));
				SW_DEBUG("app update time: %f",time[1]/nr);
				SW_DEBUG("message pump time: %f",time[2]/nr);
				SW_DEBUG("world update time: %f",time[3]/nr);
				SW_DEBUG("scene update time: %f",time[4]/nr);
				SW_DEBUG("gui update time: %f",time[5]/nr);
				SW_DEBUG("ogre  time: %f \n",time[6]/nr);


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
			SW_DEBUG("update time was %f",t);
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
		catch (CEGUI::Exception & e)
		{
			ERRORMSG("Error message: %s",e.getMessage().c_str());
			return;
		}



		timer2.reset();

		CEGUIUtility::injectTimePulse (frametime/1000.0);

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
		//MusicManager::instance().update();

		// Do the update for the sounds
		SoundHelper::signalSoundManager ();
	}

	SW_DEBUG ("Application: run function has run its course");


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
			SW_DEBUG ("option name=[%s], val=[%s]", it->second.name.c_str (), it->second.currentValue.c_str ());
		}

		Ogre::ConfigOptionMap::iterator opt_it = myCfgMap.find ("Video Mode");
		if (opt_it != myCfgMap.end ())
		{
			SW_DEBUG ("Currently selected video mode: %s",  opt_it->second.currentValue.c_str ());
			returnValue = opt_it->second.currentValue;
			std::string sLeft, sRight;
			size_t nPos = returnValue.find (" ");
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
	SW_DEBUG("init ogre");

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

    m_ogre_root->getRenderSystem()->addListener(this);
	Options::getInstance ()->setUsedVideoDriver (m_ogre_root->getRenderSystem()->getName ());

	setApplicationIcon ();

	int videoModeWidth = 0;
	int videoModeHeight = 0;
	std::string usedResolution ("");
	usedResolution = retrieveRenderSystemWindowSize (videoModeWidth, videoModeHeight);

	Options::getInstance ()->setUsedResolution (usedResolution);
	SW_DEBUG ("Stored used resolution into global options [%s]", usedResolution.c_str ());
	SW_DEBUG ("Size should be %d x %d", videoModeWidth, videoModeHeight);

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
			SW_DEBUG ("Windowed (Fullscreen) mode selected; desktop at %d x %d.", desktopWidth, desktopHeight);
			SW_DEBUG ("Correcting settings.");

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

	std::string aspectRatioString = SumwarsHelper::getSingletonPtr ()->getNearestAspectRatioStringForWindowSize (videoModeWidth, videoModeHeight);
	SumwarsHelper::getSingletonPtr ()->setPrefferedAspectRatioString (aspectRatioString);


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
	SW_DEBUG("configure ogre");

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
	SW_DEBUG("initalizing resources");
#ifdef NOMIPMAPS
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(0);
#endif

	Ogre::ConfigFile cf;

#if defined (__APPLE__)
	cf.load(SumwarsHelper::macPath() + "/resources.cfg");
#else
	const std::string operationalPath = SumwarsHelper::getStorageBasePath() + "/" + SumwarsHelper::userPath();
	cf.load(operationalPath + "/resources.cfg");
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
			//Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
			SumwarsHelper::getSingletonPtr ()->addResourceLocation (archName, typeName, secName);
		}
	}
	
	Ogre::String savePath = SumwarsHelper::getStorageBasePath() + "/" + SumwarsHelper::savePath();
	SumwarsHelper::getSingletonPtr ()->addResourceLocation (savePath, "FileSystem", "Savegame");
	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation(savePath, "FileSystem", "Savegame");

#if defined(WIN32)
	// TODO: FIX: Augustin Preda (2013.02.07): What if you don't install windows in C:\Windows ?
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
	SW_DEBUG("initializing internationalisation");
	Ogre::StringVectorPtr path = Ogre::ResourceGroupManager::getSingleton().findResourceLocation("translation", "*");
	SW_DEBUG("Application initializing Gettext lib with [%s]", path.get ()->at (0).c_str ());
	Gettext::init("", path.get()->at(0));
	SW_DEBUG("Application initializing Gettext locale is [%s]", Gettext::getLocale ());
	return true;
}

bool Application::initCEGUI()
{
	SW_DEBUG("init CEGUI\n");
    
	// Log level
	new CEGUI::DefaultLogger();	
	CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);
	CEGUI::DefaultLogger::getSingleton().setLogFilename(SumwarsHelper::getStorageBasePath() + "/" + SumwarsHelper::userPath() + "/CEGUI.log");
	
	// Bootstrap the CEGUI System
	SW_DEBUG ("Bootstrapping the CEGUI system");
	CEGUI::OgreRenderer::bootstrapSystem();

	SW_DEBUG ("Getting the XML parser");

	CEGUI::XMLParser* parser = CEGUI::System::getSingleton().getXMLParser();
	if (NULL == parser)
	{
		WARNING ("No XML parser available to CEGUI!!!");
		return false;
	}
	if (parser->isPropertyPresent("SchemaDefaultResourceGroup"))
		parser->setProperty("SchemaDefaultResourceGroup", "GUI_XML_schemas");
    
	SW_DEBUG ("Loading (CEGUI) schemes...");

	// Load schemes
	SW_DEBUG ("Loading scheme: SWB.scheme");
	CEGUIUtility::loadScheme ((CEGUI::utf8*)"SWB.scheme", (CEGUI::utf8*)"GUI");
	SW_DEBUG ("Loading scheme: TaharezLook.scheme");
	CEGUIUtility::loadScheme ((CEGUI::utf8*)"TaharezLook.scheme", (CEGUI::utf8*)"GUI");
	SW_DEBUG ("Loading scheme: SumWarsExtras.scheme");
	CEGUIUtility::loadScheme ((CEGUI::utf8*)"SumWarsExtras.scheme", (CEGUI::utf8*)"GUI");
	
	// Load imagesets
	CEGUIUtility::loadImageset("skills.imageset");

	SW_DEBUG ("Creating hardcoded images from file");

	try
	{
		CEGUIUtility::addManagedImageFromFile ("SumWarsLogo.png", "SumWarsLogo.png");
		CEGUIUtility::addManagedImageFromFile ("worldMap.png", "worldMap.png", (CEGUI::utf8*)"GUI");
	}
	catch (CEGUI::Exception& e)
	{
		SW_DEBUG("CEGUI exception %s",e.getMessage().c_str());
	}
    m_cegui_system = CEGUI::System::getSingletonPtr();
	
	/*CEGUI::LuaScriptModule &scriptm(CEGUI::LuaScriptModule::create());
	m_cegui_system->setScriptingModule(&scriptm);*/

	SW_DEBUG ("Setting cursor defaults");

	// Set the mouse cursor.
	CEGUIUtility::setDefaultMouseCursor (m_cegui_system, Options::getInstance ()->getCeguiCursorSkin (), "MouseArrow");

	// Set the tooltip to use.
	CEGUIUtility::setDefaultTooltip (m_cegui_system, Options::getInstance ()->getCeguiSkin (), "Tooltip");

	CEGUI::Tooltip* defaultTT = CEGUI::System::getSingletonPtr ()->getDefaultGUIContext ().getDefaultTooltipObject ();
	if (0 != defaultTT)
	{
		SW_DEBUG ("Default tooltip is: %s", defaultTT->getNamePath ().c_str ());
		defaultTT->setHoverTime (0.5f); // set it to 0.5 seconds.
	}

	CEGUI::System::getSingletonPtr ()->getDefaultGUIContext ().markAsDirty ();


#if 0
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
#endif

	// Note: you could also add custom fonts here, by calling CEGUIUtility::addFont.
	// But all fonts can now be specified in the skin's scheme file. This means, we don't need to specify any fonts here.
	SW_DEBUG ("Creating fonts");

	// Set the default font to use based on the current display resolution.
	int configuredWidth, configuredHeight;
	SumwarsHelper::getSizesFromResolutionString (Options::getInstance ()->getUsedResolution (), configuredWidth, configuredHeight);
	std::string defaultFontName = SumwarsHelper::getSingletonPtr ()->getRecommendedDefaultFontForWindowSize (configuredWidth, configuredHeight);
	SW_DEBUG ("For the current display resolution (%d x %d), the system has decreed that the recommended font is: %s", configuredWidth, configuredHeight, defaultFontName.c_str ());
	CEGUIUtility::setDefaultFont ((CEGUI::utf8*)defaultFontName.c_str ());

	SW_DEBUG ("Creating own factory for tooltips");
	// Insert own factories. // TODO: check if this is really needed. Couldn't the custom tooltip just be added to the scheme?
	std::stringstream ss;
	ss << Options::getInstance ()->getCeguiSkin () << "/CustomTooltip";
#ifdef CEGUI_07
	CEGUI::WindowFactoryManager::getSingleton().addFalagardWindowMapping ("SumwarsTooltip", "DefaultWindow", ss.str ().c_str (), "Falagard/Default");
#else
	CEGUI::WindowFactoryManager::getSingleton().addFalagardWindowMapping ("SumwarsTooltip", "DefaultWindow", ss.str ().c_str (), "Core/Default");
#endif

#ifdef SUMWARS_BUILD_TOOLS
	CEGUI::WindowFactoryManager::getSingleton().addFactory< CEGUI::TplWindowFactory<DebugCameraTab> >();
	CEGUI::WindowFactoryManager::getSingleton().addFactory< CEGUI::TplWindowFactory<GuiDebugTab> >();
	CEGUI::WindowFactoryManager::getSingleton().addFactory< CEGUI::TplWindowFactory<IconEditorTab> >();
	CEGUI::WindowFactoryManager::getSingleton().addFactory< CEGUI::TplWindowFactory<BenchmarkTab> >();
	CEGUI::WindowFactoryManager::getSingleton().addFactory< CEGUI::TplWindowFactory<LuaScriptTab> > ();
	//CEGUI::WindowFactoryManager::getSingleton().addFactory< CEGUI::TplWindowFactory<ReloadTab> >();
	CEGUI::WindowFactoryManager::getSingleton().addFactory< CEGUI::TplWindowFactory<TextFileEditWindow> >();
	//CONTENTEDITOR
#endif

	return true;
}

//TODO: rename function. It doesn't have to point to openAL as a sound lib.
bool Application::initOpenAL()
{
	SW_DEBUG ("initOpenAL");
	//SoundSystem::init();
	//bool err = SoundSystem::checkErrors();
	//return !err;
	try
	{
		// Get the path to use for storing data.
		// This will be relative to the user directory on the user OS.
		Ogre::String operationalPath = SumwarsHelper::getStorageBasePath() + "/" + SumwarsHelper::userPath();

		// Set the logging details
		SoundManagerLogger::setLoggerTarget (operationalPath + "/gussoundlib.log", 4);

		SoundManagerFactory::getPtr ()->Register ("openal", GOpenAl::OpenAlManagerUtil::createSoundManager);
		SoundManager::setPtr (SoundManagerFactory::getPtr ()->CreateObject ("openal"));

		// this is a temporary hack. Should be integrated into the class.
		((GOpenAl::OpenAlManagerUtil*)SoundManager::getPtr ())->setListenerPosition (0, 0, 0);

		SoundManager::getPtr ()->getMusicPlayer ()->setFadeDuration (3200);		// in milliseconds
		SoundManager::getPtr ()->getMusicPlayer ()->setRepeat (true);			// only affects initial behavior. Playlists can override this.

		// make sure the first call to elapse time won't return many seconds. 
		SoundHelper::signalSoundManager ();
	}
	catch (std::exception &e)
	{
		SW_DEBUG ("Application::initOpenAL caught exception: %s", e.what ());
		return false;
	}

	return true;
}



bool Application::createDocument()
{
	SW_DEBUG("create document\n");
	m_document = new Document();
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
	SW_DEBUG("create view\n");
	m_main_window = new MainWindow(m_ogre_root, m_cegui_system,m_window,m_document);

#ifdef SUMWARS_BUILD_TOOLS
	new DebugPanel();
	DebugPanel::getSingleton().init(false);
	new ContentEditor();
	ContentEditor::getSingleton().init(false);
#endif

	TooltipManager *mgr = new TooltipManager();
	mgr->setFadeInTime(200.0f);
	mgr->setFadeOutTime(200.0f);
	mgr->setVisibleTime(0.0f);

	// TODO: Augustin Preda (2013.06.22): investigate whether the clipboard should not be moved from the createView function.
	// It doesn't seem to make sense to have it here.

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

	//SWOgreResListener myListener;
	//Ogre::ResourceGroupManager::getSingleton().addResourceGroupListener (&myListener);

	if (datagroups & World::DATA_IMAGES)
	{
		SW_DEBUG("Loading images.");
		files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("itempictures","*.png");
		for (it = files->begin(); it != files->end(); ++it)
		{

			file = it->filename;

			CEGUIUtility::addManagedImageFromFile (file, file, (CEGUI::utf8*)"itempictures");

			updateStartScreen(0.1);
		}

		// Imagesets laden
		files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("itempictures","*.imageset");
		for (it = files->begin(); it != files->end(); ++it)
		{

			file = it->filename;

			CEGUIUtility::loadImageset (file);

			updateStartScreen(0.2);
		}
	
		files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("emotionsets","*.imageset");
		for (it = files->begin(); it != files->end(); ++it)
		{

			file = it->filename;

			CEGUIUtility::loadImageset (file);

			updateStartScreen(0.3);
		}
	
	}

	if (datagroups & World::DATA_MODELS)
	{
		SW_DEBUG("Loading models.");
		Ogre::ResourceGroupManager::getSingleton().loadResourceGroup("General");
		updateStartScreen(0.35);
	}
	if (datagroups & World::DATA_PARTICLESYSTEMS)
	{
		SW_DEBUG("Loading particlesystems.");
		//Ogre::ResourceGroupManager::getSingleton().addResourceGroupListener (&myListener);
		
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
					updateStartScreen(0.36);
				}
				catch (Ogre::Exception& e)
				{
					SW_DEBUG("failed with exception %s",e.what());
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
			SW_DEBUG("Loading ability data.");
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
		SW_DEBUG("Loading playerclasses.");
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
		SW_DEBUG("Loading items.");
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
		
#ifdef SUMWARS_BUILD_TOOLS
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
		SW_DEBUG("Loading renderinfo.");
		
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
		SW_DEBUG("Loading sound.");
		
		// Sounds
		files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("sounddata","*.xml");
		for (it = files->begin(); it != files->end(); ++it)
		{
			file = it->archive->getName();
			file += "/";
			file += it->filename;

			//SoundSystem::loadSoundData(file.c_str());
			SW_DEBUG ("Loading sound file [%s]", file.c_str ());
			SoundHelper::loadSoundGroupsFromFile (file);

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

	//Ogre::ResourceGroupManager::getSingleton().removeResourceGroupListener (&myListener);

	return true;
}



/**
 * \brief Loads the menu music and commences playing it.
 * Typically, this would be placed in the menu class, but that would not allow playing the music while the loading is performed
 * (as the menu is not yet created).
 */
void Application::loadAndPlayMenuMusic ()
{
	// Let's use shuffle and repeat by default.
	SoundManager::getPtr ()->getMusicPlayer ()->setRepeat (true);
	SoundManager::getPtr ()->getMusicPlayer ()->setShuffle (true);

	Ogre::FileInfoListPtr files;
	Ogre::FileInfoList::iterator it;
	std::string xmlFileName;

	files = Ogre::ResourceGroupManager::getSingleton ().findResourceFileInfo ("main_menu", "main_menu_playlist.xml");
	for (it = files->begin (); it != files->end (); ++it)
	{
		xmlFileName = it->archive->getName ();
		xmlFileName.append ("/");
		xmlFileName.append (it->filename);

		SW_DEBUG ("Located resource file for menu music: %s", xmlFileName.c_str ());
		if (! SoundHelper::loadPlaylistFromXMLFile (xmlFileName))
		{
			ERRORMSG ("Could not successfully load playlist from given file: %s", xmlFileName.c_str ());
		}
	}

	// Make the sound manager elapse time, so that it starts from 0 at the next operations.
	// This avoids skipping sounds when the manager is not called for a long time due to other time consuming operations.
	SoundHelper::signalSoundManager ();

	// Commence the play.
	SoundManager::getPtr ()->getMusicPlayer ()->play ();
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
		//SoundSystem::cleanupBuffers();
		//TODO: investigate if needed with new system.
	}
	
	if (datagroups & World::DATA_MUSIC)
	{
		// just stop the music to release the buffers
		//MusicManager::instance().stop();

		SoundManager::getPtr ()->getMusicPlayer ()->stop ();
	}
}

void  Application::update()
{
	if (World::getWorld() != 0)
	{
		World* world = World::getWorld();
		if (world->getDataReloadRequests() != World::DATA_NONE)
		{
			SW_DEBUG("reload request %x",world->getDataReloadRequests());
			
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
	{
		SW_DEBUG ("Update start screen while running...");
		return;
	}

	if (m_timer.getTime() < 20)
	{
		return;
	}
	SoundHelper::signalSoundManager ();

	DEBUGX("update time %f  perc: %f",m_timer.getTime(), percent);
	m_main_window->update(m_timer.getTime()/1000);
	m_main_window->setRessourceLoadingBar(percent);
	CEGUIUtility::injectTimePulse (m_timer.getTime()/1000.0);

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
		SW_DEBUG ("Application got a window closing event.");

		if (m_document != 0)
		{
			m_document->setState (Document::END_GAME);
		}
	}

	// The state was set internally, so the game will know it needs to But it needs time to do this.
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

void Application::eventOccurred(const Ogre::String &eventName, const Ogre::NameValuePairList *parameters)
{
    if (m_ogre_root)
    {
        if (eventName == "DeviceLost")
        {
        }
        else if (eventName == "DeviceCreated" || eventName == "DeviceRestored")
        {
            // handle lost device restored situations
            // DeviceCreated is called when Device is dragged from one Display to another
            // DeviceRestored is called when RenderWindow is minimized with Alt+Tab or the Windows key
            m_ogre_root->clearEventTimes();
            #if (OGRE_VERSION < ((1 << 16) | (9 << 8) | 0))
                Ogre::TexturePtr tex = Ogre::TextureManager::getSingletonPtr()->getByName("minimap_tex", "General");
            #else
                Ogre::TexturePtr tex = Ogre::TextureManager::getSingletonPtr()->getByName("minimap_tex", "General").staticCast<Ogre::Texture>();
            #endif
            tex->getBuffer()->getRenderTarget()->getViewport(0)->update();
            CEGUI::System::getSingleton().invalidateAllCachedRendering();
        }
    }
}
