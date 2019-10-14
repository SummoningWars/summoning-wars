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
		 * \fn SavegameList (Document* doc, const std::string& ceguiSkinName);)
		 * \brief Constructor
		 * \param doc Document
		 * \param ceguiSkinName The name of the cegui skin to use for any widgets created internally.
		 */
		SavegameList (Document* doc, const std::string& ceguiSkinName);
	
		/**
		 * \fn virtual void update()
		 * \brief Update the current widget's (window) content.
		 */
		virtual void update();
		
		/**
		 * \brief Selects the default savegame stored in the options
		 */
		void selectDefaultSavegame();
		
		/**
		 * \fn virtual void updateTranslation
		 * \brief Updates the translations. Will reapply the texts to various items contained in this widget.
		 */
		virtual void updateTranslation();
		
		/**
		 * \fn bool onDeleteCharConfirmClicked(const CEGUI::EventArgs& evt)
		 * \brief Handle the confirmation for a character deletion request (Equivalent to answering "yes")
		 */
		bool onDeleteCharConfirmClicked(const CEGUI::EventArgs& evt);
		
		
		/**
		 * \fn bool onDeleteCharAbortClicked(const CEGUI::EventArgs& evt)
		 * \brief Handle the abort for a character deletion request (Equivalent to answering "no")
		 */
		bool onDeleteCharAbortClicked(const CEGUI::EventArgs& evt);
		

		
	private:
		/**
		 * \fn bool onItemButtonHover(const CEGUI::EventArgs& evt)
		 * \brief Handle the hovering of a menu item
		 */
		bool onItemButtonHover(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onSavegameChosen(const CEGUI::EventArgs& evt)
		 * \brief Handle the selection of a saved game in the list.
		 */
		bool onSavegameChosen(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onSavegameDoubleClick(const CEGUI::EventArgs& evt)
		 * \brief Handle the selection of a saved game in the list.
		 */
		bool onSavegameDoubleClick(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onNewCharClicked(const CEGUI::EventArgs& evt)
		 * \brief Handle the click on the button to create a New character.
		 */
		bool onNewCharClicked(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onDeleteCharClicked(const CEGUI::EventArgs& evt)
		 * \brief Handle the click on a character's Delete button
		 */
		bool onDeleteCharClicked(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onSavegameSelected(const CEGUI::EventArgs& evt)
		 * \brief Handle the selection of a saved game in the list.
		 */
		bool onSavegameSelected(const CEGUI::EventArgs& evt);

		/**
		  *\brief Holds SaveItemRoot window belonging to currently selected savefile
		  */
		CEGUI::Window *m_currentSelected;


        /**
          *\brief Holds holds the number of character selection windows currently visible
          */
        int m_numCurrentCharacterButtons;

		/**
		 * \brief The name of the CEGUI skin to use.
		 */
		std::string m_ceguiSkinName;

		/**
		 * \brief The mapping between the file names and the buttons used for the chars.
		 */
		std::map <std::string, std::string> m_fileSaveMapping;
};

#endif
