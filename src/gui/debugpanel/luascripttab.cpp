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

#include "luascripttab.h"
#include "CEGUI/CEGUI.h"
#include <iostream>
#include "textfileeditwindow.h"

#include "OgreString.h"

using namespace CEGUI;

CEGUI::String LuaScriptTab::WidgetTypeName = "LuaScriptTab";

LuaScriptTab::LuaScriptTab(const CEGUI::String& type, const CEGUI::String& name): CEGUI::Window(type, name), DebugTab()
{
	setText("Lua");
	m_newFileCtr = 0;
	
	m_tabLayout = WindowManager::getSingleton().loadWindowLayout("LuaScriptTab.layout");
	m_tabLayout->setPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
	m_tabLayout->setSize(UVector2(UDim(1.0f, 0.0f), UDim(1.0f, 0.0f)));

	m_fileTabControl = static_cast<TabControl*>(m_tabLayout->getChild("luaScriptTab/FileTabControl"));
	m_filePathEditBox = static_cast<Editbox*>(m_tabLayout->getChild("luaScriptTab/fileDirectoryEditBox"));
	m_filePathEditBox->setText("./data/lua/debug.lua");
	
	createMenu();
	
	this->addChildWindow(m_tabLayout);

	m_fileTabControl->subscribeEvent(TabControl::EventSelectionChanged, CEGUI::Event::Subscriber(&LuaScriptTab::handleTabChanged, this));
	
}

void LuaScriptTab::update(OIS::Keyboard *keyboard, OIS::Mouse *mouse)
{

}

void LuaScriptTab::onSized(CEGUI::WindowEventArgs& e)
{
    CEGUI::Window::onSized(e);
}

void LuaScriptTab::onTextChanged(CEGUI::WindowEventArgs& e)
{
    CEGUI::Window::onTextChanged(e);
}

void LuaScriptTab::onMouseMove(CEGUI::MouseEventArgs& e)
{
    CEGUI::Window::onMouseMove(e);
}

void LuaScriptTab::onMouseWheel(CEGUI::MouseEventArgs& e)
{
    CEGUI::Window::onMouseWheel(e);
}

void LuaScriptTab::onMouseButtonDown(CEGUI::MouseEventArgs& e)
{
    CEGUI::Window::onMouseButtonDown(e);
}

void LuaScriptTab::onMouseButtonUp(CEGUI::MouseEventArgs& e)
{
    CEGUI::Window::onMouseButtonUp(e);
}

void LuaScriptTab::onMouseClicked(CEGUI::MouseEventArgs& e)
{
    CEGUI::Window::onMouseClicked(e);
}

void LuaScriptTab::onMouseDoubleClicked(CEGUI::MouseEventArgs& e)
{
    CEGUI::Window::onMouseDoubleClicked(e);
}

void LuaScriptTab::onMouseTripleClicked(CEGUI::MouseEventArgs& e)
{
    CEGUI::Window::onMouseTripleClicked(e);
}

void LuaScriptTab::onKeyDown(CEGUI::KeyEventArgs& e)
{
    CEGUI::Window::onKeyDown(e);
}

void LuaScriptTab::onKeyUp(CEGUI::KeyEventArgs& e)
{
    CEGUI::Window::onKeyUp(e);
}

void LuaScriptTab::onCharacter(CEGUI::KeyEventArgs& e)
{
    CEGUI::Window::onCharacter(e);
}

bool LuaScriptTab::handleNew(const CEGUI::EventArgs& e)
{
	TextFileEditWindow *win = static_cast<TextFileEditWindow*>(WindowManager::getSingleton().createWindow("TextFileEditWindow"));
	m_fileTabControl->addTab(win);
	win->setPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
	win->setSize(UVector2(UDim(1.0f, 0.0f), UDim(1.0f, 0.0f)));
	win->handleTextChanged(CEGUI::EventArgs());
	m_newFileCtr++;
	return true;
}

bool LuaScriptTab::handleOpen(const CEGUI::EventArgs& e)
{
	m_fb = new FileBrowser();
	m_fb->init("/home/stefan/Dev/s07c/sumwars", FileBrowser::FB_TYPE_OPEN_FILE, true);
	m_fb->m_acceptBtn->subscribeEvent(PushButton::EventClicked, CEGUI::Event::Subscriber(&LuaScriptTab::handleFileBrowserAcceptClicked, this));
	m_fb->m_cancelBtn->subscribeEvent(PushButton::EventClicked, CEGUI::Event::Subscriber(&LuaScriptTab::handleFileBrowserCancelClicked, this));
	
	return true;
}

bool LuaScriptTab::handleSave(const CEGUI::EventArgs& e)
{
	TextFileEditWindow* win = static_cast<TextFileEditWindow*>(m_fileTabControl->getTabContentsAtIndex(m_fileTabControl->getSelectedTabIndex()));
	win->save();
	return true;
}

bool LuaScriptTab::handleClose(const CEGUI::EventArgs& e)
{
	TextFileEditWindow* win = static_cast<TextFileEditWindow*>(m_fileTabControl->getTabContentsAtIndex(m_fileTabControl->getSelectedTabIndex()));
	win->close();
	m_fileTabControl->removeTab(m_fileTabControl->getSelectedTabIndex());
	WindowManager::getSingleton().destroyWindow(win);
	return true;
}

bool LuaScriptTab::handleTabChanged(const CEGUI::EventArgs& e)
{
	TextFileEditWindow* win = static_cast<TextFileEditWindow*>(m_fileTabControl->getTabContentsAtIndex(m_fileTabControl->getSelectedTabIndex()));
	m_filePathEditBox->setText(win->getFilepath());
	return true;
}

bool LuaScriptTab::handleFileBrowserAcceptClicked(const CEGUI::EventArgs& e)
{
	if(m_fb->getType() == FileBrowser::FB_TYPE_OPEN_FILE)
	{
		TextFileEditWindow *win = static_cast<TextFileEditWindow*>(WindowManager::getSingleton().createWindow("TextFileEditWindow"));
		m_fileTabControl->addTab(win);
		win->setPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
		win->setSize(UVector2(UDim(1.0f, 0.0f), UDim(1.0f, 0.0f)));
		win->load(m_fb->getCurrentSelected());
		m_newFileCtr++;
	}

	m_fb->destroy();
	delete m_fb;
	m_fb = 0;

	return true;
}

bool LuaScriptTab::handleFileBrowserCancelClicked(const CEGUI::EventArgs& e)
{
	m_fb->destroy();
	delete m_fb;
	m_fb = 0;
	return true;
}


void LuaScriptTab::createMenu()
{
	m_menubar = static_cast<CEGUI::Menubar*>(m_tabLayout->getChild("luaScriptTab/MenuBar"));
	
	MenuItem *fileItem = static_cast<MenuItem*>(WindowManager::getSingleton().createWindow("TaharezLook/MenuItem", "luaScriptTab/MenuBar/FileItem"));
	fileItem->setText("File");
	m_menubar->addItem(fileItem);
	
	PopupMenu *filePopup = static_cast<PopupMenu*>(WindowManager::getSingleton().createWindow("TaharezLook/PopupMenu", "luaScriptTab/MenuBar/FilePopup"));
	
	MenuItem *it = static_cast<MenuItem*>(WindowManager::getSingleton().createWindow("TaharezLook/MenuItem", "luaScriptTab/MenuBar/FileItemNew"));
	it->setText("New");
	filePopup->addItem(it);
	it->subscribeEvent(MenuItem::EventClicked, CEGUI::Event::Subscriber(&LuaScriptTab::handleNew, this));
	
	it = static_cast<MenuItem*>(WindowManager::getSingleton().createWindow("TaharezLook/MenuItem", "luaScriptTab/MenuBar/FileItemOpen"));
	it->setText("Open");
	filePopup->addItem(it);
	it->subscribeEvent(MenuItem::EventClicked, CEGUI::Event::Subscriber(&LuaScriptTab::handleOpen, this));
	
	it = static_cast<MenuItem*>(WindowManager::getSingleton().createWindow("TaharezLook/MenuItem", "luaScriptTab/MenuBar/FileItemSave"));
	it->setText("Save");
	filePopup->addItem(it);
	it->subscribeEvent(MenuItem::EventClicked, CEGUI::Event::Subscriber(&LuaScriptTab::handleSave, this));
	
	it = static_cast<MenuItem*>(WindowManager::getSingleton().createWindow("TaharezLook/MenuItem", "luaScriptTab/MenuBar/FileItemClose"));
	it->setText("Close");
	filePopup->addItem(it);
	it->subscribeEvent(MenuItem::EventClicked, CEGUI::Event::Subscriber(&LuaScriptTab::handleClose, this));
	
	fileItem->setPopupMenu(filePopup);
}
