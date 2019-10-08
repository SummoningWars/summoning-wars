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

#include "dropitem.h"
#include "itemfactory.h"

DropItem::DropItem(int id)
	: GameObject(id)
{
	init();
	m_item =0;
}

DropItem::DropItem(Item* item)
	: GameObject(item->m_id)
{
	m_item = item;
	setSubtype(item->m_subtype);
	init();
}

void DropItem::init()
{
	setBaseType(DROPITEM);
	setType("DROPITEM");
	setState(STATE_ACTIVE);
	setAngle(Random::random()*3.14159*2);

	float hadd = 0.1;
	float acc = -20;
	float v0 =4;
	setHeight(2.0 +hadd);
	
	
	float t = -v0/(2*acc)+sqrt(v0*v0/(4*acc*acc) - 2/acc);
	m_drop_speed = 4/t;
	m_angle_x = 4;
	
	m_time =0;
}

DropItem::~DropItem()
{
	
}

void DropItem::toString(CharConv* cv)
{
	GameObject::toString(cv);
	m_item->toString(cv);
	cv->toBuffer(m_angle_x);
}

void DropItem::fromString(CharConv* cv)
{
	GameObject::fromString(cv);
	
	int id;
	Item::Subtype subtype;
	char type;
	
	cv->fromBuffer(type);
	cv->fromBuffer(subtype);
	cv->fromBuffer(id);
	setSubtype(subtype);
	if (id != getId())
	{
		ERRORMSG("inconsistent dropitem: id %i item id %i",getId(), id);
	}
	
	if (m_item != 0)
		delete m_item;
	
	
	m_item = ItemFactory::createItem((Item::Type) type, subtype,id);
	m_item->fromString(cv);
	cv->fromBuffer(m_angle_x);

	m_time = cv->getDelay();
}


bool DropItem::update(float time)
{
	m_time += time;
	float hadd = 0.1;
			
	if (getHeight()>hadd)
	{
		float acc = -20;
		float v0 =4;
		setHeight(hadd+ 2 + v0*m_time/1000 + acc*m_time*m_time/1000000);
		m_angle_x -= m_drop_speed*time/1000;
		
		if (getHeight() <= hadd)
		{
			setHeight(hadd);
			m_angle_x=0;
		}
		//DEBUG("angle_x %f",m_angle_x); 
		DEBUGX("speed %f height %f angle %f time %f",m_drop_speed,getHeight(),m_angle_x,m_time); 
	}
	return true;
}

int DropItem::getLocationId()
{
	short sx = (int) (getPosition().m_x*2.0 + 0.5);
	short sy = (int) (getPosition().m_y*2.0 + 0.5);
	return 10000*sx + sy;
}

float DropItem::getActionPercent()
{
	float hadd = 0.1;
	if (getHeight() > hadd)
		return m_time/300;
	
	return fmod(m_time,2000);
}

std::string DropItem::getActionString()
{
	float hadd = 0.1;
	if (getHeight() > hadd)
		return "drop";
	
	return "dropped";
}

