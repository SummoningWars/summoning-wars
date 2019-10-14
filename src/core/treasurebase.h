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

#ifndef __SUMWARS_CORE_TREASUREBASE_H__
#define __SUMWARS_CORE_TREASUREBASE_H__

#include "dropslot.h"
#include "worldobject.h"
#include "fixedbase.h"

/**
 * \struct TreasureBasicData
 * \brief Informationen zum Erstellen eines Treasure Objekts
 */
struct TreasureBasicData
{
	/**
	 * \var GameObject::Type m_subtype
	 * \brief Informationen zum Subtyp
	 */
	GameObject::Type m_subtype;
	
	/**
	 * \var DropSlot m_drop_slots[4]
	 * \brief Beschreibung der Items die der Schatz droppen kann
	 */
	DropSlot m_drop_slots[4];
	
	/**
	 * \var std::string m_name
	 * \brief Name des Monsters
	 */
	std::string m_name;
	
	/**
	 * \var FixedObjectData m_fixed_data
	 * \brief Basisdaten analog zu FixedObject
	 */
	FixedObjectData m_fixed_data;
};

#endif // __SUMWARS_CORE_TREASUREBASE_H__

