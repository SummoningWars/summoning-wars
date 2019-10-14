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

#include "treasure.h"
#include "item.h"
#include "itemfactory.h"

Treasure::Treasure(int id)
	: FixedObject( id)
{
	init();
}

Treasure::Treasure(int id, TreasureBasicData& data)
	: FixedObject( id)
{
	init();
	
	for (int i=0; i<4; i++)
	{
		m_drop_slots[i] = data.m_drop_slots[i];
	}
	
	setSubtype(data.m_subtype);
	setLayer(data.m_fixed_data.m_layer);
	*(getShape()) = data.m_fixed_data.m_shape;
}

bool Treasure::init ()
{
	FixedObject::init();

	setLayer(LAYER_BASE);

	setState(STATE_ACTIVE,false);
	setSubtype("treasure");
	setType("TREASURE");
	
	m_interaction_flags = COLLISION_DETECTION | EXACT_MOUSE_PICKING | USABLE;
	m_open = false;
	return true;
}

bool  Treasure::update (float time)
{
	if (m_open)
	{
		m_open_timer += time;
	}
	
	if (m_open_timer > 3000 && getState() != STATE_INACTIVE)
	{
		setState(STATE_INACTIVE);
	}
	return true;
}

bool Treasure::reactOnUse(int id)
{
	
	if (!m_open)
	{
		addToNetEventMask(NetEvent::DATA_FLAGS);
		
		SW_DEBUG("treasure opened");
		m_open = true;
		m_open_timer = 0;
		
		Item* si;
		for (int i=0;i<4;i++)
		{
			si = ItemFactory::createItem(m_drop_slots[i]);
			if (si!=0)
			{
				getRegion()->dropItem(si,getShape()->m_center);
			}
		}
	}
	return true;
}

void Treasure::toString(CharConv* cv)
{
	FixedObject::toString(cv);
	cv->toBuffer(m_open);
}

void Treasure::fromString(CharConv* cv)
{
	FixedObject::fromString(cv);
	cv->fromBuffer(m_open);
}

void Treasure::getFlags(std::set<std::string>& flags)
{
	if (m_open)
	{
		flags.insert("open");
	}
}

void Treasure::writeNetEvent(NetEvent* event, CharConv* cv)
{
	FixedObject::writeNetEvent(event,cv);
	
	if (event->m_data & NetEvent::DATA_FLAGS)
	{
		cv->toBuffer(m_open);
	}
}

void Treasure::processNetEvent(NetEvent* event, CharConv* cv)
{
	FixedObject::processNetEvent(event,cv);
	
	if (event->m_data & NetEvent::DATA_FLAGS)
	{
		cv->fromBuffer(m_open);
	}
}



