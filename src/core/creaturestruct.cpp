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

#include "creaturestruct.h"
#include "damage.h"
#include "eventsystem.h"
#include "xmlutil.h"

void CreatureBaseAttr::init()
{
	DEBUGX("init CreatureBaseAttrMod");
		// alles nullen
	m_level =1;
	m_armor =0;
	m_block =0;
	m_max_health =0;
	m_attack =0;
	m_power =0;
	m_strength =0;
	m_dexterity =0;
	m_magic_power =0;
	m_willpower =0;
	for (int i=0; i<4; i++)
	{
		m_resistances[i] =0;
		m_resistances_cap[i] =0;
	}
	m_walk_speed =0;
	m_attack_speed=0;
	m_step_length = 1;
	m_special_flags=0;
	m_abilities.clear();
	m_immunity =0;
	m_max_experience = 100;
	m_attack_range = 1;
}

void CreatureBaseAttr::operator=(CreatureBaseAttr other)
{
	m_level =other.m_level;
	m_armor =other.m_armor;
	m_block =other.m_block;
	m_max_health =other.m_max_health;
	m_attack =other.m_attack;
	m_power =other.m_power;
	m_strength =other.m_strength;
	m_dexterity =other.m_dexterity;
	m_magic_power =other.m_magic_power;
	m_willpower =other.m_willpower;
	for (int i=0; i<4; i++)
	{
		m_resistances[i] =other.m_resistances[i];
		m_resistances_cap[i] = other.m_resistances_cap[i];
	}
	m_walk_speed =other.m_walk_speed;
	m_attack_speed=other.m_attack_speed;
	m_step_length = other.m_step_length;
	m_special_flags=other.m_special_flags;
	m_abilities = other.m_abilities;
	m_immunity =other.m_immunity;
	m_max_experience = other.m_max_experience;
	m_attack_range = other.m_attack_range;
}

void CreatureBaseAttr::writeToXML(TiXmlNode* node)
{
	TiXmlElement* elem = node->ToElement();
	if (elem == 0) 
		return;
	
	XMLUtil::setAttribute(elem, "armor",m_armor,0);
	XMLUtil::setAttribute(elem, "block",m_block,0);
	XMLUtil::setDoubleAttribute(elem, "max_health",m_max_health,1);
	XMLUtil::setAttribute(elem, "attack",m_attack,0);
	XMLUtil::setAttribute(elem, "power",m_power,0);
	XMLUtil::setAttribute(elem, "strength",m_strength,0);
	XMLUtil::setAttribute(elem, "dexterity",m_dexterity,0);
	XMLUtil::setAttribute(elem, "willpower",m_willpower,0);
	XMLUtil::setAttribute(elem, "magic_power",m_magic_power,0);
	
	XMLUtil::setAttribute(elem, "walk_speed",m_walk_speed,2000);
	XMLUtil::setAttribute(elem, "attack_speed",m_attack_speed,1500);
	
	XMLUtil::setDoubleAttribute(elem, "step_length",m_step_length,1);
	XMLUtil::setDoubleAttribute(elem, "attack_range",m_attack_range,1);
	XMLUtil::setDoubleAttribute(elem, "experience",m_max_experience,0);
	
	XMLUtil::setAttribute(elem, "level",m_level,1500);
	
	XMLUtil::setAttribute(elem, "resistances_physical",m_resistances[Damage::PHYSICAL],0);
	XMLUtil::setAttribute(elem, "resistances_fire",m_resistances[Damage::FIRE],0);
	XMLUtil::setAttribute(elem, "resistances_ice",m_resistances[Damage::ICE],0);
	XMLUtil::setAttribute(elem, "resistances_air",m_resistances[Damage::AIR],0);
	
	XMLUtil::setAttribute(elem, "resistances_cap_physical",m_resistances_cap[Damage::PHYSICAL],50);
	XMLUtil::setAttribute(elem, "resistances_cap_fire",m_resistances_cap[Damage::FIRE],50);
	XMLUtil::setAttribute(elem, "resistances_cap_ice",m_resistances_cap[Damage::ICE],50);
	XMLUtil::setAttribute(elem, "resistances_cap_air",m_resistances_cap[Damage::AIR],50);
	
	// write abilities
	std::map<std::string, AbilityInfo>::iterator ait;
	
	// point where to insert additional subobjects
	TiXmlElement* insert_point = 0;
	TiXmlElement inserter("dummy");	// dummy node just for being compied by tinyxml x(
		
	// insert new abilities after the last one
	for (TiXmlElement* subelem = node->FirstChildElement("Ability"); subelem != 0; subelem = subelem->NextSiblingElement("Ability"))
	{
		insert_point = subelem;
	}
	
	// set of ability XML elements which were found
	std::set<TiXmlElement*> ablt_found;
	for (ait = m_abilities.begin(); ait != m_abilities.end(); ++ait)
	{
		// search for the ability with the correct type
		TiXmlElement* ablt_node = 0;
		for (TiXmlElement* subelem = node->FirstChildElement("Ability"); subelem != 0; subelem = subelem->NextSiblingElement("Ability"))
		{
			if (subelem->Attribute("type") != 0 && ait->first == subelem->Attribute("type"))
			{
				ablt_node = subelem;
				break;
			}
		}
		
		if (ablt_node == 0)
		{
			// none found, create the node
			ablt_node = XMLUtil::insertNodeAfter(elem, insert_point, &inserter);
			ablt_node->SetValue("Ability");
			insert_point = ablt_node;
		}
		
		// write Data
		AbilityInfo& ablt = ait->second;
		
		XMLUtil::setAttribute(ablt_node, "type",ait->first);
		XMLUtil::setDoubleAttribute(ablt_node, "time",ablt.m_time,0);
		XMLUtil::setAttribute(ablt_node, "timer_nr",ablt.m_timer_nr,-1);
		XMLUtil::setDoubleAttribute(ablt_node, "timer_value",ablt.m_timer,-1);
		XMLUtil::setDoubleAttribute(ablt_node, "rating",ablt.m_rating,-1);
		XMLUtil::setDoubleAttribute(ablt_node, "all_target_rating",ablt.m_all_target_rating,0);
		XMLUtil::setDoubleAttribute(ablt_node, "all_target_rating_radius",ablt.m_all_target_rating_range,-1);
		XMLUtil::setDoubleAttribute(ablt_node, "random_rating",ablt.m_random_rating,0.01);
		
		ablt_found.insert(ablt_node);
	}
	
	// delete all remaining ability elements
	for (TiXmlElement* subelem = node->FirstChildElement("Ability"); subelem != 0;  )
	{
		TiXmlElement* todel = subelem;
		subelem = subelem->NextSiblingElement("Ability");
		if (ablt_found.count(todel) == 0)
		{
			elem->RemoveChild(todel);
		}
	}
	
	// write the Immunities
	std::set<std::string> immunities;
	if (m_immunity & (1<<Damage::BLIND)) immunities.insert("blind");
	if (m_immunity & (1<<Damage::POISONED)) immunities.insert("poisoned");
	if (m_immunity & (1<<Damage::BERSERK)) immunities.insert("berserk");
	if (m_immunity & (1<<Damage::CONFUSED)) immunities.insert("confused");
	if (m_immunity & (1<<Damage::MUTE)) immunities.insert("mute");
	if (m_immunity & (1<<Damage::PARALYZED)) immunities.insert("paralyzed");
	if (m_immunity & (1<<Damage::FROZEN)) immunities.insert("frozen");
	if (m_immunity & (1<<Damage::BURNING)) immunities.insert("burning");

	std::set<TiXmlElement*> imm_found;
	for (std::set<std::string>::iterator it = immunities.begin(); it != immunities.end(); ++it)
	{
		// find the right immunity element
		TiXmlElement* imm_node = 0;
		for (TiXmlElement* subelem = node->FirstChildElement("Immunity"); subelem != 0; subelem = subelem->NextSiblingElement("Immunity"))
		{
			if (subelem->Attribute("type") != 0 && (*it) == subelem->Attribute("type"))
			{
				imm_node = subelem;
				break;
			}
		}
		
		if (imm_node == 0)
		{
			// none found, create the node
			imm_node = XMLUtil::insertNodeAfter(elem, insert_point, &inserter);
			imm_node->SetValue("Immunity");
			insert_point = imm_node;
		}
		
		XMLUtil::setAttribute(imm_node, "type",*it);
		
		imm_found.insert(imm_node);
	}
	
	// delete all remaining Immunity elements
	for (TiXmlElement* subelem = node->FirstChildElement("Immunity"); subelem != 0;  )
	{
		TiXmlElement* todel = subelem;
		subelem = subelem->NextSiblingElement("Immunity");
		if (imm_found.count(todel) == 0)
		{
			elem->RemoveChild(todel);
		}
	}
	
}

int CreatureBaseAttr::getValue(std::string valname)
{
	if (valname =="level")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_level );
		return 1;
	}
	else if (valname =="strength")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_strength );
		return 1;
	}
	else if (valname =="dexterity")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_dexterity );
		return 1;
	}
	else if (valname =="willpower")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_willpower );
		return 1;
	}
	else if (valname =="magic_power")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_magic_power );
		return 1;
	}
	else if (valname =="max_health")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_max_health );
		return 1;
	}
	else if (valname =="block")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_block );
		return 1;
	}
	else if (valname =="attack")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_attack );
		return 1;
	}
	else if (valname =="power")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_power );
		return 1;
	}
	else if (valname =="armor")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_armor );
		return 1;
	}
	else if (valname =="resist_fire")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_resistances[Damage::FIRE] );
		return 1;
	}
	else if (valname =="resist_ice")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_resistances[Damage::ICE] );
		return 1;
	}
	else if (valname =="resist_air")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_resistances[Damage::AIR] );
		return 1;
	}
	else if (valname =="resist_phys")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_resistances[Damage::PHYSICAL] );
		return 1;
	}
	else if (valname =="max_resist_ice")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_resistances_cap[Damage::ICE] );
		return 1;
	}
	else if (valname =="max_resist_air")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_resistances_cap[Damage::AIR] );
		return 1;
	}
	else if (valname =="max_resist_fire")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_resistances_cap[Damage::FIRE] );
		return 1;
	}
	else if (valname =="max_resist_phys")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_resistances_cap[Damage::PHYSICAL] );
		return 1;
	}
	else if (valname =="attack_speed")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_attack_speed );
		return 1;
	}
	else if (valname =="walk_speed")
	{
		lua_pushinteger(EventSystem::getLuaState() ,m_walk_speed );
		return 1;
	}
	
	return 0;
}

bool CreatureBaseAttr::setValue(std::string valname, int& event_mask)
{
	if (valname =="level")
	{
		m_level = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		event_mask |= NetEvent::DATA_ATTRIBUTES_LEVEL;
		return true;
	}
	else if (valname =="strength")
	{
		m_strength = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		event_mask |= NetEvent::DATA_ATTRIBUTES_LEVEL;
		return true;
	}
	else if (valname =="dexterity")
	{
		m_dexterity = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		event_mask |= NetEvent::DATA_ATTRIBUTES_LEVEL;
		return true;
	}
	else if (valname =="willpower")
	{
		m_willpower = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		event_mask |= NetEvent::DATA_ATTRIBUTES_LEVEL;
		return true;
	}
	else if (valname =="magic_power")
	{
		m_magic_power = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		event_mask |= NetEvent::DATA_ATTRIBUTES_LEVEL;
		return true;
	}
	else if (valname =="max_health")
	{
		m_max_health = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		event_mask |= NetEvent::DATA_ATTRIBUTES_LEVEL;
		return true;
	}
	else if (valname =="block")
	{
		m_block = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="attack")
	{
		m_attack = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="armor")
	{
		m_armor = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="resist_fire")
	{
		m_resistances[Damage::FIRE] = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
		
	}
	else if (valname =="resist_ice")
	{
		m_resistances[Damage::ICE] = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="resist_air")
	{
		m_resistances[Damage::AIR] = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="resist_phys")
	{
		m_resistances[Damage::PHYSICAL] = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="max_resist_ice")
	{
		m_resistances_cap[Damage::ICE] = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="max_resist_air")
	{
		m_resistances_cap[Damage::AIR] = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="max_resist_fire")
	{
		m_resistances_cap[Damage::FIRE] = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="max_resist_phys")
	{
		m_resistances_cap[Damage::PHYSICAL] = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="attack_speed")
	{
		m_attack_speed = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		event_mask |= NetEvent::DATA_ATTACK_WALK_SPEED;
		return true;
	}
	else if (valname =="walk_speed")
	{
		m_walk_speed = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		event_mask |= NetEvent::DATA_ATTACK_WALK_SPEED;
		return true;
	}
	
	return false;
}


void CreatureBaseAttrMod::init()
{
	DEBUGX("init CreatureBaseAttrMod");
		// alles nullen
	m_darmor =0;
	m_dblock =0;
	m_dmax_health =0;
	m_dattack =0;
	m_dpower =0;
	m_dstrength =0;
	m_ddexterity =0;
	m_dmagic_power =0;
	m_dwillpower =0;
	for (int i=0; i<4; i++)
	{
		m_dresistances[i] =0;
		m_dresistances_cap[i] =0;
	}
	m_dwalk_speed =0;
	m_dattack_speed=0;
	m_xspecial_flags=0;
	m_time =0;
	m_xabilities.clear();
	m_ximmunity =0;
	m_flag = "";
}


void CreatureBaseAttrMod::operator=(CreatureBaseAttrMod other)
{
	m_darmor =other.m_darmor;
	m_dblock =other.m_dblock;
	m_dmax_health =other.m_dmax_health;
	m_dattack =other.m_dattack;
	m_dpower =other.m_dpower;
	m_dstrength =other.m_dstrength;
	m_ddexterity =other.m_ddexterity;
	m_dmagic_power =other.m_dmagic_power;
	m_dwillpower =other.m_dwillpower;
	for (int i=0; i<4; i++)
	{
		m_dresistances[i] =other.m_dresistances[i];
		m_dresistances_cap[i] =other.m_dresistances_cap[i];
	}
	m_dwalk_speed =other.m_dwalk_speed;
	m_dattack_speed=other.m_dattack_speed;
	m_xspecial_flags=other.m_xspecial_flags;
	m_time = other.m_time;
	m_xabilities= other.m_xabilities;
	m_flag = other.m_flag;
	m_ximmunity = other.m_ximmunity;
}

void CreatureBaseAttrMod::writeToXML(TiXmlNode* node)
{
	TiXmlElement* elem = node->ToElement();
	if (elem == 0) 
		return;
	
	XMLUtil::setAttribute(elem, "darmor",m_darmor,0);
	XMLUtil::setAttribute(elem, "dblock",m_dblock,0);
	XMLUtil::setDoubleAttribute(elem, "dmax_health",m_dmax_health,0);
	XMLUtil::setAttribute(elem, "dattack",m_dattack,0);
	XMLUtil::setAttribute(elem, "dpower",m_dpower,0);
	XMLUtil::setAttribute(elem, "dstrength",m_dstrength,0);
	XMLUtil::setAttribute(elem, "ddexterity",m_ddexterity,0);
	XMLUtil::setAttribute(elem, "dwillpower",m_dwillpower,0);
	XMLUtil::setAttribute(elem, "dmagic_power",m_dmagic_power,0);
	XMLUtil::setAttribute(elem, "flag",m_flag);
	
	XMLUtil::setAttribute(elem, "dwalk_speed",m_dwalk_speed,0);
	XMLUtil::setAttribute(elem, "dattack_speed",m_dattack_speed,0);
	
	XMLUtil::setAttribute(elem, "dresistances_physical",m_dresistances[Damage::PHYSICAL],0);
	XMLUtil::setAttribute(elem, "dresistances_fire",m_dresistances[Damage::FIRE],0);
	XMLUtil::setAttribute(elem, "dresistances_ice",m_dresistances[Damage::ICE],0);
	XMLUtil::setAttribute(elem, "dresistances_air",m_dresistances[Damage::AIR],0);
	
	XMLUtil::setAttribute(elem, "dresistances_cap_physical",m_dresistances_cap[Damage::PHYSICAL],0);
	XMLUtil::setAttribute(elem, "dresistances_cap_fire",m_dresistances_cap[Damage::FIRE],0);
	XMLUtil::setAttribute(elem, "dresistances_cap_ice",m_dresistances_cap[Damage::ICE],0);
	XMLUtil::setAttribute(elem, "dresistances_cap_air",m_dresistances_cap[Damage::AIR],0);
}

int CreatureBaseAttrMod::getValue(std::string valname)
{
	if (valname =="strength")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_dstrength );
		return 1;
	}
	if (valname =="time")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_time );
		return 1;
	}
	else if (valname =="dexterity")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_ddexterity );
		return 1;
	}
	else if (valname =="willpower")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_dwillpower );
		return 1;
	}
	else if (valname =="magic_power")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_dmagic_power );
		return 1;
	}
	else if (valname =="max_health")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_dmax_health );
		return 1;
	}
	else if (valname =="block")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_dblock );
		return 1;
	}
	else if (valname =="attack")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_dattack );
		return 1;
	}
	else if (valname =="power")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_dpower );
		return 1;
	}
	else if (valname =="armor")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_darmor );
		return 1;
	}
	else if (valname =="flag")
	{
		lua_pushstring(EventSystem::getLuaState() , m_flag.c_str() );
		return 1;
	}
	else if (valname =="resist_fire")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_dresistances[Damage::FIRE] );
		return 1;
	}
	else if (valname =="resist_ice")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_dresistances[Damage::ICE] );
		return 1;
	}
	else if (valname =="resist_air")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_dresistances[Damage::AIR] );
		return 1;
	}
	else if (valname =="resist_phys")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_dresistances[Damage::PHYSICAL] );
		return 1;
	}
	else if (valname =="max_resist_ice")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_dresistances_cap[Damage::ICE] );
		return 1;
	}
	else if (valname =="max_resist_air")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_dresistances_cap[Damage::AIR] );
		return 1;
	}
	else if (valname =="max_resist_fire")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_dresistances_cap[Damage::FIRE] );
		return 1;
	}
	else if (valname =="max_resist_phys")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_dresistances_cap[Damage::PHYSICAL] );
		return 1;
	}
	else if (valname =="attack_speed")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_dattack_speed );
		return 1;
	}
	else if (valname =="walk_speed")
	{
		lua_pushinteger(EventSystem::getLuaState() ,m_dwalk_speed );
		return 1;
	}
	
	return 0;
}


bool CreatureBaseAttrMod::setValue(std::string valname)
{
	if (valname =="time")
	{
		m_time = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="strength")
	{
		m_dstrength = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="dexterity")
	{
		m_ddexterity = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="willpower")
	{
		m_dwillpower = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="magic_power")
	{
		m_dmagic_power = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="max_health")
	{
		m_dmax_health = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="block")
	{
		m_dblock = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="attack")
	{
		m_dattack = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="armor")
	{
		m_darmor = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="flag")
	{
		m_flag = lua_tostring(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="resist_fire")
	{
		m_dresistances[Damage::FIRE] = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
		
	}
	else if (valname =="resist_ice")
	{
		m_dresistances[Damage::ICE] = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="resist_air")
	{
		m_dresistances[Damage::AIR] = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="resist_phys")
	{
		m_dresistances[Damage::PHYSICAL] = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="max_resist_ice")
	{
		m_dresistances_cap[Damage::ICE] = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="max_resist_air")
	{
		m_dresistances_cap[Damage::AIR] = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="max_resist_fire")
	{
		m_dresistances_cap[Damage::FIRE] = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="max_resist_phys")
	{
		m_dresistances_cap[Damage::PHYSICAL] = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="attack_speed")
	{
		m_dattack_speed = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="walk_speed")
	{
		m_dwalk_speed = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	
	return false;
}

int CreatureDynAttr::getValue(std::string valname)
{
	if (valname =="health")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_health );
		return 1;
	}
	else if (valname =="experience")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_experience );
		return 1;
	}
	else if (valname =="blind_time")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_status_mod_time[Damage::BLIND]);
		return 1;
		
	}
	else if (valname =="poisoned_time")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_status_mod_time[Damage::POISONED]);
		return 1;
	}
	else if (valname =="berserk_time")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_status_mod_time[Damage::BERSERK]);
		return 1;
	}
	else if (valname =="confused_time")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_status_mod_time[Damage::CONFUSED]);
		return 1;
	}
	else if (valname =="mute_time")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_status_mod_time[Damage::MUTE]);
		return 1;
	}
	else if (valname =="paralyzed_time")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_status_mod_time[Damage::PARALYZED]);
		return 1;
	}
	else if (valname =="frozen_time")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_status_mod_time[Damage::FROZEN]);
		return 1;
	}
	else if (valname =="burning_time")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_status_mod_time[Damage::BURNING]);
		return 1;
	}
	
	return 0;
}

bool CreatureDynAttr::setValue(std::string valname, int& event_mask)
{
	if (valname =="health")
	{
		m_health = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		event_mask |= NetEvent::DATA_HP;
		return true;
	}
	else if (valname =="experience")
	{
		m_experience = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		event_mask |= NetEvent::DATA_EXPERIENCE;
		return true;
	}
	else if (valname =="blind_time")
	{
		m_status_mod_time[Damage::BLIND] = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		event_mask |= NetEvent::DATA_STATUS_MODS;
		return true;
	}
	else if (valname =="poisoned_time")
	{
		m_status_mod_time[Damage::POISONED] = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		event_mask |= NetEvent::DATA_STATUS_MODS;
		return true;
	}
	else if (valname =="berserk_time")
	{
		m_status_mod_time[Damage::BERSERK] = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		event_mask |= NetEvent::DATA_STATUS_MODS;
		return true;
	}
	else if (valname =="confused_time")
	{
		m_status_mod_time[Damage::CONFUSED] = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		event_mask |= NetEvent::DATA_STATUS_MODS;
		return true;
	}
	else if (valname =="mute_time")
	{
		m_status_mod_time[Damage::MUTE] = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		event_mask |= NetEvent::DATA_STATUS_MODS;
		return true;
	}
	else if (valname =="paralyzed_time")
	{
		m_status_mod_time[Damage::PARALYZED] = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		event_mask |= NetEvent::DATA_STATUS_MODS;
		return true;
	}
	else if (valname =="frozen_time")
	{
		m_status_mod_time[Damage::FROZEN] = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		event_mask |= NetEvent::DATA_STATUS_MODS;
		return true;
	}
	else if (valname =="burning_time")
	{
		m_status_mod_time[Damage::BURNING] = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		event_mask |= NetEvent::DATA_STATUS_MODS;
		return true;
	}
	
	return false;
}

void CreatureDynAttr::operator=(CreatureDynAttr other)
{
	m_health = other.m_health;
	m_experience = other.m_experience;
	m_last_attacker_id = other.m_experience;
	for (int i=0; i<NR_EFFECTS; i++)
		m_effect_time[i] = other.m_effect_time[i];
	for (int i=0; i<NR_STATUS_MODS; i++)
	{
		m_status_mod_time[i] = other.m_status_mod_time[i];
		m_status_mod_immune_time[i] = other.m_status_mod_immune_time[i];
	}
	m_temp_mods = other.m_temp_mods;
	m_timer = other.m_timer;
}

void CreatureDynAttrMod::operator=(CreatureDynAttrMod other)
{
	m_dhealth = other.m_dhealth;
	for (int i=0; i< NR_STATUS_MODS; i++)
	{
		m_dstatus_mod_immune_time[i] = other.m_dstatus_mod_immune_time[i];
	}
}

void CreatureDynAttrMod::writeToXML(TiXmlNode* node)
{
	TiXmlElement* elem = node->ToElement();
	if (elem == 0) 
		return;
	
	XMLUtil::setDoubleAttribute(elem, "dhealth",m_dhealth,0);
	XMLUtil::setDoubleAttribute(elem, "dstatus_blind_immune_time",m_dstatus_mod_immune_time[Damage::BLIND],0);
	XMLUtil::setDoubleAttribute(elem, "dstatus_poison_immune_time",m_dstatus_mod_immune_time[Damage::POISONED],0);
	XMLUtil::setDoubleAttribute(elem, "dstatus_berserk_immune_time",m_dstatus_mod_immune_time[Damage::BERSERK],0);
	XMLUtil::setDoubleAttribute(elem, "dstatus_confuse_immune_time",m_dstatus_mod_immune_time[Damage::CONFUSED],0);
	XMLUtil::setDoubleAttribute(elem, "dstatus_mute_immune_time",m_dstatus_mod_immune_time[Damage::MUTE],0);
	XMLUtil::setDoubleAttribute(elem, "dstatus_paralyze_immune_time",m_dstatus_mod_immune_time[Damage::PARALYZED],0);
	XMLUtil::setDoubleAttribute(elem, "dstatus_frozen_immune_time",m_dstatus_mod_immune_time[Damage::FROZEN],0);
	XMLUtil::setDoubleAttribute(elem, "dstatus_burning_immune_time",m_dstatus_mod_immune_time[Damage::BURNING],0);
}

int CreatureDynAttrMod::getValue(std::string valname)
{
	if (valname =="health")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_dhealth );
		return 1;
	}
	else if (valname =="blind_immune_time")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_dstatus_mod_immune_time[Damage::BLIND]);
		return 1;
		
	}
	else if (valname =="poisoned_immune_time")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_dstatus_mod_immune_time[Damage::POISONED]);
		return 1;
	}
	else if (valname =="berserk_immune_time")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_dstatus_mod_immune_time[Damage::BERSERK]);
		return 1;
	}
	else if (valname =="confused_immune_time")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_dstatus_mod_immune_time[Damage::CONFUSED]);
		return 1;
	}
	else if (valname =="mute_immune_time")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_dstatus_mod_immune_time[Damage::MUTE]);
		return 1;
	}
	else if (valname =="paralyzed_immune_time")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_dstatus_mod_immune_time[Damage::PARALYZED]);
		return 1;
	}
	else if (valname =="frozen_immune_time")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_dstatus_mod_immune_time[Damage::FROZEN]);
		return 1;
	}
	else if (valname =="burning_immune_time")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_dstatus_mod_immune_time[Damage::BURNING]);
		return 1;
	}
	
	return 0;
}


bool CreatureDynAttrMod::setValue(std::string valname)
{
	if (valname =="health")
	{
		m_dhealth = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="blind_immune_time")
	{
		m_dstatus_mod_immune_time[Damage::BLIND] = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="poisoned_immune_time")
	{
		m_dstatus_mod_immune_time[Damage::POISONED] = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="berserk_immune_time")
	{
		m_dstatus_mod_immune_time[Damage::BERSERK] = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="confused_immune_time")
	{
		m_dstatus_mod_immune_time[Damage::CONFUSED] = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="mute_immune_time")
	{
		m_dstatus_mod_immune_time[Damage::MUTE] = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="paralyzed_immune_time")
	{
		m_dstatus_mod_immune_time[Damage::PARALYZED] = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="frozen_immune_time")
	{
		m_dstatus_mod_immune_time[Damage::FROZEN] = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="burning_immune_time")
	{
		m_dstatus_mod_immune_time[Damage::BURNING] = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	
	return false;
}

void CreatureSpeakText::operator=(const CreatureSpeakText& other)
{
	m_text = other.m_text;
	m_time = other.m_time;
	m_displayed_time = other.m_displayed_time;
	m_in_dialogue = other.m_in_dialogue;
	m_emotion = other.m_emotion;
	m_script_action = other.m_script_action;
}

bool CreatureSpeakText::empty()
{
	return (m_text == "");
}

void CreatureSpeakText::clear()
{
	m_text = "";
	m_in_dialogue = false;
}

std::string EmotionSet::getEmotionImage(std::string emotion)
{
	std::map<std::string, std::string>::iterator it;
	it = m_emotion_images.find(emotion);
	if (it == m_emotion_images.end())
		return m_default_image;
	
	return it->second;
}

