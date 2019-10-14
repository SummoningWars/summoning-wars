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

#ifndef __SUMWARS_GUI_WINDOW_H__
#define __SUMWARS_GUI_WINDOW_H__


#include "document.h"
#include "gettext.h"

// CEGUI includes
#include "CEGUI/CEGUI.h"

/**
 * \fn class Window
 * \brief Wrapperklasse fuer ein CEGUI:: Window
 */
class Window
{
	public:
		/**
		* \fn Window(Document* doc)
		* \brief Konstruktor
		*/
		Window(Document* doc):
			m_window(0),
			m_document(doc)
		{}
		
		/**
		* \fn virtual ~Window()
		* \brief Destruktur
		*/
		virtual ~Window()
		{}
		
		/**
		 * \fn virtual void update() =0
		 * \brief aktualisiert den Inhalt des Fensters
		 */
		virtual void update() =0;
		
		/**
		 * \fn virtual void updateTranslation
		 * \brief aktualisiert die Uebersetzungen
		 */
		virtual void updateTranslation() =0;
		
		/**
		 * \fn bool consumeEvent(const CEGUI::EventArgs& evt)
		 * \brief Funktion die Events verbraucht, gibt immer true zurueck
		 */
		bool consumeEvent(const CEGUI::EventArgs& evt)
		{
			return true;
		}
		
		/**
		 * \fn CEGUI::Window* getCEGUIWindow()
		 * \brief Gibt das zugrunde liegende CEGUI Window aus
		 */
		CEGUI::Window* getCEGUIWindow()
		{
			return m_window;
		}
		
		/**
		 * \brief Returns the main document
		 */
		Document* getDocument()
		{
			return m_document;
		}
		
		
	protected:
		/**
		 * \var CEGUI::Window* m_window
		 * \brief das darunter liegende CEGUI Window
		 */
		CEGUI::Window* m_window;
		
		/**
		 * \var Document* m_document
		 * \brief Zeiger auf das Dokument
		 */
		Document* m_document;
		
		
	
};


#endif // __SUMWARS_GUI_WINDOW_H__


