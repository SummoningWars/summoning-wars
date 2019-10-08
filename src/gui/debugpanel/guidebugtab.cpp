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

#include "guidebugtab.h"
#include "CEGUI/CEGUI.h"

#include <iostream>

using namespace CEGUI;

CEGUI::String GuiDebugTab::WidgetTypeName = "GuiDebugTab";

GuiDebugTab::GuiDebugTab(const CEGUI::String& type, const CEGUI::String& name): CEGUI::Window(type, name), DebugTab()
{
	setText("Gui");
	
	m_tabLayout = WindowManager::getSingleton().loadWindowLayout("GuiDebugTab.layout");
	m_tabLayout->setPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
	m_tabLayout->setSize(UVector2(UDim(1.0f, 0.0f), UDim(1.0f, 0.0f)));

	this->addChildWindow(m_tabLayout);
}

void GuiDebugTab::initialiseComponents(void )
{
	CEGUI::Window::initialiseComponents();
}

void GuiDebugTab::update(OIS::Keyboard *keyboard, OIS::Mouse *mouse)
{
	CEGUI::Window *win = CEGUI::System::getSingleton().getWindowContainingMouse();
	m_tabLayout->getChild("windowUnderMouseStaticText")->setText(win->getName());
	m_tabLayout->getChild("typeStaticText")->setText(win->getType());

	std::ostringstream floatStringHelper;
    floatStringHelper << win->getSize().d_x.d_offset;
	m_tabLayout->getChild("xSizeLabel")->setText(floatStringHelper.str());
	
    floatStringHelper << win->getSize().d_y.d_offset;
	m_tabLayout->getChild("ySizeLabel")->setText(floatStringHelper.str());
	
	m_tabLayout->getChild("contentsTextbox")->setText(win->getText());
}


void GuiDebugTab::onSized(CEGUI::WindowEventArgs& e)
{
	CEGUI::Window::onSized(e);
}

