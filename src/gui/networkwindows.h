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

#ifndef __SUMWARS_GUI_NETWORKWINDOWS_H__
#define __SUMWARS_GUI_NETWORKWINDOWS_H__


#include "window.h"


/**
 * \class HostGameWindow
 * \brief Window for game hosting
 */
class HostGameWindow : public Window
{
	public:
		/**
		 * \fn HostGameWindow (Document* doc)
		 * \brief Konstruktor
		 */
		HostGameWindow (Document* doc);
	
		/**
		 * \fn virtual void update()
		 * \brief updates the content of the window.
		 */
		virtual void update();
		
		/**
		 * \fn virtual void updateTranslation
		 * \brief updates the translations
		 */
		virtual void updateTranslation();
	
	private:
		/**
		 * \fn bool onStartHostGame(const CEGUI::EventArgs& evt)
		 * \brief Handle the start of a multiplayer game (the command to start it via a button) as host.
		 */
		bool onStartHostGame(const CEGUI::EventArgs& evt);

		/**
		 * \fn bool onCancelHostGame(const CEGUI::EventArgs& evt)
		 * \brief Handle the cancel of a multiplayer game host command.
		 */
		bool onCancelHostGame(const CEGUI::EventArgs& evt);
};


/**
 * \class JoinGameWindow
 * \brief Window for game joining
 */
class JoinGameWindow : public Window
{
	public:
		/**
		 * \fn JoinGameWindow (Document* doc)
		 * \brief Konstruktor
		 */
		JoinGameWindow (Document* doc);
	
		/**
		 * \fn virtual void update()
		 * \brief updates the content of the window.
		 */
		virtual void update();
		
		/**
		 * \fn virtual void updateTranslation
		 * \brief updates the translations
		 */
		virtual void updateTranslation();
	
	private:
		/**
		 * \fn bool onStartJoinGame(const CEGUI::EventArgs& evt)
		 * \brief Handle the start of a multiplayer game (the command to start it via a button) as a client.
		 */
		bool onStartJoinGame(const CEGUI::EventArgs& evt);

		/**
		 * \fn bool onCancelJoinGame(const CEGUI::EventArgs& evt)
		 * \brief Handle the cancel of a multiplayer game join command.
		 */
		bool onCancelJoinGame(const CEGUI::EventArgs& evt);

};
#endif // __SUMWARS_GUI_NETWORKWINDOWS_H__

