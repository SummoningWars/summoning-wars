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

#ifndef SKILLTREE_H
#define SKILLTREE_H

#include "window.h"
#include "OISKeyboard.h"
/**
 * \class SkillTree
 * \brief Fenster Faehigkeitenbaum
 */
class SkillTree : public Window
{
	public:
		/**
		 * \fn SkillTree (Document* doc, OIS::Keyboard *keyboard)
		 * \param keyboard OIS keyboard
		 * \brief Konstruktor
		 */
		SkillTree (Document* doc, OIS::Keyboard *keyboard);
	
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
		 * \fn bool onAbilityHover(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Eintritt der Maus in Bereich ueber einer Faehigkeit
		 */
		bool onAbilityHover(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onAbilityHoverLeave(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Verlassen des Bereichs ueber einer Faehigkeit
		 */
		bool onAbilityHoverLeave(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn void updateAbilityTooltip(unsigned int pos)
		 * \brief Aktualisiert den Tooltip fuer die Faehigkeit ueber der die Maus ist
		 */
		void updateAbilityTooltip(unsigned int pos);
		
		/**
		 * \fn bool onSkillMouseClicked(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Mausklick auf eine Faehigkeit im Skilltree
		 */
		bool onSkillMouseClicked(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onSkillLearnMouseClicked(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Mausklick auf den Lernen Button zu einer Faehigkeit im Skilltree
		 */
		bool onSkillLearnMouseClicked(const CEGUI::EventArgs& evt);
		
		
		/**
		 * \brief Handles clicks to the close button
		 */
		bool onCloseButtonClicked(const CEGUI::EventArgs& evt);
		
		/**
		 * \var OIS::Keyboard *m_keyboard
		 * \brief Repraesentation der Tastatur
		 */
		OIS::Keyboard *m_keyboard;
		
		/**
		 * \var int m_player_id
		 * \brief ID des Spielers
		 */
		int m_player_id;
		
		/**
		 * \brief number of skilltree tabs
		 */
		int m_nr_tabs;
		
		/**
		 * \brief number of skills
		 */
		int m_nr_skills;
		
		/**
		 * \brief number of dependency connections
		 */
		int m_nr_dependencies;
		
		/**
		 * \brief number of shortkey labels
		 */
		int m_shortkey_labels;
};

#endif
