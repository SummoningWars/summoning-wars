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

#ifndef __SUMWARS_GUI_LISTITEM_H__
#define __SUMWARS_GUI_LISTITEM_H__

// Need tp add the cross version header here, as it may define symbols related to CEGUI_07

// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"

#include "CEGUI/CEGUI.h"

/**
 * \class ListItem
 * \brief Eintrag einer CEGUI Auswahlliste
 */
class ListItem : public CEGUI::ListboxTextItem
{
	public:
		ListItem(const CEGUI::String& skin, const CEGUI::String& text, unsigned int id=0) : ListboxTextItem(text,id)
		{
#ifdef CEGUI_07
			setSelectionBrushImage(skin, "MultiListSelectionBrush");
#else
			CEGUI::String summedUpName (skin);
			summedUpName.append ("/MultiListSelectionBrush");
			setSelectionBrushImage (summedUpName);

#endif
			
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
		StrListItem(const CEGUI::String& skin, const CEGUI::String& text, std::string data, unsigned int id=0) : ListItem(skin, text,id)
		{
#ifdef CEGUI_07
			setSelectionBrushImage(skin, "MultiListSelectionBrush");
#else
			CEGUI::String summedUpName (skin);
			summedUpName.append ("/MultiListSelectionBrush");
			setSelectionBrushImage (summedUpName);
#endif
			m_data = data;
		}

		std::string m_data;
};

#endif // __SUMWARS_GUI_LISTITEM_H__
