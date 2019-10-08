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

#include "debugpanel.h"
#include "guidebugtab.h"
#include "debugcameratab.h"
#include "luascripttab.h"
#include "iconeditortab.h"
#include "benchmarktab.h"
#include "contenttab.h"
//#include "reloadtab.h"
#include "OgreRoot.h"

using namespace CEGUI;

template<> DebugPanel* Ogre::Singleton<DebugPanel>::ms_Singleton = 0;

void DebugPanel::init(bool visible)
{
	m_guiSystem = System::getSingletonPtr();
	m_winManager = WindowManager::getSingletonPtr();
	m_gameScreen = m_winManager->getWindow("GameScreen");
	m_lastVisibilitySwitch = 0;
	createPanel(visible);
}

void DebugPanel::toogleVisibility()
{
	if((Ogre::Root::getSingleton().getTimer()->getMilliseconds() - m_lastVisibilitySwitch) > 500)
	{
		m_rootWindow->setVisible(!m_rootWindow->isVisible());
		m_lastVisibilitySwitch = Ogre::Root::getSingleton().getTimer()->getMilliseconds();
	}
}


void DebugPanel::createPanel(bool visible)
{
	m_rootWindow = m_winManager->createWindow("TaharezLook/FrameWindow", "DebugPanel");
	m_rootWindow->setPosition(UVector2(UDim(0.1f, 0.0f), UDim(0.1f, 0.0f)));
	m_rootWindow->setSize(UVector2(UDim(0.6f, 0.0f), UDim(0.6f, 0.0f)));
	m_rootWindow->setText((CEGUI::utf8*)"Debug Panel");
	m_gameScreen->addChildWindow(m_rootWindow);

	m_rootWindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, Event::Subscriber(&DebugPanel::handleCloseWindow, this));
	
	m_tabControl = static_cast<TabControl*>(m_winManager->createWindow("TaharezLook/TabControl", "DebugPanelTabControl"));
	m_tabControl->setPosition(UVector2(UDim(0.03f, 0.0f), UDim(0.06f, 0.0f)));
	m_tabControl->setSize(UVector2(UDim(0.95f, 0.0f), UDim(0.9f, 0.0f)));
	m_rootWindow->addChildWindow(m_tabControl);
	
	GuiDebugTab *guiTab = static_cast<GuiDebugTab*>(m_winManager->createWindow("GuiDebugTab", "GuiDebugTab"));
	guiTab->setPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
	guiTab->setSize(UVector2(UDim(1.0f, 0.0f), UDim(1.0f, 0.0f)));
	addTabWindow("GuiDebugTab", guiTab);

	LuaScriptTab *luaTab = static_cast<LuaScriptTab*>(m_winManager->createWindow("LuaScriptTab", "LuaScriptTab"));
	luaTab->setPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
	luaTab->setSize(UVector2(UDim(1.0f, 0.0f), UDim(1.0f, 0.0f)));
	addTabWindow("LuaScriptTab", luaTab);
	
	IconEditorTab *iconEdTab = static_cast<IconEditorTab*>(m_winManager->createWindow("IconEditorTab", "IconEditorTab"));
	iconEdTab->setPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
	iconEdTab->setSize(UVector2(UDim(1.0f, 0.0f), UDim(1.0f, 0.0f)));
	addTabWindow("IconEditorTab", iconEdTab);

	BenchmarkTab *benchTab = static_cast<BenchmarkTab*>(m_winManager->createWindow("BenchmarkTab", "BenchmarkTab"));
	benchTab->setPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
	benchTab->setSize(UVector2(UDim(1.0f, 0.0f), UDim(1.0f, 0.0f)));
	addTabWindow("BenchmarkTab", benchTab);
	
	DebugCameraTab *camTab = static_cast<DebugCameraTab*>(m_winManager->createWindow("DebugCamera", "DebugCamera"));
	camTab->setPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
	camTab->setSize(UVector2(UDim(1.0f, 0.0f), UDim(1.0f, 0.0f)));
	addTabWindow("DebugCamera", camTab);
	/* not yet implemented
	ReloadTab *reloadTab = static_cast<ReloadTab*>(m_winManager->createWindow("ReloadTab", "ReloadTab"));
	reloadTab->setPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
	reloadTab->setSize(UVector2(UDim(1.0f, 0.0f), UDim(1.0f, 0.0f)));
	addTabWindow("ReloadTab", reloadTab);
	*/
	if(!visible)
	  m_rootWindow->setVisible(visible);
}

void DebugPanel::addTabWindow(std::string name, DebugTab* tab)
{
	if(!tabExists(name))
	{
		m_tabs[name] = tab;
		m_tabControl->addTab(dynamic_cast<CEGUI::Window*>(tab));
	}
}

bool DebugPanel::tabExists(std::string tabName)
{
	if(m_tabs.find(tabName) != m_tabs.end())
		return true;

	return false;
}


void DebugPanel::update(OIS::Keyboard *keyboard, OIS::Mouse *mouse)
{
	if(m_tabs.size() < 1)
		return;
	
	if(keyboard->isKeyDown(OIS::KC_LCONTROL) && keyboard->isKeyDown(OIS::KC_D))
		toogleVisibility();
	
	std::map<std::string, DebugTab*>::iterator iter;
	for (iter = m_tabs.begin(); iter != m_tabs.end(); iter++)
	{
		DebugTab *dt = iter->second;
		dt->update(keyboard, mouse);
	}
}


bool DebugPanel::handleCloseWindow(const CEGUI::EventArgs& e)
{
	m_rootWindow->hide();
	return true;
}


DebugPanel* DebugPanel::getSingletonPtr(void)
{
	return ms_Singleton;
}

DebugPanel& DebugPanel::getSingleton(void)
{
	assert( ms_Singleton );
	return ( *ms_Singleton );
}


