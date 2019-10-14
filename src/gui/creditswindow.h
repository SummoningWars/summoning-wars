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

#ifndef __SUMWARS_GUI_CREDITSWINDOW_H__
#define __SUMWARS_GUI_CREDITSWINDOW_H__

#include "window.h"

/**
 * \class CreditsWindow
 * \brief Window for displaying credits
 */
class CreditsWindow : public Window
{
	public:
		/**
		 * \fn CreditsWindow (Document* doc, const std::string& ceguiSkinName)
		 * \brief Constructor
		 * \param doc The document object to be used for interacting with the system.
		 * \param ceguiSkinName The name of the CEGUI skin to use when creating internal widgets.
		 */
		CreditsWindow (Document* doc, const std::string& ceguiSkinName);
	
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
		 * \brief Timer that counts the time, the Window is shown
		 */
		Timer m_shown_timer;

		/**
		 * \brief The name of the CEGUI skin to use.
		 */
		std::string m_ceguiSkinName;
};

#endif // __SUMWARS_GUI_CREDITSWINDOW_H__
