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

#ifndef TEXTFILEEDITWINDOW_H
#define TEXTFILEEDITWINDOW_H

#ifdef CEGUI_07
#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/CEGUIEvent.h>
#else
#include "CEGUI/Window.h"
#include "CEGUI/Event.h"
#endif

#include "filebrowser/filebrowser.h"

#include "boost/filesystem/path.hpp"

class TextFileEditWindow : public CEGUI::Window
{
public:
    TextFileEditWindow(const CEGUI::String& type, const CEGUI::String& name);
	void close();
	void save();
	bool load(const CEGUI::String &m_file);
    CEGUI::String getFilepath() { return m_file.string(); }
	void setFilepath(CEGUI::String path);
	
	virtual bool handleTextChanged(const CEGUI::EventArgs& e);
	virtual bool handleCharacterKey(const CEGUI::EventArgs& e);
	virtual bool handleFileBrowserSaveClicked(const CEGUI::EventArgs& e);
	virtual bool handleFileBrowserCancelClicked(const CEGUI::EventArgs& e);
	
protected:
	CEGUI::Event::Connection m_handleTextChangedConnection;
    boost::filesystem::path m_file;
	CEGUI::MultiLineEditbox *m_textEditBox;
	bool m_isDirty;
public:
	FileBrowser *m_fb;
	static CEGUI::String WidgetTypeName;
private:
	void getNewFileName();
	
	/**
	 * \brief The name of the CEGUI skin to use.
	 */
	std::string m_ceguiSkinName;

};

#endif // TEXTFILEEDITWINDOW_H
