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

#include "scriptobject.h"
#include "eventsystem.h"

ScriptObject::ScriptObject(int id)
	: WorldObject(id)
{
	std::stringstream stream;
	stream << "scriptobjectvar["<<getId()<<"]={}";
	EventSystem::doString(stream.str().c_str());
	
	setState(STATE_ACTIVE);
	setType("SCRIPTOBJECT");
	
	m_interaction_flags =  EXACT_MOUSE_PICKING | USABLE;
}

ScriptObject::~ScriptObject()
{
	std::stringstream stream;
	stream << "scriptobjectvar["<<getId()<<"]=nil";
	EventSystem::doString(stream.str().c_str());
	
	std::multimap<TriggerType, Event*>::iterator it;
	for (it = m_events.begin(); it != m_events.end(); ++it)
	{
		delete it->second;
	}
}

bool ScriptObject::takeDamage(Damage* damage)
{
	WorldObject::takeDamage(damage);
	return true;
}

int ScriptObject::getValue(std::string valname)
{
	
	int ret = WorldObject::getValue(valname);
	if (ret ==0)
	{
		std::stringstream stream;
		stream << "return scriptobjectvar["<<getId()<<"]['"<<valname<<"']";
		EventSystem::doString(stream.str().c_str());
		return 1;
	}
	
	return ret;
}

bool ScriptObject::setValue(std::string valname)
{
	bool ret = false;
	if (valname == "speed")
	{
		Vector speed = EventSystem::getVector(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		SW_DEBUG("speed %f %f",speed.m_x,speed.m_y);
		setSpeed(speed);
		return 1;
	}
	else
	{
	 	ret = WorldObject::setValue(valname);
	}
	
	if (ret ==false)
	{
		std::string value  = lua_tostring(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		std::stringstream stream;
		stream << "scriptobjectvar["<<getId()<<"]['"<<valname<<"'] = "<<value;
		EventSystem::doString(stream.str().c_str());
		return true;
	}
	
	return ret;
}

bool  ScriptObject::update ( float time)
{
	WorldObject::update(time);
	
	// TODO: Kollisionen etc ??
	getShape()->m_center += getSpeed()*time;
	
	return true;
}


void ScriptObject::writeNetEvent(NetEvent* event, CharConv* cv)
{
	WorldObject::writeNetEvent(event,cv);
	
	
}


void ScriptObject::processNetEvent(NetEvent* event, CharConv* cv)
{
	WorldObject::processNetEvent(event,cv);
}


void ScriptObject::addEvent(TriggerType trigger, Event* event)
{
	m_events.insert(std::make_pair(trigger,event));
}

void ScriptObject::activateTrigger(Trigger* trigger)
{
	std::multimap<TriggerType, Event*>::iterator it, itend, jt;
	TriggerType type;
	type = trigger->getType();
		
	DEBUGX("trigger: %s",type.c_str());
		
	// Schleife ueber die ausgeloesten Events
	it = m_events.lower_bound(type);
	itend = m_events.upper_bound(type);
	while (it != itend)
	{
		jt = it;
		++it;
			
		// vom Trigger definierte Variablen einfuegen
		EventSystem::doString((char*) trigger->getLuaVariables().c_str());
		DEBUGX("lua code \n %s",trigger->getLuaVariables().c_str());
			
			// Event ausfuehren
		bool ret = EventSystem::executeEvent(jt->second);
			
		if (ret)
		{
			DEBUGX("event on trigger: %s",type.c_str());
		}
			
		// einmalige Ereignisse loeschen, wenn erfolgreich ausgefuehrt
		if (jt->second->getOnce() &&  ret)
		{
			delete jt->second;
			m_events.erase(jt);
		}
	}
}

