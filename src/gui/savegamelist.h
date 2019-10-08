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

#ifndef SAVEGAMELIST_H
#define SAVEGAMELIST_H

#include "window.h"

#include "listitem.h"

/**
 * \class SavegameList
 * \brief Fenster Charakter Info
 */
class SavegameList : public Window
{
	public:
		/**
		 * \fn SavegameList (Document* doc)
		 * \brief Konstruktor
		 * \param doc Dokument
		 */
		SavegameList (Document* doc);
	
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
		 * \fn bool onDeleteCharConfirmClicked(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Klick auf Button Charakter loeschen (yes)
		 */
		bool onDeleteCharConfirmClicked(const CEGUI::EventArgs& evt);
		
		
		/**
		 * \fn bool onDeleteCharAbortClicked(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Klick auf Button Charakter loeschen (no)
		 */
		bool onDeleteCharAbortClicked(const CEGUI::EventArgs& evt);
		

		
	private:
		/**
		 * \fn bool onSavegameChosen(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Auswahl eines Savegames in der Liste
		 */
		bool onSavegameChosen(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onSavegameDoubleClick(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Auswahl eines Savegames in der Liste
		 */
		bool onSavegameDoubleClick(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onNewCharClicked(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Klick auf Button neuer Charakter
		 */
		bool onNewCharClicked(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onDeleteCharClicked(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Klick auf Button Charakter loeschen
		 */
		bool onDeleteCharClicked(const CEGUI::EventArgs& evt);
		
		

		/**
		 * \fn bool onSavegameSelected(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Auswahl eines Savegames
		 */
		bool onSavegameSelected(const CEGUI::EventArgs& evt);

		/**
		  *\brief Holds SaveItemRoot window belonging to currently selected savefile
		  */
		CEGUI::Window *m_currentSelected;

};

#endif
