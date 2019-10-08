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


#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H



// needed to be able to create the CEGUI renderer interface
#include "CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h"

// CEGUI includes

#include "OgreRoot.h"
#include "OgreRenderWindow.h"
#include "OgreStringConverter.h"
#include "OgreEntity.h"

// OIS
#include "OISEvents.h"
#include "OISInputManager.h"
#include "OISMouse.h"
#include "OISKeyboard.h"
#include "OISJoyStick.h"

#include "document.h"
#include "window.h"
#include "scene.h"

/**
 * \class MainWindow
 * \brief Implementiert das Hauptfenster der Applikation
 */
class MainWindow :
	public OIS::MouseListener,
	public OIS::KeyListener
{

	public:


	/**
	 * \fn MainWindow(Ogre::Root* ogreroot, CEGUI::System* ceguisystem,Ogre::RenderWindow* window,Document* doc)
	 * \param ogreroot Zeiger auf das Basisobjekt von Ogre
	 * \param ceguisystem Zeiger auf das Basisobjekt von CEGUI
	 * \param window Fenster in die Applikation gestartet wird
	 * \param doc Zeiger auf das Dokument
	 */
	MainWindow(Ogre::Root* ogreroot, CEGUI::System* ceguisystem,Ogre::RenderWindow* window,Document* doc);

	/**
	 * \fn ~MainWindow()
	 * \brief Destruktor
	 */
	~MainWindow();

	/**
	 * \fn init()
	 * \brief Initialisiert das Objekt
	 * \return true, wenn erfolgreich, sonst false
	 */
	bool init();

	/**
	 * \fn void update(float time)
	 * \brief aktualisiert die Anzeige
	 * \param time vergangene Zeit in ms
	 */
	void update(float time);

	/**
	 * \fn bool setupGameScreen()
	 * \brief Erstellt den Hauptspielbildschirm
	 * \return true, wenn erfolgreich, sonst false
	 */
	bool setupGameScreen();

	/**
	 * \fn void setupControlPanel()
	 * \brief Erzeugt die Kontrollleiste
	 */
	void setupControlPanel();

	/**
	 * \fn void setupInventory()
	 * \brief Erzeugt das Fenster Inventar
	 */
	void setupInventory();

	/**
	 * \fn void setupCharInfo()
	 * \brief Erzeugt das Fenster CharakterInfo
	 */
	void setupCharInfo();

	/**
	 * \fn void setupSkilltree()
	 * \brief Erzeugt das Fenster Skilltree
	 */
	void setupSkilltree();

	/**
	 * \fn void setupChatWindow()
	 * \brief Erzeugt das Fenster Chat
	 */
	void setupChatWindow();
	
	/**
	 * \fn void setupCursorItemImage()
	 * \brief Erzeugt das Bild fuer am Cursor befestigte Items
	 */
	void setupCursorItemImage();

	/**
	 * \fn bool setupMainMenu()
	 * \brief Erstellt das Hauptmenue
	 * \return true, wenn erfolgreich, sonst false
	 */
	bool setupMainMenu();


	/**
	 * \fn bool setupObjectInfo()
	 * \brief Erstellt die Leiste mit Informationen zu einem Objekt
	 */
	bool setupObjectInfo();
	
	/**
	 * \fn bool setupObjectInfo()
	 * \brief Erstellt die Fenster mit Informationen zu Gegenstaenden
	 */
	bool setupItemInfo();
	
	/**
	 * \fn bool setupPartyInfo()
	 * \brief erstellt die Fenster mit Informationen zur Party
	 */
	bool setupPartyInfo();
	
	/**
	 * \fn bool setupCharCreate
	 * \brief erstellt Fenster zur Charaktererstellung
	 */
	bool setupCharCreate();
	
	/**
	 * \fn void setupQuestInfo()
	 * \brief erstellt Fenster Questinfo
	 */
	void setupQuestInfo();
	
	/**
	 * \fn void setupMinimap()
	 * \brief erstellt Fenster Minimap
	 */
	void setupMinimap();
		
	
	/**
	 * \fn void setupTrade()
	 * \brief erstellt Fenster Handel
	*/
	void setupTrade();
	
	/**
	 * \fn void setupWorldmap()
	 * \brief Erstellt Weltkarte Fenster
	 */
	void setupWorldmap();
	
	/**
	 * \fn void setupRegionInfo()
	 * \brief Erstellt Fenster mit Informationen zur Region
	 */
	void setupRegionInfo();
	
	/**
	 * \fn void setupSaveExitWindow()
	 * \brief erstellt Fenster Speichern & Beenden
	 */
	void setupSaveExitWindow();
    
    /**
	 * \fn void setupErrorDialogWindow()
	 * \brief unknown
	 */
	void setupErrorDialogWindow();
	
	/**
	 * \fn void setupChatContent()
	 * \brief Erstellt Fenster fuer Chattext
	 */
	void setupChatContent();
	
	/**
	 * \fn bool initInputs()
	 * \brief Initialisiert die Eingabegeraete
	 */
	bool initInputs();

	/**
	 * \fn void updateMainMenu()
	 * \brief Aktualisiert das Hauptmenue
	 */
	void updateMainMenu();

	/**
	 * \fn void updateCursorItemImage()
	 * \brief aktualisiert das Bild fuer den am Cursor befestigten Gegenstand
	 */
	void updateCursorItemImage();

	/**
	 * \fn void updateObjectInfo()
	 * \brief Aktualisiert die Leiste mit Informationen zu einem Objekt
	 */
	void updateObjectInfo();
	
	/**
	 * \fn void updateObjectInfo()
	 * \brief Aktualisiert die Fenster mit Informationen zu Gegenstaenden
	 */
	void updateItemInfo();

	/**
	 * \fn void updatePartyInfo()
	 * \brief Aktualisiert die Informationen zu Partymitgliedern
	 */
	void updatePartyInfo();
	
	/**
	 * \fn void updateCharCreate
	 * \brief aktualisiert Fenster zur Charaktererstellung
	 */
	void updateCharCreate();
	
	
	
	/**
	 * \fn void updateRegionInfo()
	 * \brief Erstellt Fenster mit Informationen zur Region
	 */
	void updateRegionInfo();
	
	/**
	 * \fn void updateFloatingText()
	 * \brief aktualisiert die Schadenanzeiger
	 */
	void updateFloatingText();
	
	/**
	 * \fn void updateChatContent()
	 * \brief Aktualisiert Fenster fuer Chattext
	 */
	void updateChatContent();
	
	/**
	 * \brief Aktualisiert die Musik
	 */
	void updateMusic();

	/**
	 * \brief Updates the sound system
	 */
	void updateSound();
	
	/**
	 * \fn bool mouseMoved(const OIS::MouseEvent &evt)
	 * \brief Behandelt Mausbewegung
	 */
	bool mouseMoved(const OIS::MouseEvent &evt);

	/**
	 * \fn bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID)
	 * \brief Behandelt Druecken von Maustasten
	 */
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID);

	/**
	 * \fn bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID)
	 * \brief Behandelt Loslassen von Maustasten
	 */
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID);
	
	/**
	 * \fn bool onDropItemClicked(const CEGUI::EventArgs& evt)
	 * \brief Behandelt das Senden einer Nachricht
	 * \param evt CEGUI Event Parameter
	 */
	bool onDropItemClicked(const CEGUI::EventArgs& evt);
	
	

	/**
	 * \fn bool keyPressed(const OIS::KeyEvent &evt)
	 * \brief Behandelt Druecken von Tasten auf der Tastatur
	 */
	bool keyPressed(const OIS::KeyEvent &evt);

	/**
	 * \fn bool keyReleased(const OIS::KeyEvent &evt)
	 * \brief Behandelt Loslassen von Tasten auf der Tastatur
	 */
	bool keyReleased(const OIS::KeyEvent &evt);

	/**
	 * \fn void setWindowExtents(int width, int height)
	 * \brief Setzt die Groesse des Fensters
	 * \param width Breite
	 * \param height Hoehe
	 */
	void setWindowExtents(int width, int height);

	/**
	 * \fn Vector getIngamePos(float x, float y)
	 * \brief Berechnet die Ingameposition zu gegebenen Koordinaten auf dem Bildschirm
	 * \param screenx x-Position auf dem Bildschirm 
	 * \param screeny y-Position auf dem Bildschirm
	 * \param relative wenn true, werden die Koordinaten als relative Koordinaten interpretiert (Bereich 0-1)
	 */
	Vector getIngamePos(float screenx, float screeny, bool relative = false);
	
	/**
	 * \fn bool consumeEvent(const CEGUI::EventArgs& evt)
	 * \brief Funktion die Events verbraucht, gibt immer true zurueck
	 */
	bool consumeEvent(const CEGUI::EventArgs& evt);
	
	/**
	 * \fn bool onPartyMemberImageClicked(const CEGUI::EventArgs& evt)
	 * \brief Behandelt Click auf Partymitglied
	 */
	bool onPartyMemberClicked(const CEGUI::EventArgs& evt);
	
	/**
	 * \brief Stellt ein, ob das Spiel gestartet werden kann (notwendig um auf das Laden von Ressourcen zu warten)
	 * \param ready true, sobald alle Daten geladen sind
	 */
	void setReadyToStart(bool ready);
	
	/**
	 * \brief Setzt die Fortschrittsanzeige des Ressourcen ladens
	 * \param percent Prozentsatz der Forschrittsanzeige
	 */
	void setRessourceLoadingBar(float percent);
	
	private:

	/**
	 * \var Ogre::Root *m_ogre_root
	 * \brief Basisobjekt der Ogre Engine
	 */
	Ogre::Root *m_ogre_root;

	/**
	 * \var CEGUI::System* m_cegui_system
	 * \brief Basisobjekt von CEGUI
	 */
	CEGUI::System* m_cegui_system;

	/**
	 * \var Ogre::RenderWindow *m_window
	 * \brief Fenster der Applikation
	 */
	Ogre::RenderWindow *m_window;

	/**
	 * \var Ogre::SceneManager* m_scene_manager
	 * \brief aktueller Szenemanager
	 */
	Ogre::SceneManager* m_scene_manager;


	/**
	 * \var OIS::InputManager *m_ois
	 * \brief Basisobjekt von OIS
	 */
	OIS::InputManager *m_ois;

	/**
	 * \var OIS::Mouse *m_mouse
	 * \brief Repraesentation der Maus
	 */
	OIS::Mouse *m_mouse;

	/**
	 * \var OIS::Keyboard *m_keyboard
	 * \brief Repraesentation der Tastatur
	 */
	OIS::Keyboard *m_keyboard;


	/**
	 * \var Document* m_document
	 *  \brief Dokument
	 */
	Document* m_document;

	/**
	 * \fn CEGUI::Window* m_main_menu
	 * \brief Hauptmenue
	 */
	CEGUI::Window* m_main_menu;

	/**
	 * \var CEGUI::Window* m_game_screen
	 * \brief Fenster, in dem das eigentliche Spiel laeuft
	 */
	CEGUI::Window* m_game_screen;

	/**
	 * \var Scene* m_scene
	 * \brief Szene die dargestellt wird
	 */
	Scene* m_scene;

	/**
	 * \var bool m_gui_hit
	 * \brief wird auf true gesetzt, wenn bei einem Mausklick ein GUI Element getroffen wird
	 */
	bool m_gui_hit;
	
	/**
	 * \var std::map<std::string, Window*> m_sub_windows
	 * \brief Unterfenster sortiert nach Name
	 */
	std::map<std::string, Window*> m_sub_windows;
	
	/**
	 * \var Timer m_key_repeat_timer
	 * \brief Timer fuer Tastenwiederholung
	 */
	Timer m_key_repeat_timer;
	
	/**
	 * \var bool m_key_repeat
	 * \brief Zeigt an, ob fuer die gedrueckte Taste die Tastenwiederholung aktiv ist
	 */
	bool m_key_repeat;
	
	/**
	 * \var unsigned int m_key
	 * \brief gedrueckte Taste
	 */
	unsigned int m_key;
	
	/**
	 * \var int m_highlight_id
	 * \brief ID des gerade per Highlight markierten Objektes
	 */
	int m_highlight_id;
	
	/**
	 * \brief zeigt an, ob alle Ressourcen geladen wurden und somit das Spiel gestartet werden kann
	 */
	bool m_ready_to_start;

};

#endif
