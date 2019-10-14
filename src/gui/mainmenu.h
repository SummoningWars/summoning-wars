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

#ifndef __SUMWARS_GUI_MAINMENU_H__
#define __SUMWARS_GUI_MAINMENU_H__

#include "OgreRoot.h"
#include "window.h"
#include "graphicobject.h"

#ifdef SUMWARS_BUILD_WITH_ONLINE_SERVICES
#include "onlineservicesmanager.h"
#endif

class SavegameList;

/**
 * \class MainMenu
 * \brief Fenster Charakter Info
 */
#ifdef SUMWARS_BUILD_WITH_ONLINE_SERVICES
class MainMenu
    : public Window
    , public Ogre::FrameListener
    , public OnlineServicesManager::StatusListener
#else
class MainMenu
	: public Window
	, public Ogre::FrameListener
#endif
{
public:
    /**
     * \fn MainMenu (Document* doc)
     * \brief Constructor
	 * \param doc Document object.
	 * \param ceguiSkinName The name of the cegui skin to use for any widgets created internally.
     */
    MainMenu (Document* doc, const std::string& ceguiSkinName);


	// ------------------------- Frame Listener functions ----------------------

    /**
     * \fn virtual bool frameStarted (const FrameEvent &evt)
     * \brief Called when a frame is about to begin rendering.
     */
    bool frameStarted ( const Ogre::FrameEvent &evt );

    /**
     * \fn virtual bool frameStarted (const FrameEvent &evt)
     * \brief Called just after a frame has been rendered.
     */
    bool frameEnded ( const Ogre::FrameEvent &evt );


	// ------------------------- Other functions ----------------------

    /**
     * \fn virtual void update()
     * \brief Updates the window contents
     */
    virtual void update();

    /**
     * \fn virtual void updateTranslation
     * \brief updates the translations
     */
    virtual void updateTranslation();


	/**
	 * \brief Sets the visibiliy state of the SaveGameList
	 * \param show new visibility status
	 */
	void setSavegameListVisible(bool show);
	
	/**
	 * \brief updates the Savegamelist
	 */
	void updateSaveGameList();

#if SUMWARS_BUILD_WITH_ONLINE_SERVICES
    /**
     * \fn bool onLoginToOnlineService(const CEGUI::EventArgs& evt)
     * \brief Handles a click login button (opens a login dialog)
     */
    bool onLoginToOnlineService ( const CEGUI::EventArgs& evt );

    /**
     * \fn bool onLoginDialogLoginButton(const CEGUI::EventArgs& evt)
     * \brief Handles a click to the sumwars online service from the Login Dialog
     */
    bool onLoginDialogLoginButton ( const CEGUI::EventArgs& evt );

    /**
     * \fn bool onLoginDialogCancelButton ( const CEGUI::EventArgs& evt );
     * \brief Handles a click to the sumwars online service from the Login Dialog
     */
    bool onLoginDialogCancelButton ( const CEGUI::EventArgs& evt );


    /**
     * \fn bool onLoginFinished ( const CEGUI::EventArgs& evt );
     * \brief Handles when a Login task is finished from OnlineServicesManager::StatusListener
     */
    virtual void onLoginFinished(std::vector<OnlineServicesManager::CharacterLite*> &characters);

    /**
     * \fn bool onLogoutFinished ( const CEGUI::EventArgs& evt );
     * \brief Handles when a Logout task is finished from OnlineServicesManager::StatusListener
     */
    virtual void onLogoutFinished();

    /**
     * \fn bool onSyncCharFinished ( const CEGUI::EventArgs& evt );
     * \brief Handles when a Sync task is finished from OnlineServicesManager::StatusListener
     */
    virtual void onSyncCharFinished();

#endif

private:

    /**
     * \fn bool createSavegameList()
     * \brief Creates the savegame list in main menu
     */
    void createSavegameList();

    /**
     * \fn bool onMainMenuButtonHover(const CEGUI::EventArgs& evt)
     * \brief Handle the hovering of a button in the main menu
     */
    bool onMainMenuButtonHover (const CEGUI::EventArgs& evt);

	/**
     * \fn bool onStartSinglePlayer(const CEGUI::EventArgs& evt)
     * \brief Handles start of the SinglePlayer mode
     */
    bool onStartSinglePlayer ( const CEGUI::EventArgs& evt );

    /**
     * \fn bool onStartMultiPlayer(const CEGUI::EventArgs& evt)
     * \brief Handles start of the MultiPlayer mode when joining a game
     */
    bool onStartMultiPlayer ( const CEGUI::EventArgs& evt );

    /**
     * \fn bool onStartMultiPlayerHost(const CEGUI::EventArgs& evt)
     * \brief Handles start of the MultiPlayer mode when hosting a game
     */
    bool onStartMultiPlayerHost ( const CEGUI::EventArgs& evt );

    /**
     * \fn bool onShowCredits(const CEGUI::EventArgs& evt)
     * \brief Handles clicks on the credits button
     */
    bool onShowCredits ( const CEGUI::EventArgs& evt );

    /**
     * \fn bool onShown(const CEGUI::EventArgs& evt)
     * \brief Handles events when window is shown
     */
    bool onShown ( const CEGUI::EventArgs& evt );

    /**
     * \fn bool onHidden(const CEGUI::EventArgs& evt)
     * \brief Handles events when window is hidden
     */
    bool onHidden ( const CEGUI::EventArgs& evt );

    /**
     * \fn bool onShowOptions(const CEGUI::EventArgs& evt)
     * \brief Handles clicks on the Options button
     */
    bool onShowOptions ( const CEGUI::EventArgs& evt );

    /**
     * \fn bool onQuitGameHost(const CEGUI::EventArgs& evt)
     * \brief Handles clicks on quit hosting a multiplayer game
     */
    bool onQuitGameHost ( const CEGUI::EventArgs& evt );

    /**
     * \fn void createScene()
     * \brief Creates the background scene
     */
    void createScene();
	
	/**
	 * \brief Updates the preview of the savegame character
	 */
    void updateCharacterView();

    /**
     * \fn void createCharacterMenu()
     * \brief Creates the GUI for character selection
     */

    void createCharacterMenu();

    /**
     * \var Ogre::SceneNode *m_mainNode;
     * \brief A link to the MainMenu Ogre::SceneNode
     */
    Ogre::SceneNode *m_mainNode;

    /**
     * \var Ogre::SceneManager *m_sceneMgr;
     * \brief A pointer to the MainMenu Ogre::SceneManager
     */
    Ogre::SceneManager *m_sceneMgr;

    /**
     *\var bool m_sceneCreated;
     *\brief Sets to true if the scene was successfully created
     */
    bool m_sceneCreated;

	/**
     *\var bool *m_mainMenuCamera;
     *\brief The main Menu camera
     */
    Ogre::Camera *m_mainMenuCamera;
	
	/**
     *\var bool *m_gameCamera;
     *\brief Saves the game camera while the main menu is displayed
     */
    Ogre::Camera *m_gameCamera;

    /**
     * \var SaveGameList m_saveGameList;
     * \brief A pointer to the savegame list window
     */
    SavegameList *m_saveGameList;
	
	/**
	 * \brief The root CEGUI node of the start menu
	 */
	CEGUI::Window* m_starMenuRoot;

	/**
	 * \brief Name of the previewed savegame player 
	 */
	std::string m_savegame_player;
	
	/**
	 * \brief GraphicObjekt for the previewed savegame player 
	 */
	GraphicObject* m_savegame_player_object;
	
	/**
	 * \brief Random action that is executed by the savegame player
	 */
	Action m_savegame_player_action;

	/**
	 * \brief The name of the CEGUI skin to use.
	 */
	std::string m_ceguiSkinName;

    /**
     *\var bool m_userLoggedIn;
     *\brief Sets to true if the user is logged in
     */
    bool m_userLoggedIn;

};

#endif // __SUMWARS_GUI_MAINMENU_H__
