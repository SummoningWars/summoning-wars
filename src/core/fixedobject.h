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


#ifndef FIXEDOBJECT_H
#define FIXEDOBJECT_H
#include <string>

#include "worldobject.h"



/**
 * \class FixedObject
 * \brief Dieses Objekt stellt ein fixes Objekt dar. Es kann seine Position in der Welt nicht ver&auml;ndern.
 */
class FixedObject : public WorldObject {
/**
 * Public stuff
 */
public:

	/**
	 * \fn FixedObject( int id)
	 * \param id ID des Objekts
	 * \brief Konstruktor
	 */
	FixedObject( int id);

	/**
	 * \fn FixedObject( int id, Subtype object_subtype)
	 * \brief Konstruktor
	 */
	FixedObject( int id, Subtype object_subtype);

	/**
	* \fn virtual bool init()
	* \brief Initialisierung von unbewegten Gegenständen
	* \return bool Gibt an, ob die Initialisierung erfolgreich war
	*/
	virtual bool init ();

	/**
	 * \fn virtual bool  update (float time)
	 * \brief aktualisiert das Objekt nach dem eine bestimmte Zeit vergangen ist
	 * \param time vergangene Zeit in ms
	 */
	virtual bool  update (float time);


};
#endif //FIXEDOBJECT_H

