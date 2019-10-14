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

#ifndef __SUMWARS_GUI_APPLICATION_H__
#define __SUMWARS_GUI_APPLICATION_H__

#include "OgreTimer.h"
#include "OgreWindowEventUtilities.h"
#include "OgreLogManager.h"
#include "OgreMeshManager.h"

// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"

// needed to be able to create the CEGUI renderer interface
#ifdef CEGUI_07
#include "CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h"
#include "CEGUI/CEGUISystem.h"
#include "CEGUI/CEGUIInputEvent.h"
#include "CEGUI/CEGUIWindow.h"
#include "CEGUI/CEGUIWindowManager.h"
#include "CEGUI/CEGUISchemeManager.h"
#include "CEGUI/CEGUIFontManager.h"
#include "CEGUI/elements/CEGUIFrameWindow.h"
#else
#include "CEGUI/RendererModules/Ogre/Renderer.h"
#include "CEGUI/System.h"
#include "CEGUI/InputEvent.h"
#include "CEGUI/Window.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/SchemeManager.h"
#include "CEGUI/FontManager.h"
#include "CEGUI/widgets/FrameWindow.h"
#endif


// CEGUI includes

#include <stdio.h>

#include "mainwindow.h"
#include "timer.h"
#include "graphicmanager.h"

/**
 * \class Application
 * \brief Basisklasse der Anwendung
 */

class Application
    : public Ogre::WindowEventListener, public Ogre::RenderSystem::Listener

{

	public:
		/**
		 * \fn Application()
		 * \brief Konstruktor
		 */
		Application();

		/**
		 * \fn ~Application()
		 * \brief Destruktor
		 */
		virtual ~Application();

		/**
		 * \fn run()
		 * \brief Startet die Applikation
		 */
		void run();

		/**
		 * \fn Document* getDocument()
		 * \brief Gibt einen Zeiger auf das Dokument aus
		 */
		Document* getDocument()
		{
			return m_document;
		}

	private:
		// ------------------------- Window Listener functions ----------------------

		/**
		 * \brief Resize the window. Adjust mouse clipping area for GUI?
		 */
		virtual void windowResized (Ogre::RenderWindow *rw);

		/**
		 * \brief The window received the command to close.
		 * It's recommended to return false in this function if there is a complex shutdown procedure to run (resource unload, etc.)
		 */
		virtual bool windowClosing (Ogre::RenderWindow *rw);

		/**
		 * \brief React to the window losing the focus.
		 * (You could trigger non-rendering functions here, such as pausing or resuming the music. Keep in mind synchronization issues for multithreading!)
		 */
		virtual void windowFocusChange (Ogre::RenderWindow* rw);

        /**
         * \brief React to certain Render system events
         */
        virtual void eventOccurred(const Ogre::String &eventName, const Ogre::NameValuePairList *parameters);

		// ------------------------- Other functions ----------------------

		/**
		* \fn init()
		* \brief Initialisiert die Anwendung
		 */
		bool init();

		/**
		 * \fn initOgre()
		 * \brief Initialisiert die Ogre Rendering Engine
		 */
		bool initOgre();

		/**
		 * \fn configureOgre()
		 * \brief Konfiguriert die Ogre Rendering Engine
		 */
		bool configureOgre();

		/**
		 * \fn setupResources()
		 * \brief Stellt die Ressourcen fuer Ogre und CEGUI ein
		 */
		bool setupResources();

		/**
		 * \fn initGettext()
		 * \brief Initialisiert Gettext
		 */
		bool initGettext();

		/**
		 * \fn initCEGUI()
		 * \brief Initialisiert CEGUI
		 */
		bool initCEGUI();

		/**
		 * \brief initialisiert OpenAL
		 */
		bool initOpenAL();

		/**
		 * \fn bool createView()
		 * \brief Erzeugt die Ansichten
		 */
		bool createView();

		/**
		 * \fn bool createDocument()
		 * \brief Erzeugt das Document
		 */
		bool createDocument();

		/**
		 * \brief loads the ressources specified by \a datagroups
		 * \param datagroups bitmask consisting of members of \ref World::DataGroups
		 */
		bool loadResources(int datagroups=0xfffffff);
		
		/**
		 * \brief Loads the menu music and commences playing it.
		 * Typically, this would be placed in the menu class, but that would not allow playing the music while the loading is performed
		 * (as the menu is not yet created).
		 */
		void loadAndPlayMenuMusic ();

		/**
		 * \brief clears the Ressources specified by \a datagroups
		 * \param datagroups bitmask consisting of members of \ref World::DataGroups
		 */
		void cleanup(int datagroups=0xfffffff);

		/**
		 * \fn virtual void update()
		 * \brief Aktualisierungen
		 */
		virtual void update();

		/**
		 * \fn void updateStartScreen(float percent)
		 * \brief aktualisiert den Startbildschirm
		 * \param percent Prozentsatz, zu dem das Laden der Daten fortgeschritten ist
		 */
		void updateStartScreen(float percent);
        
		/**
		 * \brief Sets (or updates) the icon to use for the application. The code inside the function is OS dependant.
		 */
		void setApplicationIcon ();

		// TODO: move to sumwarshelper?
		/**
		 * \brief Retrieves (via output parameters) the settings requested by the user for the video mode size
		 * \param[out] videoModeWidth The horizontal screen resolution of the video mode.
		 * \param[out] videoModeHeight The vertical screen resolution of the video mode.
		 * \return The full name of the entry for the resolution.
		 * \author Augustin Preda (if anything doesn't work, he's the one to bash).
		 */
		std::string retrieveRenderSystemWindowSize (int& videoModeWidth, int& videoModeHeight);


		// Member
		/**
		 * \var Ogre::Root *m_ogre_root
		 * \brief Basisobjekt der Ogre Engine
		 */
		Ogre::Root *m_ogre_root;

		/**
		 * \var CEGUI::System* m_cegui_system
		 * \brief Basisobjekt von CEGUI
		 */
		CEGUI::System *m_cegui_system;

		/**
		 * \var CEGUI::OgreRenderer* m_ogre_cegui_renderer
		 * \brief Objekt zum Rendern von CEGUI mittels Ogre
		 */
		//CEGUI::OgreRenderer *m_ogre_cegui_renderer;

		/**
		 * \var Ogre::RenderWindow *m_window
		 * \brief Fenster der Applikation
		 */
		Ogre::RenderWindow *m_window;

		/**
		 * \var Ogre::SceneManager *m_scene_manager
		 * \brief Objekt fuer das Management der Szene
		 */
 		Ogre::SceneManager *m_scene_manager;
        

 		/**
		 * \var bool m_shutdown
		 * \brief true, wenn die Anwendung beendet werden soll
		 */
		bool m_shutdown;


	protected:
		/**
		 * \var Document* m_document
		 * \brief Dokument, enthaelt alle Daten fuer die Darstellung
		 */
		Document* m_document;

		/**
		 * \var MainWindow* m_main_window
		 * \brief Hauptfenster der Applikation
		 */
		MainWindow* m_main_window;

		/**
		 * \var Timer m_timer
		 * \brief Timer fuer verschiedene Verwendungszwecke
		 */
		Timer m_timer;
		
		/**
		 * \brief Set to true if the application has reached the run loop
		 */
		bool m_running;
};

#endif // __SUMWARS_GUI_APPLICATION_H__
