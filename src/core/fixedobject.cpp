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

#include "fixedobject.h"

//Constructors/Destructors
FixedObject::FixedObject( int id) : WorldObject(id)
{
	bool tmp=FixedObject::init();
	if (!tmp)
	{
		DEBUG("Initialisierung des Fixed Objects fehlgeschlagen!");
	}
}



FixedObject::FixedObject( int id,  Subtype object_subtype)  : WorldObject( id)
{
	setSubtype(object_subtype);
	bool tmp=FixedObject::init();
	if (!tmp)
	{
		DEBUG("Initialisierung des Fixed Objects fehlgeschlagen!");
	}
}

//Methods
bool FixedObject::init()
{
	//eigene Initialisierung
	setType("FIXED_OBJECT");
	setState(STATE_STATIC,false);

	clearNetEventMask();
	return true;
}

bool FixedObject::update(float time)
{
	return true;
}

