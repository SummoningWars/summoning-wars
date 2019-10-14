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

#ifndef __SUMWARS_CORE_QUEST_H__
#define __SUMWARS_CORE_QUEST_H__

#include "debug.h"
#include "eventsystem.h"

#include <string>
#include <sstream>

/**
 * \class Quest
 * \brief Klasse fuer eine Aufgabe, die die Spieler erledigen koennen
 */
class Quest
{
	public:
		/**
		* \enum State
		* \brief Aufzaehlung von Zustaenden fuer ein Quest
		*/
		enum State
		{
			NOT_STARTED =0,
			STARTED = 1,
			FINISHED = 2,
			FAILED = 3
		};
		
		/**
		 * \brief Konstruktor
		 * \param name Name des Quests
		 * \param table_name Name der Lua Tabelle in der die Variablen zu dem Quest gespeichert werden
		 */
		Quest(TranslatableString name, std::string table_name);
		
		/**
		 * \fn virtual ~Quest()
		 * \brief Destruktor
		 */
		virtual ~Quest();
		
		/**
		 * \fn void setInit(const char* init)
		 * \brief Setzt die Anweisungen fuer die Initialisierung
		 * \param init Zeiger auf Lua Code
		 */
		void setInit(const char* init);
		
		/**
		 * \fn void setDescription(const char*)
		 * \brief Setzt die Beschreibung des Quests
		 * \param descr Zeiger auf den Lua Code, der die Beschreibung zurueck gibt
		 */
		void setDescription(const char* descr);
		
		/**
		 * \fn void init()
		 * \brief Initialisiert das Quest
		 */
		void init();
		
		/**
		 * \fn State getState()
		 * \brief Gibt den Status des Quests aus
		 */
		State getState();
		
		/**
		 * \fn std::string getDescription()
		 * \brief Gibt die Beschreibung des Quests aus
		 */
		std::string getDescription();
		
		/**
		 * \brief Gibt den Name aus
		 */
		TranslatableString getName();
		
		/**
		 * \fn void toString(CharConv* cv)
		 * \brief Konvertiert das Objekt in einen String und schreibt ihn in der Puffer
		 * \param cv Ausgabepuffer
		 */
		virtual void toString(CharConv* cv);


		/**
		 * \fn void fromString(CharConv* cv)
		 * \brief Erzeugt das Objekt aus einem String
		 * \param cv Eingabepuffer
		 */
		virtual void fromString(CharConv* cv);
		
		/**
		 * \brief Setzt die Gettextdomain
		 */
		void setGettextDomain(std::string gettext_domain)
		{
			m_gettext_domain = gettext_domain;
		}
		
	
	private:
		/**
		 * \brief Name of the quest that is intended to be translated and displayed 
		 */
		TranslatableString m_name;
		
		/**
		 * \var std::string m_table_name
		 * \brief Name der Lua Tabelle in der die Variablen zu dem Quest gespeichert werden
		 */
		std::string m_table_name;
		
		
		/**
		 * \var int m_initialisation
		 * \brief Initialisierung des Quest (Handle auf eine Folge von Lua Anweisungen)
		 */
		int m_initialisation;
		
		
		/**
		 * \var int m_description
		 * \brief Handle auf eine Folge von Lua Anweisungen, die die Beschreibung ausgibt
		 */
		int m_description;
		
		/**
		 * \var std::string m_description_code
		 * \brief Lua Code, der die Beschreibung zurueck gibt
		 */
		std::string m_description_code;
		
		/**
		 * \brief gettext domain used for translating the description
		 */
		std::string m_gettext_domain;
		
};

#endif // __SUMWARS_CORE_QUEST_H__
