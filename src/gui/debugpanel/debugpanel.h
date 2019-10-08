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

#ifndef DEBUGPANEL_H
#define DEBUGPANEL_H

#include <OgreSingleton.h>
#include "CEGUI/CEGUI.h"
#include "debugtab.h"

namespace OIS
{
	class Keyboard;
	class Mouse;
}

#include <map>

class DebugPanel : public Ogre::Singleton<DebugPanel>
{
public:
	/**
	* \fn void init(bool visible);
	* \brief Initialises the panel
	* \param visible Whether the panel is visible or not at the begining
	*/
	void init(bool visible);
	
	
	/**
	* \fn void toogleVisibility();
	* \brief Toggle the visibility
	*/
	void toogleVisibility();

	
	/**
	* \fn void update();
	* \brief Updates all registered tabs
	*/
	void update(OIS::Keyboard *keyboard, OIS::Mouse *mouse);

	/**
	* \fn void addTabWindow(CEGUI::Window *tab);
	* \brief Adds a tab DebugTab window to the DebugPanel
	* \param tab The DebugTab to add
	*/
	void addTabWindow(std::string name, DebugTab *tab);

	/**
	* \fn void tabExists(std::string tabName);
	* \brief Checks if a DebugTab exists
	* \param tabName Name of the DebugTab to check
	*/
	bool tabExists(std::string tabName);
	
	static DebugPanel& getSingleton(void);
	static DebugPanel* getSingletonPtr(void);


protected:
	/**
	* \fn createPanel(bool visible);
	* \brief Creates the panel
	* \param visible Whether the panel is visible or not at the begining
	*/
	void createPanel(bool visible);
	
	/**
	* \fn handleCloseWindow(const CEGUI::EventArgs& e);
	* \brief Handles clicks on the "Close Window" button
	* \param e Event Args from CEGUI
	*/
	virtual bool handleCloseWindow(const CEGUI::EventArgs& e);
	
private:
	/**
	* \var 	CEGUI::System *m_guiSystem;
	* \brief  Pointer to the CEGUI::System
	*/
	CEGUI::System *m_guiSystem;
	
	/**
	* \var 	CEGUI::WindowManager *m_winManager;
	* \brief  Pointer to the CEGUI::WindowManager
	*/
	CEGUI::WindowManager *m_winManager;
	
	/**
	* \var 	CEGUI::Window *m_rootWindow;
	* \brief  Pointer to the root window of this panel
	*/
	CEGUI::Window *m_rootWindow;

	/**
	* \var 	CEGUI::Window* m_gameScreen;
	* \brief  Pointer to the root gamescreen
	*/
	CEGUI::Window* m_gameScreen;

	/**
	* \var 	std::map<std::string, DebugTab*>;
	* \brief Holds all registered DebugTabs
	*/
	std::map<std::string, DebugTab*> m_tabs;
    CEGUI::TabControl* m_tabControl;
	
	/**
	* \var 	float m_lastVisibilitySwitch;
	* \brief Buffers the last visibility toggle time
	*/
	long m_lastVisibilitySwitch;
	
};

#endif // DEBUGPANEL_H
