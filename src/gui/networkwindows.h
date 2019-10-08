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

#ifndef NETWORKWINDOW_H
#define NETWORKWINDOW_H


#include "window.h"


/**
 * \class HostGameWindow
 * \brief Fenster Spiel hosten
 */
class HostGameWindow : public Window
{
	public:
	/**
	 * \fn HostGameWindow (Document* doc
	 * \brief Konstruktor
	 */
		HostGameWindow (Document* doc);
	
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
		 * \fn bool onStartHostGame(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Start eines Multiplayer Spiels als Host
		 */
		bool onStartHostGame(const CEGUI::EventArgs& evt);
};


/**
 * \class JoinGameWindow
 * \brief Fenster Spiel hosten
 */
class JoinGameWindow : public Window
{
	public:
	/**
	 * \fn JoinGameWindow (Document* doc
	 * \brief Konstruktor
	 */
		JoinGameWindow (Document* doc);
	
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
	 * \fn bool onStartJoinGame(const CEGUI::EventArgs& evt)
	 * \brief Behandelt Start eines Multiplayer Spiels als Host
		 */
		bool onStartJoinGame(const CEGUI::EventArgs& evt);
};
#endif

