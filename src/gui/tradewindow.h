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

#ifndef __SUMWARS_GUI_TRADEWINDOW_H__
#define __SUMWARS_GUI_TRADEWINDOW_H__

#include "itemwindow.h"

/**
 * \class TradeWindow
 * \brief Fenster den Handel mit NPCs
 */
class TradeWindow : public ItemWindow
{
	public:
		/**
	 * \fn TradeWindow (Document* doc)
	 * \brief Konstruktor
		 */
		TradeWindow (Document* doc, const std::string& ceguiSkinName);
	
		/**
		 * \fn virtual void update()
		 * \brief aktualisiert den Inhalt des Fensters
		 */
		virtual void update();
		
		/**
		 * \fn void reset()
		 * \brief Setzt das Fenster zurueck
		 */
		void reset();

		/**
		 * \fn virtual void updateTranslation
		 * \brief aktualisiert die Uebersetzungen
		 */
		virtual void updateTranslation();
	
	private:
		/**
		 * \fn bool onTradeItemHover(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Eintritt der Maus in Bereich ueber einem Item
		 */
		bool onTradeItemHover(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onTradeItemMouseButtonPressed(const CEGUI::EventArgs& evt);
		 * \brief Behandelt Druecken der linken Maustaste ueber einen Item
		 */
		bool onTradeItemMouseButtonPressed(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onTradeAreaMouseButtonPressed(const CEGUI::EventArgs& evt);
		 * \brief Behandelt Druecken der linken Maustaste ueber auf dem Handelsfenster
		 */
		bool onTradeAreaMouseButtonPressed(const CEGUI::EventArgs& evt);

		/**
		 * \fn bool onTradeItemMouseButtonReleased(const CEGUI::EventArgs& evt);
		 * \brief Behandelt Loslassen der linken Maustaste ueber einen Item
		 */
		bool onTradeItemMouseButtonReleased(const CEGUI::EventArgs& evt);
		
		
		/**
		 * \fn bool onTradeNextItems(const CEGUI::EventArgs& evt);
		 * \brief Behandelt Druck auf einen der Buttons zum weiterschalten
		 */
		bool onTradeNextItems(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onCloseTrade(const CEGUI::EventArgs& evt);
		 * \brief Behandelt Druck auf einen der Buttons zum weiterschalten
		 */
		bool onCloseTrade(const CEGUI::EventArgs& evt);
		
		/**
		 * \var int m_big_sheet
		 * \brief Seite der grossen Items die angezeigt wird
		 */
		int m_big_sheet;
		
		/**
		 * \var int m_medium_sheet
		 * \brief Seite der mittleren Items die angezeigt wird
		 */
		int m_medium_sheet;
		
		/**
		 * \var int m_small_sheet
		 * \brief Seite der kleinen Items die angezeigt wird
		 */
		int m_small_sheet;

		/**
		 * \brief The name of the CEGUI skin to use.
		 */
		std::string m_ceguiSkinName;

};

#endif // __SUMWARS_GUI_TRADEWINDOW_H__

