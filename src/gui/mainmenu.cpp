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

#include "OgreEntity.h"
#include "mainmenu.h"
#include <OgreRenderWindow.h>
#include <OgreSubEntity.h>
#include <OgreMeshManager.h>
#include <OgreParticleSystem.h>
#include "listitem.h"
#include "savegamelist.h"
#include "messageboxes.h"
#include "graphicmanager.h"
#include "scene.h"
#include "version.h"

#include <iostream>

// Sound operations helper.
#include "soundhelper.h"

// Allow the use of the sound manager.
#include "gussound.h"

// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"

using gussound::SoundManager;


MainMenu::MainMenu (Document* doc, const std::string& ceguiSkinName)
        : Window (doc)
		, m_ceguiSkinName (ceguiSkinName)
		, m_starMenuRoot (0)
{
	SW_DEBUG ("MainMenu created");

	m_savegame_player ="";
	m_savegame_player_object =0;
	
	m_savegame_player_action.m_type = "noaction";
	m_savegame_player_action.m_time = 2000;
	m_savegame_player_action.m_elapsed_time = 0;
	
	
	CEGUI::PushButton* btn;
#if 0
	// Load the holder (this should be aspect ratio dependent).
	CEGUI::Window* main_menu_holder = CEGUIUtility::loadLayoutFromFile ("mainmenu_holder.layout");
	if (!main_menu_holder)
	{
		WARNING ("WARNING: Failed to load [%s]", "mainmenu_holder.layout");
	}
	CEGUI::Window* wndHolder = CEGUIUtility::getWindowForLoadedLayoutEx (main_menu_holder, "MainMenu_Holder");
#endif
	// Load the main menu (will be aspect ratio independent)
	m_starMenuRoot = CEGUIUtility::loadLayoutFromFile ("mainmenu.layout");
	if (!m_starMenuRoot)
	{
		WARNING ("WARNING: Failed to load [%s]", "mainmenu.layout");
	}

	SW_DEBUG ("Placing layout into holder");
	m_starMenuRoot->setVisible (true);

	//m_window = main_menu_holder;
    m_window = m_starMenuRoot;// Augustin Preda, 2014.01.08: switched main window to the holder.

	// Augustin Preda, 2014.01.12: Please revise this. This is highly dependent on internal workings of the CEGUI lib.
	// As it seems there have been some recent changes, this led to the callback not being triggered as expected.
	m_window->setMousePassThroughEnabled(true);
    m_window->subscribeEvent(CEGUI::Window::EventShown, CEGUI::Event::Subscriber(&MainMenu::onShown, this));
    m_window->subscribeEvent(CEGUI::Window::EventHidden, CEGUI::Event::Subscriber(&MainMenu::onHidden, this));

    // Button for a single-player game
	std::string widgetName (CEGUIUtility::getNameForWidget("SinglePlayerButton"));
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, widgetName));
	if (btn)
	{
		btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenu::onStartSinglePlayer, this));
		btn->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&MainMenu::onMainMenuButtonHover, this));
	}

	// Button used for joining a (remote) server.
	widgetName = CEGUIUtility::getNameForWidget("ServerJoinButton");
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, widgetName));
	if (btn)
	{

		btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenu::onStartMultiPlayer, this));
		btn->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&MainMenu::onMainMenuButtonHover, this));
	}

    // Button used for hosting a multiplayer game (creating the server)
	widgetName = CEGUIUtility::getNameForWidget("ServerHostButton");
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, widgetName));
	if (btn)
	{
		btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenu::onStartMultiPlayerHost, this));
		btn->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&MainMenu::onMainMenuButtonHover, this));
	}

    // Button used for accessing the game's credits
	widgetName = CEGUIUtility::getNameForWidget("CreditsButton");
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, widgetName));
	if (btn)
	{
		btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenu::onShowCredits, this));
		btn->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&MainMenu::onMainMenuButtonHover, this));
	}

    // Button used for accessing the game's options
	widgetName = CEGUIUtility::getNameForWidget("MainOptionsButton");
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, widgetName));
	if (btn)
	{
		btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenu::onShowOptions, this));
		btn->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&MainMenu::onMainMenuButtonHover, this));
	}

    // Button used for exiting the game
	widgetName = CEGUIUtility::getNameForWidget("EndGameButton");
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, widgetName));
	if (btn)
	{
		btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenu::onQuitGameHost, this));
		btn->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&MainMenu::onMainMenuButtonHover, this));
	}

	CEGUI::Window* verlbl;

	widgetName = CEGUIUtility::getNameForWidget("SumwarsVersionLabel");
	verlbl = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, widgetName);
	if (verlbl)
	{
		verlbl->setText(CEGUI::String("Version: ").append(SUMWARS_VERSION));
		SW_DEBUG ("Set sumwars version text to : [%s]", verlbl->getText ().c_str ());
	}

	CEGUI::FrameWindow* lbl;


#ifdef SUMWARS_BUILD_WITH_ONLINE_SERVICES
    btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("LoginButton"));
    btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenu::onLoginToOnlineService, this));

    CEGUI::FrameWindow* login_dialog = static_cast<CEGUI::FrameWindow*>(CEGUIUtility::loadLayoutFromFile ("LoginDialog.layout"));
    CEGUIUtility::addChildWidget (m_window, login_dialog);
	login_dialog->hide();

    btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("LoginDialog/LoginButton"));
    btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenu::onLoginDialogLoginButton, this));

    btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("LoginDialog/CancelButton"));
    btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenu::onLoginDialogCancelButton, this));

    OnlineServicesManager::getSingleton().registerLoginStatusListener(this);
#else
	widgetName = CEGUIUtility::getNameForWidget("SumwarsOnlineIndicatorLabel");
    lbl = static_cast<CEGUI::FrameWindow*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, widgetName));
    lbl->hide();

	widgetName = CEGUIUtility::getNameForWidget("LoginButton");
    btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, widgetName));
    btn->hide();
#endif

	Ogre::Root *root = Ogre::Root::getSingletonPtr();
	m_sceneMgr = root->createSceneManager(Ogre::ST_GENERIC, "MainMenuSceneManager");
	m_mainMenuCamera = m_sceneMgr->createCamera("MainMenuCamera");
	m_mainMenuCamera->setNearClipDistance(0.1f);
	m_mainMenuCamera->setFarClipDistance(10000);

    m_sceneCreated = false;
    m_userLoggedIn = false;

    createSavegameList();

    updateTranslation();
}


void MainMenu::update()
{
	updateCharacterView();
}


void MainMenu::updateTranslation()
{
	CEGUI::Window* widget;

	// Update the button labels.

	std::string widgetName (CEGUIUtility::getNameForWidget("SinglePlayerButton"));
	widget = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, widgetName);
	if (widget)
	{
		widget->setText((CEGUI::utf8*) gettext("Single player"));
	}

	widgetName = CEGUIUtility::getNameForWidget("ServerJoinButton");
	widget = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, widgetName);
	if (widget)
	{
		widget->setText((CEGUI::utf8*) gettext("Join game"));
	}

	widgetName = CEGUIUtility::getNameForWidget("ServerHostButton");
	widget = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, widgetName);
	if (widget)
	{
		widget->setText((CEGUI::utf8*) gettext("Host game"));
	}

	widgetName = CEGUIUtility::getNameForWidget("CreditsButton");
	widget = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, widgetName);
	if (widget)
	{
		widget->setText((CEGUI::utf8*) gettext("Credits"));
	}

	widgetName = CEGUIUtility::getNameForWidget("MainOptionsButton");
	widget = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, widgetName);
	if (widget)
	{
		widget->setText((CEGUI::utf8*) gettext("Options"));
	}

	widgetName = CEGUIUtility::getNameForWidget("EndGameButton");
	widget = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, widgetName);
	if (widget)
	{
		widget->setText((CEGUI::utf8*) gettext("Quit"));
	}


	// Update message windows
	//CEGUI::FrameWindow* lbl;
	//lbl = static_cast<CEGUI::FrameWindow*>(CEGUIUtility::getWindow ("DeleteCharLabel")); // note: this has no in-between slash!
	//if (lbl) lbl->setText((CEGUI::utf8*) gettext("Really delete savegame?"));

	// Propagation onto sub-windows
	if (m_saveGameList != 0)
	{
		m_saveGameList->updateTranslation ();

		// The save game list doesn't forward all item changes, so also call the full update.
		m_saveGameList->update ();
	}
}


/**
 * \fn bool onMainMenuButtonHover(const CEGUI::EventArgs& evt)
 * \brief Handle the hovering of a button in the main menu
 */
bool MainMenu::onMainMenuButtonHover (const CEGUI::EventArgs& evt)
{
	SoundHelper::playAmbientSoundGroup ("main_menu_hover_item");
	return true;
}
#ifdef SUMWARS_BUILD_WITH_ONLINE_SERVICES
bool MainMenu::onLoginToOnlineService ( const CEGUI::EventArgs& evt )
{
	if(!m_userLoggedIn)
	    static_cast<CEGUI::FrameWindow*>(CEGUI::WindowManager::getSingleton().getWindow("LoginDialog"))->show();
    else
        OnlineServicesManager::getSingleton().logout();

    return true;
}

bool MainMenu::onLoginDialogLoginButton ( const CEGUI::EventArgs& evt )
{
    if(!m_userLoggedIn)
    {
        static_cast<CEGUI::FrameWindow*>(CEGUI::WindowManager::getSingleton().getWindow("LoginDialog"))->show();
        CEGUI::String username = static_cast<CEGUI::FrameWindow*>(CEGUI::WindowManager::getSingleton().getWindow("LoginDialog/UsernameEditbox"))->getText();
        CEGUI::String pw = static_cast<CEGUI::FrameWindow*>(CEGUI::WindowManager::getSingleton().getWindow("LoginDialog/PasswordEditbox"))->getText();
		static_cast<CEGUI::FrameWindow*>(CEGUI::WindowManager::getSingleton().getWindow("LoginDialog/PasswordEditbox"))->setText("");
        OnlineServicesManager::getSingleton().login(username.c_str(), pw.c_str());
    }

	return true;
}


bool MainMenu::onLoginDialogCancelButton ( const CEGUI::EventArgs& evt )
{
    static_cast<CEGUI::FrameWindow*>(CEGUI::WindowManager::getSingleton().getWindow("LoginDialog"))->hide();
	return true;
}

void MainMenu::onLoginFinished(std::vector<OnlineServicesManager::CharacterLite*> &characters)
{
    std::vector<OnlineServicesManager::CharacterLite*>::iterator iter;
    for(iter = characters.begin(); iter != characters.end(); iter++)
        std::cout << (*iter)->name << std::endl;

    static_cast<CEGUI::FrameWindow*>(CEGUI::WindowManager::getSingleton().getWindow("LoginDialog"))->hide();
    //static_cast<CEGUI::FrameWindow*>(CEGUI::WindowManager::getSingleton().getWindow("LoginButton/Label"))->setText((CEGUI::utf8*) gettext("Logout"));
    std::string username = static_cast<CEGUI::FrameWindow*>(CEGUI::WindowManager::getSingleton().getWindow("LoginDialog/UsernameEditbox"))->getText().c_str();
    static_cast<CEGUI::FrameWindow*>(CEGUI::WindowManager::getSingleton().getWindow("SumwarsOnlineIndicatorLabel"))->setText((CEGUI::utf8*) (gettext("Logged in as: ") + username).c_str());
    m_userLoggedIn = true;
    m_saveGameList->update();

}

void MainMenu::onLogoutFinished()
{
    //static_cast<CEGUI::FrameWindow*>(CEGUI::WindowManager::getSingleton().getWindow("LoginButton/Label"))->setText((CEGUI::utf8*) gettext("Login"));
    static_cast<CEGUI::FrameWindow*>(CEGUI::WindowManager::getSingleton().getWindow("SumwarsOnlineIndicatorLabel"))
            ->setText((CEGUI::utf8*) gettext("Not logged in."));
	m_userLoggedIn = false;
    m_saveGameList->update();
}

void MainMenu::onSyncCharFinished()
{
    DEBUGX("Character sync finished");
}

#endif


bool MainMenu::onStartSinglePlayer(const CEGUI::EventArgs& evt)
{
	SoundHelper::playAmbientSoundGroup ("main_menu_click_item");
	m_document->onButtonStartSinglePlayer();
    return true;
}

bool MainMenu::onStartMultiPlayer(const CEGUI::EventArgs& evt)
{
	SoundHelper::playAmbientSoundGroup ("main_menu_click_item");
	m_document->onButtonJoinGame();
    return true;
}

bool MainMenu::onStartMultiPlayerHost(const CEGUI::EventArgs& evt)
{
	SoundHelper::playAmbientSoundGroup ("main_menu_click_item");
    m_document->onButtonHostGame();
    return true;
}

bool MainMenu::onShowCredits(const CEGUI::EventArgs& evt)
{
	SoundHelper::playAmbientSoundGroup ("main_menu_click_item");
    m_document->onButtonCredits();
    return true;
}

bool MainMenu::onShown( const CEGUI::EventArgs& evt )
{
	SW_DEBUG ("Main menu shown");
	Ogre::Root *root = Ogre::Root::getSingletonPtr();
    if (!m_sceneCreated)
	{
        createScene();
		m_gameCamera = root->getAutoCreatedWindow()->getViewport(0)->getCamera();
		
		// we need a scene to display the character, so set the savegame now
		m_saveGameList->selectDefaultSavegame();
	}
	m_saveGameList->update();

	root->getAutoCreatedWindow()->getViewport(0)->setCamera(m_mainMenuCamera);
    root->addFrameListener(this);
	
	CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "MainMenuRoot")->setAlpha(0);

	// Also switch to the menu's playlist.
	try
	{
		SoundManager::getPtr ()->getMusicPlayer ()->switchToPlaylist ("menu");
	}
	catch (std::exception& e)
	{
		SW_DEBUG ("Encountered error while trying to switch to the menu playlist: %s", e.what ());
	}
    return true;
}

bool MainMenu::onHidden( const CEGUI::EventArgs& evt )
{
	SW_DEBUG ("Main menu hidden");
	Ogre::Root *root = Ogre::Root::getSingletonPtr();
    if (m_sceneCreated)
	{
		root->getAutoCreatedWindow()->getViewport(0)->setCamera(m_gameCamera);
	}
	CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "MainMenuRoot")->setAlpha(1);

    root->removeFrameListener(this);
	
    return true;
}

void MainMenu::updateSaveGameList()
{
	m_saveGameList->update();
}

void MainMenu::setSavegameListVisible(bool show)
{
	CEGUI::FrameWindow* savegameList = (CEGUI::FrameWindow*) CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "SaveGameSelectionFrame");
	if (savegameList->isVisible() != show)
	{
		savegameList->setVisible(show);
		// hide/show some buttons along with savegamelist
		CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "SinglePlayerButton")->setVisible(show);
		CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ServerJoinButton")->setVisible(show);
		CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ServerHostButton")->setVisible(show);
		
		if (show == true)
		{
			m_saveGameList->update();
		}
	}
}


bool MainMenu::onShowOptions(const CEGUI::EventArgs& evt)
{
	SoundHelper::playAmbientSoundGroup ("main_menu_click_item");
    m_document->onButtonOptionsClicked();
    return true;
}


bool MainMenu::onQuitGameHost(const CEGUI::EventArgs& evt)
{
	SoundHelper::playAmbientSoundGroup ("main_menu_click_item");
    m_document->onButtonSaveExitClicked();
    return true;
}



bool MainMenu::frameStarted(const Ogre::FrameEvent& evt)
{
    
	/*m_mainMenuCamera->setPosition(Ogre::Vector3(0,0,-0.6)+m_mainMenuCamera->getPosition());
	std::stringstream ss;
    ss << "V3: " << m_mainMenuCamera->getPosition().x << " " << m_mainMenuCamera->getPosition().y << " " << m_mainMenuCamera->getPosition().z << " " << std::endl;
	Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, ss.str().c_str());
	*/
	return Ogre::FrameListener::frameStarted(evt);
}


bool MainMenu::frameEnded(const Ogre::FrameEvent& evt)
{
	/*
	if(m_sceneMgr)
		m_sceneMgr->getEntity("MainMenuZombie")->getAnimationState("zombieWalk")->addTime(evt.timeSinceLastFrame);
	*/
    return Ogre::FrameListener::frameEnded(evt);
}

void MainMenu::createScene()
{
    if (!m_sceneCreated)
    {
		try
		{
			m_mainNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode("MainMenuMainNode");
			m_mainNode->setPosition(Ogre::Vector3::ZERO);

			m_mainMenuCamera->setPosition(Ogre::Vector3(-12.2126, 0.597, -14.1));
			m_mainMenuCamera->lookAt(Ogre::Vector3::ZERO);
			m_mainMenuCamera->setOrientation(Ogre::Quaternion(0.986127f, 0.120615f, -0.1132f, 0.0138457f));

			Ogre::SceneNode *n = m_mainNode->createChildSceneNode();
			Ogre::Entity *e;
			Ogre::MeshPtr ptrMesh; // 2014.01.12: re-added.
			Ogre::ParticleSystem *p;
			Ogre::Light *l;

			m_sceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.3, 2));


			l = m_sceneMgr->createLight("mainMen_MoonLight");
			l->setType(Ogre::Light::LT_POINT);
			l->setPosition(Ogre::Vector3(0.929331, 16.9939, -29.9981));
			l->setDirection(0, 0, 1);
			l->setAttenuation(100, 0.2, 0.8, 0);
			l->setCastShadows(true);
			l->setDiffuseColour(Ogre::ColourValue(1, 1, 1, 1));
			l->setSpecularColour(Ogre::ColourValue(0.45098, 0.45098, 0.47451, 1));
			l->setPowerScale(1);

			l = m_sceneMgr->createLight("mainMen_FireLight1");
			l->setType(Ogre::Light::LT_POINT);
			l->setPosition(Ogre::Vector3(-10.5044, 0.121838, -21.5031));
			l->setDirection(0, 0, 1);
			l->setAttenuation(100, 0.5, 0.02, 0.008);
			l->setCastShadows(false);
			l->setDiffuseColour(Ogre::ColourValue(0.443137, 0.215686, 0.145098, 1));
			l->setSpecularColour(Ogre::ColourValue(0.407843, 0.176471, 0.0588235, 1));
			l->setPowerScale(3);

			Ogre::NameGenerator localNameGen ("SWMO");
		
			ptrMesh = Ogre::MeshManager::getSingletonPtr ()->load ("long_sw.mesh", "General");
			n = m_mainNode->createChildSceneNode();
			//e = m_sceneMgr->createEntity (Ogre::String ("long_mesh_01"), Ogre::String ("this_does_not_exist.mesh"));
			e = m_sceneMgr->createEntity (localNameGen.generate (), "long_sw.mesh");
			e->getSubEntity(0)->setMaterialName("Item");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-13.7902, 0.0892202, -24.5334));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(0.852776, 0.413687, 0.248387, -0.199852));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("gold.mesh");
			e->getSubEntity(0)->setMaterialName("Item");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-10.4538, 0.990349, -26.0507));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("leathArm.mesh");
			e->getSubEntity(0)->setMaterialName("Item");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-12.9917, -0.00965142, -23.8061));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(0.985556, 0, 0.16935, 0));
		
			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("heal_2.mesh");
			e->getSubEntity(0)->setMaterialName("Item");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-11.8726, 0.990349, -25.4361));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("heal_1.mesh");
			e->getSubEntity(0)->setMaterialName("Item");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-12.286, 0.990349, -25.537));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("box_m3.mesh");
			e->getSubEntity(0)->setMaterialName("gimcrack");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-12.0797, -0.00965118, -25.5948));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(0.998848, 0, 0.0479781, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("box_m2.mesh");
			e->getSubEntity(0)->setMaterialName("gimcrack");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-12.3237, -0.00965214, -28.5043));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(0.975342, 0, 0.220697, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("box_m2.mesh");
			e->getSubEntity(0)->setMaterialName("gimcrack");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-10.4654, -0.00965118, -26.1655));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(0.992546, 0, -0.121869, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("buckler.mesh");
			e->getSubEntity(0)->setMaterialName("Item");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-13.3333, 0.356877, -25.1495));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(0.928377, 0.304607, 0.111193, -0.181566));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("campFire.mesh");
			e->getSubEntity(0)->setMaterialName("env_waldland");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-10.4943, -0.00965166, -21.3849));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("tree4.mesh");
			e->getSubEntity(0)->setMaterialName("env_blaetter");
			e->getSubEntity(1)->setMaterialName("env_waldland");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-1.38935, -0.00965309, -32.0907));
			n->setScale(Ogre::Vector3(2.55586, 2.41194, 2.55586));
			n->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("chestBox.mesh");
			e->getSubEntity(0)->setMaterialName("gimcrack");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-15.4144, -0.00964898, -22.8803));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(0.598323, 4.69282e-006, 0.801255, 2.4053e-006));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("stool_konz.mesh");
			e->getSubEntity(0)->setMaterialName("gimcrack");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-8.98199, -0.00965166, -21.4096));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("stool_konz.mesh");
			e->getSubEntity(0)->setMaterialName("gimcrack");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-8.39954, -0.00965166, -26.3683));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("chestLid.mesh");
			e->getSubEntity(0)->setMaterialName("gimcrack");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-15.5435, 0.746074, -23.2522));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(0.374504, -0.624886, 0.501524, -0.466624));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("stones3.mesh");
			e->getSubEntity(0)->setMaterialName("env_waldland");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-10.5055, -0.00965118, -27.8262));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(0.743145, 0, -0.669131, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("tree3.mesh");
			e->getSubEntity(0)->setMaterialName("env_blaetter");
			e->getSubEntity(1)->setMaterialName("env_waldland");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-23.2304, -0.00965118, -34.9225));
			n->setScale(Ogre::Vector3(1.62948, 1.5, 1.62948));
			n->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("gobDog.mesh");
			e->getSubEntity(0)->setMaterialName("Monster_Aisen");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-9.9569, -0.00965178, -24.6495));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(0.354291, 0, -0.935135, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("gold_rng.mesh");
			e->getSubEntity(0)->setMaterialName("Item");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-10.1393, 0.990349, -25.7856));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("stone2.mesh");
			e->getSubEntity(0)->setMaterialName("env_waldland");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-5.92453, -0.00965118, -29.1621));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("stone2.mesh");
			e->getSubEntity(0)->setMaterialName("env_waldland");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-7.36273, -0.00965309, -28.5372));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("stone2.mesh");
			e->getSubEntity(0)->setMaterialName("env_waldland");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-3.0643, -0.00965214, -45.8015));
			n->setScale(Ogre::Vector3(5, 5, 5));
			n->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("stone2.mesh");
			e->getSubEntity(0)->setMaterialName("env_waldland");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-6.39304, -0.00965166, -27.8417));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("stone_l1.mesh");
			e->getSubEntity(0)->setMaterialName("env_waldland");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-16.307, -0.385546, -31.6074));
			n->setScale(Ogre::Vector3(1, 1, 1.57439));
			n->setOrientation(Ogre::Quaternion(0.963639, -0.000392383, -0.267202, 0.00166609));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("stone_l1.mesh");
			e->getSubEntity(0)->setMaterialName("env_waldland");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-4.65489, -0.00965118, -26.1386));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(0.995396, 0, -0.0958458, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("stones1.mesh");
			e->getSubEntity(0)->setMaterialName("env_waldland");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-14.9308, 0, -31.1942));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(0.973379, 6.74019e-011, -0.2292, -2.41509e-010));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("stones2.mesh");
			e->getSubEntity(0)->setMaterialName("env_waldland");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-20.6536, 0, -23.9446));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(0.973379, 6.74019e-011, -0.2292, -2.41509e-010));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("stones3.mesh");
			e->getSubEntity(0)->setMaterialName("env_waldland");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-19.3292, 0, -24.7372));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(0.612217, 2.04367e-010, -0.790689, -1.4527e-010));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("stones3.mesh");
			e->getSubEntity(0)->setMaterialName("env_waldland");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-10.1034, -0.00965118, -31.4112));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("stones3.mesh");
			e->getSubEntity(0)->setMaterialName("env_waldland");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-6.93122, -0.00965214, -42.3686));
			n->setScale(Ogre::Vector3(6, 6, 6));
			n->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("tree2.mesh");
			e->getSubEntity(0)->setMaterialName("env_blaetter");
			e->getSubEntity(1)->setMaterialName("env_waldland");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-17.1033, -0.00965214, -38.6637));
			n->setScale(Ogre::Vector3(1.30359, 1.2, 1.30359));
			n->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("tree4.mesh");
			e->getSubEntity(0)->setMaterialName("env_blaetter");
			e->getSubEntity(1)->setMaterialName("env_waldland");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-7.77576, -0.00965118, -37.878));
			n->setScale(Ogre::Vector3(1.41222, 1.3, 1.41222));
			n->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

			n = m_mainNode->createChildSceneNode();
			e = m_sceneMgr->createEntity("vase2.mesh");
			e->getSubEntity(0)->setMaterialName("env_konzil");
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-14.3816, -0.00965166, -23.7364));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

			p = m_sceneMgr->createParticleSystem("mainMen_Particle#0","CampFire");
			n = m_mainNode->createChildSceneNode();
			n->attachObject(p);
			n->setPosition(Ogre::Vector3(-10.5017, 0.150328, -21.4926));
			n->setScale(Ogre::Vector3(1, 1, 1));
			n->setOrientation(Ogre::Quaternion(1, 0, 0, 0));

			Ogre::Plane plane0(Ogre::Vector3::UNIT_Y, 0);
			Ogre::MeshManager::getSingletonPtr()->createPlane("mainMen_Plane#0", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane0, 10, 10, 6, 6 ,true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
			e = m_sceneMgr->createEntity("mainMen_Plane#0");
			e->setMaterialName("grass1");
			n = m_mainNode->createChildSceneNode();
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-12.0042, -0.00965166, -29.7625));
			n->setScale(Ogre::Vector3(5, 1, 5));
			n->setOrientation(Ogre::Quaternion(0.418659, -1.48815e-010, -0.908144, -2.01801e-010));

			Ogre::Plane plane1(Ogre::Vector3::UNIT_Y, 0);
			Ogre::MeshManager::getSingletonPtr()->createPlane("mainMen_Plane#1", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane1, 10, 10, 6, 6 ,true, 1, 1, 1, Ogre::Vector3::UNIT_Z);
			e = m_sceneMgr->createEntity("mainMen_Plane#1");
			e->setMaterialName("env_nightsky");
			n = m_mainNode->createChildSceneNode();
			n->attachObject(e);
			n->setPosition(Ogre::Vector3(-2.64068, 14.1513, -57.029));
			n->setScale(Ogre::Vector3(5.8, 1, 4.83));
			n->setOrientation(Ogre::Quaternion(0.248194, -0.15685, 0.597531, 0.74616));

			m_sceneCreated = true;
		}
		catch (std::exception& e)
		{
			SW_DEBUG ("[MainMenu] Caught exception in scene creation: [%s]", e.what ());
		}
	}
    
}

void MainMenu::updateCharacterView()
{
	bool update = false;
	Ogre::SceneManager* scene_mgr = Ogre::Root::getSingleton().getSceneManager("MainMenuSceneManager");
	GraphicManager::setSceneManager(scene_mgr);
	
	Player* pl = m_document->getLocalPlayer();

	static Ogre::Timer actiontimer;
	
	if ((pl ==0 && m_savegame_player!=""))
	{
		DEBUGX("deleting inv player");
		m_savegame_player="";
		GraphicManager::destroyGraphicObject(m_savegame_player_object);
		m_savegame_player_object =0;
		update = true;
	}

	if (pl !=0)
	{
		std::string correctname = pl->getNameId();
		correctname += pl->getPlayerLook().m_render_info;
		
		if ((correctname != m_savegame_player))
		{
			DEBUGX("updating inv player %s to %s",m_savegame_player.c_str(), correctname.c_str());
			GraphicManager::destroyGraphicObject(m_savegame_player_object);
			m_savegame_player_object =0;
			update = true;
		}
		
		m_savegame_player = correctname;
		
		if (m_savegame_player_object ==0)
		{
			update = true;
			
			GraphicObject::Type type = pl->getPlayerLook().m_render_info;
			m_savegame_player_object = GraphicManager::createGraphicObject(type,pl->getNameId(), pl->getId());
			
			m_savegame_player_object->getTopNode()->setPosition(Ogre::Vector3(-9.27905, -0.0096519, -22.9244));
			m_savegame_player_object->getTopNode()->setOrientation(Ogre::Quaternion(-0.190809, 0, 0.981627, 0));
			m_savegame_player_object->setExactAnimations(true);
		}
		
		update |= Scene::updatePlayerGraphicObject(m_savegame_player_object,pl);
	}	
	
	if (m_savegame_player_object !=0)
	{
		// set action for the renderer call
		Action actsave = *(pl->getAction());
		
		float frametime = actiontimer.getMicroseconds ()/1000.0;
		m_savegame_player_action.m_elapsed_time += frametime;
		if (m_savegame_player_action.m_elapsed_time > m_savegame_player_action.m_time)
		{
			// determine new action
			if (m_savegame_player_action.m_type != "noaction")
			{
				// never play to actions directly after each other
				m_savegame_player_action.m_type = "noaction";
				m_savegame_player_action.m_time = pl->getActionTime(m_savegame_player_action.m_type);
			}
			else
			{
				m_savegame_player_action.m_type = "noaction";
				
				// get a random action
				if (Random::random() < 0.5)
				{
					std::map<std::string, AbilityInfo>::iterator it;
					std::map<std::string, AbilityInfo>& abl = pl->getBaseAttrMod()->m_abilities;
					int abl_found = 0;
					for (it = abl.begin(); it != abl.end(); ++it)
					{
						if (it->first == "noaction" || it->first == "die" || it->first == "walk" || it->first == "dead" 
							|| it->first == "take_item" || it->first == "use" || it->first == "speak" || it->first == "trade")
							continue;
						
						Action::ActionInfo* aci2 = Action::getActionInfo(it->first);
						if (aci2->m_target_type == Action::PASSIVE)
							continue;
							
						abl_found++;
						
						if (Random::randi(abl_found) == 0)
						{
							m_savegame_player_action.m_type = it->first;
						}
					}
				}
				
				// determine time of the action
				m_savegame_player_action.m_time = pl->getActionTime(m_savegame_player_action.m_type);
				
				// apply attack speed
				Action::ActionType baseact = "noaction";
				Action::ActionInfo* ainfo = Action::getActionInfo(m_savegame_player_action.m_type);
				if (ainfo != 0)
				{
					baseact = ainfo->m_base_action;
				}
				
				if (baseact == "attack" || baseact == "range_attack" || baseact == "holy_attack" || m_savegame_player_action.m_type == "magic_attack")
				{
					float atksp = MathHelper::Min((short) 5000,pl->getBaseAttrMod()->m_attack_speed);
					m_savegame_player_action.m_time *= 1000000/atksp;
				}
				
				// reduce speed (just looks better :P )
				m_savegame_player_action.m_time *= 1.3;
			}
			m_savegame_player_action.m_elapsed_time = 0;
		}
		
		pl->setAction(m_savegame_player_action);
		std::string act = pl->getActionString();
		float perc = m_savegame_player_action.m_elapsed_time / m_savegame_player_action.m_time;
		
		m_savegame_player_object->updateAction(act,perc);
		m_savegame_player_object->update(0);
		
		pl->setAction(actsave);
	}
	actiontimer.reset();
}

void MainMenu::createSavegameList()
{
    // Select the frame for the saved games in the menu
	std::string widgetName (CEGUIUtility::getNameForWidget("SaveGameSelectionFrame"));
	CEGUI::FrameWindow* savegameList = static_cast<CEGUI::FrameWindow*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_starMenuRoot, widgetName));
    savegameList->setInheritsAlpha(false);

	SavegameList* sgl = new SavegameList (m_document, m_ceguiSkinName);
    m_saveGameList = sgl;

	CEGUIUtility::addChildWidget (savegameList, m_saveGameList->getCEGUIWindow());
    m_saveGameList->update();
    m_saveGameList->updateTranslation();

	// TODO: this implementation is really wacky... not consistent with the rest.
	// Will need to bring the entire implementation to a common base.
    MessageQuestionWindow * delchar = new MessageQuestionWindow (m_document,
		std::string ("messagequestionwindow.layout"), 
		std::string (gettext("Really delete savegame?")),
		std::string (gettext("Yes")),
		CEGUI::Event::Subscriber(&SavegameList:: onDeleteCharConfirmClicked, m_saveGameList),
		std::string (gettext("No")),
		CEGUI::Event::Subscriber(&SavegameList::onDeleteCharAbortClicked, m_saveGameList));

	CEGUIUtility::addChildWidget (m_window, delchar->getCEGUIWindow());
}
