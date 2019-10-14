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

#ifndef __SUMWARS_CORE_GRIDUNIT_H__
#define __SUMWARS_CORE_GRIDUNIT_H__

#include "worldobject.h"
#include <list>
#include <map>
#include "dropitem.h"
#include "debug.h"
#include <vector>


/**
 * \struct Gridunit
 * \brief Struktur fuer ein 4x4 Feld der Welt#
 */
struct Gridunit
{
	
	
	private:

		/**
		* \brief Liste der toten Spieler auf dem Feld
		*/
		std::vector<WorldObject*> m_dead;
	
	
		/**
		* \brief Liste der festen Objekte auf dem Feld
		*/
	
		std::vector<WorldObject*> m_fixed;
	
		/**
		* \brief Liste der Kreaturen auf dem Feld
		*/
		std::vector<WorldObject*> m_creatures;
	

	public:

		/**
	 	 * \fn Gridunit()
		 * \brief Konstruktor
		 */
		Gridunit();

		/**
		 * \fn ~Gridunit()
		 * \brief Destruktor
		 */
		~Gridunit()
		{
		}



		/**
		 * \brief Gibt die Anzahl der Objekte in einer der Gruppen aus.
		 * \param group Gruppe deren Objekte ausgegeben werden
		 */
		int getObjectsNr(WorldObject::Group group);
		

		/**
		 * \fn WorldObject** getObjects(WorldObject::Group group)
		 * \brief Gibt die Objekte einer Gruppe als Array aus
		 * \param group Gruppe deren Objekte ausgegeben werden
		 * \return Array mit Zeigern auf die Objekte
		 */
		std::vector<WorldObject*>& getObjects(WorldObject::Group group)
		{
			if (group & WorldObject::CREATURE) return m_creatures;
			else if (group == WorldObject::FIXED) return m_fixed;
			else if (group == WorldObject::DEAD) return m_dead;
			else return m_dead;
		}

		/**
		 * \fn bool insertObject(WorldObject* object)
		 * \brief Fuegt das angegebenen Objekt in das Feld ein
		 * \param object einzufuegendes Objekt
		 * return gibt an, ob die Operation erfolgreich war
		 */
		bool insertObject(WorldObject* object,WorldObject::Group group = WorldObject::AUTO);

		/**
		 * \fn deleteObject(WorldObject* object, short index=-1)
		 * \brief Loescht das angegebene Objekt aus dem Feld
		 * \param object zu loeschendes Objekt
		 * \param index Index des Objektes im Array. Angabe beschleunigt die Funktion, ist aber optional
		 * \return gibt an, ob die Operation erfolgreich war
		 */
		bool deleteObject(WorldObject* object, short index=-1);

		/**
		 * \fn bool moveObject(WorldObject* object,WorldObject::Group group )
		 * \brief verschiebt das Objekt in die angegebene Gruppe
		 * \param object das zu verschiebende Objekt
		 * \param group Gruppe in die das Objekt verschoben wird
		 */
		bool moveObject(WorldObject* object,WorldObject::Group group );
		

};


#endif // __SUMWARS_CORE_GRIDUNIT_H__
