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

#ifndef __SUMWARS_CORE_SPAWNPOINT_H__
#define __SUMWARS_CORE_SPAWNPOINT_H__
#include <string>

#include "worldobject.h"
#include "random.h"
#include "objectfactory.h"
#include "region.h"
#include <list>

/**
 * \class Spawnpoint
 * \brief Dieses Objekt stellt einen Ort dar, an dem Monster die Welt betreten
 */
class Spawnpoint : public WorldObject {
/**
	 * Public stuff
 */
	public:
	
	
	/**
	 * \fn Spawnpoint( int id)
	 * \param id Objekt-ID
	 * \brief Konstruktor
	 * \param name Name der Monstergruppe die dieser Spawnpoint erzeugt
	 * \param id ID des Spawnpoints
	 */
	Spawnpoint(MonsterGroupName name, int id=0);
	
	/**
	 * \fn virtual bool init()
	 * \brief Initialisierung von internen Daten
	 * \return bool Gibt an, ob die Initialisierung erfolgreich war
	 */
	virtual bool init ();
	
	/**
	 * \fn virtual bool  update (float time)
	 * \brief aktualisiert das Objekt nachdem time Millisekunden vergangen sind
	 * \param time vergangene Zeit in Millisekunden
	 */
	virtual bool  update (float time);
	
	
	/**
	 * \fn void setRespawnTime(float t)
	 * \brief Setzt die Zeit bis zum Neuerscheinen der Monster
	 */
	void setRespawnTime(float t);


	private:
		
		/**
		 * \var MonsterGroupName m_spawned_monsters
		 * \brief die Monster die von diesem Spawnpoint erzeugt werden
		 */
		MonsterGroupName m_spawned_monsters;
		
		/**
		 * \var float m_time
		 * \brief interner Timer, nach Ablaufen werden die Monster neu erzeugt
		 */
		float m_time;
		
		/**
		 * \var float m_respawn_time
		 * \brief Zeitdauer bis zum neu Spawnen der Monster
		 */
		float m_respawn_time;
	
};
#endif // __SUMWARS_CORE_SPAWNPOINT_H__

