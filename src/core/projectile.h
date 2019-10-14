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

#ifndef __SUMWARS_CORE_PROJECTILE_H__
#define __SUMWARS_CORE_PROJECTILE_H__

#include "debug.h"
#include "charconv.h"
#include "worldobject.h"
#include "damage.h"
#include <algorithm>
#include <math.h>
#include "networkstruct.h"
#include <list>
#include "gameobject.h"
#include "projectilebase.h"
#include "fraction.h"
class World;



/**
 * \class Projectile
 * \brief Klasse fuer bewegliche Gegenstaende und Erscheinungen
 */
class Projectile : public GameObject
{
	public:

	/**
	 * \enum Flags
	 * \brief Zaehlt Sondereigenschaften von Geschossen auf
	 */
	enum Flags
	{
		EXPLODES = 1,
		MULTI_EXPLODES = 2,
		PIERCING = 4,
		BOUNCING = 8,
		PROB_BOUNCING = 16
		
	};
	
	/**
	 * \fn Projectile( ProjectileType type, Damage* dmg,  int id=0)
	 * \param subtype Typ des Projektils
	 * \param id ID des Projektils
	 * \brief Konstruktor
	 */
	Projectile(Subtype subtype,int id=0);
	
	/**
	 * \fn Projectile(ProjectileBasicData &data,int id=0)
	 * \param data Daten fuer die Initialisierung
	 * \param id ID des Projektils
	 * \brief Konstruktor
	 */
	Projectile(ProjectileBasicData &data,int id=0);
	
	/**
	 * \fn virtual ~Projectile()
	 * \brief Destruktor
	 */
	virtual ~Projectile()
	{
		if (m_damage != 0)
			delete m_damage;
	}
	
	/**
	 * \fn virtual update ( float time)
	 * \brief Aktualisiert das Projektil, nachdem eine bestimmte Zeit vergangen ist. Alle Aktionen des Objekts werden auf diesem Weg ausgeloest. 
	 * \param time Menge der vergangenen Zeit in Millisekunden
	 * \return bool, der angibt, ob die Aktualisierung fehlerfrei verlaufen ist
 	*/
	virtual  bool  update ( float time);

	/**
	 * \fn void destroy()
	 * \brief Zerstoert das Geschoss
	 */
	void destroy();
		
	/**
	 * \fn void setCounter(short cnt)
	 * \brief Setzt den internen Zaehler
	 * \param cnt Zaehler
	 */
	void setCounter(short cnt)
	{
		m_counter = cnt;
	}
	
	/**
	 * \fn void setMaxRadius(float mr)
	 * \brief setzt den maximalen Radius
	 * \param mr maximaler Radius
	 */
	void setMaxRadius(float mr)
	{
		m_max_radius = mr;
	}
	
	/**
	 * \fn void setRadius(float r)
	 * \brief Setzt den Radius
	 * \param r Radius
	 */
	void setRadius(float r)
	{
		if (m_implementation == "grow_effect_at_each" || m_implementation == "grow")
		{
			m_max_radius =r;
		}
		else
		{
			getShape()->m_radius = r;
		}
	}
	
	/**
	 * \fn void setFlags(char f)
	 * \brief Setzt die Flags
	 * \param f Flags
	 */
	void setFlags(char f)
	{
		m_flags = f;
	}
	
	/**
	 * \fn void addFlags(char f)
	 * \brief Fuegt Flags hinzu
	 * \param f Flags
	 */
	void addFlags(char f)
	{
		m_flags |= f;
	}
	
	/**
	 * \fn Damage* getDamage()
	 * \brief Gibt Struktur mit Schadensinformationen aus
	 * \return Damage Objekt
	 * 
	 */
	Damage* getDamage()
	{
		return m_damage;
	}
	
	/**
	 * \fn void setDamage(Damage* dmg)
	 * \brief Setzt die Schadensinformationen
	 * \param dmg Damage Objekt
	 */
	void setDamage(Damage* dmg);
	
	/**
	 * \fn void setGoalObject(int id)
	 * \brief Setzt das ZielObjekt
	 */
	void setGoalObject(int id)
	{
		m_goal_object = id;
	}
	
	/**
	 * \fn virtual void toString(CharConv* cv)
	 * \brief Konvertiert das Objekt in einen String und schreibt ihn in der Puffer
	 * \param cv Ausgabepuffer
	 */
	virtual void toString(CharConv* cv);
			
			
	/**
	 * \fn virtual void fromString(CharConv* cv)
	 * \brief Erzeugt das Objekt aus einem String
	 * \param cv Eingabepuffer
	 */
	virtual void fromString(CharConv* cv);
	
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
	 * \fn float getTimer()
	 * \brief Gibt Wert des Timers aus
	 */
	float getTimer()
	{
		return m_timer;
	}
	
	/**
	 * \fn float getTimerLimit()
	 * \brief Gibt Wert des Timerlimits aus
	 */
	float getTimerLimit()
	{
		return m_timer_limit;
	}
	
	/** 
	 * \fn Fraction::Id getFraction()
	* \brief Gibt die Fraktion aus
	*/
	Fraction::Id getFraction()
	{
		return m_fraction;
	}
	
	/**
	 * \fn void setFraction(Fraction::Id fr)
	 * \brief setzt die Fraktion
	 * \param fr Fraktion
	 */
	void setFraction(Fraction::Id fr)
	{
		m_fraction = fr;
	}
	
	/**
	 * \fn bool virtual getDestroyed()
	 * \brief Gibt zurueck ob das Objekt zerstoert ist
	 */
	virtual bool getDestroyed()
	{
		return getState() == STATE_DESTROYED;
	}
	
		/**
	 * \fn std::string getActionString()
	 * \brief Gibt die aktuelle Aktion als String aus
		 */
	virtual std::string getActionString();
	
		
	/**
	 * \fn virtual float getActionPercent()
	 * \brief Gibt den Prozentsatz, zu dem die aktuelle Aktion fortgeschritten ist aus
	 */
	virtual float getActionPercent();
	
	/**
	 * \fn virtual void getFlags(std::set<std::string>& flags)
	 * \brief Gibt den Status der bekannten Flags aus
	 * \param flags Ausgabeparameter: Flags
	 */
	virtual void getFlags(std::set<std::string>& flags);
	
	
	protected:
		
		/**
		 * \fn void setTimerLimit(float limit)
		 * \brief Setzt das Timer Limit
		 * \param limit neues Limit
		 */
		void setTimerLimit(float limit)
		{
			m_timer_limit = limit;
			addToNetEventMask(NetEvent::DATA_TIMER);
		}
		
		/**
		* \fn void handleFlying(float dtime)
		* \brief Fuehrt update fuer Objekt im Zustand FLYING aus
		*/
		void handleFlying(float dtime);
			
		/**
		* \fn void handleFlying(float dtime)
		* \brief Fuehrt update fuer Objekt im Zustand GROWING aus
		*/
		void handleGrowing(float dtime);
		
		/**
		* \fn void handleFlying(float dtime)
		* \brief Fuehrt update fuer Objekt im Zustand STABLE aus
		*/
		void handleStable(float dtime);
		
		/**
		 * \fn void doEffect(GameObject* target=0)
		 * \brief Fuehrt den Effekt aus
		 */
		void doEffect(GameObject* target=0);
		
		/**
		* \var Damage* m_damage
		* \brief Schaden den das Projektil anrichtet
		*/
		Damage* m_damage;
		
		/**
		 * \var std::set<int> m_hit_objects_ids;
		 * \brief Menge aller bereits getroffenen Objekte
		*/
		std::set<int> m_hit_objects_ids;
		
		/**
		* \var char m_flags
		* \brief einige Einstellungen
		*/
		char m_flags;
		
		/**
		* \var float m_max_radius
		* \brief maximaler Radius den das Objekt erreicht
		*/
		float m_max_radius;
		
		/**
		* \var int m_goal_object
		* \brief Zielobjekt
		*/
		int m_goal_object;
		
		
		/**
		* \var float m_timer
		* \brief Timer, Verwendung je nach Art des Projektils und aktuellem Zustand
		*/
		float m_timer;
		
		/**
		* \var float m_timer_limit
		* \brief Wert, bei dessen Erreichen durch den Timer eine Statusveraenderung eintritt
		*/
		float m_timer_limit;
		
		/**
		 * \brief Prozentsatz zu dem der Timer abgelaufen ist, wenn der Effekt ausgeloest wird
		 */
		float m_crit_percent;
		
		/**
		* \var m_counter
		* \brief Zaehler, Verwendung je Art des Projektils und aktuellem Zustand
		*/
		int m_counter;
		
		/**
		 * \var Fraction::Id m_fraction
		 * \brief Fraktion des Objektes
		 */
		Fraction::Id m_fraction;
		
		/**
		 * \var std::string m_implementation
		 * \brief Steuert die Verhaltensweise
		 */
		std::string m_implementation;

};

/**
 * Liste von Projektilen sortiert nach ihrer ID
 */
typedef std::map<int,Projectile*> ProjectileMap;



#endif // __SUMWARS_CORE_PROJECTILE_H__
