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

#ifndef __SUMWARS_GUI_CHARCREATE_H__
#define __SUMWARS_GUI_CHARCREATE_H__

#include "window.h"


/**
 * \class CharCreate
 * \brief Create and control the character window
 */
class CharCreate : public Window
{
	public:
		/**
		 * \fn CharCreate (Document* doc)
		 * \brief Constructor.
		 */
		CharCreate (Document* doc, const std::string& ceguiSkinName);
	
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
		
		/**
		 * \fn void updateClassList()
		 * \brief aktualisiert die Liste der Spielerklassen
		 */
		void updateClassList();
		
	private:
		/**
		 * \fn bool onGUIItemClick (const CEGUI::EventArgs& evt)
		 * \brief Handle the click of gui items.
		 */
		bool onGUIItemClick (const CEGUI::EventArgs& evt);

		/**
		 * \fn bool onGUIItemHover(const CEGUI::EventArgs& evt)
		 * \brief Handle the hovering of gui items.
		 */
		bool onGUIItemHover (const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onClassSelected(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Auswahl einer Klasse
		 */
		bool onClassSelected(const CEGUI::EventArgs& evt);
	
		/**
		 * \fn bool onLookSelected(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Auswahl eines Aussehens
		 */
		bool onLookSelected(const CEGUI::EventArgs& evt);
	
		/**
		 * \fn bool onButtonAbort(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Druck auf Knopf Abbrechen
		 */
		bool onButtonAbort(const CEGUI::EventArgs& evt);
	
		/**
		 * \fn bool onButtonCharCreate(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Druck auf Knopf Charakter erstellen
		 */
		bool onButtonCharCreate(const CEGUI::EventArgs& evt);

		/**
		 * \brief The name of the CEGUI skin to use.
		 */
		std::string m_ceguiSkinName;
};

#endif // __SUMWARS_GUI_CHARCREATE_H__
