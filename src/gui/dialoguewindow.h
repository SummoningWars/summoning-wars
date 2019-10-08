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

#ifndef DIALOGUEWINDOW_H
#define DIALOGUEWINDOW_H

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
		 * \brief Konstruktor
		 * \param doc Zeiger auf das Dokument
		 */
		DialogueWindow(Document* doc, Scene* scene);
		
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
		 * \brief Behandelt das Klick auf eine Antwort in einem Gespraech
		 * \param evt CEGUI Event Parameter
		 */
		bool onAnswerClicked(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool bool onTextClicked(const CEGUI::EventArgs& evt)
		 * \brief regiert auf einen Click auf einen Dialogtext
		 */
		bool onTextClicked(const CEGUI::EventArgs& evt);
		
	private:
		/**
		 * \var Scene* m_scene
		 * \brief  Zeiger auf die dargestellt Szene
		 */
		Scene* m_scene;
};

#endif

