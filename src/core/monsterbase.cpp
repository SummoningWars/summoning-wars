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

#include "monsterbase.h"

void MonsterAIVars::writeToXML(TiXmlNode* node)
{
	TiXmlElement* elem = node->ToElement();
	if (elem == 0) 
		return;
	
	XMLUtil::setDoubleAttribute(elem,"ai_sight_range",m_sight_range,12);
	XMLUtil::setDoubleAttribute(elem,"ai_shoot_range",m_shoot_range,m_sight_range);
	XMLUtil::setDoubleAttribute(elem,"ai_action_range",m_action_range,m_sight_range);
	XMLUtil::setDoubleAttribute(elem,"ai_randaction_prob",m_randaction_prob,0);
	XMLUtil::setDoubleAttribute(elem,"ai_chase_distance",m_chase_distance,20);
	XMLUtil::setDoubleAttribute(elem,"ai_warn_radius",m_warn_radius,4);
}

void MonsterBasicData::operator=(MonsterBasicData& other)
{
	m_base_attr = other.m_base_attr;
	m_type = other.m_type;
	m_subtype = other.m_subtype;
	
	m_race = other.m_race;
	m_fraction_name = other.m_fraction_name;
	m_ai_vars = other.m_ai_vars;
	m_radius = other.m_radius;
	m_layer = other.m_layer;
	m_name = other.m_name;
	m_emotion_set = other.m_emotion_set;
	
	for (int i=0; i<4; i++)
	{
		m_drop_slots[i] = other.m_drop_slots[i];
	}
}

void MonsterBasicData::writeToXML(TiXmlNode* node)
{
	TiXmlElement* elem = node->ToElement();
	if (elem == 0) 
		return;
	
	if (m_layer == (WorldObject::LAYER_BASE | WorldObject::LAYER_AIR))
	{
		elem->SetAttribute("layer","NORMAL");
	}
	else if (m_layer ==  WorldObject::LAYER_BASE)
	{
		elem->SetAttribute("layer","BASE");
	}
	else if (m_layer ==  WorldObject::LAYER_AIR)
	{
		elem->SetAttribute("layer","AIR");
	}
	else if (m_layer ==  WorldObject::LAYER_NOCOLLISION)
	{
		elem->SetAttribute("layer","NOCOLLISION");
	}
	
	//elem->SetAttribute("subtype",m_subtype.c_str());
	elem->SetAttribute("race",m_race.c_str());
	XMLUtil::setAttribute(elem,"fraction",m_fraction_name.c_str(),"MONSTER");
	elem->SetAttribute("name",m_name.getRawText().c_str());
	
	// point where to insert additional subobjects
	TiXmlElement* insert_point = 0;
	
	// search for the position after potential <Mesh> and <Image> elements
	insert_point = node->FirstChildElement("Mesh");
	if (insert_point == 0)
		insert_point = node->FirstChildElement("RenderInfo");
	
	//TiXmlElement* insert_point_tmp; // 2011.10.23: found as unused.
	TiXmlElement inserter("dummy");	// dummy node just for being compied by tinyxml x(
		
	// detect attribute element
	TiXmlElement* drops =  node->FirstChildElement("Dropslots");
	if (drops == 0)
	{
		drops = XMLUtil::insertNodeAfter(elem, insert_point, &inserter);
		drops->SetValue("Dropslots");
		insert_point = drops;
	}
	
	// Write DropSlots
	TiXmlElement* dropslot = drops->FirstChildElement("Dropslot");
	TiXmlElement* drop_insert_point = dropslot;
	// loop to ensure, that the number of dropslots is correct
	for (int i=0; i<4; i++)
	{
		if (m_drop_slots[i].m_max_level<0)
		{
			// no item should be dropped
			continue;
		}
		
		if (dropslot == 0)
		{
			// create new Element
			dropslot = XMLUtil::insertNodeAfter(drops, drop_insert_point, &inserter);
			dropslot->SetValue("Dropslot");
			drop_insert_point = dropslot;
		}
		
		// write Data
		XMLUtil::setDoubleAttribute(dropslot, "pbig",m_drop_slots[i].m_size_probability[Item::BIG],0);
		XMLUtil::setDoubleAttribute(dropslot, "pmedium",m_drop_slots[i].m_size_probability[Item::MEDIUM],0);
		XMLUtil::setDoubleAttribute(dropslot, "psmall",m_drop_slots[i].m_size_probability[Item::SMALL],0);
		XMLUtil::setDoubleAttribute(dropslot, "pgold",m_drop_slots[i].m_size_probability[Item::GOLD],0);
		
		XMLUtil::setAttribute(dropslot, "min_level",m_drop_slots[i].m_min_level,0);
		XMLUtil::setAttribute(dropslot, "max_level",m_drop_slots[i].m_max_level,0);
		XMLUtil::setAttribute(dropslot, "min_gold",m_drop_slots[i].m_min_gold,1);
		XMLUtil::setAttribute(dropslot, "max_gold",m_drop_slots[i].m_max_gold,1);
		
		XMLUtil::setDoubleAttribute(dropslot, "magic_prob",m_drop_slots[i].m_magic_probability,0);
		XMLUtil::setDoubleAttribute(dropslot, "magic_power",m_drop_slots[i].m_magic_power,0);

		dropslot = dropslot->NextSiblingElement("Dropslot");
	}
	
	// delete all remaining Dropslots Elements
	while (dropslot != 0)
	{
		TiXmlElement* todel = dropslot;
		dropslot = dropslot->NextSiblingElement("Dropslot");
		drops->RemoveChild(todel);
	}
	
	// detect basic attribute
	TiXmlElement* attr =  node->FirstChildElement("BasicAttributes");
	if (attr == 0)
	{
		attr = XMLUtil::insertNodeAfter(elem, insert_point, &inserter);
		attr->SetValue("BasicAttributes");
		insert_point = attr;
	}
	
	m_base_attr.writeToXML(attr);
	
	TiXmlElement* geom =  node->FirstChildElement("Geometry");
	if (geom == 0)
	{
		geom = XMLUtil::insertNodeAfter(elem, insert_point, &inserter);
		geom->SetValue("Geometry");
		insert_point = geom;
	}
	geom->SetDoubleAttribute("radius", m_radius);
	
	
	TiXmlElement* ai =  node->FirstChildElement("AI");
	if (ai == 0)
	{
		ai = XMLUtil::insertNodeAfter(elem, insert_point, &inserter);
		ai->SetValue("AI");
		insert_point = ai;
	}
	m_ai_vars.writeToXML(ai);
	
	
}