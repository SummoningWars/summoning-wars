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

#include "mainwindow.h"

#include <string>
#include <sstream>

#include "gettext.h"
#include "player.h"
#include "graphicmanager.h"

#include "charinfo.h"
#include "controlpanel.h"
#include "inventory.h"
#include "skilltree.h"
#include "mainmenu.h"
#include "savegamelist.h"
#include "chatline.h"
#include "partyinfo.h"
#include "charcreate.h"
#include "questinfo.h"
#include "minimapwindow.h"
#include "tradewindow.h"
#include "optionswindow.h"
#include "networkwindows.h"
#include "worldmap.h"
#include "messageboxes.h"
#include "dialoguewindow.h"
#include "creditswindow.h"
//#include "music.h"
#include "tooltipmanager.h"

// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"

// Access the OS clipboard.
#include "clipboard.h"

// Allow the use of the sound manager.
#include "gussound.h"


#ifdef SUMWARS_BUILD_TOOLS
#include "debugpanel.h"
#include "contenteditor.h"
#endif

// Generic colour settings.
#include "tooltipsettings.h"

// Helper for sound operations
#include "soundhelper.h"

using gussound::SoundManager;


MainWindow::MainWindow(Ogre::Root* ogreroot, CEGUI::System* ceguisystem,Ogre::RenderWindow* window,Document* doc)
{
	m_ogre_root = ogreroot;
	m_cegui_system = ceguisystem;
	m_window = window;
	m_document = doc;
	m_scene_manager = m_ogre_root->getSceneManager("DefaultSceneManager");

	// Initialisieren
	bool res = init();
	if (res==false)
	{
		ERRORMSG("GUI erzeugen fehlgeschlagen");
	}
	
	m_key =0;
	m_highlight_id =0;
	m_ready_to_start = false;
}

bool MainWindow::init()
{
	bool result = true;

	m_ceguiSkinName = Options::getInstance ()->getCeguiSkin ();
	SW_DEBUG ("Main Window (init): Cegui skin name found in options as: [%s]", m_ceguiSkinName.c_str ());

	// Set-up the main menu.
	SW_DEBUG ("Setting up the root window.");
	result &= setupRootWindow();

	// Initialize the input devices
	SW_DEBUG ("Initializing input devices.");
	result &= initInputs();

	SW_DEBUG ("Initializing graphics.");
	GraphicManager::init();

	// Set-up the main menu.
	SW_DEBUG ("Initializing main menu.");
	result &= setupMainMenu();
	CEGUIUtility::addChildWidget (m_root_window, m_main_menu);

	// Once the main menu is created, set it as the root sheet. It shall be used in the game screen set-up.
	CEGUIUtility::setRootSheet (CEGUI::System::getSingletonPtr(), m_root_window);

	// Set-up the main game window/screen
	SW_DEBUG ("Initializing main game screen.");
	result &= setupGameScreen();
	CEGUIUtility::addChildWidget (m_root_window, m_game_screen);

	// Dump the contents to the log for later.
	//CEGUIUtility::dumpFullWindowToLog (m_root_window);

	return result;
}


MainWindow::~MainWindow()
{
	m_ois->destroyInputObject(m_mouse);
	m_ois->destroyInputObject(m_keyboard);
	OIS::InputManager::destroyInputSystem(m_ois);

	// Dynamisch angelegte Objekte loeschen
	delete m_scene;
}

bool MainWindow::initInputs()
{
	DEBUGX("init inputs");
	OIS::ParamList pl;
	// Handle fuer das aktuelle Fenster
	unsigned long hWnd;
	m_window->getCustomAttribute("WINDOW", &hWnd);
	// OIS mitteilen fuer welches Fenster die Eingaben abgefangen werden sollen
	pl.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

	if(!Options::getInstance()->getGrabMouseInWindowedMode() && !m_window->isFullScreen())
	{
	#ifndef WIN32
		pl.insert( std::make_pair(std::string("x11_mouse_grab"), std::string("false") ) );
		pl.insert( std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
	#else
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
	#endif
	}

	m_ois = OIS::InputManager::createInputSystem( pl );

	// Maus initialisieren
	m_mouse = static_cast<OIS::Mouse*>(m_ois->createInputObject( OIS::OISMouse, true ));
	m_mouse->setEventCallback(this);
	m_mouse->getMouseState().width = (int) m_window->getWidth();
	m_mouse->getMouseState().height = (int) m_window->getHeight();

	// Tastatur initialisieren
	m_keyboard = static_cast<OIS::Keyboard*>(m_ois->createInputObject( OIS::OISKeyboard, true));
	m_keyboard->setEventCallback(this);
	m_keyboard->setTextTranslation (OIS::Keyboard::Unicode);
	return true;
}


bool MainWindow::setupRootWindow ()
{
	try
	{
		CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();

		// Oberstes Fenster der Hauptmenue Schicht
		m_root_window = win_mgr.createWindow("DefaultWindow", "SW");
		m_root_window->setMousePassThroughEnabled (true);
	}
	catch (CEGUI::Exception & e)
	{
		ERRORMSG("Error message: %s",e.getMessage().c_str());
		return false;
	}

	return true;
}


bool MainWindow::setupMainMenu()
{
	try
	{
		CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();

		// Oberstes Fenster der Hauptmenue Schicht
		m_main_menu = win_mgr.createWindow("DefaultWindow", "MainMenu");

		std::string layoutFile ("startscreen.layout");
		CEGUI::Window* start_screen_holder = CEGUIUtility::loadLayoutFromFile (layoutFile);

		SW_DEBUG ("Loaded layout file [%s]", layoutFile.c_str ());

		if (start_screen_holder)
		{
			CEGUIUtility::addChildWidget (m_main_menu, start_screen_holder);
			std::string widgetName;
			CEGUI::Window* item;

			//widgetName (CEGUIUtility::getNameForWidget("MainMenu/StartScreenRoot"));
			//item = CEGUIUtility::getWindowForLoadedLayoutEx (start_screen_holder, widgetName);
			//item->setMousePassThroughEnabled (true);
			
			widgetName = CEGUIUtility::getNameForWidget("StartScreenImage");
			item = CEGUIUtility::getWindowForLoadedLayoutEx (start_screen_holder, widgetName);
			item->setMousePassThroughEnabled (true);

			widgetName = CEGUIUtility::getNameForWidget("LoadRessourcesProgressBar");
			item = CEGUIUtility::getWindowForLoadedLayoutEx (start_screen_holder, widgetName);
			item->setMousePassThroughEnabled (true);
		}
		else
		{
            ERRORMSG ("WARNING: Failed to load [%s]", layoutFile.c_str ());
		}

		SW_DEBUG ("Creating credits window");

		CreditsWindow* crd = new CreditsWindow (m_document, m_ceguiSkinName);
		m_sub_windows["CreditsWindow"] = crd;
		crd->getCEGUIWindow ()->setMousePassThroughEnabled (true);
		CEGUIUtility::addChildWidget (m_main_menu, crd->getCEGUIWindow());

		SW_DEBUG ("Creating main menu");

		Window * wnd = new MainMenu(m_document, m_ceguiSkinName);
		m_sub_windows["MainMenu"] = wnd;
		wnd->getCEGUIWindow ()->setMousePassThroughEnabled (true);
		CEGUIUtility::addChildWidget (m_main_menu, wnd->getCEGUIWindow());

		SW_DEBUG ("Creating character screen");

		wnd = new CharCreate (m_document, m_ceguiSkinName);
		m_sub_windows["CharCreate"] = wnd;
		wnd->getCEGUIWindow ()->setMousePassThroughEnabled (true);
		CEGUIUtility::addChildWidget (m_main_menu, wnd->getCEGUIWindow());
		
		SW_DEBUG ("Creating options window");

		wnd = new OptionsWindow(m_document,m_keyboard, m_ceguiSkinName);
		m_sub_windows["Options"] = wnd;
		wnd->getCEGUIWindow ()->setMousePassThroughEnabled (true);
		CEGUIUtility::addChildWidget (m_root_window, wnd->getCEGUIWindow());
		
		SW_DEBUG ("Creating game hosting screen");

		wnd = new HostGameWindow(m_document);
		m_sub_windows["HostGame"] = wnd;
		wnd->getCEGUIWindow ()->setMousePassThroughEnabled (true);
		CEGUIUtility::addChildWidget (m_main_menu, wnd->getCEGUIWindow());
		
		SW_DEBUG ("Creating game joining screen");

		wnd = new JoinGameWindow(m_document);
		m_sub_windows["JoinGame"] = wnd;
		wnd->getCEGUIWindow ()->setMousePassThroughEnabled (true);
		CEGUIUtility::addChildWidget (m_main_menu, wnd->getCEGUIWindow());
		
	// Verbinden mit dem Document
	}
	catch (CEGUI::Exception & e)
	{
		ERRORMSG("Error message: %s",e.getMessage().c_str());
		return false;
	}
	return true;
}

void MainWindow::update(float time)
{
	// Eingaben abfangen
	m_mouse->capture();
	m_keyboard->capture();

	TooltipManager::getSingleton().update(time);

#ifdef SUMWARS_BUILD_TOOLS
	DebugPanel::getSingleton().update(m_keyboard, m_mouse);
	ContentEditor::getSingleton().update(m_keyboard, m_mouse);
#endif

	if (m_document->getGUIState()->m_left_mouse_pressed || m_document->getGUIState()->m_right_mouse_pressed)
	{
		Vector pos = getIngamePos(m_mouse->getMouseState().X.abs,m_mouse->getMouseState().Y.abs);
		m_document->getGUIState()->m_clicked = pos;
	}
	
	// Tastenwiederholung erzeugen
	if (m_key !=0)
	{
		if (m_key_repeat)
		{
			if ( m_key_repeat_timer.getTime()>60)
			{
				m_key_repeat_timer.start();
				CEGUIUtility::injectChar (m_cegui_system, m_key);
				
				// Sonderbehandlung fuer Backspace
				if (m_key == 8)
				{
					CEGUIUtility::injectKeyDown (m_cegui_system, OIS::KC_BACK);
				}
			}
		}
		else
		{
			if ( m_key_repeat_timer.getTime()>400)
			{
				m_key_repeat_timer.start();
				m_key_repeat = true;
			}
		}
	}

	
	if (m_document->getModified() & Document::SAVEGAME_MODIFIED)
	{
		m_scene->updateCharacterView();
		m_document->setModified(m_document->getModified() & (~Document::SAVEGAME_MODIFIED));
	}
	
	// Sprache anpassen
	if (Gettext::getLocaleChanged())
	{
		DEBUGX("language changed");
		std::map<std::string, Window*>::iterator it;
		for (it= m_sub_windows.begin(); it != m_sub_windows.end(); ++it)
		{
			it->second->updateTranslation();
		}
	}
	
	// Flags, welche Fenster gezeigt werden
	int wflags = m_document->getGUIState()->m_shown_windows;

	
	
	// Testen ob die Anzeige der Fenster geaendert werden muss
	if (m_document->getModified() & Document::GUISHEET_MODIFIED)
	{
		if (m_document->getGUIState()->m_sheet ==  Document::MAIN_MENU)
		{
			updateMainMenu();
			//CEGUIUtility::setRootSheet (m_cegui_system, m_main_menu);
			m_main_menu->show ();
			m_main_menu->activate ();
			m_game_screen->hide ();

			//MusicManager::instance().stop();
			//SoundManager::getPtr ()->getMusicPlayer ()->stop ();
		}

		if (m_document->getGUIState()->m_sheet ==  Document::GAME_SCREEN)
		{
			//CEGUIUtility::setRootSheet (m_cegui_system, m_game_screen);
			m_game_screen->show ();
			m_game_screen->activate ();
			m_main_menu->hide ();

			//MusicManager::instance().stop();
			//SoundManager::getPtr ()->getMusicPlayer ()->stop ();
			
			// one silent update of the belt
			// to avoid silly sounds on startup
			static_cast<ItemWindow*>(m_sub_windows["ControlPanel"])->setSilent(true);
			m_sub_windows["ControlPanel"]->update();
			static_cast<ItemWindow*>(m_sub_windows["ControlPanel"])->setSilent(false);
			
		}
		m_document->setModified(m_document->getModified() & (~Document::GUISHEET_MODIFIED));
	}
	
	// Testen ob Anzeige der Subfenster geaendert werden muss
	if (m_document->getModified() & Document::WINDOWS_MODIFIED)
	{
		DEBUGX("new shown windows %x",wflags);

		/*// Auswahlliste Savegames  anzeigen wenn entsprechendes Flag gesetzt
		CEGUI::FrameWindow* savelist = (CEGUI::FrameWindow*) CEGUIUtility::getWindow ("SavegameMenu");
		if (wflags & Document::SAVEGAME_LIST)
		{
			
			savelist->setVisible(true);
			m_sub_windows["SavegameList"]->update();
		}
		else
		{
			savelist->setVisible(false);
		}*/

		// Menu Spielstart anzeigen wenn entsprechendes Flag gesetzt
		CEGUI::String widgetName = CEGUIUtility::getNameForWidget("MainMenu/MainMenuRoot");
		CEGUI::FrameWindow* start_menu = static_cast<CEGUI::FrameWindow*> (CEGUIUtility::getWindow (widgetName));

		MainMenu* main_menu = static_cast<MainMenu*>(m_sub_windows["MainMenu"]);
		if (wflags & Document::START_MENU)
		{
			if (start_menu)
			{
				start_menu->setVisible (true);
			}
			if (main_menu)
			{
				main_menu->setSavegameListVisible (true);
			}
		}
		else
		{
			if (wflags & Document::CHAR_CREATE)
			{
				if (start_menu)
				{
					start_menu->setVisible(true);
				}
				if (main_menu)
				{
					main_menu->setSavegameListVisible(false);
				}
			}
			else
			{
				if (start_menu)
				{
					start_menu->setVisible(false);
				}
			}
		}
		
		CEGUI::FrameWindow* credits = (CEGUI::FrameWindow*) CEGUIUtility::getWindow ("MainMenu/CreditsWindow_Holder");
		if (wflags & Document::CREDITS)
		{
			credits->setVisible(true);
		}
		else
		{
			credits->setVisible(false);
		}
		
		CEGUI::FrameWindow* host_game = (CEGUI::FrameWindow*) CEGUIUtility::getWindow ("MainMenu/HostGameWindow_Holder");
		if (wflags & Document::HOST_GAME)
		{
			m_sub_windows["HostGame"]->update();
			host_game->setVisible(true);
		}
		else
		{
			host_game->setVisible(false);
		}
		
		CEGUI::FrameWindow* join_game = (CEGUI::FrameWindow*) CEGUIUtility::getWindow ("MainMenu/JoinGameWindow_Holder");
		if (wflags & Document::JOIN_GAME)
		{
			m_sub_windows["JoinGame"]->update();
			join_game->setVisible(true);
		}
		else
		{
			join_game->setVisible(false);
		}
		
		CEGUI::FrameWindow* options = 0;
		if (CEGUIUtility::isWindowPresent ("OptionsWindow_Holder"))
		{
			options = (CEGUI::FrameWindow*) CEGUIUtility::getWindow ("OptionsWindow_Holder");
		}
		else
		{
			options = (CEGUI::FrameWindow*) CEGUIUtility::getWindow ("OptionsWindow");
		}
		if (wflags & Document::OPTIONS)
		{
			static_cast<OptionsWindow*>(m_sub_windows["Options"])->reset();
			options->setVisible(true);
			options->activate ();
		}
		else
		{
			options->setVisible(false);
		}
		
		
		CEGUI::FrameWindow* char_create = (CEGUI::FrameWindow*) CEGUIUtility::getWindow ("MainMenu/CharCreate");
		if (wflags & Document::CHAR_CREATE)
		{
			if (!char_create->isVisible())
			{	
				static_cast<CharCreate*>(m_sub_windows["CharCreate"])->updateClassList();
			}
			char_create->setVisible(true);
			char_create->activate();
		}
		else
		{
			char_create->setVisible(false);
		}
        
		CEGUI::FrameWindow* error_dialog = (CEGUI::FrameWindow*) CEGUIUtility::getWindow ("ErrorDialogWindow");
        if (wflags & Document::MESSAGE)
        {
            m_sub_windows["errorDialog"]->update();
            error_dialog->setVisible(true);
			error_dialog->activate();
            //error_dialog->setModalState(true);
        }
        else
        {
            error_dialog->setVisible(false);
            //error_dialog->setModalState(false);
        }

		if (m_document->getGUIState()->m_sheet ==  Document::GAME_SCREEN)
		{
			// Charinfo anzeigen wenn entsprechendes Flag gesetzt
			CEGUI::FrameWindow* char_info = (CEGUI::FrameWindow*) CEGUIUtility::getWindow ("GameScreen/CharInfo_Holder");
			if (wflags & Document::CHARINFO)
			{
				char_info->setVisible(true);
			}
			else
			{
				char_info->setVisible(false);
			}
	
			// Inventar anzeigen wenn entsprechendes Flag gesetzt
			CEGUI::FrameWindow* inventory = (CEGUI::FrameWindow*) CEGUIUtility::getWindow ("GameScreen/Inventory_Holder");
			if (wflags & Document::INVENTORY)
			{
				if (!inventory->isVisible())
				{
					inventory->setVisible(true);
					// make one silent update to avoid that new inventory items play sounds
					static_cast<ItemWindow*>(m_sub_windows["Inventory"])->setSilent(true);
					m_sub_windows["Inventory"]->update();
					static_cast<ItemWindow*>(m_sub_windows["Inventory"])->setSilent(false);
				}
			}
			else
			{
				inventory->setVisible(false);
				static_cast<ItemWindow*>(m_sub_windows["ControlPanel"])->setSilent(false);
			}
			
			// show/hide control panel
			CEGUI::FrameWindow* control_panel = (CEGUI::FrameWindow*) CEGUIUtility::getWindow ("GameScreen/ControlPanel_Holder");
			if (wflags & Document::CONTROL_PANEL)
			{
				if (!control_panel->isVisible())
				{
					control_panel->setVisible(true);
					
					// one silent update of the belt
					// to avoid silly sounds on startup
					static_cast<ItemWindow*>(m_sub_windows["ControlPanel"])->setSilent(true);
					m_sub_windows["ControlPanel"]->update();
					static_cast<ItemWindow*>(m_sub_windows["ControlPanel"])->setSilent(false);
				}
			}
			else
			{
				control_panel->setVisible(false);
			}
			
			// QuestInfo anzeigen wenn entsprechendes Flag gesetzt
			CEGUI::FrameWindow* quest_info = (CEGUI::FrameWindow*) CEGUIUtility::getWindow ("GameScreen/QuestInfo_Holder");
			if (wflags & Document::QUEST_INFO)
			{
				quest_info->setVisible(true);
				m_sub_windows["QuestInfo"]->update();
			}
			else
			{
				quest_info->setVisible(false);
			}
			
			CEGUI::FrameWindow* minimap = (CEGUI::FrameWindow*) CEGUIUtility::getWindow ("GameScreen/MinimapWindow_Holder");
			if (wflags & Document::MINIMAP)
			{
				minimap->setVisible(true);
			}
			else
			{
				minimap->setVisible(false);
			}
			
			CEGUI::FrameWindow* trade = (CEGUI::FrameWindow*) CEGUIUtility::getWindow ("GameScreen/TradeWindow_Holder");
			if (wflags & Document::TRADE)
			{
								
				if (!trade->isVisible())
				{
					static_cast<TradeWindow*>(m_sub_windows["Trade"])->reset();
					trade->setVisible(true);
					
					// make one silent update to avoid that new inventory items play sounds
					static_cast<ItemWindow*>(m_sub_windows["Trade"])->setSilent(true);
					m_sub_windows["Trade"]->update();
					static_cast<ItemWindow*>(m_sub_windows["Trade"])->setSilent(false);
				}
			}
			else
			{
				trade->setVisible(false);
			}
			
			CEGUI::FrameWindow* worldmap = (CEGUI::FrameWindow*) CEGUIUtility::getWindow ("GameScreen/WorldmapWindow_Holder");
			if (wflags & Document::WORLDMAP)
			{
				m_sub_windows["Worldmap"]->update();
				worldmap->setVisible(true);
			}
			else
			{
				worldmap->setVisible(false);
			}
			
			CEGUI::FrameWindow* save_exit = (CEGUI::FrameWindow*) CEGUIUtility::getWindow ("GameScreen/SaveExitWindow");
			if (wflags & Document::SAVE_EXIT)
			{
				save_exit->setVisible(true);
				save_exit->setModalState(true);
			}
			else
			{
				save_exit->setVisible(false);
				save_exit->setModalState(false);
			}
			
			// Chat Fenster anzeigen wenn entsprechendes Flag gesetzt
			CEGUI::FrameWindow* chat_window = (CEGUI::FrameWindow*) CEGUIUtility::getWindow ("GameScreen/Chatline");
			if (wflags & Document::CHAT)
			{
				// Fokus setzen, wenn das Fenster gerade geoeffnet wurde
				if (!chat_window->isVisible())
				{
					CEGUI::Editbox* chatline;
					chatline = static_cast<CEGUI::Editbox*>(CEGUIUtility::getWindow ("GameScreen/Chatline"));
					chat_window->setVisible(true);
					chatline->activate();
				}
				
				chat_window->setVisible(true);
			}
			else
			{
				chat_window->setVisible(false);
				// Chatzeile deaktivieren
				CEGUI::Editbox* chatline;
				chatline = static_cast<CEGUI::Editbox*>(CEGUIUtility::getWindow ("GameScreen/Chatline"));
				chatline->deactivate();
			}
			
			CEGUI::FrameWindow* party_info = (CEGUI::FrameWindow*) CEGUIUtility::getWindow ("GameScreen/PartyInfo");
			if (wflags & Document::PARTY)
			{
				party_info->setVisible(true);
			}
			else
			{
				party_info->setVisible(false);
			}
	
			if (m_document->getLocalPlayer()!=0)
			{
				// Skilltree anzeigen wenn entsprechendes Flag gesetzt
				
				CEGUI::TabControl* skilltree = (CEGUI::TabControl*) CEGUIUtility::getWindow ("GameScreen/Skilltree_Holder");
				if (wflags & Document::SKILLTREE)
				{
					skilltree->setVisible(true);
				}
				else
				{
					skilltree->setVisible(false);
				}
				
			}
		}
		
		m_document->setModified(m_document->getModified() & ~Document::WINDOWS_MODIFIED);
	}
	
	if (m_document->getGUIState()->m_sheet ==  Document::MAIN_MENU)
	{
		updateMainMenu();
	}
	
	m_sub_windows["CreditsWindow"]->update();
	
	if (wflags & Document::OPTIONS)
	{
		// Fenster Optionen aktualisieren
		m_sub_windows["Options"]->update();
	}

	// Musik aktualisieren
	updateSound();

	// Objekte aus dem Dokument darstellen
	if (m_document->getLocalPlayer()!=0 && m_document->getLocalPlayer()->getRegion()!=0)
	{
		// ObjectInfo aktualisieren
		updateObjectInfo();
		
		updateItemInfo();

		updateRegionInfo();
		updateChatContent();

		updateFloatingText();
		
		// Szene aktualisieren
		m_scene->update(time);
		
		// Bild am Curso aktualisieren
		updateCursorItemImage();
		
		// Informationen ueber Partymitglieder aktualisieren
		updatePartyInfo();

		if (wflags & Document::CHARINFO)
		{
			// Fenster CharacterInfo aktualisieren
			m_sub_windows["CharInfo"]->update();
		}

		if (wflags & Document::CHAT)
		{
			// Fenster CharacterInfo aktualisieren
			m_sub_windows["Chatline"]->update();
		}

		if (wflags & Document::MINIMAP)
		{
			// Fenster Minimap aktualisieren
			m_sub_windows["Minimap"]->update();
		}
		
		
		// Steuerleiste aktualisieren
		m_sub_windows["ControlPanel"]->update();


		if (wflags & Document::INVENTORY)
		{
			// Fenster Inventar aktualisieren
			m_sub_windows["Inventory"]->update();
		}
		if (wflags & Document::SKILLTREE)
		{
			// Skilltree aktualisieren
			m_sub_windows["SkillTree"]->update();
		}
		
		if (wflags & Document::PARTY)
		{
			// Skilltree aktualisieren
			m_sub_windows["PartyInfo"]->update();
		}
		
		if (wflags & Document::TRADE)
		{
			// Skilltree aktualisieren
			m_sub_windows["Trade"]->update();
		}
		
		m_sub_windows["DialogueWindow"]->update();
		
		// + Buttons fuer Levelup aktualisieren
		bool vis = false;
		CEGUI::PushButton* btn;
		btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindow ("GameScreen/CharInfoUpgradeButton"));
		if (!(wflags & Document::CHARINFO) && m_document->getLocalPlayer()->getAttributePoints() >0)
		{
			vis = true;
		}
		
		if (btn->isVisible() != vis)
		{
			btn->setVisible(vis);
		}
		
		vis = false;
		btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindow ("GameScreen/SkillUpgradeButton"));
		if (!(wflags & Document::SKILLTREE) && m_document->getLocalPlayer()->getSkillPoints() >0)
		{
			vis = true;
		}
		
		if (btn->isVisible() != vis)
		{
			btn->setVisible(vis);
		}
		
		/*
		vis = false;
		CEGUI::Window* label = CEGUIUtility::getWindow ("MinimapImage");
		if (wflags & Document::MINIMAP)
		{
			vis = true;
		}
		if (label->isVisible() != vis)
		{
			if (vis)
			{
				label->setProperty("Image", CEGUIUtility::getImageNameWithSkin ("minimap", "minimap_img")); 
			}
			label->setVisible(vis);
		}
		*/
	}
}


bool MainWindow::setupGameScreen()
{
	DEBUGX("setup game screen");
	try
	{
		// Szene erzeugen
		m_scene = new Scene(m_document,m_window);

		// GUI Elemente erzeugen
		CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();

		// Oberstes Fenster der Hauptmenue Schicht
    m_game_screen =  win_mgr.createWindow("DefaultWindow", "GameScreen");
    m_game_screen->setMousePassThroughEnabled(true);

		// Kontrollleiste anlegen
		setupControlPanel();

		// Inventar Fenster anlegen
		setupInventory();

		// CharInfo Fenster anlegen
		setupCharInfo();

		// Skilltree Fenster anlegen
		setupSkilltree();
		
		setupQuestInfo();

		// Leiste fuer Item/Objekt-Info anlegen
		setupObjectInfo();
		
		setupItemInfo();
		setupRegionInfo();
		setupSaveExitWindow();
        setupErrorDialogWindow();
		setupChatContent();
		
		// Chatfenster anlegen
		setupChatWindow();
		
		// Fenster fuer Partyinfos
		setupPartyInfo();
		
		// Bild fuer das Item am Cursor
		setupCursorItemImage();
		
		setupMinimap();
		
		setupTrade();
		
		setupWorldmap();

		setupDialogWindow ();
		
		CEGUI::Window* label;
		CEGUI::Window* temporary_parent = CEGUIUtility::getWindow ("MainMenu/MainMenuRoot");
		label = win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "StaticImage"), "CharacterPreviewImage");
		CEGUIUtility::addChildWidget (temporary_parent, label);

		label->setProperty("FrameEnabled", "false");
		label->setProperty("BackgroundEnabled", "false");
		label->setPosition(CEGUI::UVector2(cegui_reldim(0.5f), cegui_reldim( 0.15)));
		CEGUIUtility::setWidgetSizeRel (label, 0.5f, 0.7f);
		label->setMousePassThroughEnabled(true);
		label->setInheritsAlpha(false);
#ifdef CEGUI_07
		label->setProperty("Image", CEGUIUtility::getImageNameWithSkin ("character", "character_img")); 
#else
		label->setProperty("Image", "character_img"); 
#endif
		label->setVisible(false);
		
		label = win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "StaticText"), "CharacterPreviewBackground");
		CEGUIUtility::addChildWidget (temporary_parent, label);
		label->setProperty("FrameEnabled", "false");
		label->setProperty("BackgroundEnabled", "true");
		label->setPosition(CEGUI::UVector2(cegui_reldim(0.5f), cegui_reldim( 0.0)));
		CEGUIUtility::setWidgetSizeRel (label, 0.5f, 1.0f);
		label->setMousePassThroughEnabled(true);
		label->setVisible(false);
		label->moveToBack();
	}
	catch (CEGUI::Exception & e)
	{
		ERRORMSG("Error message: %s",e.getMessage().c_str());
		return false;
	}

	return true;
}

void MainWindow::setupControlPanel()
{
	Window* wnd = new ControlPanel(m_document);
	m_sub_windows["ControlPanel"] = wnd;
	
	
	// Charakterinfo anfangs ausblenden
	CEGUIUtility::addChildWidget (m_game_screen, wnd->getCEGUIWindow());
	
	// zwei weitere Buttons die Skilltree bzw Charinfo oeffnen, wenn Punkte zu verteilen sind
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::PushButton* btn;
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "ImageButton"), "CharInfoUpgradeButton"));
	CEGUIUtility::addChildWidget (m_game_screen, btn);
	btn->setPosition(CEGUI::UVector2(cegui_reldim(0.02f), cegui_reldim( 0.65f)));
	CEGUIUtility::setWidgetSizeRel (btn, 0.05f, 0.07f);
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonCharInfoClicked, static_cast<ControlPanel*>(wnd)));
	btn->setVisible(false);
	if (btn->isPropertyPresent ("NormalImage"))
	{
		btn->setProperty("NormalImage", CEGUIUtility::getImageNameWithSkin ("CharScreen", "PlusBtnReleased"));
	}
	if (btn->isPropertyPresent ("DisabledImage"))
	{
		btn->setProperty("DisabledImage", CEGUIUtility::getImageNameWithSkin ("CharScreen", "PlusBtnReleased")); 	 
	}
	if (btn->isPropertyPresent ("HoverImage"))
	{
		btn->setProperty("HoverImage", CEGUIUtility::getImageNameWithSkin ("CharScreen", "PlusBtnReleased")); 	 
	}
	if (btn->isPropertyPresent ("PushedImage"))
	{
		btn->setProperty("PushedImage", CEGUIUtility::getImageNameWithSkin ("CharScreen", "PlusBtnPressed"));
	}
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "ImageButton"), "SkillUpgradeButton"));
	CEGUIUtility::addChildWidget (m_game_screen, btn);

	btn->setPosition(CEGUI::UVector2(cegui_reldim(0.92f), cegui_reldim( 0.65f)));
	CEGUIUtility::setWidgetSizeRel (btn, 0.05f, 0.07f);
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonSkilltreeClicked, static_cast<ControlPanel*>(wnd)));
	btn->setVisible(false);
	
	
	if (btn->isPropertyPresent ("NormalImage"))
	{
		btn->setProperty("NormalImage", CEGUIUtility::getImageNameWithSkin ("CharScreen", "PlusBtnReleased"));
	}
	if (btn->isPropertyPresent ("DisabledImage"))
	{
		btn->setProperty("DisabledImage", CEGUIUtility::getImageNameWithSkin ("CharScreen", "PlusBtnReleased"));
	}
	if (btn->isPropertyPresent ("HoverImage"))
	{
		btn->setProperty("HoverImage", CEGUIUtility::getImageNameWithSkin ("CharScreen", "PlusBtnReleased"));
	}
	if (btn->isPropertyPresent ("PushedImage"))
	{
		btn->setProperty("PushedImage", CEGUIUtility::getImageNameWithSkin ("CharScreen", "PlusBtnPressed"));
	}

}

void MainWindow::setupCharInfo()
{
	Window* wnd = new CharInfo(m_document);
	m_sub_windows["CharInfo"] = wnd;
	
	
	// Charakterinfo anfangs ausblenden
	CEGUIUtility::addChildWidget (m_game_screen, wnd->getCEGUIWindow());

	wnd->getCEGUIWindow()->setVisible(false);
}

void MainWindow::setupDialogWindow()
{
	Window* wnd = new DialogueWindow(m_document, m_scene, m_ceguiSkinName);
	m_sub_windows["DialogueWindow"] = wnd;

	// Connect the new widget to the game screen.
	CEGUIUtility::addChildWidget (m_game_screen, wnd->getCEGUIWindow());

	wnd->getCEGUIWindow()->setVisible(false);
}

void MainWindow::setupInventory()
{
	
	Window* wnd = new Inventory(m_document);
	m_sub_windows["Inventory"] = wnd;
	
	// Inventar anfangs ausblenden
	CEGUIUtility::addChildWidget (m_game_screen, wnd->getCEGUIWindow());
	wnd->getCEGUIWindow()->setVisible(false);
	

}

void MainWindow::setupQuestInfo()
{
	Window* wnd = new QuestInfo (m_document, m_ceguiSkinName);
	m_sub_windows["QuestInfo"] = wnd;
	
	// anfangs ausblenden
	CEGUIUtility::addChildWidget (m_game_screen, wnd->getCEGUIWindow());
	wnd->getCEGUIWindow()->setVisible(false);
}


void MainWindow::setupSkilltree()
{
	SkillTree* wnd = new SkillTree (m_document ,m_keyboard, m_ceguiSkinName);
	m_sub_windows["SkillTree"] = wnd;
	
	// Skilltree anfangs ausblenden

	CEGUIUtility::addChildWidget (m_game_screen, wnd->getCEGUIWindow());
	wnd->getCEGUIWindow()->setVisible(false);

}

void MainWindow::setupChatWindow()
{
	Window* wnd = new ChatLine(m_document, m_ceguiSkinName);
	m_sub_windows["Chatline"] = wnd;
	
	
	// Inventar anfangs ausblenden
	CEGUIUtility::addChildWidget (m_game_screen, wnd->getCEGUIWindow());
	wnd->getCEGUIWindow()->setVisible(false);
}

void MainWindow::setupCursorItemImage()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	m_custom_cursor = win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "StaticImage"), "CursorItemImage");
	CEGUIUtility::addChildWidget (m_root_window, m_custom_cursor);

	m_custom_cursor->setProperty("FrameEnabled", "false");
	m_custom_cursor->setProperty("BackgroundEnabled", "false");
	m_custom_cursor->setPosition(CEGUI::UVector2(cegui_reldim(0.05f), cegui_reldim( 0.05)));
	CEGUIUtility::setWidgetSizeRel (m_custom_cursor, 0.04f, 0.06f);

	// Just use a default image.
	m_custom_cursor->setProperty ("Image", "Portrait/Portrait");

	m_custom_cursor->setVisible(false);
	m_custom_cursor->setAlwaysOnTop(true);
	m_custom_cursor->setMousePassThroughEnabled(true);
	m_custom_cursor->setID(0);
}

void MainWindow::setupMinimap()
{
	Window* wnd = new MinimapWindow (m_document, m_ceguiSkinName);
	m_sub_windows["Minimap"] = wnd;
	CEGUIUtility::addChildWidget (m_game_screen, wnd->getCEGUIWindow());
}

bool MainWindow::setupObjectInfo()
{
	DEBUGX("setup object info");

	// Leiste fuer Informationen
	CEGUI::Window* monster_health_holder = CEGUIUtility::loadLayoutFromFile ("monsterhealthbar.layout");
	if (monster_health_holder)
	{
		CEGUIUtility::addChildWidget (m_game_screen, monster_health_holder);
	}
	
	return true;
}

bool MainWindow::setupItemInfo()
{
	DEBUGX("setup object info");

	// Fenstermanager
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label;

	// Leiste fuer Informationen
	label = win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "StaticText"), "ItemInfoLabel");
	CEGUIUtility::addChildWidget (m_game_screen, label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "true");
	if (label->isPropertyPresent ("BackgroundColours"))
	{
		label->setProperty("BackgroundColours", "tl:77000000 tr:77000000 bl:77000000 br:77000000"); 
	}
	else if (label->isPropertyPresent ("BackgroundColour"))
	{
		label->setProperty("BackgroundColour", "B2000000"); 
	}
	label->setPosition(CEGUI::UVector2(cegui_reldim(0.2f), cegui_reldim( 0.02f)));
	CEGUIUtility::setWidgetSizeRel (label, 0.08f, 0.03f);
	label->setText("");
	label->setVisible(false);
	label->setAlpha(0.9);
	label->setProperty("MousePassThroughEnabled","true");

	return true;
}


bool MainWindow::setupPartyInfo()
{
	
	Window* wnd = new PartyInfo(m_document, m_ceguiSkinName);
	m_sub_windows["PartyInfo"] = wnd;
	
	// PartyInfo anfangs ausblenden
	CEGUIUtility::addChildWidget (m_game_screen, wnd->getCEGUIWindow());
	wnd->getCEGUIWindow()->setVisible(false);
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::Window* img;
	CEGUI::ProgressBar* bar;
	
	std::ostringstream stream;
	
	for (int i=0; i<7; i++)
	{
		stream.str("");
		stream << "PartyMemberImage";
		stream << i;
		
		img = win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "StaticImage"), stream.str());
		CEGUIUtility::addChildWidget (m_game_screen, img);
		img->setProperty("FrameEnabled", "true");
		img->setProperty("BackgroundEnabled", "true");
		img->setPosition(CEGUI::UVector2(cegui_reldim(0.01f), cegui_reldim( 0.01f + 0.1f *i)));
		CEGUIUtility::setWidgetSizeRel (img, 0.05f, 0.07f);
		img->setID(i);
		img->setVisible(false);
		img->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&MainWindow::onPartyMemberClicked, this));
		
		stream.str("");
		stream << "PartyMemberHealthBar";
		stream << i;
		
		bar = static_cast<CEGUI::ProgressBar*>(win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "ProgressBar"), stream.str()));
		CEGUIUtility::addChildWidget (m_game_screen, bar);
		bar->setPosition(CEGUI::UVector2(cegui_reldim(0.01f), cegui_reldim( 0.08f+ 0.1f *i)));
		CEGUIUtility::setWidgetSizeRel (bar, 0.05f, 0.02f);
		bar->setProgress(1.0);
		bar->setVisible(false);
	}
	
	return true;
}

void MainWindow::setupTrade()
{
	Window* wnd = new TradeWindow(m_document, m_ceguiSkinName);
	m_sub_windows["Trade"] = wnd;
	
	
	// Inventar anfangs ausblenden
	CEGUIUtility::addChildWidget (m_game_screen, wnd->getCEGUIWindow());
	wnd->getCEGUIWindow()->setVisible(false);
}


void MainWindow::setupWorldmap()
{
	Window* wnd = new Worldmap (m_document, m_ceguiSkinName);
	m_sub_windows["Worldmap"] = wnd;
	
	
	// Inventar anfangs ausblenden
	CEGUIUtility::addChildWidget (m_game_screen, wnd->getCEGUIWindow());
	wnd->getCEGUIWindow()->setVisible(false);
}

void MainWindow::setupRegionInfo()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::Window* label;
	label = win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "StaticText"), "RegionInfoLabel");
	CEGUIUtility::addChildWidget (m_game_screen, label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "false");
	label->setProperty("HorzFormatting", "HorzCentred");
	label->setPosition(CEGUI::UVector2(cegui_reldim(0.25f), cegui_reldim( 0.05)));
	CEGUIUtility::setWidgetSizeRel (label, 0.5f, 0.06f);
	label->setVisible(false);
	label->setAlwaysOnTop(true);
	label->setMousePassThroughEnabled(true);
	label->setID(0);
}

void MainWindow::setupSaveExitWindow()
{
	Window* wnd = new SaveExitWindow(m_document);
	m_sub_windows["saveExit"] = wnd;
	
	// Inventar anfangs ausblenden
	CEGUIUtility::addChildWidget (m_game_screen, wnd->getCEGUIWindow ());
	wnd->getCEGUIWindow()->setVisible(false);
}

void MainWindow::setupErrorDialogWindow()
{
	Window* wnd = new ErrorDialogWindow (m_document);
	m_sub_windows["errorDialog"] = wnd;
	CEGUIUtility::addChildWidget (m_root_window, wnd->getCEGUIWindow ());
	wnd->getCEGUIWindow()->setVisible(false);
	
	wnd = new WarningDialogWindow(m_document);
	m_sub_windows["warningDialog"] = wnd;
	CEGUIUtility::addChildWidget (m_root_window, wnd->getCEGUIWindow ());
	wnd->getCEGUIWindow()->setVisible(false);

#if 0
	wnd = new MessageQuestionWindow (m_document);
	m_sub_windows["questionDialog"] = wnd;
	CEGUIUtility::addChildWidget (m_main_menu, wnd->getCEGUIWindow());
	wnd->getCEGUIWindow()->setVisible(false);
#endif
}

void MainWindow::setupChatContent()
{
	SW_DEBUG ("Setting up chat content GUI window");
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::Window* label;
	label = static_cast<CEGUI::MultiLineEditbox*>(win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "StaticText"), "ChatContent"));
	CEGUIUtility::addChildWidget (m_game_screen, label);
	label->setProperty("FrameEnabled", "false");
	//label->setProperty("BackgroundEnabled", "false");
	label->setPosition(CEGUI::UVector2(cegui_reldim(0.07f), cegui_reldim( 0.63f)));
	CEGUIUtility::setWidgetSizeRel (label, 0.43f, 0.2f);
	if (label->isPropertyPresent ("BackgroundColours"))
	{
		label->setProperty("BackgroundColours", "tl:44000000 tr:44000000 bl:44000000 br:44000000");
	}
	else if (label->isPropertyPresent ("BackgroundColour"))
	{
		label->setProperty("BackgroundColour", "B2000000");
	}
	label->setProperty("VertFormatting", "VertCentred");
	label->setProperty("HorzFormatting", "WordWrapLeftAligned");
	label->setText("");
	label->setAlwaysOnTop(true);
	label->setMousePassThroughEnabled(true);
	label->setID(0);
}

void  MainWindow::setWindowExtents(int width, int height){
	//Set Mouse Region.. if window resizes, we should alter this to reflect as well
	const OIS::MouseState &ms = m_mouse->getMouseState();
	ms.width = width;
	ms.height = height;
}


void  MainWindow::updateMainMenu()
{
	CEGUI::Window* img = 0;
	if (CEGUIUtility::isWindowPresent ("MainMenu/StartScreenRoot"))
	{
		img  = CEGUIUtility::getWindow ("MainMenu/StartScreenRoot");
		img->setMousePassThroughEnabled (true);
	}
	if (CEGUIUtility::isWindowPresent ("MainMenu/StartScreenRoot/StartScreenImage"))
	{
		img  = CEGUIUtility::getWindow ("MainMenu/StartScreenRoot/StartScreenImage");
		img->setMousePassThroughEnabled (true);
	}
	CEGUI::Window* label;
	label = CEGUIUtility::getWindow ("MainMenu/MainMenuRoot/CharacterPreviewImage");
	CEGUI::Window* label2;
	label2 = CEGUIUtility::getWindow ("MainMenu/MainMenuRoot/CharacterPreviewBackground");
	
	if (0 != img)
	{
		int wflags = m_document->getGUIState()->m_shown_windows;
		if (wflags & (Document::SAVEGAME_LIST | Document::CHAR_CREATE))
		{
			img->setVisible(false);
		}
		else
		{
			img->setVisible(true);
		}
	}
	
	m_sub_windows["MainMenu"]->update();
}

void  MainWindow::updateCharCreate()
{
	m_sub_windows["CharCreate"]->update();

}


void MainWindow::updateCursorItemImage()
{
	Player* player = m_document->getLocalPlayer();
	Item* item = 0;
	item = player->getEquipement()->getItem(Equipement::CURSOR_ITEM);
	
	CEGUI::Window* label = CEGUIUtility::getWindowForLoadedLayoutEx (m_root_window, "CursorItemImage");
	
	if (item == 0)
	{
		// Kein Item mehr am Cursor
		// Bild verstecken
		
		if (label->isVisible())
		{
			label->setVisible(false);
		}
	}
	else
	{
		// Es befindet sich ein Item am Cursor
		// Bild zeigen
		
		if (! label->isVisible())
		{
			label->setVisible(true);
		}	
		
		// Groesse des Items am Cursor ermitteln
		unsigned int size = item->m_size;

			
		if (size != label->getID())
		{
			// Groesse des Labels setzen
			if (size == Item::BIG)
			{
				CEGUIUtility::setWidgetSizeRel (label, 0.060f, 0.075f);
			}
			else if (size == Item::MEDIUM)
			{
				CEGUIUtility::setWidgetSizeRel (label, 0.045f, 0.06f);
			}
			else if (size == Item::SMALL)
			{
				CEGUIUtility::setWidgetSizeRel (label, 0.033f, 0.043f);
			}
			
			label->setID(size);
		}
		
		std::ostringstream out_stream("");
		out_stream<< ItemWindow::getItemImage(item->m_subtype);
		
		if (label->getProperty("Image")!=out_stream.str())
		{
			label->setProperty("Image", out_stream.str());
			OIS::MouseEvent me(m_mouse,m_mouse->getMouseState ());
			
			mouseMoved(me);
		}
		
	}
}

void MainWindow::updateObjectInfo()
{
	Player* player = m_document->getLocalPlayer();
	Region* reg = player->getRegion();
	if (reg ==0)
		return;
	
	// Ogre Name des Objektes auf das der Mauszeiger zeigt
	std::string objname = "";

	// Fenstermanager
	CEGUI::Window* label = CEGUIUtility::getWindow ("GameScreen/ObjectInfoRoot/ObjectInfoLabel");
	CEGUI::Window* itmlabel = CEGUIUtility::getWindow ("GameScreen/ItemInfoLabel");
	CEGUI::ProgressBar* bar = static_cast<CEGUI::ProgressBar*>(CEGUIUtility::getWindow ("GameScreen/ObjectInfoRoot/MonsterHealthProgressBar"));
	
	
	// Position der Maus
	float x = m_mouse->getMouseState().X.abs;
	float y = m_mouse->getMouseState().Y.abs;


	// Position des Mausklicks relativ zum Viewport
	Ogre::Viewport* viewport = m_scene->getViewport();
	float relx = x*1.0/(viewport->getActualWidth());
	float rely = y*1.0/(viewport->getActualHeight());

	// Strahl von der Kamera durch den angeklickten Punkt
	Ogre::Camera* camera = m_scene->getCamera();
	Ogre::Ray ray = camera->getCameraToViewportRay(relx,rely);

	// Query zum Anfragen auf getroffene Objekte
	Ogre::RaySceneQuery * query = m_scene_manager->createRayQuery(ray);
	query->setQueryTypeMask(Ogre::SceneManager::ENTITY_TYPE_MASK);
	query->setSortByDistance (true);
	query->setQueryMask(Scene::WORLDOBJECT | Scene::ITEM);
	
	// Anfrage ausfuehren
	Ogre::RaySceneQueryResult& result = query->execute();
	Ogre::RaySceneQueryResult::iterator it;
	
	int id=0;
	int objid =0;
	bool search_for_object = true;
	
	if (reg->getCutsceneMode () == true || player->getDialogue () != 0)
	{
		objid = 0;
		search_for_object = false;
	}
	else if (m_mouse->getMouseState().buttons !=0)
	{
		// fokussiertes Objekt nicht wechseln, wenn Maustaste gedrueckt
		if (m_document->getGUIState()->m_clicked_object_id != 0)
		{
			objid = m_document->getGUIState()->m_clicked_object_id;
			GameObject* go = reg->getObject(objid);
			search_for_object = false;
			if (go == 0)
			{
				search_for_object = true;
			}
			else if (go->getState() != WorldObject::STATE_ACTIVE)
			{
				search_for_object = true;
			}
		}
		else
		{
			search_for_object = false;
		}
	}
	
	if (search_for_object)
	{

		WorldObject* wo;
		GameObject* go;
		float dist, mindist = 100000;
		for (it = result.begin(); it != result.end(); ++it)
		{
			dist = it->distance;
			
			// unsichtbare Objekte nicht fokussierbar
			if (it->movable->getVisible() == false)
			{
				continue;
			}
			
			try
			{
				id = Ogre::any_cast<int>(it->movable->getUserAny());
			}
			catch (Ogre::Exception&)
			{
				WARNING("Object %s has no ID",it->movable->getName().c_str());
				continue;
			}
			DEBUGX("ray scene query dist %f obj %s mask %x id %i",it->distance, it->movable->getName().c_str(),it->movable->getQueryFlags(), id);
			// nur aktive Objekte beruecksichtigen
			go = reg->getGameObject(id);
			if (go == 0)
			{
				continue;
			}
			if (go->getState() != WorldObject::STATE_ACTIVE)
				continue;
			
			// verbuendete Einheiten bekommen einen Malus - dadurch selektiert man im Kampf bevorzugt Feinde
			wo = dynamic_cast<WorldObject*>(go);
			if (wo != 0)
			{
				if (wo->checkInteractionFlag(WorldObject::USABLE) == false)
				{
					continue;
				}
				
				if (World::getWorld()->getRelation(m_document->getLocalPlayer()->getFraction(), wo->getFraction()) == Fraction::ALLIED)
				{
					dist *=10;
				}
			}
			
			// Pruefung gegen das Polygon
			// get the entity to check
			
			Ogre::Entity *pentity = dynamic_cast<Ogre::Entity*>(it->movable);
			bool rayhit = true;
			if (((wo!=0 && wo->checkInteractionFlag(WorldObject::EXACT_MOUSE_PICKING)) || go->getType() == "DROPITEM")  && pentity != 0)
			{
				rayhit = false;
				
				// mesh data to retrieve
				size_t vertex_count;
				size_t index_count;
				Ogre::Vector3 *vertices;
				unsigned long *indices;
	
				// get the mesh information
				Scene::getMeshInformation(pentity->getMesh(), vertex_count, vertices, index_count, indices,
										pentity->getParentNode()->_getDerivedPosition(),
										pentity->getParentNode()->_getDerivedOrientation(),
										pentity->getParentNode()->_getDerivedScale());
	
				// test for hitting individual triangles on the mesh
				for (int i = 0; i < static_cast<int>(index_count)-2; i += 3)
				{
					// check for a hit against this triangle
					// I hope that volatile avoids that these variables are optimized out
					volatile int index1 = indices[i];
					volatile int index2 = indices[i+1];
					volatile int index3 = indices[i+2];
					if (index1 >=0 && index2>=0 && index3>=0
						&& index1 < (int)vertex_count && index2 < (int)vertex_count &&  index3 < (int)vertex_count)
					{
					
						std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, vertices[index1],
								vertices[index2], vertices[index3], true, false);
		
						// if it was a hit check if its the closest
						if (hit.first)
						{
							if ( (hit.second < dist))
							{
								// this is the closest so far, save it off
								dist = hit.second;
							}
							rayhit = true;
						}
					}
					else
					{
						ERRORMSG("Invalid indices: %i %i %i",index1,index2,index3);
					}
				}
	
				// free the verticies and indicies memory
				delete[] vertices;
				delete[] indices;
				
				if (rayhit)
				{
					dist /=10;
				}
				
				if (go->getType() == "DROPITEM")
					rayhit = true;
			}
			
			// Objekt wird vom Kamerastrahl geschnitten
			// Distance zum Spieler ausrechnen
			
			if (dist<mindist && rayhit)
			{
				// Objekt ist das bisher naechste
				objid = id;
				mindist = dist;
			}
		}
		m_document->getGUIState()->m_cursor_object=objname;

	}
	
	if (objid==0)
	{
		// aktuell kein Objekt unterm Mauszeiger
		m_document->getGUIState()->m_cursor_object_id =0;
		m_document->getGUIState()->m_cursor_item_id =0;

		label->setVisible(false);
		bar->setVisible(false);
		itmlabel->setVisible(false);
			
	}
	
	if (objid==player->getId())
	{
		label->setVisible(false);
		bar->setVisible(false);
		itmlabel->setVisible(false);
		m_document->getGUIState()->m_cursor_object_id = objid;
		objid=0;
	}
	
	std::string highlightcol = "white";
	std::string highlightmat = "_highlight_alpha_nodepth";
	
	std::string name;
	std::ostringstream string_stream;
	if (objid!=0)
	{
		// es gibt ein ein Objekt unterm Mauszeiger
		// zur ID gehoerendes Objekt
		GameObject* go = reg->getGameObject(objid);
		if (go != 0)
		{
			DEBUGX("highlight object %i,",objid);
			WorldObject* cwo = dynamic_cast<WorldObject*>(go);
			Creature* cr;
				
			if (cwo !=0 && cwo->getState() == WorldObject::STATE_ACTIVE)
			{
				// Objekt existiert
				m_document->getGUIState()->m_cursor_object_id = objid;
				m_document->getGUIState()->m_cursor_item_id =0;
				cr = dynamic_cast<Creature*>(cwo);
				if (cr != 0)
				{
					if (!bar->isVisible())
						bar->setVisible(true);
					
					// Objekt ist ein Lebewesen
					// Lebenspunkte anfuegen
					string_stream<< cr->getName().getTranslation();
					float perc = cr->getDynAttr()->m_health / cr->getBaseAttrMod()->m_max_health;
					if (bar->getProgress() != perc)
					{
						bar->setProgress(perc);
					}
				}
				else
				{
					bar->setVisible(false);
				}
				
				if (!label->isVisible())
				{
					label->setVisible(true);
				}
				name = string_stream.str();
				if (label->getText() != (CEGUI::utf8*) name.c_str())
				{
					label->setText((CEGUI::utf8*) name.c_str());
				}
				
				if (World::getWorld()->getRelation(cwo->getFraction(),player) == Fraction::HOSTILE)
				{
					highlightcol = Options::getInstance()->getEnemyHighlightColor();
				}
			}
			else
			{
				label->setVisible(false);
				bar->setVisible(false);
			}
			
			DropItem* di = dynamic_cast<DropItem*>(go);
			if (di != 0)
			{
				Item* itm = di->getItem();
				m_document->getGUIState()->m_cursor_item_id = id;
				m_document->getGUIState()->m_cursor_object_id =0;
				
				if (!m_document->getGUIState()->m_item_labels)
				{
					itmlabel->setVisible(true);
				}
				
				name = itm->getName();
				
				float len = itmlabel->getArea().getWidth().d_scale;
				
				if (itmlabel->getText() != (CEGUI::utf8*) name.c_str())
				{
					const CEGUI::Font* font = itmlabel->getFont();
					float width = font->getTextExtent((CEGUI::utf8*) name.c_str());
					CEGUIUtility::Rectf rect = m_game_screen->getInnerRectClipper();
					len = width / rect.getWidth();
					
					itmlabel->setText((CEGUI::utf8*) name.c_str());
				}
				
				if (fabs( itmlabel->getArea().getWidth().d_scale - len) > 0.001)
				{
					DEBUGX("old value %f new value %f",itmlabel->getArea().getWidth().d_scale, len);
					CEGUIUtility::setWidgetSizeRel (itmlabel, len, 0.03f);
				}
				
				std::string propold = itmlabel->getProperty("TextColours").c_str();
				std::string propnew = "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF";
				if (itm->m_rarity == Item::MAGICAL)
				{
					std::string magicColour = TooltipSettings::getGroundItemMagicalHexColourCode ();
					std::stringstream ssColour;
					ssColour << "tl:" << magicColour << " tr:" << magicColour << " bl:" << magicColour << " br:" << magicColour;
					propnew = ssColour.str ();
					//propnew = "tl:FF8888FF tr:FF8888FF bl:FF8888FF br:FF8888FF";
				}
				if (propold != propnew)
				{
					itmlabel->setProperty("TextColours", propnew); 
				}
				
				
				std::pair<float,float> rpos = m_scene->getProjection(di->getPosition());
				itmlabel->setPosition(CEGUI::UVector2(CEGUI::UDim(MathHelper::Max(0.0,rpos.first-0.03),0), CEGUI::UDim(MathHelper::Max(0.0,rpos.second-0.05),0)));
			}
			else
			{
				itmlabel->setVisible(false);
			}
		}
		else
		{
			m_document->getGUIState()->m_cursor_object_id =0;
			label->setVisible(false);
			bar->setVisible(false);
			objid = 0;
		}
	}

	int hlid = objid;
	if (hlid == player->getId())
		hlid =0;
	
	if (m_highlight_id !=  hlid)
	{
		GraphicObject* grobj = m_scene->getGraphicObject(m_highlight_id);
		if (grobj != 0)
		{
			grobj->setHighlight(false, highlightcol + highlightmat);
		}
		
		if (objid != 0)
		{
			grobj = m_scene->getGraphicObject(hlid);
			if (grobj != 0)
			{
				grobj->setHighlight(true, highlightcol + highlightmat);
			}
		}
		m_highlight_id = hlid;
	}

}

void MainWindow::updateItemInfo()
{
	// Zaehler wie viele Labels fuer Items existieren
	static int lcount =0;
	
	// Fenstermanager
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label = CEGUIUtility::getWindow ("GameScreen/ItemInfoLabel");
	
	
	
	DropItem* di;
	
	Player* player = m_document->getLocalPlayer();
	Vector plpos = player->getShape()->m_center;
	
	DropItemMap& itms = player->getRegion()->getDropItems();
	DropItemMap::iterator it;
	
	if (m_document->getGUIState()->m_item_labels)
	{
		label->setVisible(false);
		
		// Alt gedrueckt
		// fuer alle Items ein Label erstellen
		std::ostringstream stream;
		int nr =0;
		std::string name;
		std::pair<float,float> rpos;
		
		float height = 0.030;
		int num = (int) (0.9/height);
		// fuer jede 0.025 hohe Spalte ein Element des Vektors
		// fuer jede Spalte eine Liste von Wertepaaren: Anfang, Ende
		std::vector< std::list < std::pair<float,float> > > itempos(num);
		
		// Lokales Koordinatensystem fuer die Items herstellen
		Vector pu,pl,pll,pmr,pml,ox,oy;
		pu = getIngamePos(0.5,0.1,true);
		pl = getIngamePos(0.5,0.9,true);
		pll = getIngamePos(0,0.9,true);
		pmr = getIngamePos(1.0,0.5,true);
		pml = getIngamePos(0,0.5,true);
		
		oy = pu - pl;
		ox = pmr - pml;
		
		Vector po = pll - pml;
		po.projectOn(ox);
		po = pll - po;
		
		// Abfangen numerischer Schwankungen
		static Vector oldoy = Vector(0,-1);
		
		if (fabs(oldoy.angle(oy)) < 0.01 && fabs(oy.getLength() - oldoy.getLength())<0.01 )
		{
			oy = oldoy;
		}
		else
		{
			oldoy = oy;
		}
		DEBUGX("oy %f %f",oy.m_x, oy.m_y);
		
		// Skalierung der Achsen
		float oxl = ox.getLength();
		float oyl = oy.getLength();
		
		//float xscal = oxl;
		float yscal = oyl*height/0.9;
		
		ox.normalize();
		oy.normalize();

		DEBUGX("scal %f %f",xscal,yscal);
		
		// Ursprung des neuen Koordinatensystems
		Vector ypart = po;
		float ylen = ypart*oy;
		ypart.projectOn(oy);
		Vector xpart = po - ypart;
		
		DEBUGX("xpart %f %f",xpart.m_x, xpart.m_y);
		DEBUGX("ypart %f %f",ypart.m_x, ypart.m_y);
		float yoffs = ylen/yscal - roundf(ylen/yscal);
		
		
		Vector o = xpart + oy*(yscal*( roundf(ylen/yscal)));
		
		DEBUGX("pll %f %f %f",pll.m_x, pll.m_y,ylen);
		DEBUGX("o %f %f ", o.m_x, o.m_y);
		
		
		DEBUGX("\nitems\n");
		
		// Distanz zur wahren Position
		float optdist,dist;
		int optrow;
		float optcol;
		float len;
		// Begrenzung fuer den Schriftzug
		float lbound=0,rbound =1;
		int app;
		
		std::string propnew, propold;
		
		Vector tpos,pos;
		for (it = itms.begin(); it != itms.end();++it)
		{
			
			di = it->second;
			
			
			name = di->getItem()->getName();
			DEBUGX(" ");
			DEBUGX(" %s %i",name.c_str(), di->getId());
			
			tpos = di->getPosition();
			tpos -= o;
			
			pos.m_x = tpos*ox;
 			pos.m_y = tpos*oy;
			
			DEBUGX("tpos %f %f  %f",(tpos+o).m_x, (tpos+o).m_y,yscal);
			DEBUGX("test %f %f %f   oy %f %f",o*oy/yscal, (tpos+o)*oy/yscal, pos.m_y/yscal,oy.m_x, oy.m_y);
			
			// Position auf dem Bildschirm ermitteln
			DEBUGX("pos %f %f  -- %f",pos.m_x, pos.m_y,pos.m_y/yscal);
			rpos.first = pos.m_x / oxl;
			rpos.second = 0.9-pos.m_y / oyl*0.9;
			DEBUGX("rpos %f %f  - %f",rpos.first, rpos.second, rpos.second/height);
			
			// nur Items die wirklich sichtbar sind behandeln
			if (rpos.first<0 || rpos.first>1 || rpos.second<0 || rpos.second>0.9)
			{
				continue;
			}
			
			
			DEBUGX("optpos %i %f",optrow,optcol);
			
			// Laenge des Schriftzugs
			const CEGUI::Font* font = label->getFont();
			float width = font->getTextExtent((CEGUI::utf8*) name.c_str());
			CEGUIUtility::Rectf rect = m_game_screen->getInnerRectClipper();
			len = width / rect.getWidth();
			
			// eine Laenge fuer die Darstellung (mit Rand), eine ohne
			float reallen = len;
			float margin = 0.004;
			len +=2*margin;
			
			DEBUGX("length %f",len);
			
			std::list < std::pair<float,float> >::iterator jt, optjt;
			// optimale Position ermitteln
			// Distanz zur wahren Position
			optdist = 1000000;
			
			// beste gefundene Platzierung
			rpos.first -= len/2;
			optrow= (int) (rpos.second/height);
			optcol= rpos.first;
			app = 1;
			
			
			
			// schleife ueber die Zeilen
			int row, centerrow = optrow;
			float col= rpos.second;
			int i;
			for (i=0; i<2*num; i++)
			{
				
				lbound=0;
				rbound =1;
				
				
				
				row = centerrow;
				if (i%2 == 0)
					row += (i+1)/2;
				else
					row -= (i+1)/2;
				
				if (row<0 || row >= num)
					continue;
				
				// Zeilen ueberspringen die so weit wegliegen, dass garantiert schon eine bessere Platzierung gefunden ist
				if ((rpos.second- row*height)*(rpos.second- row*height) > optdist)
					continue;
				
				// iterieren ueber die Liste der Gegenstaende, die bisher in der Zeile liegen
				// (genauer: die Luecken zwischen den Labels)
				for (jt = itempos[row].begin(); jt != itempos[row].end(); ++jt)
				{
					rbound = jt->first;
					
					// Test ob das Label in die Luecke passt
					if (rbound -lbound < len)
					{
						lbound = jt->second;
						continue;
					}
					
					// Optimale Platzierung des Labels in der Luecke ermitteln
					if (rpos.first+len > rbound)
					{
						col = rbound-len;
					}
					else if (rpos.first<lbound)
					{
						col = lbound;
					}
					else
						col = rpos.first;
					
					dist = (rpos.first-col)*(rpos.first-col) + (centerrow-row)*(centerrow-row)*height*height;
					if (dist+0.0001 < optdist)
					{
						optdist = dist;
						optcol = col;
						optrow = row;
						optjt = jt;
						app = 0;
						
						DEBUGX("optdist %f pos %i %f ",optdist, row, col);
						DEBUGX("lbound %f rbound %f",lbound,rbound);
					}
					
					lbound = jt->second;
				}
				
				// Option, das Label hinter dem letzten bestehenden Label einzufuegen
				rbound = 1;
				if (rbound-lbound >= len)
				{
					if (rpos.first<lbound)
					{
						col = lbound;
					}
					else
						col = rpos.first;
					
					dist = (rpos.first-col)*(rpos.first-col) + (centerrow-row)*(centerrow-row)*height*height;
					
					if (dist+0.0001 < optdist)
					{
						app = 1;
						optdist = dist;
						optcol = col;
						optrow = row;
						
						DEBUGX("optdist %f optrow %i pos %i %f  app %i",optdist, optrow, row, col,app);
						DEBUGX("lbound %f rbound %f",lbound,rbound);
					}
					
				}
				
				
				
			}
			
			if (optdist > 10000)
				continue;
			
			DEBUGX("center %i rpos %f  %f",centerrow,rpos.second, rpos.second-centerrow*height);
			
			rpos.first=optcol;
			rpos.second=(optrow + yoffs) *height;
			
			
			
			DEBUGX("optdist %f optrow %i pos ",optdist, optrow);
			DEBUGX("final position %f %f",rpos.second, rpos.first);
			
			if (app == 1)
			{
				itempos[optrow].push_back(std::make_pair(rpos.first,rpos.first+len));
			}
			else
			{
				DEBUGX("insert before %f %f",optjt->first, optjt->second);
				itempos[optrow].insert(optjt,std::make_pair(rpos.first,rpos.first+len));
			}
			
			if (rpos.first<0 || rpos.first+len>1 || rpos.second<0 || rpos.second+height>0.9)
				continue;
			
			if (nr >= lcount)
			{
				stream.str("");
				stream << "ItemLabel" << nr;

				lcount ++;
				label = win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "StaticText"), stream.str());
				CEGUIUtility::addChildWidget (m_game_screen, label);
				label->setProperty("FrameEnabled", "false");
				label->setProperty("BackgroundEnabled", "true");
				if (label->isPropertyPresent ("BackgroundColours"))
				{
					label->setProperty("BackgroundColours", "tl:77000000 tr:77000000 bl:77000000 br:77000000"); 
				}
				else if (label->isPropertyPresent ("BackgroundColour"))
				{
					label->setProperty("BackgroundColour", "B2000000");
				}
				label->setPosition(CEGUI::UVector2(cegui_reldim(0.2f), cegui_reldim( 0.02f)));
				CEGUIUtility::setWidgetSizeRel (label, reallen, 0.03f);
				label->setText("");
				label->setAlpha(0.9);
				label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&MainWindow::onDropItemClicked, this));
				
			}
			else
			{
				stream.str("");
				stream << "GameScreen/ItemLabel" << nr;
				label = CEGUIUtility::getWindow (stream.str());
				
			}
			label->setVisible(true);
			
			
			if (label->getText() != (CEGUI::utf8*) name.c_str())
			{
	
				label->setText((CEGUI::utf8*) name.c_str());
			}
			
			if (fabs( label->getArea().getWidth().d_scale - reallen) > 0.001)
			{
				DEBUGX("old value %f new value %f",label->getArea().getWidth().d_scale, len);
				CEGUIUtility::setWidgetSizeRel (label, reallen, 0.03f);
			}
			
			label->setID(it->first);
			
			
			label->setPosition(CEGUI::UVector2(CEGUI::UDim(MathHelper::Max(0.0f,rpos.first+margin),0), CEGUI::UDim(MathHelper::Max(0.0f,rpos.second),0)));
			
			propold = label->getProperty("TextColours").c_str();
			propnew = "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF";
			if (di->getItem()->m_rarity == Item::MAGICAL)
			{
				std::string magicColour = TooltipSettings::getGroundItemMagicalHexColourCode ();
				std::stringstream ssColour;
				ssColour << "tl:" << magicColour << " tr:" << magicColour << " bl:" << magicColour << " br:" << magicColour;
				propnew = ssColour.str ();
				//propnew = "tl:FF8888FF tr:FF8888FF bl:FF8888FF br:FF8888FF";
			}
			if (propold != propnew)
			{
				label->setProperty("TextColours", propnew); 
			}
			nr++;

		}
		
		
		for (; nr<lcount; nr++)
		{
			stream.str("");
			stream << "GameScreen/ItemLabel" << nr;
			
			label = CEGUIUtility::getWindow (stream.str());
			label->setVisible(false);
		}
		
	}
	else
	{
		std::ostringstream stream;
		for (int i=0; i<lcount; i++)
		{
			stream.str("");
			stream << "GameScreen/ItemLabel" << i;

			label = CEGUIUtility::getWindow (stream.str());
			label->setVisible(false);
		}
	}
}


void MainWindow::updatePartyInfo()
{
	int windows = m_document->getGUIState()->m_shown_windows;
	if (windows & Document::PARTY)
	{
		// Fenster PartyInfo aktualisieren
		m_sub_windows["PartyInfo"]->update();
	}
	
	CEGUI::Window* img;
	CEGUI::ProgressBar* bar;
	
	std::ostringstream stream;
	
	int i=0;
	if (!( windows & Document::PARTY || windows & Document::CHARINFO || windows & Document::QUEST_INFO)) 
	{
		float hperc;
		Player* player = m_document->getLocalPlayer();
		Player* pl;
		Party* party = World::getWorld()->getParty(player->getFraction());
		
		std::set<int>::iterator it;
		// Schleife ueber die Mitglieder der Party des Spielers
		for (it =  party->getMembers().begin(); it!= party->getMembers().end(); ++it)
		{
			// eigenen Spieler ueberspringen
			if (*it == player->getId())
				continue;
			
			pl =  static_cast<Player*>(World::getWorld()->getPlayer(*it));
			if (pl ==0)
				continue;
			
			stream.str("");
			stream << "GameScreen/";
			stream << "PartyMemberImage";
			stream << i;
			img = CEGUIUtility::getWindow (stream.str());
			img->setID(pl->getId());
			
			// Bild setzen und anzeigen
			if (!img->isVisible())
			{
				img->setVisible(true);
			}
			std::string imagestr = pl->getEmotionImage(pl->getSpeakText().m_emotion);
			if (img->getProperty("Image") != imagestr)
			{
				img->setProperty("Image",imagestr);
			}
			
			if (img->getTooltipText() != (CEGUI::utf8*) pl->getName().getRawText().c_str())
			{
				img->setTooltipText((CEGUI::utf8*) pl->getName().getRawText().c_str());
			}
			
			stream.str("");
			stream << "GameScreen/";
			stream << "PartyMemberHealthBar";
			stream << i;
			bar = static_cast<CEGUI::ProgressBar*>(CEGUIUtility::getWindow (stream.str()));
			
			// Fortschrittsbalken setzen und anzeigen
			hperc = pl->getDynAttr()->m_health / pl->getBaseAttrMod()->m_max_health;
			if (bar->getProgress() != hperc)
			{
				bar->setProgress(hperc);
			}
			
			stream.str("");
			stream << (int) pl->getDynAttr()->m_health<<"/"<< (int) pl->getBaseAttrMod()->m_max_health;
			if (bar->getTooltipText() != stream.str())
			{
				bar->setTooltipText(stream.str());
			}
			
			if (!bar->isVisible())
			{
				bar->setVisible(true);
			}
			
			i++;
		}
	}
	
	
	// restliche Bilder verstecken
	for (;i<7; i++)
	{
		stream.str("");
		stream << "GameScreen/PartyMemberImage" << i;
		img = CEGUIUtility::getWindow (stream.str());
		if (img->isVisible())
		{
			img->setVisible(false);
		}
		
		stream.str("");
		stream << "GameScreen/PartyMemberHealthBar" << i;
		bar = static_cast<CEGUI::ProgressBar*>(CEGUIUtility::getWindow (stream.str()));
		if (bar->isVisible())
		{
			bar->setVisible(false);
		}

	}
}

void MainWindow::updateRegionInfo()
{
	static int region = -1;
	static int id = 0;
	static Timer timer;
	
	bool refresh = false;
	
	Player* pl = m_document->getLocalPlayer();	
	if (pl->getRegion() != 0)
	{
		// update, when a new player is set or when a new region is set.
		if (pl->getId() != id)
		{
			refresh = true;
			id = pl->getId();
		}
		
		if (pl->getRegion()->getId() != region)
		{
			refresh = true;
			region = pl->getRegion()->getId();
			//MusicManager::instance().stop();
			//SoundManager::getPtr ()->getMusicPlayer ()->stop ();
		}
		
		if (refresh)
		{
			SW_DEBUG ("*** main window expecting refresh for region [%s]", pl->getRegion ()->getName ().c_str ());
			timer.start();

			MinimapWindow* myMinimap = static_cast<MinimapWindow*> (m_sub_windows["Minimap"]);
			myMinimap->reloadIconsOnNextUpdate ();

			// Also change the playlist to the one for the new region.
			try
			{
				SoundManager::getPtr ()->getMusicPlayer ()->switchToPlaylist (pl->getRegion ()->getName ());
			}
			catch (std::exception& e)
			{
				WARNING ("Caught exception: [%s]", e.what ());
			}
		}
		
		float time = timer.getTime();
		
		bool vis = false;
		if (time > 500 &&  time < 3000)
		{
			vis = true;
		}
		
	
		CEGUI::Window* label;
		label = CEGUIUtility::getWindow ("GameScreen/RegionInfoLabel");
		
		// Only show when the minimap is not opened.
		int wflags = m_document->getGUIState()->m_shown_windows;
		vis &= !(wflags & Document::MINIMAP);
		
		if (label->isVisible() != vis)
		{
			label->setVisible(vis);
			
			if (vis == true)
			{
				label->setText( (CEGUI::utf8*) dgettext("sumwars",pl->getRegion()->getName().c_str()));
			}
		}
		
	}
}

void MainWindow::updateFloatingText()
{
	// Zaehler wie viele Labels fuer Schaden existieren
	static int lcount =0;
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label;
			
	Player* player = m_document->getLocalPlayer();
	Region* reg = player->getRegion();
	std::map<int,FloatingText*>& dmgvis = reg->getFloatingTexts();
	std::map<int,FloatingText*>::iterator it;
	
	int nr =0;
	std::stringstream stream;
	
	std::pair<float,float> pos;
	
	float maxtime;
	
	for (it = dmgvis.begin(); it != dmgvis.end(); ++it)
	{
		FloatingText& dmgv = *(it->second);
		
		maxtime = dmgv.m_maxtime;
		pos = m_scene->getProjection(dmgv.m_position,1.0f);
		pos.second -= dmgv. m_float_offset *(maxtime -dmgv.m_time)/maxtime;
		
		
		// nur Kreaturen behandeln, die wirklich zu sehen sind
		if (pos.first <0 || pos.first >1 || pos.second <0 || pos.second >1)
			continue;
		
		stream.str("");
		stream << "FloatingTextLabel";
		stream << nr;
		
		std::string col = dmgv.m_colour;
		std::string colour = std::string("tl:") + col + " tr:" + col + " bl:" + col + " br:"+ col;
		
		if (nr >= lcount)
		{
			lcount ++;
			label = win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "StaticText"), stream.str());
			CEGUIUtility::addChildWidget (m_game_screen, label);
			label->setProperty("FrameEnabled", "false");
			label->setProperty("BackgroundEnabled", "false");
			label->setText("");
			label->setAlpha(0.9);
			label->setFont("DejaVuSerif-12");
			
			label->setProperty("TextColours",colour ); 
			label->	setMousePassThroughEnabled(true);
		}
		else
		{
			stream.str("");
			stream << "GameScreen/FloatingTextLabel" << nr;
			label = CEGUIUtility::getWindow (stream.str());
		}
		
		if (label->getProperty("TextColours") != colour)
		{
			label->setProperty("TextColours",colour );
		}
		
		const std::string& text =  dmgv.m_text.getTranslation();
		
		if (label->getText() != (CEGUI::utf8*) text.c_str())
		{
			label->setText((CEGUI::utf8*) text.c_str());
		}
		
		std::string fontname = "DejaVuSerif-16";
		if (dmgv.m_size == 1)
		{
			fontname = "DejaVuSerif-12";
		}
		if (label->getFont()->getProperty("Name") != fontname)
		{
			label->setFont(fontname);
		}
		
		const CEGUI::Font* font = label->getFont();
			
		float width = font->getTextExtent((CEGUI::utf8*) text.c_str())+15;
		float height = font->getFontHeight() +15;
			
		CEGUIUtility::setWidgetSizeAbs (label, width, height);
		label->setPosition(CEGUI::UVector2(CEGUI::UDim(pos.first,-width/2), CEGUI::UDim(pos.second,-height)));
		
		float fadetime = 0.4* maxtime;
		if (dmgv.m_time < fadetime)
		{
			label->setAlpha(0.9 * dmgv.m_time / fadetime);
		}
		else
		{
			label->setAlpha(0.9);
		}
		
		label->setVisible(true);
		nr ++;
	}
	
	for (; nr<lcount; nr++)
	{
		stream.str("");
		stream << "GameScreen/FloatingTextLabel" << nr;
			
		label = CEGUIUtility::getWindow (stream.str());
		label->setVisible(false);
	}
}

void MainWindow::updateChatContent()
{
	static Timer timer;
	int windows = m_document->getGUIState()->m_shown_windows;
	
	Player* pl = m_document->getLocalPlayer();
	
	CEGUI::Window* label;
	label =  CEGUIUtility::getWindow ("GameScreen/ChatContent");
	
	// escape all the [ brackets
	// CEGUI >= 0.7 uses [brackets] for markup
	std::string rawtext = pl->getMessages();
	size_t pos = 0;
	while ((pos = rawtext.find('[',pos)) != std::string::npos)
	{
		rawtext.replace(pos,1,"\\[");
		pos += 2;
	}
	
	if (label->getText() != (CEGUI::utf8*) rawtext.c_str())
	{
		label->setText((CEGUI::utf8*) rawtext.c_str() );
	}
	
	bool vis = true;
	if (label->getText() == "" || ( windows & Document::PARTY || windows & Document::CHARINFO || windows & Document::QUEST_INFO))
	{
		vis = false;
	}
	else
	{
		// Fenster auf die richtige Groesse bringen
		const CEGUI::Font* fnt = label->getFont();
		CEGUIUtility::Size area(CEGUI::System::getSingleton().getRenderer()->getDisplaySize());

		CEGUI::String text = label->getText();
		CEGUI::UVector2 vec = CEGUIUtility::getWindowSizeForText(text.c_str(), fnt);
		
		float width = vec.d_x.d_scale;
		float maxwidth = area.d_width * 0.43;
		width += 10;
		if (width > maxwidth)
		{
			width = maxwidth;
		}
		
		CEGUIUtility::Size larea = area;
		larea.d_width = width;
		//float height =  vec.asRelative(larea).d_y; 

		//float relwidth = width / area.d_width;
		//float relheight = (height+6) / area.d_height;
		
		if ( fabs(label->getArea().getWidth().d_scale - vec.d_x.d_scale) > 0.0001
			|| fabs(label->getArea().getHeight().d_scale - vec.d_y.d_scale) > 0.0001)
		{
			label->setPosition(CEGUI::UVector2(cegui_reldim(0.07f), cegui_reldim( 0.82f - vec.d_y.d_scale)));
			//label->setSize(CEGUI::UVector2(cegui_reldim(relwidth), cegui_reldim(relheight)));
			CEGUIUtility::setWidgetSize (label, vec);
			DEBUGX("old size %f %f",label->getArea().getWidth().d_scale, label->getArea().getHeight().d_scale);
			DEBUGX("new size %f %f", vec.d_x.d_scale, vec.d_y.d_scale);
		}
		
		pl->updateMessageTimer(timer.getTime());
	}
	
	if (label->isVisible() != vis)
	{
		label->setVisible(vis);
		
	}
	
	timer.start();
}

void MainWindow::updateSound()
{
	// play all region sounds from the last updated
	Player* player = m_document->getLocalPlayer();
	if (player != 0)
	{
		Region* reg = player->getRegion();
		if (reg != 0)
		{
			const std::list<PlayedSound*> sounds = reg->getPlayedSounds();
			for (std::list<PlayedSound*>::const_iterator it = sounds.begin(); it != sounds.end(); ++it)
			{
				//SoundSystem::playAmbientSound((*it)->m_soundname, (*it)->m_volume, &((*it)->m_position));

				// TODO:XXX: verify how sounds come from the region.
				float posX = (*it)->m_position.m_x;
				float posY = (*it)->m_position.m_y;
				float posZ = 0.0;
				SoundHelper::playSoundGroupAtPosition ((*it)->m_soundname, posX, posY, posZ);

				//try
				//{
				//	DEBUG ("Will try to play sound group [%s]",  (*it)->m_soundname.c_str ());

				//	std::string soundToPlay;
				//	DEBUG ("Checking if valid");
				//	soundToPlay = SoundManager::getPtr ()->getSoundGroup ((*it)->m_soundname)->getRandomSound ();
				//	DEBUG ("Chosen sound file from group [%s]",  soundToPlay.c_str ());

				//	SoundManager::getPtr ()->getRepository ()->getSound (soundToPlay)->play3D (posX, posY, posZ);
				//}
				//catch (std::exception& e)
				//{
				//	DEBUG ("Caught exception while trying to play sound group [%s]: %s", (*it)->m_soundname.c_str (), e.what ());
				//}
			}
		}
	}
	
	//SoundSystem::update();
}


Vector MainWindow::getIngamePos(float screenx, float screeny, bool relative)
{
	// Position des Mausklicks relativ zum Viewport
	Ogre::Viewport* viewport = m_scene->getViewport();
	float relx,rely;
	if (relative)
	{
		relx = screenx;
		rely = screeny;
	}
	else
	{
		relx = screenx*1.0/(viewport->getActualWidth());
		rely = screeny*1.0/(viewport->getActualHeight());
	}
	DEBUGX("relative Koordinaten %f %f",relx,rely);

	// Strahl von der Kamera durch den angeklickten Punkt
	Ogre::Camera* camera = m_scene->getCamera();
	Ogre::Ray ray = camera->getCameraToViewportRay(relx,rely);

	// Ursprung und Richtung des Strahles
	const Ogre::Vector3& orig = ray.getOrigin();
	DEBUGX("ray orig %f %f %f",orig.x,orig.y,orig.z);
	const Ogre::Vector3& dir = ray.getDirection();
	DEBUGX("ray dir %f %f %f",dir.x,dir.y,dir.z);

	// Schnittpunkt mit der Ebene y=0 ausrechnen
	
	Vector ret;
	float dy = dir.y;
	if (dir.y>=0)
	{
		dy = -0.001;
	}
	
	
	// Durchstosspunkt durch die Ebene y=0
	Ogre::Vector3 p = orig + dir*(orig.y/(-dy));
	DEBUGX("schnittpunkt %f %f %f",p.x,p.y,p.z);

	// Umrechnen in Spielkoordinaten
	ret.m_x = p.x/GraphicManager::g_global_scale;
	ret.m_y = p.z/GraphicManager::g_global_scale;
	DEBUGX("Punkt in Spielkoordinaten %f %f",ret.m_x,ret.m_y);
	
	return ret;

}

// MouseListener
bool MainWindow::mouseMoved(const OIS::MouseEvent &evt)
{
    //DEBUG("injection position %i %i",evt.state.X.abs,evt.state.Y.abs);
    if(CEGUIUtility::injectMouseWheelChange (m_cegui_system, evt.state.Z.rel))
        m_document->onMouseMove(evt.state.X.rel, evt.state.Y.rel, 0);
    else
        m_document->onMouseMove(evt.state.X.rel, evt.state.Y.rel,evt.state.Z.rel);
	
	try
	{
		CEGUI::Window* label = CEGUIUtility::getWindowForLoadedLayoutEx (m_root_window, "CursorItemImage");
	
		int off = 0;
		if (label->getID() == Item::BIG)
			off = 24;
		if (label->getID() == Item::MEDIUM)
			off = 16;
		if (label->getID() == Item::SMALL)
			off = 12;
	
	
		label->setPosition(CEGUI::UVector2(CEGUI::UDim(0,MathHelper::Max(0,evt.state.X.abs-off)),CEGUI::UDim(0,MathHelper::Max(0,evt.state.Y.abs- off))));
	}
	catch (std::exception& e)
	{
		SW_DEBUG ("Caught exception: [%s]", e.what ());
	}
	
	return CEGUIUtility::injectMousePosition (m_cegui_system, evt.state.X.abs, evt.state.Y.abs);
}



/**
	React to a mouse button being pressed.
	@note: Input is only supported for the first 3 mouse buttons (left, right, middle) for the time being.
*/
bool MainWindow::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID btn)
{
	m_gui_hit = false;

	CEGUI::MouseButton button = CEGUI::NoButton;

	if (btn == OIS::MB_Left)
	{
		button = CEGUI::LeftButton;
	}
	else if (btn == OIS::MB_Middle)
	{
		button = CEGUI::MiddleButton;
	}
	else if (btn == OIS::MB_Right)
	{
		button = CEGUI::RightButton;
	}
	else
	{
		// For the time being no input from more than 3 buttons is allowed.
		return false;
	}

    bool ret = false; // CEGUI 0.7 changed its default behaviour a little bit and creates a invisible root window that always processes events

    // if the returned window is anything else than "GameScreen" then CEGUI needs to process the input
	CEGUI::Window* mouseWindow = CEGUIUtility::getWindowContainingMouse (m_cegui_system);

	SW_DEBUG ("Mouse Pressed. Mouse window is [%s]", mouseWindow->getName ().c_str ());
	if (! (mouseWindow->getName () == "SW")) 
    {
        ret = true; // set ret to false if the events where processed by the invisible window
		CEGUIUtility::injectMouseButtonDown (btn);
    }
    
	if (m_document->getGUIState()->m_sheet ==  Document::MAIN_MENU && m_ready_to_start)
	{
		m_document->onStartScreenClicked();
		return true;
	}

	// Koordinaten an denen der Mausklick stattfand
	int x =evt.state.X.abs;
	int y =evt.state.Y.abs;
	DEBUGX("mouse coords %i %i",x,y);

	Player* player = m_document->getLocalPlayer();
	if (player!=0)
	{
		// Spiel ist mit Server verbunden
		if (btn == OIS::MB_Middle)
		{
			m_document->getGUIState()->m_middle_mouse_pressed=true;
		}

		// Testet, dass man nicht auf die untere Steuerleiste geklickt hat
		if (!ret)
		{
			if (player->getEquipement()->getItem(Equipement::CURSOR_ITEM)!=0)
			{
				// Spieler hat Item in der Hand, fallen lassen
				m_document->dropCursorItem();
			}
			else
			{

				Vector pos = getIngamePos(x,y);

				if (btn == OIS::MB_Left)
				{
					m_document->getGUIState()->m_left_mouse_pressed=true;
					m_document->onLeftMouseButtonClick(pos);
				}
				else if (btn == OIS::MB_Right)
				{
					m_document->getGUIState()->m_right_mouse_pressed=true;
					m_document->onRightMouseButtonClick(pos);
				}
			}
		}

	}

	return ret;
}


/**
	React to the mouse button being released.
	@note: Input is only supported for the first 3 mouse buttons (left, right, middle) for the time being.
*/
bool MainWindow::mouseReleased (const OIS::MouseEvent &evt, OIS::MouseButtonID btn)
{
	// TODO: There are 2 separate if-else blocks; check if it makes sense to unite them,
	// or the sequence of events is really necessary.
	if (btn == OIS::MB_Left)
	{
		DEBUGX("Button release");
		m_document->getGUIState ()->m_left_mouse_pressed=false;
		m_document->getGUIState ()->m_clicked_object_id=0;
	}
	else if (btn == OIS::MB_Right)
	{
		DEBUGX("Right Button release");
		m_document->getGUIState ()->m_right_mouse_pressed=false;
		m_document->getGUIState ()->m_clicked_object_id=0;
	}
	else if (btn == OIS::MB_Middle)
	{
		m_document->getGUIState ()->m_middle_mouse_pressed=false;
	}
	else
	{
		// For the time being no input from more than 3 buttons is allowed.
		return false;
	}

	return CEGUIUtility::injectMouseButtonUp (m_cegui_system, btn);
}



bool MainWindow::keyPressed(const OIS::KeyEvent &evt) {
	unsigned int ch = evt.text;
	
	DEBUGX("keycode %i %s %x",evt.key,m_keyboard->getAsString(evt.key).c_str(), ch);
	if (m_document->getGUIState()->m_shown_windows & Document::OPTIONS)
	{
		if (static_cast<OptionsWindow*>(m_sub_windows["Options"])->requestsForKey())
		{
			static_cast<OptionsWindow*>(m_sub_windows["Options"])->setKeyCode(evt.key);
			return true;
		}
	}
	
	if (evt.key == OIS::KC_UNASSIGNED)
	{
		ch &= 0xff;
		CEGUIUtility::injectChar (m_cegui_system, (CEGUI::utf32) ch);
		return true;
	}
	
    if (evt.key == OIS::KC_S)
    {
        //m_scene_manager->setShadowTextureSelfShadow(!m_scene_manager->getShadowTextureSelfShadow());
    }
	
	bool ret = CEGUIUtility::injectKeyDown (m_cegui_system, evt.key);
	
	ret |= CEGUIUtility::injectChar (ch);
	
	if (ch != 0 &&  (m_document->getGUIState()->m_shown_windows & Document::CHAT))
	{
		m_key = ch;
		m_key_repeat_timer.start();
		m_key_repeat = false;
	}
	
	
	if (evt.key == OIS::KC_RSHIFT || evt.key == OIS::KC_LSHIFT)
	{
		m_document->getGUIState()->m_shift_hold = true;
	}

	// Test for copy paste operations.
	if (evt.key == OIS::KC_C)
	{
		if (m_keyboard->isModifierDown (OIS::Keyboard::Ctrl))
		{
			SW_DEBUG ("MainWindow:: ctrl + c pressed!");
			SWUtil::Clipboard::getSingletonPtr ()->copy ();
		}
	}
	else if (evt.key == OIS::KC_X)
	{
		if (m_keyboard->isModifierDown (OIS::Keyboard::Ctrl))
		{
			SW_DEBUG ("MainWindow:: ctrl + X pressed!");
			SWUtil::Clipboard::getSingletonPtr ()->cut ();
		}
	}
	else if (evt.key == OIS::KC_V)
	{
		if (m_keyboard->isModifierDown (OIS::Keyboard::Ctrl))
		{
			SW_DEBUG ("MainWindow:: ctrl + V pressed!");
			SWUtil::Clipboard::getSingletonPtr ()->paste ();
		}
	}
	
	if (!ret)
	{
		ret = m_document->onKeyPress(evt.key);
	}
	return ret;
}

bool MainWindow::keyReleased(const OIS::KeyEvent &evt)
{

	m_key =0;
	m_key_repeat = false;
	
	bool ret = CEGUIUtility::injectKeyUp (m_cegui_system, evt.key);
	
	if (evt.key == OIS::KC_RSHIFT || evt.key == OIS::KC_LSHIFT)
	{
		m_document->getGUIState()->m_shift_hold = false;
	}

	if (!ret)
	{
		ret = m_document->onKeyRelease(evt.key);
	}

	return ret;
}

bool MainWindow::onDropItemClicked(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();
	
	DEBUGX("pick up item %i",id);
	m_document->onDropItemClick(id);
	return true;
}




bool MainWindow::consumeEvent(const CEGUI::EventArgs& evt)
{
	return true;
}

bool MainWindow::onPartyMemberClicked(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();
	
	Player* pl = static_cast<Player*> (World::getWorld()->getLocalPlayer());
	if (World::getWorld() ==0 || pl ==0 || pl->getRegion() ==0)
		return true;
	
	// Spieler auf den die Aktion wirkt
	WorldObject* pl2 = pl->getRegion()->getObject(id);
	if (pl2 ==0)
		return true;
	
	SW_DEBUG("party member Click %i",id);
	
	m_document->getGUIState()->m_cursor_object_id = id;
	if (we.button == CEGUI::LeftButton)
	{
		m_document->onLeftMouseButtonClick(pl2->getPosition());
	}

	if (we.button == CEGUI::RightButton)
	{
		m_document->onRightMouseButtonClick(pl2->getPosition());
	}

	return true;
}

void MainWindow::setReadyToStart(bool ready)
{
	m_ready_to_start = ready;
	
	CEGUI::ProgressBar* bar = static_cast<CEGUI::ProgressBar*>(CEGUIUtility::getWindow ("MainMenu/StartScreenRoot/LoadRessourcesProgressBar"));
	bar->setVisible(!ready);

	// Also hide the background picture
	if (CEGUIUtility::isWindowPresent ("MainMenu/StartScreenRoot/StartScreenImage"))
	{
		CEGUI::Window* backgroundPicture = CEGUIUtility::getWindow ("MainMenu/StartScreenRoot/StartScreenImage");
		backgroundPicture->setVisible (false);
		backgroundPicture->setMousePassThroughEnabled (true);
	}
	if (CEGUIUtility::isWindowPresent ("MainMenu/StartScreenRoot"))
	{
		CEGUI::Window* backgroundPicture = CEGUIUtility::getWindow ("MainMenu/StartScreenRoot");
		backgroundPicture->setVisible (false);
		backgroundPicture->setMousePassThroughEnabled (true);
	}
	
	// we have finished loading so we will simulate the click to get straight in to the main menu
	m_document->onStartScreenClicked();

	
}

void MainWindow::setRessourceLoadingBar(float percent)
{
	CEGUI::ProgressBar* bar = static_cast<CEGUI::ProgressBar*>(CEGUIUtility::getWindow ("MainMenu/StartScreenRoot/LoadRessourcesProgressBar"));
	bar->setProgress(percent);
}

