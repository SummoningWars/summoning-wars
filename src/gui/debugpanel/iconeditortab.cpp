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

#include "iconeditortab.h"
#include "CEGUI/CEGUI.h"
using namespace CEGUI;

CEGUI::String IconEditorTab::WidgetTypeName = "IconEditorTab";

IconEditorTab::IconEditorTab(const CEGUI::String& type, const CEGUI::String& name): CEGUI::Window(type, name), DebugTab()
{
	setText("IconEd");

	CEGUI::Window *m_tabLayout = WindowManager::getSingleton().loadWindowLayout("IconEditTab.layout");
	m_tabLayout->setPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
	m_tabLayout->setSize(UVector2(UDim(1.0f, 0.0f), UDim(1.0f, 0.0f)));
	this->addChildWindow(m_tabLayout);
}


void IconEditorTab::update(OIS::Keyboard *keyboard, OIS::Mouse *mouse)
{

}

void IconEditorTab::onSized(CEGUI::WindowEventArgs& e)
{
	CEGUI::Window::onSized(e);
}

void IconEditorTab::onTextChanged(CEGUI::WindowEventArgs& e)
{
	CEGUI::Window::onTextChanged(e);
}

void IconEditorTab::onMouseMove(CEGUI::MouseEventArgs& e)
{
	CEGUI::Window::onMouseMove(e);
}

void IconEditorTab::onMouseWheel(CEGUI::MouseEventArgs& e)
{
	CEGUI::Window::onMouseWheel(e);
}

void IconEditorTab::onMouseButtonDown(CEGUI::MouseEventArgs& e)
{
	CEGUI::Window::onMouseButtonDown(e);
}

void IconEditorTab::onMouseButtonUp(CEGUI::MouseEventArgs& e)
{
	CEGUI::Window::onMouseButtonUp(e);
}

void IconEditorTab::onMouseClicked(CEGUI::MouseEventArgs& e)
{
	CEGUI::Window::onMouseClicked(e);
}

void IconEditorTab::onMouseDoubleClicked(CEGUI::MouseEventArgs& e)
{
	CEGUI::Window::onMouseDoubleClicked(e);
}

void IconEditorTab::onMouseTripleClicked(CEGUI::MouseEventArgs& e)
{
	CEGUI::Window::onMouseTripleClicked(e);
}

void IconEditorTab::onKeyDown(CEGUI::KeyEventArgs& e)
{
	CEGUI::Window::onKeyDown(e);
}

void IconEditorTab::onKeyUp(CEGUI::KeyEventArgs& e)
{
	CEGUI::Window::onKeyUp(e);
}

void IconEditorTab::onCharacter(CEGUI::KeyEventArgs& e)
{
	CEGUI::Window::onCharacter(e);
}
