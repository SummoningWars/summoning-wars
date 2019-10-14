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

#include "gridunit.h"

Gridunit::Gridunit()
{
}

bool Gridunit::insertObject(WorldObject* object, WorldObject::Group g)
{
	// Ebene in der das Objekt eingeordnet wird
	if (g == WorldObject::AUTO)
	{
		g = object->getGroup();
	}

	// Zeiger auf Array in das eingefuegt wird
	std::vector<WorldObject*>& arr = getObjects(g);

	// Element einfuegen, Zahler erhoehen
	arr.push_back(object);
	object->getGridLocation()->m_index = arr.size()-1;
		
	DEBUGX("inserted object %p, as number %i in group %i",object,arr.size()-1,g);
	return true;
}


bool  Gridunit::moveObject(WorldObject* object,WorldObject::Group group )
{
	bool ret = deleteObject(object);
	if (ret==false)
		return false;

	insertObject(object,group);
	
    return true;
}

bool Gridunit::deleteObject(WorldObject* object, short index)
{
	// Ebene aus der geloescht werden soll
	WorldObject::Group g = object->getGroup();

	// Zeiger auf Array aus dem geloescht wird
	std::vector<WorldObject*>& arr = getObjects(g);

	int size = arr.size();
	//DEBUG ("deleting obj %i (%s) from group %i", object->getId (), object->getName ().getRawText ().c_str (), g);
	if (index != -1 && arr[index]==object)
	{
		// Overwrite the object that will be deleted.
		arr[index] = arr[size-1];
		arr[index]->getGridLocation()->m_index = index;

		if (size == 1)
		{
			arr.clear ();
		}
		else
		{
			arr.pop_back();
		}
		return true;
	}
	else
	{
		// Suchen an welcher Stelle sich das Objekt befindet
		for (int i=0;i<size;i++)
		{
			DEBUGX(" testing object %i",i);
			if (arr[i]==object)
			{
				// Objekt gefunden
				// Letztes Objekt an die Stelle des geloeschten kopieren
				arr[i] = arr[size-1];
				arr[i]->getGridLocation()->m_index = i;
				//DEBUG ("resizing (v2) arr to new size: %d", size - 1);
				if (size == 1)
				{
					arr.clear ();
				}
				else
				{
					arr.pop_back();
				}
				return true;
			}
		}
	}

	// Objekt nicht gefunden, Fehler anzeigen
	ERRORMSG("Object %s (%f %f) not found in group %i",object->getNameId().c_str(),object->getShape()->m_center.m_x,object->getShape()->m_center.m_y, g);
	return false;

}


int  Gridunit::getObjectsNr(WorldObject::Group group)
{
	if (group & WorldObject::CREATURE)
	{
		return m_creatures.size();
	}
	else if (group == WorldObject::FIXED)
	{
		return m_fixed.size();
	}
	else if (group == WorldObject::DEAD)
	{
		return m_dead.size();
	}
	else 
	{
		ERRORMSG("unknown group %i",group);
	}
	return 0;
}


