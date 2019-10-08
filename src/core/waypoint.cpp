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

#include "waypoint.h"
#include "world.h"
#include "player.h"

Waypoint::Waypoint( int id) : FixedObject( id)
{
	init();
}

bool Waypoint::init ()
{
	FixedObject::init();

	getShape()->m_type = Shape::RECT;
	getShape()->m_extent = Vector(2.4,2.2);
	setLayer(LAYER_BASE | LAYER_AIR);

	setState(STATE_ACTIVE,false);
	setSubtype("waypoint");
	
	m_interaction_flags = EXACT_MOUSE_PICKING | USABLE;
	
	m_active = false;
	return true;
}

bool  Waypoint::update (float time)
{
	if (World::getWorld()->isServer())
	{
		// alle Spieler im Umkreis von 12m suchen
		WorldObjectMap *players = getRegion()->getPlayers();
		WorldObjectMap::iterator it;

		Shape s;
		s.m_type = Shape::CIRCLE;
		s.m_center = getShape()->m_center;
		s.m_radius = 10;

		Player* pl;

		for (it = players->begin(); it != players->end(); ++it)
		{
			pl = dynamic_cast<Player*>(it->second);
			if (pl !=0 && pl->getShape()->intersects(s))
			{
				if (m_active == false)
				{
					addToNetEventMask(NetEvent::DATA_TIMER);
				}
				
				m_active = true;
				
				// Spieler ist in der Naehe des Wegpunktes, aktivieren
				pl->addWaypoint(getRegion()->getId(),true);
			}
		}
	}
	return true;
}


bool Waypoint::reactOnUse(int id)
{
	if (World::getWorld()->isServer())
	{
		Player* pl;
		pl = dynamic_cast<Player*>(World::getWorld()->getPlayer(id));

		pl->setUsingWaypoint(true);
	}
	return true;
}

void Waypoint::getFlags(std::set<std::string>& flags)
{
	WorldObject::getFlags(flags);
	
	if (m_active)
		flags.insert("active");
}

void Waypoint::writeNetEvent(NetEvent* event, CharConv* cv)
{
	FixedObject::writeNetEvent(event,cv);
	
	if (event->m_data & NetEvent::DATA_TIMER)
	{
		cv->toBuffer(m_active);
	}
}

void Waypoint::processNetEvent(NetEvent* event, CharConv* cv)
{
	FixedObject::processNetEvent(event,cv);
	
	if (event->m_data & NetEvent::DATA_TIMER)
	{
		cv->fromBuffer(m_active);
	}
}


void Waypoint::toString(CharConv* cv)
{
	FixedObject::toString(cv);
	
	cv->toBuffer(m_active);
}

void Waypoint::fromString(CharConv* cv)
{
	FixedObject::fromString(cv);
	
	cv->fromBuffer(m_active);
}