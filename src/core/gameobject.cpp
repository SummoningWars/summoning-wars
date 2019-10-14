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

#include "gameobject.h"
#include "world.h"
#include "region.h"

GameObject::GameObject( int id)
:	m_id(0),
	m_region_id(-1),
	m_state(STATE_ACTIVE),
	m_base_type(WORLDOBJECT),
	m_type(),
	m_subtype(),
	m_event_mask(0),
	m_height(0),
	m_layer(LAYER_BASE),
	m_speed(0,0),
	m_destroyed(false)
{
	if (id !=0)
	{
		m_id = id;
	}
	else if (World::getWorld() !=0)
	{
		m_id = World::getWorld()->getValidId();
	}
	
}


void GameObject::setState(State s, bool throwevent)
{
	m_state = s;
	
	// einige Zustaende werden nicht ueber das Netzwerk angeglichen
	if (throwevent)
	{
		addToNetEventMask(NetEvent::DATA_STATE);
	}
}

void  GameObject::setType(Type type)
{
	m_type = type;
}

void GameObject::setSubtype(Subtype subtype)
{
	m_subtype = subtype;
}

void GameObject::setLayer(short layer)
{
	m_layer = layer;
}

Region* GameObject::getRegion()
{
	if (World::getWorld() == 0)
		return 0;

	return World::getWorld()->getRegion(m_region_id);
}

void GameObject::setSpeed(Vector speed, bool throwevent)
{
	m_speed =speed;
	if (m_speed.getLength() >0.001f)
	{
		getShape()->m_angle = m_speed.angle();
	}
	if (throwevent)
	{
		m_event_mask |= NetEvent::DATA_SPEED;
	}
}

void GameObject::setAngle(float angle)
{
	m_shape.m_angle = angle;
}

void  GameObject::setPosition(Vector pos)
{
	m_shape.m_center = pos;
}

void GameObject::toString(CharConv* cv)
{
	cv->toBuffer(m_type);
	cv->toBuffer(m_subtype);
	cv->toBuffer(m_id);
	cv->toBuffer(m_shape.m_center.m_x);
	cv->toBuffer(m_shape.m_center.m_y);
	cv->toBuffer(m_height);
	cv->toBuffer((char) m_shape.m_type);
	if (m_shape.m_type==Shape::RECT)
	{
		cv->toBuffer(m_shape.m_extent.m_x);
		cv->toBuffer(m_shape.m_extent.m_y);
	}
	else
	{
		cv->toBuffer(m_shape.m_radius);
		cv->toBuffer(m_shape.m_radius);
	}
	cv->toBuffer((char) m_layer);
	cv->toBuffer(m_shape.m_angle);

	cv->toBuffer((char) m_state);
}

void GameObject::fromString(CharConv* cv)
{
	char ctmp;

	// Typ, Subtyp und ID werden extern gelesen
	cv->fromBuffer(m_shape.m_center.m_x) ;
	cv->fromBuffer(m_shape.m_center.m_y);
	cv->fromBuffer(m_height);
	cv->fromBuffer(ctmp);
	m_shape.m_type = (Shape::ShapeType) ctmp;
	if (m_shape.m_type==Shape::RECT)
	{
		cv->fromBuffer(m_shape.m_extent.m_x);
		cv->fromBuffer(m_shape.m_extent.m_y);
	}
	else
	{
		cv->fromBuffer(m_shape.m_radius);
		cv->fromBuffer(m_shape.m_radius);
	}
	cv->fromBuffer(ctmp);
	m_layer  = (Layer) ctmp;
	cv->fromBuffer(m_shape.m_angle);

	cv->fromBuffer(ctmp);
	m_state = (State) ctmp;
}

TranslatableString GameObject::getName()
{
	return TranslatableString(ObjectFactory::getObjectName(m_subtype));
}


std::string GameObject::getNameId()
{
	std::ostringstream out_stream;

	out_stream.str("");
	out_stream << m_subtype << ":" << getId();
	return out_stream.str();
}

void GameObject::writeNetEvent(NetEvent* event, CharConv* cv)
{
	
	if (event->m_data & NetEvent::DATA_TYPE)
	{
		cv->toBuffer(m_type);
		cv->toBuffer(m_subtype);
	}
	
	if (event->m_data & NetEvent::DATA_SHAPE)
	{
		cv->toBuffer(m_shape.m_center.m_x);
		cv->toBuffer(m_shape.m_center.m_y);
		cv->toBuffer((char) m_shape.m_type);
		if (m_shape.m_type==Shape::RECT)
		{
			cv->toBuffer(m_shape.m_extent.m_x);
			cv->toBuffer(m_shape.m_extent.m_y);
		}
		else
		{
			cv->toBuffer(m_shape.m_radius);
			cv->toBuffer(m_shape.m_radius);
		}
		cv->toBuffer((char) m_layer);
		cv->toBuffer(m_shape.m_angle);
		cv->toBuffer(m_height);
	}
	
	/*
	if (event->m_data & NetEvent::DATA_STATE)
	{
		cv->toBuffer((char) getState());
	}
	
	if (event->m_data & NetEvent::DATA_SPEED)
	{
		cv->toBuffer(getSpeed().m_x);
		cv->toBuffer(getSpeed().m_y);
	}
*/
}

void GameObject::processNetEvent(NetEvent* event, CharConv* cv)
{
	
	if (event->m_data & NetEvent::DATA_TYPE)
	{
		cv->fromBuffer(m_type);
		cv->fromBuffer(m_subtype);
	}
	
	if (event->m_data & NetEvent::DATA_SHAPE)
	{
		
		char ctmp;

		cv->fromBuffer(m_shape.m_center.m_x) ;
		cv->fromBuffer(m_shape.m_center.m_y);
		cv->fromBuffer(ctmp);
		m_shape.m_type = (Shape::ShapeType) ctmp;
		if (m_shape.m_type==Shape::RECT)
		{
			cv->fromBuffer(m_shape.m_extent.m_x);
			cv->fromBuffer(m_shape.m_extent.m_y);
		}
		else
		{
			cv->fromBuffer(m_shape.m_radius);
			cv->fromBuffer(m_shape.m_radius);
		}
		cv->fromBuffer(ctmp);
		m_layer  = (Layer) ctmp;
		cv->fromBuffer(m_shape.m_angle);
		cv->fromBuffer(m_height);
	}
	
	/*
	if (event->m_data & NetEvent::DATA_STATE)
	{
		char ctmp;
		cv->fromBuffer(ctmp);
		setState((State) ctmp);
		DEBUGX("object %i changed state to %i",getId(),ctmp);
	}
	
	if (event->m_data & NetEvent::DATA_SPEED)
	{
		cv->fromBuffer(m_speed.m_x);
		cv->fromBuffer(m_speed.m_y);

	}
	*/
}


void GameObject::addToNetEventMask(int mask)
{
	m_event_mask |= mask;
}

int GameObject::getValue(std::string valname)
{
	if (valname =="id")
	{
		lua_pushinteger(EventSystem::getLuaState() , getId() );
		return 1;
	}
	else if (valname == "region")
	{
		Region* reg = getRegion();
		std::string rname;
		if (reg !=0)
		{
			rname = reg->getName();
		}
		else
		{
			rname = "noregion";
		}
		
		lua_pushstring(EventSystem::getLuaState() ,rname.c_str() );
		return 1;
	}
	else if (valname == "subtype")
	{
		lua_pushstring(EventSystem::getLuaState() ,getSubtype().c_str() );
		return 1;
	}
	else if (valname == "angle")
	{
		lua_pushnumber(EventSystem::getLuaState() , getShape()->m_angle *180/3.14159);
		return 1;
	}
	else if (valname == "height")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_height);
		return 1;
	}
	else if (valname == "position")
	{
		EventSystem::pushVector(EventSystem::getLuaState(),getShape()->m_center);
		return 1;
	}
	else if (valname == "speed")
	{
		EventSystem::pushVector(EventSystem::getLuaState(),m_speed);
		return 1;
	}
	else if (valname == "layer")
	{
		if (m_layer == WorldObject::LAYER_BASE)
		{
			lua_pushstring(EventSystem::getLuaState() ,"base");
		}
		else  if (m_layer ==WorldObject::LAYER_AIR)
		{
			lua_pushstring(EventSystem::getLuaState() ,"air");
		}
		else  if (m_layer ==WorldObject::LAYER_DEAD)
		{
			lua_pushstring(EventSystem::getLuaState() ,"dead");
		}
		else  if (m_layer == (WorldObject::LAYER_BASE | WorldObject::LAYER_AIR))
		{
			lua_pushstring(EventSystem::getLuaState() ,"normal");
		}
		else  if (m_layer == WorldObject::LAYER_NOCOLLISION)
		{
			lua_pushstring(EventSystem::getLuaState() ,"nocollision");
		}
		else
		{
			lua_pushstring(EventSystem::getLuaState() ,"nolayer");
		}
		return 1;
	}
	else if (valname == "type")
	{
			
		lua_pushstring(EventSystem::getLuaState(),getType().c_str());

		return 1;
	}
	
	return 0;
}

bool GameObject::setValue(std::string valname)
{
	if (valname == "angle")
	{
		float angle = lua_tonumber(EventSystem::getLuaState() ,-1) * 3.14159 / 180;
		lua_pop(EventSystem::getLuaState(), 1);
		addToNetEventMask(NetEvent::DATA_SHAPE);
		
		setAngle(angle);
		
		return true;
	}
	if (valname == "height")
	{
		m_height = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		addToNetEventMask(NetEvent::DATA_SHAPE);
		
		return true;
	}
	else if (valname == "state")
	{
		std::string statestr = lua_tostring(EventSystem::getLuaState(), -1);
		lua_pop(EventSystem::getLuaState(), 1);
		
		if (statestr =="active" && getState() == STATE_INACTIVE)
			setState(STATE_ACTIVE);
		if (statestr =="inactive" && getState() == STATE_ACTIVE)
			setState(STATE_INACTIVE);
	}
	else if (valname == "layer")
	{
		std::string layer = lua_tostring(EventSystem::getLuaState(), -1);
		lua_pop(EventSystem::getLuaState(), 1);
		addToNetEventMask(NetEvent::DATA_SHAPE);
		
		if (layer == "base")
		{
			setLayer(WorldObject::LAYER_BASE);
		}
		else  if (layer =="air")
		{
			setLayer(WorldObject::LAYER_AIR);
		}
		else  if (layer =="dead")
		{
			setLayer(WorldObject::LAYER_DEAD);
		}
		else  if (layer == "normal")
		{
			setLayer(WorldObject::LAYER_BASE | WorldObject::LAYER_AIR);
		}
		else  if (layer == "nocollision")
		{
			setLayer(WorldObject::LAYER_NOCOLLISION);
		}
		
		return true;
	}
	
	return false;
}

