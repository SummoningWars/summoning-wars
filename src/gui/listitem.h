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

#ifndef LISTITEM_H
#define LISTITEM_H

#include "CEGUI/CEGUI.h"

/**
 * \class ListItem
 * \brief Eintrag einer CEGUI Auswahlliste
 */
class ListItem : public CEGUI::ListboxTextItem
{
	public:
		ListItem(const CEGUI::String& text, unsigned int id=0) : ListboxTextItem(text,id)
		{
			setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
			setAutoDeleted (true);
		}
};


/**
 * \class StrListItem
 * \brief Eintrag der Auswahlliste der Savegames
 */
class StrListItem : public ListItem
{
	public:
		StrListItem(const CEGUI::String& text, std::string data, unsigned int id=0) : ListItem(text,id)
		{
			setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
			m_data = data;
		}

		std::string m_data;
};

#endif
