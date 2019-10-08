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

#include "worldobject.h"

#include "world.h"
#include "eventsystem.h"
#include "objectfactory.h"
#include "networkstruct.h"


WorldObject::WorldObject( int id)
	: GameObject(id)
{
	setBaseType(WORLDOBJECT);
	init();
	
	m_fraction = Fraction::NOFRACTION;
	m_race = "";
	m_interaction_flags =0;
	
	m_animation ="";
	m_animation_time =1;
	m_animation_elapsed_time = 0;
	m_animation_repeat =false;
	
	m_name = "";
}

bool WorldObject::isCreature()
{
	return (getBaseType() == WORLDOBJECT && getType() != "FIXED_OBJECT" && getType() != "TREASURE");
}

bool WorldObject::isLarge()
{
	if (getState() == WorldObject::STATE_STATIC)
	{
		// statische Objekte duerfen in jeder Richtung maximal 3 Gridunits ueberdecken
		Vector pos = getShape()->m_center;
		Vector ext = getShape()->getAxisExtent();
		Vector corner1 = pos - ext;
		Vector corner2 = pos + ext;
		
		int x1 = (int) floor(0.25*corner1.m_x+0.01);
		int y1 = (int) floor(0.25*corner1.m_y+0.01);
		
		int x2 = (int) floor(0.25*corner2.m_x-0.01);
		int y2 = (int) floor(0.25*corner2.m_y-0.01);
		
		if (x2-x1>2 || y2-y1>2)
			return true;
		
		return false;
	}
	else
	{
		// dynamische Objekte duerfen maximal einen Radius von 4 haben
		return (getShape()->getOuterRadius()>4);
	}
	return false;
}

bool WorldObject::moveTo(Vector newpos, bool emit_event)
{
	bool ret = true;
	if (World::getWorld()==0 || getRegion()==0)
	{
		setPosition(newpos);

		ret =  true;
	}
	else
	{
		ret =  getRegion()->moveObject(this, newpos);
		
	}
	
	if (emit_event)
	{
		addToNetEventMask(NetEvent::DATA_SHAPE);
	}
	return ret;
}


bool  WorldObject::destroy()
{
	return true;
}

bool  WorldObject::update ( float time)
{
	GameObject::update(time);
	
	if (m_animation != "")
	{
		m_animation_elapsed_time +=time;
		
		if (m_animation_elapsed_time > m_animation_time)
		{
			if (m_animation_repeat)
			{
				m_animation_elapsed_time -= m_animation_time;
			}
			else
			{
				m_animation ="";
				m_animation_time =1;
				m_animation_elapsed_time = 0;
			}
		}
	}
	
	return true;
}

void WorldObject::toString(CharConv* cv)
{
	DEBUGX("worldobject::tostring");
	GameObject::toString(cv);
	
	m_name.toString(cv);;
	cv->toBuffer(m_fraction);
	cv->toBuffer(m_race);
	cv->toBuffer(m_interaction_flags);
	
	cv->toBuffer(static_cast<short>(m_flags.size()));
	std::set<std::string>::iterator it;
	for (it = m_flags.begin(); it != m_flags.end(); ++it)
	{
		cv->toBuffer(*it);
	}
}

void WorldObject::fromString(CharConv* cv)
{
	GameObject::fromString(cv);
	
	m_name.fromString(cv);
	cv->fromBuffer(m_fraction);	
	cv->fromBuffer(m_race);
	cv->fromBuffer(m_interaction_flags);
	
	m_flags.clear();
	std::string flag;
	short nr;
	cv->fromBuffer(nr);
	for (int i=0; i<nr; i++)
	{
		cv->fromBuffer(flag);
		m_flags.insert(flag);
	}
}



int WorldObject::getValue(std::string valname)
{
	if (valname == "name")
	{
		std::string name = m_name.getTranslation();
		lua_pushstring(EventSystem::getLuaState() , name.c_str() );
		return 1;
	}
	else if (valname == "fraction")
	{
		if (getFraction() < Fraction::NEUTRAL_TO_ALL)
		{
			std::stringstream stream;
			stream << "player_" << getFraction();
			lua_pushstring(EventSystem::getLuaState(),stream.str().c_str());
		}
		else
		{
			Fraction* frac = World::getWorld()->getFraction( getFraction() );
			static std::string fractype = "";
			if (frac != 0)
			{
				fractype = frac->getType();
			}
			lua_pushstring(EventSystem::getLuaState(),fractype.c_str());
		}
		return 1;
	}
	else if (valname == "usable")
	{
		lua_pushboolean(EventSystem::getLuaState(),bool(m_interaction_flags & USABLE));
		return 1;
	}
	else if (valname == "exact_picking")
	{
		lua_pushboolean(EventSystem::getLuaState(),bool(m_interaction_flags & EXACT_MOUSE_PICKING));
		return 1;
	}
	else
	{
		return GameObject::getValue(valname);
	}

	return 0;
}


bool WorldObject::setValue(std::string valname)
{
	
	if (valname == "name")
	{
		TranslatableString name;
		EventSystem::getTranslatableString(EventSystem::getLuaState(),name ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		setName(name);
		return true;
	}
	else if (valname == "fraction")
	{
		if (getType() == "PLAYER")
		{
			DEBUG("You cant change the fraction of a player");
			return false;
		}
		std::string fraction  = lua_tostring(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		Fraction::Id id = World::getWorld()->getFractionId(fraction);
		setFraction(id);
		DEBUG("fraction is now %i",getFraction());
		return true;
	}
	else if (valname == "position")
	{
		Vector pos = EventSystem::getVector(EventSystem::getLuaState(),-1);
		
		getRegion()->getFreePlace (getShape(), getLayer(), pos);
		moveTo(pos);
		return true;
	}
	else if (valname == "usable")
	{
		bool opt = lua_toboolean(EventSystem::getLuaState() ,-1);
		if (opt)
		{
			m_interaction_flags |= USABLE;
		}
		else
		{
			m_interaction_flags &= (~USABLE);
		}
		return true;
	}
	else if (valname == "exact_picking")
	{
		bool opt = lua_toboolean(EventSystem::getLuaState() ,-1);
		if (opt)
		{
			m_interaction_flags |= EXACT_MOUSE_PICKING;
		}
		else
		{
			m_interaction_flags &= (~EXACT_MOUSE_PICKING);
		}
		return true;
	}
	else 
	{
		if (valname == "layer")
		{
			std::string layer = lua_tostring(EventSystem::getLuaState(), -1);
			short newlayer = getLayer();
			if (layer == "base")
			{
				newlayer = WorldObject::LAYER_BASE;
			}
			else  if (layer =="air")
			{
				newlayer = WorldObject::LAYER_AIR;
			}
			else  if (layer =="dead")
			{
				newlayer = WorldObject::LAYER_DEAD;
			}
			else  if (layer == "normal")
			{
				newlayer = WorldObject::LAYER_BASE | WorldObject::LAYER_AIR;
			}
			else  if (layer == "nocollision")
			{
				newlayer = WorldObject::LAYER_NOCOLLISION;
			}
			if (getRegion() != 0)
			{
				getRegion()->changeObjectLayer(this,(WorldObject::Layer) newlayer);
			}
		}
		return GameObject::setValue(valname);
	}
	
	return false;
}

void WorldObject::setFraction(Fraction::Id fr)
{
	m_fraction = fr;
}

void WorldObject::setRace(Race race)
{
	m_race = race;
}

void WorldObject::setName(TranslatableString name)
{
	m_name = name;
	addToNetEventMask(NetEvent::DATA_NAME);
}

bool WorldObject::takeDamage(Damage* damage)
{
	Trigger* tr = new Trigger("object_hit");
	tr->addVariable("defender",getId());
	tr->addVariable("attacker",damage->m_attacker_id);
	tr->addVariable("_id",getId());
	getRegion()->insertTrigger(tr);
	
	return true;
}

bool  WorldObject::reactOnUse (int id)
{
	Trigger* tr = new Trigger("object_use");
	tr->addVariable("used_object",getId());
	tr->addVariable("_id",getId());
	tr->addVariable("user",id);
	getRegion()->insertTrigger(tr);
	
	return true;
}

void WorldObject::writeNetEvent(NetEvent* event, CharConv* cv)
{
	GameObject::writeNetEvent(event,cv);
	
	if (event->m_data & NetEvent::DATA_ACTION)
	{
		cv->toBuffer(m_animation);
		cv->toBuffer(m_animation_time);
		cv->toBuffer(m_animation_repeat);
		cv->toBuffer(m_animation_elapsed_time);
	}
	
	if (event->m_data & NetEvent::DATA_NAME)
	{
		m_name.toString(cv);
	}
	
	if (event->m_data & NetEvent::DATA_FLAGS)
	{
		cv->toBuffer(static_cast<short>(m_flags.size()));
		std::set<std::string>::iterator it;
		for (it = m_flags.begin(); it != m_flags.end(); ++it)
		{
			cv->toBuffer(*it);
		}
	}
}

void WorldObject::processNetEvent(NetEvent* event, CharConv* cv)
{
	short oldlayer = getLayer();
	GameObject::processNetEvent(event,cv);
	
	// wenn die Layer geaendert wird, die alte Layer vorruebergehend wiederherstellen
	// der Region die Aenderung mitteilen und wiederherstellen
	if (oldlayer != getLayer() && getRegion() != 0)
	{
		short newlayer = getLayer();
		setLayer((WorldObject::Layer) oldlayer);
		getRegion()->changeObjectLayer(this, (WorldObject::Layer) newlayer);
		setLayer((WorldObject::Layer) newlayer);
	}
	
	if (event->m_data & NetEvent::DATA_ACTION)
	{
		cv->fromBuffer(m_animation);
		cv->fromBuffer(m_animation_time);
		cv->fromBuffer(m_animation_repeat);
		cv->fromBuffer(m_animation_elapsed_time);
	}
	
	if (event->m_data & NetEvent::DATA_NAME)
	{
		m_name.fromString(cv);
	}
	
	if (event->m_data & NetEvent::DATA_FLAGS)
	{
		m_flags.clear();
		std::string flag;
		short nr;
		cv->fromBuffer(nr);
		for (int i=0; i<nr; i++)
		{
			cv->fromBuffer(flag);
			m_flags.insert(flag);
		}
	}
}

void WorldObject::setAnimation(std::string anim, float time, bool repeat)
{
	m_animation = anim; 
	m_animation_elapsed_time =0;
	m_animation_time = time;
	m_animation_repeat = repeat;
	
	addToNetEventMask(NetEvent::DATA_ACTION);
}

void WorldObject::setFlag(std::string flag, bool set)
{
	addToNetEventMask(NetEvent::DATA_FLAGS);
	if (set == true)
	{
		m_flags.insert(flag);
	}
	else
	{
		m_flags.erase(flag);
	}
}

void WorldObject::clearFlags()
{
	m_flags.clear();
	addToNetEventMask(NetEvent::DATA_FLAGS);
}

WorldObject::Group WorldObject::getGroup()
{
	if (!isCreature())
		return WorldObject::FIXED;
	if (getState() == STATE_DEAD || getState() == STATE_DIEING )
		return WorldObject::DEAD;
	if (getType() == "PLAYER")
		return PLAYER;
	return WorldObject::CREATURE_ONLY;
			
}
