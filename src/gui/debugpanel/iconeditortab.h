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

#ifndef ICONEDITORTAB_H
#define ICONEDITORTAB_H

#include <CEGUI/CEGUIWindow.h>
#include "debugtab.h"

class IconEditorTab : public CEGUI::Window, public DebugTab
{
public:
	IconEditorTab(const CEGUI::String& type, const CEGUI::String& name);
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

public:
	static CEGUI::String WidgetTypeName;
};

#endif // ICONEDITORTAB_H
