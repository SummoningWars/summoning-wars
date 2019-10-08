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

#ifndef GUITABS_H
#define GUITABS_H

#include <contenttab.h>
#include <CEGUI/CEGUIWindow.h>
#include "OgreLog.h"

namespace Ogre
{
	class Root;
	class SceneManager;
	class Camera;
	class RenderTexture;
}

class GuiTabs : public CEGUI::Window, public ContentTab, public Ogre::LogListener
{
public:
	GuiTabs(const CEGUI::String& type, const CEGUI::String& name);
	
	virtual void update();

	virtual void messageLogged (const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName);

	/**
	 * \fn void addTabWindow(CEGUI::Window *tab);
	 * \brief Adds a tab ContentTab window to the ContentEditor
	 * \param tab The ContentTab to add
	 */
	void addTabWindow(std::string name, ContentTab *tab);
	
	/**
	 * \param evt CEGUI event arguments
	 * \brief Called upon selecting a mesh from the combobox
	 * Updates the content of the editor scene to display the selected mesh.
	 */
	bool onMeshSelected(const CEGUI::EventArgs& evt);

	/**
	 * \param evt CEGUI event arguments
	 * \brief Called upon selecting a mesh from the submesh combobox
	 */
	bool onSubMeshSelected(const CEGUI::EventArgs& evt);
	
	/**
	 * \fn void tabExists(std::string tabName);
	 * \brief Checks if a ContentTab exists
	 * \param tabName Name of the ContentTab to check
	 */
	bool tabExists(std::string tabName);
private:
	CEGUI::ListboxTextItem *m_listItem;
	CEGUI::ListboxTextItem *m_subMeshListItem;
	CEGUI::Window *m_tabLayout;
	CEGUI::MultiLineEditbox *m_CapsBox;
	CEGUI::PushButton *m_startBenchmarkButton;
	Ogre::Log *m_log;
	Ogre::Root *m_ogreRoot;
	Ogre::SceneManager *m_sceneManager;
	/**
	 * \var 	CEGUI::Window *m_rootWindow;
	 * \brief  Pointer to the root window of this panel
	 */
	CEGUI::Window *m_rootWindow;
	/**
	 * \var 	std::map<std::string, ContentTab*>;
	 * \brief Holds all registered ContentTabs
	 */
	std::map<std::string, ContentTab*> m_tabs;
    CEGUI::TabControl* m_tabControl;
	/**
	 * \var 	CEGUI::WindowManager *m_winManager;
	 * \brief  Pointer to the CEGUI::WindowManager
	 */
	CEGUI::WindowManager *m_winManager;

	void initMeshSelector();
	bool onCloseButton(const CEGUI::EventArgs& evt);
	void onAddSubMesh(const CEGUI::EventArgs& evt);
public:
	static CEGUI::String WidgetTypeName;
};

#endif // GUITABS_H
