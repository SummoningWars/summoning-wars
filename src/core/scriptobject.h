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

#ifndef SCRIPTOBJECT_H
#define SCRIPTOBJECT_H

#include "worldobject.h"
#include "event.h"

/**
 * \class ScriptObject
 * \brief Klasse fuer ausschlieslich durch Scripte gesteuerte Objekte
 */
class ScriptObject: public WorldObject
{
	public:
		/**
		 * \fn ScriptObject(int id)
		 * \brief Konstruktor
		 * \param id ID des Objekts
		 */
		ScriptObject(int id);
	
		/**
		 * \fn virtual ~ScriptObject()
		 * \brief Destruktor
		 */
		virtual ~ScriptObject();
		
				
		/**
		 * \fn bool takeDamage(Damage* damage)
		 * \brief Das Objekt nimmt Schaden in der in damage angegebenen Art und Hoehe.
		 * \param damage Schadensart und -hoehe
		 */
		virtual bool takeDamage(Damage* damage);
		
		/**
		 * \fn virtual int getValue(std::string valname)
		 * \brief Schiebt den gewuenschten Attributwert eines Objektes auf den Lua Stack
		 * \param valname Name des Wertes
		 */
		virtual int getValue(std::string valname);
	
		/**
		 * \fn virtual bool setValue(std::string valname)
		 * \brief Setzt den gewuenschten Attributwert eines Objektes
		 * \param valname Name des Wertes
		 */
		virtual bool setValue(std::string valname);
	
		/**
		 * \fn virtual bool isCreature()
		 * \brief Gibt aus, ob es sich um eine Kreatur handelt
		 */
		virtual bool isCreature()
		{
			return false;
		}
		
		/**
		 * \fn virtual bool update ( float time)
		 * \brief Aktualisiert das Object, nachdem eine bestimmte Zeit vergangen ist. Alle Aktionen des Objekts werden auf diesem Weg ausgeloest.
		 * \param time Menge der vergangenen Zeit in Millisekunden
		 * \return bool, der angibt, ob die Aktualisierung fehlerfrei verlaufen ist
		 */
		virtual  bool  update ( float time);
		
		/**
		 * \fn std::string getRenderInfo()
		 * \brief Gibt die Informationen zum Rendern der Objektes aus
		 */
		std::string getRenderInfo()
		{
			return m_render_info;
		}
		
		/**
		 * \fn void setRenderInfo(std::string render_info)
		 * \brief Setzt die INformationen zum Rendern des Objektes
		 */
		void setRenderInfo(std::string render_info)
		{
			m_render_info = render_info;
		}
		
	
		
		/**
		 * \fn virtual void writeNetEvent(NetEvent* event, CharConv* cv)
		 * \brief Schreibt die Daten zu einem NetEvent in den Bitstream
		 * \param event NetEvent das beschrieben wird
		 * \param cv Bitstream
		 */
		virtual void writeNetEvent(NetEvent* event, CharConv* cv);
	
	
		/**
		 * \fn virtual void processNetEvent(NetEvent* event, CharConv* cv)
		 * \brief Fuehrt die Wirkung eines NetEvents auf das Objekt aus. Weitere Daten werden aus dem Bitstream gelesen
		 */
		virtual void processNetEvent(NetEvent* event, CharConv* cv);
		
		
		/**
		 * \fn void addEvent(TriggerType type, Event* event)
		 * \brief Fuegt ein neues Event hinzu
		 * \param trigger Typ des Triggers durch den das Event ausgeloest wird
		 * \param event Event
		 */
		void addEvent(TriggerType trigger, Event* event);
		
		/**
		 * \fn void activateTrigger(Trigger* trigger)
		 * \brief loest alle durch den Trigger gesteuerten Events aus
		 * \param trigger Ausloeser
		 */
		void activateTrigger(Trigger* trigger);
		
	private:
		/**
		 * \var std::string m_render_info
		 * \brief Information zum Rendern des Objekts
		 */
		std::string m_render_info;
		
		
		
		/**
		 * \var std::multimap<TriggerType, Event*> m_events
		 * \brief Liste der registrierten Events, aufgelistet nach dem Trigger durch den sie ausgeloest werden
		 */
		std::multimap<TriggerType, Event*> m_events;
		
};
#endif

