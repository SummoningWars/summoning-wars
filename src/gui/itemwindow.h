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

#ifndef __SUMWARS_GUI_ITEMWINDOW_H__
#define __SUMWARS_GUI_ITEMWINDOW_H__

#include "window.h"
#include "item.h"
#include "itemlist.h"
/**
 * \class ItemWindow
 * \brief Fenster das Gegenstaende darstellen muss
 */
class ItemWindow : public Window
{
	public:
		/**
		 * \fn ItemWindow (Document* doc)
		 * \brief Konstruktor
		 */
		ItemWindow (Document* doc);
	
		/**
		 * \fn virtual void update()=0
		 * \brief aktualisiert den Inhalt des Fensters
		 */
		virtual void update() =0;

		
		/**
		 * \fn static void registerItemImage(Item::Subtype type, std::string image)
		 * \brief registiert ein Bild fuer ein bestimmten Itemtyp
		 * \param type Typ des Gegenstandes
		 * \param image Bild der Form set: ... image: ...
		 */
		static void registerItemImage(Item::Subtype type, std::string image);
		
	
		/**
		 * \fn static std::string getItemImage(Item::Subtype type)
		 * \brief Gibt zu dem angegebenen Itemtyp das zugehoerige Bild aus
		 */
		static std::string getItemImage(Item::Subtype type);
		
		/**
		 * \brief set silent status
		 * \param silent If set to true, this window will not play sounds
		 */
		void setSilent(bool silent)
		{
			m_silent = silent;
		}
		
	protected:
		
		
		/**
		 * \fn void updateItemWindowTooltip(CEGUI::Window* img, Item* item, Player* player, int gold=-1, float price_factor=1.0)
		 * \brief aktualisiert den Tooltip eines Items
		 * \param img CEGUI Fenster
		 * \param item Item das dargestellt werden soll
		 * \param player Spieler der das Item besitzt (bei Haendlern der Handelnde Spieler)
		 * \param gold Wenn nicht auf -1 gesetzt, wird geprueft, ob der Spieler sich das Item leisten kann
		 * \param price_factor Wenn ein Faktor ungleich 1 angegeben wird, so wird zusaetzlich der mit Faktor multiplizierte Wert angezeigt
		 */
		void updateItemWindowTooltip(CEGUI::Window* img, Item* item, Player* player, int gold=-1, float price_factor=1.0);
		
		/**
		 * \fn void updateItemWindow(CEGUI::Window* img, Item* item, Player* player, int gold=-1, )
		 * \brief aktualisiert das Bild eines Items
		 * \param img CEGUI Fenster
		 * \param item Item das dargestellt werden soll
		 * \param player Spieler der das Item besitzt (bei Haendlern der Handelnde Spieler)
		 * \param gold Wenn nicht auf -1 gesetzt, wird geprueft, ob der Spieler sich das Item leisten kann
		 */
		void updateItemWindow(CEGUI::Window* img, Item* item, Player* player, int gold=-1);
		
		/**
		 * \fn bool onItemHover(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Eintritt der Maus in Bereich ueber einem Item
		 */
		bool onItemHover(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onItemMouseButtonPressed(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Druecken der linken Maustaste ueber einen Item
		 */
		bool onItemMouseButtonPressed(const CEGUI::EventArgs& evt);

		/**
		 * \fn bool onItemMouseButtonReleased(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Loslassen der linken Maustaste ueber einen Item
		 */
		bool onItemMouseButtonReleased(const CEGUI::EventArgs& evt);
		
		
		
		
		/**
		 * \var static std::map<Item::Subtype, std::string> m_item_images
		 * \brief Enthaelt fuer jeden Itemtyp das passende Bild
		 */
		static std::map<Item::Subtype, std::string> m_item_images;
		
		/**
		 * \brief if set, this window may not play sounds
		 */
		bool m_silent;
		
		/**
		 * \brief if set, this window may not play sounds during current update
		 */
		bool m_silent_current_update;
};

#endif // __SUMWARS_GUI_ITEMWINDOW_H__
