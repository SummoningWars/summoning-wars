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

#ifndef __SUMWARS_CORE_EVENT_H__
#define __SUMWARS_CORE_EVENT_H__

#include <string>
#include <map>
#include <sstream>

extern "C"
{
	
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "geometry.h"

/**
 * Typ eines Triggers
 */
typedef std::string TriggerType;

/**
 * \class Trigger
 *  \brief Ausloeser fuer ein Event
 */
class Trigger
{
	public:
		/**
		 * \fn Trigger(TriggerType type)
		 * \brief Konstruktor
		 * \param type Typ des Ausloesers
		 */
		Trigger(TriggerType type);
		
		/**
		 * \fn void addVariable(std::string name, int value)
		 * \brief Fuegt eine Integer Variable hinzu
		 * \param name Name der Variable
		 * \param value Wert der Variable
		 */
		void addVariable(std::string name, int value);
		
		/**
		 * \fn void addVariable(std::string name, bool value)
		 * \brief Fuegt eine Integer Variable hinzu
		 * \param name Name der Variable
		 * \param value Wert der Variable
		 */
		void addVariable(std::string name, bool value);
		
		/**
		 * \fn void addVariable(std::string name, float value)
		 * \brief Fuegt eine float Variable hinzu
		 * \param name Name der Variable
		 * \param value Wert der Variable
			*/
		void addVariable(std::string name, float value);
		
		/**
		 * \fn void addVariable(std::string name, std::string value)
		 * \brief Fuegt eine String Variable hinzu
		 * \param name Name der Variable
		 * \param value Wert der Variable
		 */
		void addVariable(std::string name, std::string value);
		
		/**
		 * \fn void addVariable(std::string name, Vector value)
		 * \brief Fuegt eine Vector Variable hinzu
		 * \param name Name der Variable
		 * \param value Wert der Variable
		 */
		void addVariable(std::string name, Vector value);
		
		/**
		 * \fn std::string& getLuaVariables()
		 * \brief Gibt Referenz auf den String mit den Variablen fuer Lua aus
		 */
		std::string& getLuaVariables()
		{
			return m_lua_variables;
		}
		
		/**
		 * \fn TriggerType getType()
		 * \brief Gibt den Typ des Triggers aus
		 */
		TriggerType getType()
		{
			return m_type;
		}
		
		/**
		 * \fn int getObjectId()
		 * \brief Gibt die ID des Objekts aus, auf das der Trigger zusaetzlich angewendet wird
		 */
		int getObjectId()
		{
			return m_object_id;
		}
		
		/**
		 * \fn int getDialogueId()
		 * \brief Gibt die ID des Dialogs aus, der in dem Event als Umgebungsvariable gesetzt werden soll
		 */
		int getDialogueId()
		{
			return m_dialogue_id;
		}
		
		/**
		 * \brief Sets the gettext domain that is used to translate all strings generated during this event
		 * \param domain new domain
		 */
		void setTextDomain(std::string domain)
		{
			m_text_domain = domain;
		}
		
	private:
		/**
		 * \var TriggerType m_type
		 * \brief Typ des Ausloesers
		 */
		TriggerType m_type;
		
		/**
		 * \var int m_id
		 * \brief Id des Objektes auf das der Trigger zusaetzlich angewendet wird
		 */
		int m_object_id;
		
		/**
		 * \var int m_dialogue_id
		 * \brief Id des Dialogs der in dem Event als Umgebungsvariable gesetzt wird
		 */
		int m_dialogue_id;
		
		/**
		 * \brief Gettext domain that is used to translate all strings generated during this event
		 */
		std::string m_text_domain;
		
		/**
		 * \var std::string m_lua_variables
		 * \brief String der in Lua die Variablen setzt, die dem Trigger mitgegeben wurden
		 */
		std::string m_lua_variables;
};


/**
 * \class Event
 * \brief Klasse fuer ein Ereignis
 */
class Event
{
	public:
		/**
		 * \fn Event()
		 * \brief Konstruktor
		 */
		Event()
		{
			m_once = false;
			m_condition = LUA_NOREF;
			m_effect = LUA_NOREF;
			m_copy = false;
			m_gettext_domain = "sumwars-events";
		}
		
		/**
		 * \fn Event( Event& other)
		 * \brief Kopierkonstruktor
		 * \param other anderes Event
		 */
		Event( Event& other)
		{
			m_copy = true;
			m_once = other.m_once;
			m_condition = other.m_condition;
			m_effect = other.m_effect;
			m_gettext_domain = other.m_gettext_domain;
		}
		
		/**
		 * \fn ~Event()
		 * \brief Destruktor
		 */
		~Event();
		
		/**
		 * \fn void setCondition(const char * cond)
		 * \brief Setzt die Bedingung fuer die Ausfuehrung des Events
		 * \param cond Bedingung als Folge von Lua Anweisungen
		 */
		void setCondition(const char * cond);
	
		/**
		 * \fn bool checkCondition();
		 * \brief Testet die Bedingung fuer die Ausfuehrung des Events
		 */
		bool checkCondition();
		
		/**
		 * \fn void setEffect(const char * effect)
		 * \brief Setzt die Wirkung des Events
		 * \param effect Wirkung als Folge von Lua Anweisungen
		 */
		void setEffect(const char * effect);
		
		/**
		 * \fn void doEffect();
		 * \brief Fuehrt die Wirkung des Events aus
		 */
		void doEffect();
		
		/**
		 * \fn void setOnce(bool once = true)
		 * \brief Setzt, ob das Event nach der ersten Ausfuehrung geloescht wird
		 * \param once wenn auf true gesetzt, wird das Event nach der ersten Ausfuehrung geloescht
		 */
		void setOnce(bool once = true)
		{
			m_once = once;
		}
		
		/**
		 * \fn bool getOnce()
		 * \brief Gibt aus, das Event nach der ersten Ausfuehrung geloescht wird
		 */
		bool getOnce()
		{
			return m_once;
		}
		
		/**
		 * \brief Returns if this event is a copy from a \ref RegionData 
		 */
		bool isCopy()
		{
			return m_copy;
		}
		
		/**
		 * \brief Setzt die Gettextdomain
		 */
		void setGettextDomain(std::string gettext_domain)
		{
			m_gettext_domain = gettext_domain;
		}
		
	private:
		/**
	 	 * \var int m_condition
		 * \brief Bedingung unter der das Event ausgefuehrt wird (Folge von Lua Anweisungen)
		 */
		int m_condition;
		
		/**
		 * \var bool m_once
		 * \brief wenn auf true, gesetzt, wird das Event nach der ersten Ausfuehrung geloescht
		 */
		bool m_once;
		
		/**
		 * \var int m_effect
		 * \brief Wirkung des Events (Folge von Lua Anweisungen)
		 */
		int m_effect;
		
		/**
		 * \var bool m_copy
		 * \brief Gibt an, ob das Event eine Kopie ist. Bei einer Kopie werden die zugrunde liegenden Lua Code Chunks beim loeschen nicht freigegeben
		 */
		bool m_copy;
		
		/**
		 * \brief Gettextdomain that is set when the event is executed
		 */
		std::string m_gettext_domain;
};


#endif // __SUMWARS_CORE_EVENT_H__

