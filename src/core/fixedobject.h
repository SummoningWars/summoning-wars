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


#ifndef __SUMWARS_CORE_FIXEDOBJECT_H__
#define __SUMWARS_CORE_FIXEDOBJECT_H__
#include <string>

#include "worldobject.h"



/**
 * \class FixedObject
 * \brief This object is a fixed object. It can't move it's position in the world.
 */
class FixedObject : public WorldObject {
/**
 * Public stuff
 */
public:

	/**
	 * \fn FixedObject( int id)
     * \param id Object id
     * \brief Constructor
	 */
	FixedObject( int id);

	/**
	 * \fn FixedObject( int id, Subtype object_subtype)
     * \brief Constructor
	 */
	FixedObject( int id, Subtype object_subtype);

	/**
	* \fn virtual bool init()
    * \brief Initializing of unmovable objects
    * \return bool Return true if successful
	*/
	virtual bool init ();

	/**
	 * \fn virtual bool  update (float time)
     * \brief updates the object at an interval
     * \param time time since last update
	 */
	virtual bool  update (float time);


};
#endif // __SUMWARS_CORE_FIXEDOBJECT_H__

