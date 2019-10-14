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

#ifndef __SUMWARS_GUI_QUESTINFO_H__
#define __SUMWARS_GUI_QUESTINFO_H__


#include "window.h"
#include "listitem.h"


/**
 * \class QuestInfo
 * \brief Fenster Questinformationen
 */
class QuestInfo : public Window
{
	public:
		/**
		 * \fn QuestInfo (Document* doc, const std::string& ceguiSkinName)
		 * \brief Constructor
		 */
		QuestInfo (Document* doc, const std::string& ceguiSkinName);
	
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
		 * \fn bool onQuestSelected(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Auswahl einer Klasse
		 */
		bool onQuestSelected(const CEGUI::EventArgs& evt);
	
		/**
		 * \fn bool onLookSelected(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Auswahl eines anderen Filters
		 */
		bool onFilterSelected(const CEGUI::EventArgs& evt);
	
		/**
		 * \fn void updateDescription()
		 * \brief aktualisiert die Beschreibung
		 */
		void updateDescription();
	
		/**
		* \brief Handles clicks to the close button
		*/
		bool onCloseButtonClicked(const CEGUI::EventArgs& evt);

		/**
		 * \fn std::string m_questname
		 * \brief Name des aktuell angezeigten Quests
		 */
		std::string m_questname;

		/**
		 * \brief The name of the CEGUI skin to use.
		 */
		std::string m_ceguiSkinName;

};

#endif // __SUMWARS_GUI_QUESTINFO_H__
