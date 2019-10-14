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

#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <OgreSingleton.h>
#include "CEGUI/CEGUI.h"
#include "debugtab.h"
#include "OISKeyboard.h"
#include "boost/filesystem/path.hpp"

#include <map>



class FileBrowser
{
public:
	
	enum FileBrowserType
	{
		FB_TYPE_SAVE_FILE = 1,
		FB_TYPE_OPEN_FILE = 10,
		FB_TYPE_SELECT_DIRECTORY = 20
	};
	
	~FileBrowser();

	/**
	* \fn void init(CEGUI::String defaultDir, bool visible);
	* \brief Initialises the panel
	* \param dir The directory that will be opened when initialzing
	* \param type The FileBrowser type
	* \param visible Whether the panel is visible or not at the begining
	* \param ceguiSkinName The name of the CEGUI skin to use for the file browser.
	*/
	void init(CEGUI::String defaultDir, FileBrowserType type, bool visible, const std::string& ceguiSkinName);
	
	/**
	* \fn void destroy();
	* \brief Initialises the panel
	*/
	void destroy();
	
	/**
	* \fn bool handleCloseWindow(const CEGUI::EventArgs& e);
	* \brief Handles clicks on the "Close Window" button
	* \param e Event Args from CEGUI
	*/
	virtual bool handleCloseWindow(const CEGUI::EventArgs& e);
	
	/**
	* \fn bool handleBrowserDblClick(const CEGUI::EventArgs& e);
	* \brief Handles Doubleclicks on items in the browser
	* \param e Event Args from CEGUI
	*/
	virtual bool handleBrowserDblClick(const CEGUI::EventArgs& e);
	
	/**
	* \fn bool handlePopDirectory(const CEGUI::EventArgs& e);
	* \brief Handles clicks on the pop directory button
	* \param e Event Args from CEGUI
	*/
	virtual bool handlePopDirectory(const CEGUI::EventArgs& e);

	/**
	* \fn bool handleSelectionChanged(const CEGUI::EventArgs& e);
	* \brief Handles ecerything when selection in the browser is changed
	* \param e Event Args from CEGUI
	*/
	virtual bool handleSelectionChanged(const CEGUI::EventArgs& e);

	/**
	* \fn CEGUI::String getCurrentSelected();
	* \brief Returns the current selected value
	*/
	virtual CEGUI::String getCurrentSelected();

	/**
	* \fn FileBrowserType getType();
	* \brief Returns filebrowser type
	*/
	virtual FileBrowserType getType() { return m_type; }


	/**
	* \var 	CEGUI::Window *m_acceptBtn;
	* \brief  Pointer to the Accept (Open,Save,Select) button window of this panel
	*/
	CEGUI::PushButton *m_acceptBtn;
	
		/**
	* \var 	CEGUI::Window *m_cancelBtn;
	* \brief  Pointer to the Accept (Open,Save,Select) button window of this panel
	*/
	CEGUI::PushButton *m_cancelBtn;
	
protected:
	/**
	* \fn void fillBrowser();
	* \brief Fills the browser window
	*/
	void fillBrowser();

	/**
	* \fn void createLayout();
	* \brief Creates the window layout
	*/
	void createLayout();

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
	* \var 	CEGUI::Window* m_gameScreen;
	* \brief  Pointer to the root gamescreen
	*/
	CEGUI::Window* m_gameScreen;

	/**
	* \var 	CEGUI::Window *m_rootWindow;
	* \brief  Pointer to the root window of this panel
	*/
	CEGUI::Window *m_rootWindow;	

	/**
	* \var 	CEGUI::Editbox* m_pathBox;
	* \brief  Pointer to the Editbox that shows the current path
	*/
	CEGUI::Editbox* m_pathBox;

	/**
	* \var 	CEGUI::MultiColumnList* m_browserBox;
	* \brief  Holds the current directoryview
	*/
	CEGUI::MultiColumnList* m_browserBox;
	
	/**
	* \var 	CEGUI::EditBox* m_fileNameBox;
	* \brief  Pointer to the Editbox that shows the current filename
	*/
	CEGUI::Editbox* m_fileNameBox;
	
	/**
	* \var 	FileBrowserType m_type;
	* \brief  Type of the FileBrowser
	*/
	FileBrowserType m_type;
	
	/**
	* \var 	std::list<std::string> m_dirs;
	* \brief  Holds the directorys currently shown
	*/
    std::list<std::string> m_dirs;
	
	/**
	* \var 	std::list<std::string> m_files;
	* \brief  Holds the files currently shown
	*/
    std::list<std::string> m_files;

	/**
    * \var 	boost::filesystem::path m_currentPath;
	* \brief  Holds the path currently viewed
	*/
    boost::filesystem::path m_currentPath;

	/**
	 * \brief The name of the CEGUI skin to use.
	 */
	std::string m_ceguiSkinName;

};

#endif // DEBUGPANEL_H
