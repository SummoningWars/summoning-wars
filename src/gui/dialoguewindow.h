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

#ifndef __SUMWARS_GUI_DIALOGUEWINDOW_H__
#define __SUMWARS_GUI_DIALOGUEWINDOW_H__

#include "scene.h"

#include "window.h"
/**
 * \class DialogueWindow
 * \brief Klasse fuer die Darstellung der Dialoge
 */
class DialogueWindow : public Window
{
	public:
		
		/**
		 * \fn Window(Document* doc)
		 * \brief Constructor.
		 * \param doc Pointer to the document object.
		 * \param ceguiSkinName The name of the CEGUI skin to use for this window
		 */
		DialogueWindow(Document* doc, Scene* scene, const std::string& ceguiSkinName);
		
		/**
		 * \fn virtual ~DialogueWindow()
		 * \brief Destruktor
		 */
		virtual ~DialogueWindow()
		{
			
		}
		
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
		 * \fn void updateSpeechBubbles()
		 * \brief aktualisiert die Sprechblasen
		 */
		void updateSpeechBubbles();
		
		/**
		 * \fn bool onAnswerClicked(const CEGUI::EventArgs& evt);
		 * \brief Handles the click on one of the answers in a Dialogue
		 * \param evt CEGUI Event Parameter
		 */
		bool onAnswerClicked(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onAnswerEnterArea(const CEGUI::EventArgs& evt);
		 * \brief Handles the hovering with the mouse on one of the answers in a Dialogue
		 * \param evt CEGUI Event Parameter
		 */
		bool onAnswerEnterArea(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onAnswerLeaveArea(const CEGUI::EventArgs& evt);
		 * \brief Handles the stopping of hovering with the mouse on one of the answers in a Dialogue
		 * \param evt CEGUI Event Parameter
		 */
		bool onAnswerLeaveArea(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool bool onTextClicked(const CEGUI::EventArgs& evt)
		 * \brief regiert auf einen Click auf einen Dialogtext
		 */
		bool onTextClicked(const CEGUI::EventArgs& evt);
		
	private:
		/**
		 * \var Scene* m_scene
		 * \brief  Pointer to the currently presented scene
		 */
		Scene* m_scene;

		/**
		 * \brief The name of the CEGUI skin to use.
		 */
		std::string m_ceguiSkinName;

};

#endif // __SUMWARS_GUI_DIALOGUEWINDOW_H__

