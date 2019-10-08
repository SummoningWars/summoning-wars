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

#ifndef LUASCRIPTTAB_H
#define LUASCRIPTTAB_H

#include <CEGUI/CEGUIWindow.h>
#include "debugtab.h"
#include "filebrowser/filebrowser.h"

class LuaScriptTab : public CEGUI::Window, public DebugTab
{
public:
    LuaScriptTab(const CEGUI::String& type, const CEGUI::String& name);
	virtual void update(OIS::Keyboard *keyboard, OIS::Mouse *mouse);

protected:
	virtual void onSized(CEGUI::WindowEventArgs& e);
	virtual void onTextChanged(CEGUI::WindowEventArgs& e);
	virtual void onMouseMove(CEGUI::MouseEventArgs& e);
	virtual void onMouseWheel(CEGUI::MouseEventArgs& e);
	virtual void onMouseButtonDown(CEGUI::MouseEventArgs& e);
	virtual void onMouseButtonUp(CEGUI::MouseEventArgs& e);
	virtual void onMouseClicked(CEGUI::MouseEventArgs& e);
	virtual void onMouseDoubleClicked(CEGUI::MouseEventArgs& e);
	virtual void onMouseTripleClicked(CEGUI::MouseEventArgs& e);
	virtual void onKeyDown(CEGUI::KeyEventArgs& e);
	virtual void onKeyUp(CEGUI::KeyEventArgs& e);
	virtual void onCharacter(CEGUI::KeyEventArgs& e);
	
	virtual bool handleNew(const CEGUI::EventArgs& e);
	virtual bool handleOpen(const CEGUI::EventArgs& e);
	virtual bool handleSave(const CEGUI::EventArgs& e);
	virtual bool handleClose(const CEGUI::EventArgs& e);
	virtual bool handleTabChanged(const CEGUI::EventArgs& e);
	virtual bool handleFileBrowserAcceptClicked(const CEGUI::EventArgs& e);
	virtual bool handleFileBrowserCancelClicked(const CEGUI::EventArgs& e);
	
	void createMenu();
protected:
	FileBrowser *m_fb;
	int m_newFileCtr;
	CEGUI::Window *m_tabLayout;
	CEGUI::Editbox *m_filePathEditBox;
	CEGUI::Menubar *m_menubar;
	CEGUI::TabControl *m_fileTabControl;
public:
	static CEGUI::String WidgetTypeName;
};

#endif // LUASCRIPTTAB_H
