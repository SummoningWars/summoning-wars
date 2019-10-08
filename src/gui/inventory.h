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

#ifndef INVENTORY_H
#define INVENTORY_H

#include "itemwindow.h"

/**
 * \class Inventory
 * \brief Fenster das Inventar
 */
class Inventory : public ItemWindow
{
	public:
		enum State
		{
			StateSmall = 0,
			StateMedium,
			StateBig
		};


		/**
	 * \fn Inventory (Document* doc)
	 * \brief Konstruktor
		 */
		Inventory (Document* doc);
	
		/**
		 * \fn virtual void update()
		 * \brief Updates the window contents
		 */
		virtual void update();

		/**
		 * \fn virtual void updateTranslation
		 * \brief Updates the translations
		 */
		virtual void updateTranslation();

		/**
		 * \fn virtual void setState
		 * \brief Sets the Inventory state
		 */
		void setState(State s);

	private:
		/**
		 * \fn bool onSwapEquipClicked(const CEGUI::EventArgs& evt)
		 * \brief Handles clicks to the equipment swap button
		 */
		bool onSwapEquipClicked(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onDropGoldClicked(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Mausklick auf Gold fallen lassen
		 */
		bool onDropGoldClicked(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onSwitchTabClicked(const CEGUI::EventArgs& evt)
		 * \brief Handles clicks to the switch tab button
		 */
		bool onSwitchTabClicked(const CEGUI::EventArgs& evt);
		
		/**
		 * \brief Handles clicks to the close button
		 */
		bool onCloseButtonClicked(const CEGUI::EventArgs& evt);

		/**
		 * \fn bool createAnimations()
		 * \brief Creates All animations used by the Inventory GUI
		 */
		void createAnimations();
};

#endif

