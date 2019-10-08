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

// #include "item.h"
#include "itemfactory.h"

void WeaponAttr::operator=(WeaponAttr& other)
{
	m_weapon_type = other.m_weapon_type;
	m_damage = other.m_damage;
	m_attack_range = other.m_attack_range;
	m_two_handed = other.m_two_handed;
	m_dattack_speed = other.m_dattack_speed;
}


void  WeaponAttr::writeToXML(TiXmlNode* node)
{
	TiXmlElement* elem = node->ToElement();
	if (elem == 0) 
		return;
	
	XMLUtil::setAttribute(elem, "weapon_type",m_weapon_type);
	
	Damage& dmg = m_damage;
	XMLUtil::setDoubleAttribute(elem, "damage_min_physical",dmg.m_min_damage[Damage::PHYSICAL]);
	XMLUtil::setDoubleAttribute(elem, "damage_min_fire",dmg.m_min_damage[Damage::FIRE]);
	XMLUtil::setDoubleAttribute(elem, "damage_min_ice",dmg.m_min_damage[Damage::ICE]);
	XMLUtil::setDoubleAttribute(elem, "damage_min_air",dmg.m_min_damage[Damage::AIR]);

	XMLUtil::setDoubleAttribute(elem, "damage_max_physical",dmg.m_max_damage[Damage::PHYSICAL]);
	XMLUtil::setDoubleAttribute(elem, "damage_max_fire",dmg.m_max_damage[Damage::FIRE]);
	XMLUtil::setDoubleAttribute(elem, "damage_max_ice",dmg.m_max_damage[Damage::ICE]);
	XMLUtil::setDoubleAttribute(elem, "damage_max_air",dmg.m_max_damage[Damage::AIR]);

	XMLUtil::setDoubleAttribute(elem, "multiplier_physical",dmg.m_multiplier[Damage::PHYSICAL],1.0);
	XMLUtil::setDoubleAttribute(elem, "multiplier_fire",dmg.m_multiplier[Damage::FIRE],1.0);
	XMLUtil::setDoubleAttribute(elem, "multiplier_ice",dmg.m_multiplier[Damage::ICE],1.0);
	XMLUtil::setDoubleAttribute(elem, "multiplier_air",dmg.m_multiplier[Damage::AIR],1.0);

	XMLUtil::setDoubleAttribute(elem, "damage_attack",dmg.m_attack);
	XMLUtil::setDoubleAttribute(elem, "damage_power",dmg.m_power);

	XMLUtil::setAttribute(elem, "damage_blind_power",dmg.m_status_mod_power[Damage::BLIND]);
	XMLUtil::setAttribute(elem, "damage_poison_power",dmg.m_status_mod_power[Damage::POISONED]);
	XMLUtil::setAttribute(elem, "damage_berserk_power",dmg.m_status_mod_power[Damage::BERSERK]);
	XMLUtil::setAttribute(elem, "damage_confuse_power",dmg.m_status_mod_power[Damage::CONFUSED]);
	XMLUtil::setAttribute(elem, "damage_mute_power",dmg.m_status_mod_power[Damage::MUTE]);
	XMLUtil::setAttribute(elem, "damage_paralyze_power",dmg.m_status_mod_power[Damage::PARALYZED]);
	XMLUtil::setAttribute(elem, "damage_frozen_power",dmg.m_status_mod_power[Damage::FROZEN]);
	XMLUtil::setAttribute(elem, "damage_burning_power",dmg.m_status_mod_power[Damage::BURNING]);
	

	XMLUtil::setAttribute(elem, "dattack_speed",m_dattack_speed);
	XMLUtil::setDoubleAttribute(elem, "attack_range",m_attack_range);
	std::string twohanded = m_two_handed ? "yes" : "no";
	XMLUtil::setAttribute(elem, "two_handed",twohanded);
}

int WeaponAttr::getValue(std::string valname)
{
	if (valname =="weapon_type")
	{
		lua_pushstring(EventSystem::getLuaState() , m_weapon_type.c_str() );
		return 1;
	}
	else if (valname == "attack_range")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_attack_range );
		return 1;
	}
	else if (valname == "two_handed")
	{
		lua_pushboolean(EventSystem::getLuaState() , m_two_handed );
		return 1;
	}
	else if (valname == "attack_speed")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_dattack_speed );
		return 1;
	}
	else
	{
		return m_damage.getValue(valname);
	}
	return 0;
}

bool WeaponAttr::setValue(std::string valname)
{
	if (valname =="weapon_type")
	{
		m_weapon_type = lua_tostring(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname == "attack_range")
	{
		m_attack_range = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname == "two_handed")
	{
		m_two_handed = lua_toboolean(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname == "attack_speed")
	{
		m_dattack_speed =(short) lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else
	{
		return m_damage.setValue(valname);
	}
	return false;
}

ItemBasicData::ItemBasicData()
{
	m_useup_effect=0;
	m_equip_effect=0;
	m_weapon_attr=0;
	m_level_req = 0;
	m_char_req = "all";

	for (int i=0;i<NUM_MAGIC_MODS;i++)
	{
		m_modchance[i] =0;
	}
	m_min_enchant =0;
	m_max_enchant =0;

}

ItemBasicData::~ItemBasicData()
{
	if (m_useup_effect!=0)
		delete m_useup_effect;

	if (m_equip_effect!=0)
		delete m_equip_effect;

	if (m_weapon_attr!=0)
		delete m_weapon_attr;

}

void ItemBasicData::operator=(const ItemBasicData& other)
{
	if (other.m_useup_effect!=0)
	{
		m_useup_effect = new CreatureDynAttrMod;
		*m_useup_effect = *(other.m_useup_effect);
	}
	else
	{
		delete m_useup_effect;
		m_useup_effect = 0;
	}
	
	if (other.m_equip_effect!=0)
	{
		m_equip_effect = new CreatureBaseAttrMod;
		*m_equip_effect =  *(other.m_equip_effect);
	}
	else
	{
		delete m_equip_effect;
		m_equip_effect = 0;
	}
	
	if (other.m_weapon_attr!=0)
	{
		m_weapon_attr = new WeaponAttr;
		*m_weapon_attr = *(other.m_weapon_attr);
		m_weapon_attr->m_damage = other.m_weapon_attr->m_damage;
	}
	else
	{	
		delete m_weapon_attr;
		m_weapon_attr = 0;
	}
	
	m_level_req = other.m_level_req;
	m_char_req = other.m_char_req;
	m_subtype = other.m_subtype;
	m_type = other.m_type;
	m_size = other.m_size;
	m_price = other.m_price;
	for (int i=0; i< NUM_MAGIC_MODS; i++)
		m_modchance[i] = other.m_modchance[i];
	m_min_enchant = other.m_min_enchant;
	m_max_enchant = other.m_max_enchant;
	m_enchant_multiplier = other.m_enchant_multiplier;
	m_name = other.m_name;
	
	m_drop_level = other.m_drop_level;
	m_drop_probability = other.m_drop_probability;

}

void ItemBasicData::writeToXML(TiXmlNode* node)
{
	TiXmlElement* elem = node->ToElement();
	if (elem == 0) 
		return;
	
	std::string typestr = "weapon";
	if (m_type == Item::WEAPON) typestr = "weapon";
	else if (m_type == Item::ARMOR) typestr = "armor";
	else if (m_type == Item::HELMET) typestr = "helmet";
	else if (m_type == Item::GLOVES) typestr = "gloves";
	else if (m_type == Item::RING) typestr = "ring";
	else if (m_type == Item::AMULET) typestr = "amulet";
	else if (m_type == Item::SHIELD) typestr = "shield";
	else if (m_type == Item::POTION) typestr = "potion";
	XMLUtil::setAttribute(elem,"type",typestr);
	
	XMLUtil::setAttribute(elem,"subtype",m_subtype);
	XMLUtil::setAttribute(elem,"name",m_name);
	
	std::string sizestr = "small";
	if (m_size == Item::SMALL) sizestr = "small";
	else if (m_size == Item::MEDIUM) sizestr = "medium";
	else if (m_size == Item::BIG) sizestr = "big";
	
	XMLUtil::setAttribute(elem,"size",sizestr);
	
	// point where to insert additional subobjects
	TiXmlElement* insert_point = 0;
	
	// search for the position after potential <Mesh> and <Image> elements
	insert_point = node->FirstChildElement("Mesh");
	if (insert_point == 0)
		insert_point = node->FirstChildElement("RenderInfo");
	
	TiXmlElement* insert_point_tmp;
	insert_point_tmp = XMLUtil::findElementAfter(elem, insert_point, "Image");
	if (insert_point_tmp != 0)
	{
		insert_point = insert_point_tmp;
	}
	
	TiXmlElement inserter("dummy");	// dummy node just for being compied by tinyxml x(
	
	// detect attribute element
	TiXmlElement* attr =  node->FirstChildElement("Attribute");
	if (attr == 0)
	{
		attr = XMLUtil::insertNodeAfter(elem, insert_point, &inserter);
		attr->SetValue("Attribute");
	}
	
	XMLUtil::setAttribute(attr, "level_requirement",m_level_req,0);
	XMLUtil::setAttribute(attr, "character_requirement",m_char_req,"all");
	XMLUtil::setDoubleAttribute(attr, "min_enchant",m_min_enchant,0);
	XMLUtil::setDoubleAttribute(attr, "max_enchant",m_max_enchant,0);
	XMLUtil::setDoubleAttribute(attr, "enchant_multiplier",m_enchant_multiplier,1.0);
	XMLUtil::setAttribute(attr, "price",m_price,0);
	XMLUtil::setDoubleAttribute(attr, "modchance_health_mod",m_modchance[ItemFactory::HEALTH_MOD],0);
	XMLUtil::setDoubleAttribute(attr, "modchance_strength_mod",m_modchance[ItemFactory::STRENGTH_MOD],0);
	XMLUtil::setDoubleAttribute(attr, "modchance_willpower_mod",m_modchance[ItemFactory::WILLPOWER_MOD],0);
	XMLUtil::setDoubleAttribute(attr, "modchance_dexterity_mod",m_modchance[ItemFactory::DEXTERITY_MOD],0);
	XMLUtil::setDoubleAttribute(attr, "modchance_magic_power_mod",m_modchance[ItemFactory::MAGIC_POWER_MOD],0);
	XMLUtil::setDoubleAttribute(attr, "modchance_armor_mod",m_modchance[ItemFactory::ARMOR_MOD],0);
	XMLUtil::setDoubleAttribute(attr, "modchance_block_mod",m_modchance[ItemFactory::BLOCK_MOD],0);
	
	XMLUtil::setDoubleAttribute(attr, "modchance_resist_phys_mod",m_modchance[ItemFactory::RESIST_PHYS_MOD],0);
	XMLUtil::setDoubleAttribute(attr, "modchance_resist_fire_mod",m_modchance[ItemFactory::RESIST_FIRE_MOD],0);
	XMLUtil::setDoubleAttribute(attr, "modchance_resist_ice_mod",m_modchance[ItemFactory::RESIST_ICE_MOD],0);
	XMLUtil::setDoubleAttribute(attr, "modchance_resist_air_mod",m_modchance[ItemFactory::RESIST_AIR_MOD],0);
	
	XMLUtil::setDoubleAttribute(attr, "modchance_damage_phys_mod",m_modchance[ItemFactory::DAMAGE_PHYS_MOD],0);
	XMLUtil::setDoubleAttribute(attr, "modchance_damage_fire_mod",m_modchance[ItemFactory::DAMAGE_FIRE_MOD],0);
	XMLUtil::setDoubleAttribute(attr, "modchance_damage_ice_mod",m_modchance[ItemFactory::DAMAGE_ICE_MOD],0);
	XMLUtil::setDoubleAttribute(attr, "modchance_damage_air_mod",m_modchance[ItemFactory::DAMAGE_AIR_MOD],0);
	
	XMLUtil::setDoubleAttribute(attr, "modchance_damage_mult_phys_mod",m_modchance[ItemFactory::DAMAGE_MULT_PHYS_MOD],0);
	XMLUtil::setDoubleAttribute(attr, "modchance_damage_mult_fire_mod",m_modchance[ItemFactory::DAMAGE_MULT_FIRE_MOD],0);
	XMLUtil::setDoubleAttribute(attr, "modchance_damage_mult_ice_mod",m_modchance[ItemFactory::DAMAGE_MULT_ICE_MOD],0);
	XMLUtil::setDoubleAttribute(attr, "modchance_damage_mult_air_mod",m_modchance[ItemFactory::DAMAGE_MULT_AIR_MOD],0);
	
	XMLUtil::setDoubleAttribute(attr, "modchance_attack_speed_mod",m_modchance[ItemFactory::ATTACK_SPEED_MOD],0);
	XMLUtil::setDoubleAttribute(attr, "modchance_attack_mod",m_modchance[ItemFactory::ATTACK_MOD],0);
	XMLUtil::setDoubleAttribute(attr, "modchance_power_mod",m_modchance[ItemFactory::POWER_MOD],0);
	insert_point = attr;
	
	// detect UseupEffect element
	TiXmlElement* useup =  node->FirstChildElement("UseupEffect");
	if (m_useup_effect != 0)
	{
		if (useup == 0)
		{
			useup = XMLUtil::insertNodeAfter(elem, insert_point, &inserter);
			useup->SetValue("UseupEffect");
		}
		
		// write Useup effects
		m_useup_effect->writeToXML(useup);
		
		insert_point = useup;
	}
	else
	{
		// remove UseupEffect element if present
		if (useup !=0)
		{
			elem->RemoveChild(useup);
		}
	}
	
	// detect UseupEffect element
	TiXmlElement* equip =  node->FirstChildElement("EquipEffect");
	if (m_equip_effect != 0)
	{
		if (equip == 0)
		{
			equip = XMLUtil::insertNodeAfter(elem, insert_point, &inserter);
			equip->SetValue("EquipEffect");
		}
		
		// write Useup effects
		m_equip_effect->writeToXML(equip);
		
		insert_point = equip;
	}
	else
	{
		// remove UseupEffect element if present
		if (equip !=0)
		{
			elem->RemoveChild(equip);
		}
	}
	
	// detect WeaponAttribute element
	TiXmlElement* weapon =  node->FirstChildElement("WeaponAttribute");
	if (m_weapon_attr != 0)
	{
		if (weapon == 0)
		{
			weapon = XMLUtil::insertNodeAfter(elem, insert_point, &inserter);
			weapon->SetValue("WeaponAttribute");
		}
		
		// write Useup effects
		m_weapon_attr->writeToXML(weapon);
		
		insert_point = weapon;
	}
	else
	{
		// remove UseupEffect element if present
		if (weapon !=0)
		{
			elem->RemoveChild(weapon);
		}
	}
	
	// update DropChance element
	TiXmlElement* drop =  node->FirstChildElement("DropChance");
	if (drop == 0)
	{
		drop = XMLUtil::insertNodeAfter(elem, insert_point, &inserter);
		drop->SetValue("DropChance");
	}
	
	XMLUtil::setAttribute(drop, "level",m_drop_level,0);
	XMLUtil::setDoubleAttribute(drop, "probability",m_drop_probability,0);
	
}

Item::Item(int id)
{
	if (id ==0)
	{
		if (World::getWorld() !=0)
		{
			m_id = World::getWorld()->getValidId();
		}
		else
		{
			m_id =0;
		}
	}
	else
	{
		m_id = id;
	}
	m_useup_effect=0;
	m_equip_effect=0;
	m_weapon_attr=0;
	m_level_req = 0;
	m_magic_power =0;
	m_rarity = NORMAL;
	m_char_req = "all";
	m_size = SMALL;
	
	m_magic_mods.reset();
}

Item::Item(ItemBasicData& data, int id)
{
	m_magic_power =0;
	m_rarity = NORMAL;
	
	if (id ==0)
	{
		if (World::getWorld() !=0)
		{
			m_id = World::getWorld()->getValidId();
		}
		else
		{
			m_id =0;
		}
	}
	else
	{
		m_id = id;
	}
	
	m_type = data.m_type;
	m_subtype = data.m_subtype;
	m_size = data.m_size;
	m_price = data.m_price;

	if (data.m_useup_effect)
	{
		DEBUGX("copy useup effect");
		m_useup_effect = new CreatureDynAttrMod;
		*m_useup_effect = *(data.m_useup_effect);
	}
	else
	{
		m_useup_effect =0;
	}

	if (data.m_equip_effect)
	{
		DEBUGX("copy equip effect");
		m_equip_effect = new CreatureBaseAttrMod;
		*m_equip_effect = *(data.m_equip_effect);
	}
	else
	{
		m_equip_effect =0;
	}

	if (data.m_weapon_attr)
	{
		DEBUGX("copy weapon attr");

		m_weapon_attr = new WeaponAttr;
		*m_weapon_attr = *(data.m_weapon_attr);
		m_weapon_attr->m_damage = data.m_weapon_attr->m_damage;

	}
	else
	{
		m_weapon_attr =0;
	}

	m_level_req = data.m_level_req;
	m_char_req = data.m_char_req;
	
	m_magic_mods.reset();
}

Item::~Item()
{
	if (m_useup_effect) delete m_useup_effect;
	if (m_equip_effect) delete m_equip_effect;
	if (m_weapon_attr) delete  m_weapon_attr;
}


std::string Item::getName()
{
	std::ostringstream ret;

	if (m_size == GOLD)
	{
		ret << m_price << " ";
	}
    ret <<  (dgettext("sumwars",(getString()).c_str()));


	return ret.str();
}


std::string Item::getString()
{
	return ItemFactory::getItemName(m_subtype);
}


void Item::toString(CharConv* cv)
{

	cv->toBuffer((char) m_type);
	cv->toBuffer(m_subtype);
	cv->toBuffer(m_id);
	cv->toBuffer(static_cast<char>(m_rarity));
	if (m_type == GOLD_TYPE)
	{
		cv->toBuffer(m_price);
	}

}

void Item::fromString(CharConv* cv)
{
	// Daten werden extern eingelesen

	char tmp;
	cv->fromBuffer(tmp);
	m_rarity = (Rarity) tmp;
	if (m_type == GOLD_TYPE)
	{
		cv->fromBuffer(m_price);
	}
}

int Item::getValue(std::string valname)
{
	if (valname =="type")
	{
		if (m_type ==ARMOR)
			lua_pushstring(EventSystem::getLuaState() , "armor" );
		else if (m_type ==HELMET)
			lua_pushstring(EventSystem::getLuaState() , "helmet" );
		else  if (m_type ==GLOVES)
			lua_pushstring(EventSystem::getLuaState() , "gloves" );
		else  if (m_type ==WEAPON)
			lua_pushstring(EventSystem::getLuaState() , "weapon" );
		else  if (m_type ==SHIELD)
			lua_pushstring(EventSystem::getLuaState() , "shield" );
		else  if (m_type ==POTION)
			lua_pushstring(EventSystem::getLuaState() , "potion" );
		else  if (m_type ==RING)
			lua_pushstring(EventSystem::getLuaState() , "ring" );
		else  if (m_type ==AMULET)
			lua_pushstring(EventSystem::getLuaState() , "amulet" );
		else  if (m_type ==GOLD_TYPE)
			lua_pushstring(EventSystem::getLuaState() , "gold" );
		else 
			lua_pushstring(EventSystem::getLuaState() , "noitem" );
		return 1;
	}
	else if (valname =="subtype")
	{
		lua_pushstring(EventSystem::getLuaState() , m_subtype.c_str() );
		return 1;
	}
	else if (valname == "size")
	{
		if (m_size == BIG)
			lua_pushstring(EventSystem::getLuaState() , "big" );
		else if (m_size == MEDIUM)
			lua_pushstring(EventSystem::getLuaState() , "medium" );
		else if (m_size == SMALL)
			lua_pushstring(EventSystem::getLuaState() , "small" );
		else if (m_size == GOLD)
			lua_pushstring(EventSystem::getLuaState() , "gold" );
		return 1;
	}
	else if (valname == "price" || valname == "value")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_price );
		return 1;
	}
	else if (valname == "magic_power")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_magic_power );
		return 1;
	}
	else if (valname == "char_requirement")
	{
		lua_pushstring(EventSystem::getLuaState() , m_char_req.c_str() );
		return 1;		
	}
	else if (valname == "level_requirement")
	{
		lua_pushnumber(EventSystem::getLuaState() , fabs((float)m_level_req) );
		return 1;
	}
	else if (valname == "rarity")
	{
		if (m_rarity == NORMAL)
			lua_pushstring(EventSystem::getLuaState() , "normal" );
		else if (m_rarity == MAGICAL)
			lua_pushstring(EventSystem::getLuaState() , "magical" );
		else if (m_rarity == RARE)
			lua_pushstring(EventSystem::getLuaState() , "rare" );
		else if (m_rarity == UNIQUE)
			lua_pushstring(EventSystem::getLuaState() , "unique" );
		
		return 1;
	}
	else
	{
		int ret = 0;
		if (m_weapon_attr != 0)
			ret = m_weapon_attr->getValue(valname);
		if (ret != 0)
			return ret;
		
		if (m_useup_effect != 0)
			ret = m_useup_effect->getValue(valname);
		if (ret != 0)
			return ret;
		
		if (m_equip_effect != 0)
			ret = m_equip_effect->getValue(valname);
		if (ret != 0)
			return ret;
		
	}
	return 0;
}

bool Item::setValue(std::string valname)
{
	if (valname =="subtype")
	{
		m_subtype = lua_tostring(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname == "size")
	{
		std::string sizestr = lua_tostring(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		if (sizestr == "big")
			m_size = BIG;
		else if (sizestr == "medium")
			m_size = MEDIUM;
		else if (sizestr == "small")
			m_size = SMALL;
		
		return true;
	}
	else if (valname == "price" || valname == "value")
	{
		m_price = (int) lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname == "magic_power")
	{
		m_magic_power = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname == "char_requirement")
	{
		m_char_req = lua_tostring(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;		
	}
	else if (valname == "level_requirement")
	{
		m_level_req = char(lua_tonumber(EventSystem::getLuaState() ,-1));
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname == "rarity")
	{
		std::string rstr = lua_tostring(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		if (rstr == "normal")
			m_rarity = NORMAL;
		if (rstr == "magical")
			m_rarity = MAGICAL;
		if (rstr == "rare")
			m_rarity = RARE;
		if (rstr == "unique")
			m_rarity = UNIQUE;
		
		return true;
	}
	else
	{
		bool ret = false;
		if (m_weapon_attr != 0)
			ret = m_weapon_attr->setValue(valname);
		if (ret == true)
			return ret;
		
		if (m_useup_effect != 0)
			ret = m_useup_effect->setValue(valname);
		if (ret == true)
			return ret;
		
		// Equip Effekt wird auf Verdacht erzeugt und wieder geloescht, wenn kein Wert geschrieben wurde
		bool created = false;
		if (m_equip_effect != 0)
		{
			m_equip_effect = new CreatureBaseAttrMod;
			created = true;
		}
		ret = m_equip_effect->setValue(valname);
		if (ret == true)
			return ret;
		
		if (created)
		{
			delete m_equip_effect;
			m_equip_effect = 0;
		}
	}
	return false;
}


void Item::toStringComplete(CharConv* cv)
{
	toString(cv);

	if (m_type == GOLD_TYPE)
		return;

	cv->toBuffer(m_price);
	cv->toBuffer(m_level_req);
	cv->toBuffer(m_char_req);

	cv->toBuffer(m_magic_power);

	char mask = NOINFO;

	if (m_useup_effect!=0)
		mask |= USEUP_INFO;

	if (m_equip_effect!=0)
		mask |= EQUIP_INFO;

	if (m_weapon_attr!=0)
		mask |= WEAPON_INFO;

	cv->toBuffer(mask);

	int i;
	if (m_useup_effect!=0)
	{
		DEBUGX("writing useup effect");
		cv->toBuffer(m_useup_effect->m_dhealth);
		for (i=0;i<NR_STATUS_MODS;i++)
		{
			cv->toBuffer(m_useup_effect->m_dstatus_mod_immune_time[i]);
		}
	}

	if (m_equip_effect!=0)
	{
		DEBUGX("writing equip effect");
		cv->toBuffer(m_equip_effect->m_darmor);
		cv->toBuffer(m_equip_effect->m_dblock);
		cv->toBuffer(m_equip_effect->m_dmax_health);
		cv->toBuffer(m_equip_effect->m_dattack);
		cv->toBuffer(m_equip_effect->m_dstrength);
		cv->toBuffer(m_equip_effect->m_ddexterity);
		cv->toBuffer(m_equip_effect->m_dmagic_power);
		cv->toBuffer(m_equip_effect->m_dwillpower);
		for (i=0;i<4;i++)
		{
			cv->toBuffer(m_equip_effect->m_dresistances[i]);
		}

		for (i=0;i<4;i++)
		{
			cv->toBuffer(m_equip_effect->m_dresistances_cap[i]);
		}

		cv->toBuffer(m_equip_effect->m_dwalk_speed);
		cv->toBuffer(m_equip_effect->m_dattack_speed);
		cv->toBuffer(m_equip_effect->m_xspecial_flags);

		cv->toBuffer(static_cast<short>(m_equip_effect->m_xabilities.size()));
		std::set<std::string>::iterator it;
		for (it = m_equip_effect->m_xabilities.begin(); it != m_equip_effect->m_xabilities.end(); ++it)
		{
			cv->toBuffer(*it);
		}
		cv->toBuffer(m_equip_effect->m_ximmunity);

	}

	if (m_weapon_attr!=0)
	{
		DEBUGX("writing weapon attr");
		cv->toBuffer(m_weapon_attr->m_weapon_type);
		m_weapon_attr->m_damage.toString(cv);
		cv->toBuffer(m_weapon_attr->m_attack_range);
		cv->toBuffer(m_weapon_attr->m_two_handed);
		cv->toBuffer(m_weapon_attr->m_dattack_speed);

	}

	if (cv->getVersion() >= 16)
	{
		cv->toBuffer( m_magic_mods.to_string());
	}
}

void Item::fromStringComplete(CharConv* cv)
{
	fromString(cv);

	if (m_type == GOLD_TYPE)
		return;

	cv->fromBuffer(m_price);
	cv->fromBuffer(m_level_req);
	cv->fromBuffer(m_char_req);
	cv->fromBuffer(m_magic_power);

	char mask = NOINFO;
	cv->fromBuffer(mask);

	if (mask & USEUP_INFO)
	{
		if (m_useup_effect !=0)
			delete m_useup_effect;

		m_useup_effect= new CreatureDynAttrMod();
	}

	if (mask & EQUIP_INFO)
	{
		if (m_equip_effect !=0)
			delete m_equip_effect;

		m_equip_effect=new CreatureBaseAttrMod();
	}

	if (mask & WEAPON_INFO)
	{
		if (m_weapon_attr !=0)
			delete m_weapon_attr;

		m_weapon_attr = new WeaponAttr();
	}


	int i;
	if (m_useup_effect!=0)
	{
		DEBUGX("loading useup effect");
		cv->fromBuffer(m_useup_effect->m_dhealth );
		for (i=0;i<NR_STATUS_MODS;i++)
		{
			cv->fromBuffer(m_useup_effect->m_dstatus_mod_immune_time[i]);
		}
	}

	if (m_equip_effect!=0)
	{
		DEBUGX("loading equip effect");
		cv->fromBuffer(m_equip_effect->m_darmor);
		cv->fromBuffer(m_equip_effect->m_dblock);
		cv->fromBuffer(m_equip_effect->m_dmax_health);
		cv->fromBuffer(m_equip_effect->m_dattack);
		cv->fromBuffer(m_equip_effect->m_dstrength);
		cv->fromBuffer(m_equip_effect->m_ddexterity);
		cv->fromBuffer(m_equip_effect->m_dmagic_power);
		cv->fromBuffer(m_equip_effect->m_dwillpower);
		for (i=0;i<4;i++)
		{
			cv->fromBuffer(m_equip_effect->m_dresistances[i]);
		}

		for (i=0;i<4;i++)
		{
			cv->fromBuffer(m_equip_effect->m_dresistances_cap[i]);
		}

		cv->fromBuffer(m_equip_effect->m_dwalk_speed);
		cv->fromBuffer(m_equip_effect->m_dattack_speed);
		cv->fromBuffer(m_equip_effect->m_xspecial_flags );

		short nr;
		cv->fromBuffer(nr);
		Action::ActionType type;
		for (int i=0; i<nr; i++)
		{
			cv->fromBuffer(type);
			m_equip_effect->m_xabilities.insert(type);
		}
		
		cv->fromBuffer(m_equip_effect->m_ximmunity);

	}

	if (m_weapon_attr!=0)
	{
		DEBUGX("loading weapon attr");
		cv->fromBuffer(m_weapon_attr->m_weapon_type);
		m_weapon_attr->m_damage.fromString(cv);
		cv->fromBuffer(m_weapon_attr->m_attack_range);
		cv->fromBuffer(m_weapon_attr->m_two_handed);
		cv->fromBuffer(m_weapon_attr->m_dattack_speed);

	}

	if (cv->getVersion() >= 16)
	{
		std::string magicmods;
		cv->fromBuffer(magicmods);
		m_magic_mods = std::bitset<32>(magicmods);
	}

}

std::string Item::getDescription(float price_factor, ItemRequirementsMet irm)
{
    std::string defaultColor = "[colour='FF2F2F2F']";
    std::string rarityColor;
    switch (m_rarity)
    {
        case MAGICAL:
            rarityColor = "[colour='FF00C000']";
            break;
        case RARE:
            rarityColor = "[colour='FF2573D9']";
            break;
        case UNIQUE:
            rarityColor = "[colour='FFFF0000']";
            break;
        case QUEST:
            rarityColor = "[colour='FFC05600']";
            break;
        default:
            rarityColor = defaultColor;
    }

	// String fuer die Beschreibung
	std::ostringstream out_stream;
	out_stream.str("");
    out_stream<<rarityColor<<getName()<<"\n" << defaultColor;
	int i;
	// Levelbeschraenkung
	out_stream <<gettext("Value")<<": "<<m_price;
	if (price_factor != 0 && price_factor != 1)
	{
		out_stream <<"\n" << gettext("Selling Value")<<": "<<MathHelper::Max(1,int(m_price*price_factor));
	}
	if (m_level_req>0)
	{
        if (irm.m_level)
            out_stream<<"\n" << gettext("Required level")<<": "<<(int) m_level_req;
        else
            out_stream<<"\n" << "[colour='FFFF0000']" << gettext("Required level")<<": "<<(int) m_level_req << "[colour='FF2F2F2F']";
	}
	
	if (m_char_req != "15" && m_char_req != "all")
	{
        if (!irm.m_class)
            out_stream<< "[colour='FFFF0000']";
        
		size_t pos=0,pos2;
		out_stream<<"\n" << gettext("Required class")<<": ";
		
		std::string type;
		bool end = false;
		
		// alle Klassen ermitteln, die das Item verwenden koennen
		//  fuer jedes Teilstueck in dem string werden alle Klassen gesucht, die das entsprechende Tag besitzen
		std::set<std::string> classes;
		std::set<std::string>::iterator ct;
		std::map<GameObject::Subtype, PlayerBasicData*>& pdata = ObjectFactory::getPlayerData();
		std::map<GameObject::Subtype, PlayerBasicData*>::iterator it;
		do
		{
			pos2 = m_char_req.find_first_of(",|",pos);
			if (pos2 == std::string::npos)
			{
				pos2 = m_char_req.length();
				end = true;
			}
			type = m_char_req.substr(pos,pos2-pos);
			
			// Schleife ueber die Spielerklassen
			for (it = pdata.begin(); it != pdata.end(); ++it)
			{
				// Schleife ueber die Tags der Klasse
				std::list<std::string>::iterator jt;
				for (jt = it->second->m_item_req_tags.begin(); jt != it->second->m_item_req_tags.end(); ++jt)
				{
					if (*jt == type)
					{
						classes.insert(it->second->m_name);
					}
				}
				
			}
			pos = pos2+1;
		}
		while (!end);
		
		// Ausgabe schreiben
		bool first = true;
		for (ct = classes.begin(); ct != classes.end(); ++ct)
		{
			if (!first)
				out_stream<<", ";
			out_stream<<gettext(ct->c_str());
			
			first = false;
		}
		out_stream<< defaultColor; 
	}

	// Effekt beim Verbrauchen
	if (m_useup_effect)
	{
		// HP Heilung
		if (m_useup_effect->m_dhealth>0)
		{
			out_stream <<"\n"<< gettext("Heals ")<<(int) m_useup_effect->m_dhealth<<gettext(" hitpoints");
		}

		// Heilen/ Immunisieren gegen Statusmods
		for (i=0;i<8;i++)
		{
			if (	m_useup_effect->m_dstatus_mod_immune_time[i]>0)
			{
				out_stream <<"\n"<< gettext("Heals ")<<Damage::getStatusModName((Damage::StatusMods) i);
				if (m_useup_effect->m_dstatus_mod_immune_time[i]>=1000)
				{
					out_stream <<", "<< gettext("immune for ")<< (int) (m_useup_effect->m_dstatus_mod_immune_time[i]*0.001f)<<"s";
				}
			}
		}

	}

	// Daten einer Waffe
	if (m_weapon_attr)
	{
		if (m_weapon_attr->m_two_handed)
		{
			out_stream <<"\n"<< gettext("Two-handed weapon");
		}
		// Reichweite / Angriffsgeschwindigkeit
		if (m_type == WEAPON)
		{
			out_stream << "\n" << gettext("Range")<<": "<<m_weapon_attr->m_attack_range;
		}

		//out_stream << "\n" << "Angriffe: "<<m_weapon_attr->m_attack_speed*0.001f<<"/s";

		// Schaden
		std::string dmgstring = m_weapon_attr->m_damage.getDamageString(Damage::ITEM);
		if (dmgstring != "")
		{
			if (m_type == WEAPON)
			{
				out_stream << "\n" << gettext("Damage")<<":";
			}
			out_stream<<"\n"<<dmgstring;
		}

	}

	// Effekte von Ausruestungsgegenstaenden
	if (m_equip_effect)
	{
		if (m_equip_effect->m_darmor>0)
		{
			out_stream<<"\n"<<gettext("Armor")<<": "<<m_equip_effect->m_darmor;
		}

		if (m_equip_effect->m_dblock>0)
		{
			out_stream<<"\n"<<gettext("Block")<<": "<<m_equip_effect->m_dblock;
		}

		if (m_equip_effect->m_dmax_health>0)
		{
			out_stream<<"\n"<<"+"<<(int) m_equip_effect->m_dmax_health<< " "<<gettext("max hitpoints");
		}

		if (m_equip_effect->m_dstrength>0)
		{
			out_stream<<"\n"<<"+"<<m_equip_effect->m_dstrength<< " "<<gettext("Strength");
		}

		if (m_equip_effect->m_ddexterity>0)
		{
			out_stream<<"\n"<<"+"<<m_equip_effect->m_ddexterity<< " "<<gettext("Dexterity");
		}

		if (m_equip_effect->m_dmagic_power>0)
		{
			out_stream<<"\n"<<"+"<<m_equip_effect->m_dmagic_power<< " "<<gettext("Magic Power");
		}

		if (m_equip_effect->m_dwillpower>0)
		{
			out_stream<<"\n"<<"+"<<m_equip_effect->m_dwillpower<< " "<<gettext("Willpower");
		}

		for (i=0;i<4;i++)
		{
			if (m_equip_effect->m_dresistances[i]>0)
			{
				out_stream<<"\n"<<"+"<<m_equip_effect->m_dresistances[i]<<" "<<Damage::getDamageResistanceName((Damage::DamageType) i);
			}
		}

		for (i=0;i<4;i++)
		{
			if (m_equip_effect->m_dresistances_cap[i]>0)
			{
				out_stream<<"\n"<<"+"<<m_equip_effect->m_dresistances_cap[i]<<gettext(" max. ")<<Damage::getDamageResistanceName((Damage::DamageType) i);
			}
		}


		// TODO: Angriffsgeschwindigkeit
		// TODO: special Flags
		// TODO: Faehigkeiten
		// TODO: Immunitaeten

	}


	return out_stream.str();
}

std::list<std::string> Item::getDescriptionAsStringList(float price_factor, ItemRequirementsMet irm)
{
	// this mapping is necessary because order of element is differen in item and damage x(
	int elemmap[4] = {0,3,2,1};
	
    std::list<std::string> itemDescList;
    std::string defaultColor = "[colour='FF2F2F2F']";
    std::string rarityColor;
    switch (m_rarity)
    {
        case MAGICAL:
            rarityColor = "[colour='FF00A000']";
            break;
        case RARE:
            rarityColor = "[colour='FF2573D9']";
            break;
        case UNIQUE:
            rarityColor = "[colour='FFFF0000']";
            break;
        case QUEST:
            rarityColor = "[colour='FFC05600']";
            break;
        default:
            rarityColor = defaultColor;
    }
    
    // String fuer die Beschreibung
    std::ostringstream out_stream;
    out_stream.str("");
    out_stream<<rarityColor<<getName() << defaultColor << "\n";
    itemDescList.push_back(out_stream.str());
    out_stream.str("");
    
    int i;
    // Levelbeschraenkung
    out_stream <<gettext("Value")<<": "<<m_price << "\n";
    itemDescList.push_back(out_stream.str());
    out_stream.str("");
    
    if (price_factor != 0 && price_factor != 1)
    {
        out_stream << gettext("Selling Value")<<": "<<MathHelper::Max(1,int(m_price*price_factor)) << "\n";
        itemDescList.push_back(out_stream.str());
        out_stream.str("");
    }
    if (m_level_req>0)
    {
        if (irm.m_level)
        {
            out_stream<<gettext("Required level")<<": "<<(int) m_level_req << "\n";
            itemDescList.push_back(out_stream.str());
            out_stream.str("");
        }
        else
        {
            out_stream<< "[colour='FFFF0000']" << gettext("Required level")<<": "<<(int) m_level_req << "[colour='FF2F2F2F']" << "\n";
            itemDescList.push_back(out_stream.str());
            out_stream.str("");
        }
    }
    
    if (m_char_req != "15" && m_char_req != "all")
    {
        if (!irm.m_class)
        {
            out_stream<< "[colour='FFFF0000']" << "\n";
            itemDescList.push_back(out_stream.str());
            out_stream.str("");
        }
        size_t pos=0,pos2;
        out_stream<< gettext("Required class")<<": ";
        
        std::string type;
        bool end = false;
        
        // alle Klassen ermitteln, die das Item verwenden koennen
        //  fuer jedes Teilstueck in dem string werden alle Klassen gesucht, die das entsprechende Tag besitzen
        std::set<std::string> classes;
        std::set<std::string>::iterator ct;
        std::map<GameObject::Subtype, PlayerBasicData*>& pdata = ObjectFactory::getPlayerData();
        std::map<GameObject::Subtype, PlayerBasicData*>::iterator it;
        do
        {
            pos2 = m_char_req.find_first_of(",|",pos);
            if (pos2 == std::string::npos)
            {
                pos2 = m_char_req.length();
                end = true;
            }
            type = m_char_req.substr(pos,pos2-pos);
            
            // Schleife ueber die Spielerklassen
            for (it = pdata.begin(); it != pdata.end(); ++it)
            {
                // Schleife ueber die Tags der Klasse
                std::list<std::string>::iterator jt;
                for (jt = it->second->m_item_req_tags.begin(); jt != it->second->m_item_req_tags.end(); ++jt)
                {
                    if (*jt == type)
                    {
                        classes.insert(it->second->m_name);
                    }
                }
                
            }
            pos = pos2+1;
        }
        while (!end);
        
        // Ausgabe schreiben
        bool first = true;
        for (ct = classes.begin(); ct != classes.end(); ++ct)
        {
            if (!first)
                out_stream<<", ";
            out_stream<<gettext(ct->c_str());
            
            first = false;
        }
        out_stream<< defaultColor << "\n";
        itemDescList.push_back(out_stream.str());
        out_stream.str("");
    }
    
    // Effekt beim Verbrauchen
    if (m_useup_effect)
    {
        // HP Heilung
        if (m_useup_effect->m_dhealth>0)
        {
            out_stream << gettext("Heals ")<<(int) m_useup_effect->m_dhealth<<gettext(" hitpoints") << "\n";
            itemDescList.push_back(out_stream.str());
            out_stream.str("");
        }
        
        // Heilen/ Immunisieren gegen Statusmods
        for (i=0;i<8;i++)
        {
            if (    m_useup_effect->m_dstatus_mod_immune_time[i]>0)
            {
                out_stream << gettext("Heals ")<<Damage::getStatusModName((Damage::StatusMods) i) << "\n";
                itemDescList.push_back(out_stream.str());
                out_stream.str("");
                
                if (m_useup_effect->m_dstatus_mod_immune_time[i]>=1000)
                {
                    out_stream <<", "<< gettext("immune for ")<< (int) (m_useup_effect->m_dstatus_mod_immune_time[i]*0.001f)<<"s" << "\n";
                    itemDescList.push_back(out_stream.str());
                    out_stream.str("");
                }
            }
        }
        
    }
    
    // Daten einer Waffe
    if (m_weapon_attr)
    {
        if (m_weapon_attr->m_two_handed)
        {
            out_stream << gettext("Two-handed weapon") << "\n";
            itemDescList.push_back(out_stream.str());
            out_stream.str("");
        }
        // Reichweite / Angriffsgeschwindigkeit
        if (m_type == WEAPON)
        {
            out_stream << gettext("Range")<<": "<<m_weapon_attr->m_attack_range << "\n";
            itemDescList.push_back(out_stream.str());
            out_stream.str("");
        }
        
        //out_stream << "Angriffe: "<<m_weapon_attr->m_attack_speed*0.001f<<"/s";
        
        // Schaden
        std::string dmgstring = m_weapon_attr->m_damage.getDamageString(Damage::ITEM,rarityColor,&m_magic_mods);
        if (dmgstring != "")
        {
            if (m_type == WEAPON)
            {
                out_stream << gettext("Damage")<<":" << "\n";
                itemDescList.push_back(out_stream.str());
                out_stream.str("");
            }
            
            // split damage string into lines
            std::string::size_type pos = dmgstring.find("\n");
            while(pos != std::string::npos)
            {
                out_stream << dmgstring.substr(0, pos+1);
                itemDescList.push_back(out_stream.str());
                out_stream.str("");
                dmgstring.erase(0,pos+1);
                pos = dmgstring.find("\n");
            }
            out_stream << dmgstring << "\n";
            itemDescList.push_back(out_stream.str());
            out_stream.str("");
        }
    }
    
    // Effekte von Ausruestungsgegenstaenden
    if (m_equip_effect)
    {
        if (m_equip_effect->m_darmor>0)
        {
			if (m_magic_mods.test(ItemFactory::ARMOR_MOD)) 
				out_stream << rarityColor;
            out_stream<< gettext("Armor")<<": "<<m_equip_effect->m_darmor ;
			if (m_magic_mods.test(ItemFactory::ARMOR_MOD)) 
				out_stream << defaultColor;
			out_stream << "\n";
            itemDescList.push_back(out_stream.str());
            out_stream.str("");
        }
        
        if (m_equip_effect->m_dblock>0)
        {
			if (m_magic_mods.test(ItemFactory::BLOCK_MOD)) 
				out_stream << rarityColor;
            out_stream<< gettext("Block")<<": "<<m_equip_effect->m_dblock;
			if (m_magic_mods.test(ItemFactory::BLOCK_MOD)) 
				out_stream << defaultColor;
			out_stream << "\n";
            itemDescList.push_back(out_stream.str());
            out_stream.str("");
        }
        
        if (m_equip_effect->m_dmax_health>0)
        {
			if (m_magic_mods.test(ItemFactory::HEALTH_MOD)) 
				out_stream << rarityColor;
            out_stream<< "+" <<(int) m_equip_effect->m_dmax_health<< " "<<gettext("max hitpoints");
			if (m_magic_mods.test(ItemFactory::HEALTH_MOD)) 
				out_stream << defaultColor;
			out_stream << "\n";
            itemDescList.push_back(out_stream.str());
            out_stream.str("");
        }
        
        if (m_equip_effect->m_dstrength>0)
        {
			if (m_magic_mods.test(ItemFactory::STRENGTH_MOD)) 
				out_stream << rarityColor;
            out_stream<< "+"<<m_equip_effect->m_dstrength<< " "<<gettext("Strength");
			if (m_magic_mods.test(ItemFactory::STRENGTH_MOD)) 
				out_stream << defaultColor;
			out_stream << "\n";
            itemDescList.push_back(out_stream.str());
            out_stream.str("");
        }
        
        if (m_equip_effect->m_ddexterity>0)
        {
			if (m_magic_mods.test(ItemFactory::DEXTERITY_MOD)) 
				out_stream << rarityColor;
            out_stream<<"+"<<m_equip_effect->m_ddexterity<< " "<<gettext("Dexterity");
			if (m_magic_mods.test(ItemFactory::DEXTERITY_MOD)) 
				out_stream << defaultColor;
			out_stream << "\n";
            itemDescList.push_back(out_stream.str());
            out_stream.str("");
        }
        
        if (m_equip_effect->m_dmagic_power>0)
        {
			if (m_magic_mods.test(ItemFactory::MAGIC_POWER_MOD)) 
				out_stream << rarityColor;
            out_stream<< "+"<<m_equip_effect->m_dmagic_power<< " "<<gettext("Magic Power");
			if (m_magic_mods.test(ItemFactory::MAGIC_POWER_MOD)) 
				out_stream << defaultColor;
			out_stream << "\n";
            itemDescList.push_back(out_stream.str());
            out_stream.str("");
        }
        
        if (m_equip_effect->m_dwillpower>0)
        {
			if (m_magic_mods.test(ItemFactory::WILLPOWER_MOD)) 
				out_stream << rarityColor;
            out_stream<< "+"<<m_equip_effect->m_dwillpower<< " "<<gettext("Willpower");
			if (m_magic_mods.test(ItemFactory::WILLPOWER_MOD)) 
				out_stream << defaultColor;
			out_stream << "\n";
            itemDescList.push_back(out_stream.str());
            out_stream.str("");
        }
        
        for (i=0;i<4;i++)
        {
            if (m_equip_effect->m_dresistances[i]>0)
            {
				if (m_magic_mods.test(ItemFactory::RESIST_PHYS_MOD + elemmap[i])) 
					out_stream << rarityColor;
				
                out_stream<< "+"<<m_equip_effect->m_dresistances[i]<<" "<<Damage::getDamageResistanceName((Damage::DamageType) i);
				
				if (m_magic_mods.test(ItemFactory::RESIST_PHYS_MOD+elemmap[i])) 
					out_stream << defaultColor;
				out_stream << "\n";
                itemDescList.push_back(out_stream.str());
                out_stream.str("");
            }
        }
        
        for (i=0;i<4;i++)
        {
            if (m_equip_effect->m_dresistances_cap[i]>0)
            {
				out_stream << rarityColor;
				
                out_stream<< "+"<<m_equip_effect->m_dresistances_cap[i]<<gettext(" max. ")<<Damage::getDamageResistanceName((Damage::DamageType) i);
				
				out_stream << defaultColor;
				out_stream << "\n";
				
                itemDescList.push_back(out_stream.str());
                out_stream.str("");
            }
        }
        
        
        // TODO: Angriffsgeschwindigkeit
        // TODO: special Flags
        // TODO: Faehigkeiten
        // TODO: Immunitaeten
        
    }
    
/*    // Strip away all new lines at the end of the list
    size_t pos = itemDescList.back().find_first_of("\n");
    while(pos != std::string::npos)
	{
		std::string temp = itemDescList.back();
		temp = temp.erase(pos, pos+1);
		itemDescList.remove(itemDescList.back());
		itemDescList.push_back(temp);
		pos = itemDescList.back().find_first_of("\n");
		
	}
*/

    return itemDescList;
}

void Item::calcPrice()
{
	if (m_useup_effect !=0)
	{
		// Trank, beim generieren erzeugten wert nutzen
		return;
	}

	// Nutzwert des Gegenstandes
	float value =0;
	// Faktor fuer den wert
	float mult =1;

	int i;
	if (m_weapon_attr !=0)
	{
		// Wert des Schadens;
		float dvalue=0;
		
		// Schaden der Waffe
		Damage & dmg = m_weapon_attr->m_damage;
		// phys Schaden
		dvalue += 0.5*(dmg.m_min_damage[0] + dmg.m_max_damage[0])*dmg.m_multiplier[0] * (1+dmg.m_crit_perc*2);
		if (dmg.m_multiplier[0] >1)
		{
			dvalue += (dmg.m_multiplier[0]-1)*100;
		}
		// elementar Schaden
		for (i=1;i<4;i++)
		{
			dvalue += 0.5*(dmg.m_min_damage[i] + dmg.m_max_damage[i])*dmg.m_multiplier[i];
			if (dmg.m_multiplier[i] >1)
			{
				dvalue += (dmg.m_multiplier[i]-1)*100;
			}
		}
		dvalue *= (2000 + m_weapon_attr->m_dattack_speed)/1000.0;

		dvalue += dmg.m_attack*0.1;
		dvalue += dmg.m_power*0.1;
		for (i=0;i<8;i++)
		{
			dvalue += dmg.m_status_mod_power[i]*0.2;
		}
		// TODO: Flags einberechnen
		value += dvalue * MathHelper::Max(1.0f,MathHelper::Min(1.5f,sqrt(m_weapon_attr->m_attack_range)));
		
	}

	if (m_equip_effect!=0)
	{
		// Modifikation beim anlegen
		CreatureBaseAttrMod* cbasm = m_equip_effect;

		value += cbasm->m_darmor;
		value += cbasm->m_dblock;
		value += cbasm->m_dattack*0.1;
		value += cbasm->m_dmax_health*0.2;
		value += cbasm->m_dstrength;
		value += cbasm->m_ddexterity;
		value += cbasm->m_dwillpower;
		value += cbasm->m_dmagic_power;
		mult *= (1+cbasm->m_dattack_speed/2000.0);

		for (i=0;i<4;i++)
		{
			value += cbasm->m_dresistances[i]*0.6;
		}

		for (i=0;i<4;i++)
		{
			value += cbasm->m_dresistances_cap[i];
		}


		// TODO: Special Flags einberechnen

		// TODO: Immunitaeten einberechnen

		// TODO: Skills mit einberechnen

	}
	
	value = (2+0.5*(1+0.1*value)*value)* mult;

	value = MathHelper::Min (value,100000.0f);
	m_price += (int) value;
}




