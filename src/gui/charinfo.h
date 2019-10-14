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


#ifndef __SUMWARS_GUI_CHARINFO_H__
#define __SUMWARS_GUI_CHARINFO_H__

#include "window.h"

/**
 * \class CharInfo
 * \brief Fenster Charakter Info
 */
class CharInfo : public Window
{
	public:
		/**
		* \fn CharInfo (Document* doc)
		* \brief Konstruktor
		*/
		CharInfo (Document* doc);
	
		/**
		 * \fn virtual void update()
		 * \brief aktualisiert den Inhalt des Fensters
		 */
		virtual void update();
		
		/**
		 * \fn virtual void updateTranslation
		 * \brief aktualisiert die Uebersetzungen
		 */
		virtual void updateTranslation();
		
	private:
		/**
		 * \fn bool onIncreaseAttributeButtonClicked(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Klick auf Button Attribut erhoehen
		 */
		bool onIncreaseAttributeButtonClicked(const CEGUI::EventArgs& evt);
	
		/**
		 * \brief Handles clicks to the close button
		 */
		bool onCloseButtonClicked(const CEGUI::EventArgs& evt);
};

#endif // __SUMWARS_GUI_CHARINFO_H__


