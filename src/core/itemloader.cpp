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



#include "graphicmanager.h"
#include "itemloader.h"
#include "itemwindow.h"
#include "elementattrib.h"


bool ItemLoader::loadItemData(const char* pFilename)
{
	TiXmlDocument doc(pFilename);
	bool loadOkay = doc.LoadFile();

	if (loadOkay)
	{
		loadItems(&doc);
		return true;
	}
	else
	{
		ERRORMSG("Failed to load file %s", pFilename);
		return false;
	}
}

void ItemLoader::loadItems(TiXmlNode* node)
{
	TiXmlNode* child;
	if (node->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(node->Value(), "Item"))
	{
		loadItem(node);
	}
	else
	{
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			loadItems(child);
		}
	}
}

std::string ItemLoader::loadItem(TiXmlNode* node,bool silent_replace)
{
	ItemBasicData* item_data = new ItemBasicData;
	
	ElementAttrib attr;
	attr.parseElement(node->ToElement());
	
	std::string type ("");
	attr.getString("type", type);
	if (type == "armor")
		item_data->m_type = Item::ARMOR;
	else if (type == "weapon")
		item_data->m_type = Item::WEAPON;
	else if (type == "helmet")
		item_data->m_type = Item::HELMET;
	else if (type == "gloves")
		item_data->m_type = Item::GLOVES;
	else if (type == "ring")
		item_data->m_type = Item::RING;
	else if (type == "amulet")
		item_data->m_type = Item::AMULET;
	else if (type == "shield")
		item_data->m_type = Item::SHIELD;
	else if (type == "potion")
		item_data->m_type = Item::POTION;
	else
	{
		WARNING("<Item> tag without valid type attribute found (type is %s).",type.c_str());
	}
	
	attr.getString("subtype", item_data->m_subtype,"notype");
	if (item_data->m_subtype == "notype")
	{
		WARNING("<Item> tag without subtype attribute found (type is %s).",type.c_str());
	}
	
	attr.getString("name", item_data->m_name);
	if (item_data->m_name == "")
	{
		WARNING("<Item> tag without name attribute found (subtype is %s).",item_data->m_subtype.c_str());
	}
	
	
	std::string size ("");
	attr.getString("size", size);
	if (size == "small")
		item_data->m_size = Item::SMALL;
	else if (size == "medium")
		item_data->m_size = Item::MEDIUM;
	else if (size == "big")
		item_data->m_size = Item::BIG;
	else
	{
		if (item_data->m_name == "")
		{
			WARNING("<Item> tag without valid size attribute found (subtype is %s).",item_data->m_subtype.c_str());
		}
	}
	
					
	std::string consumable ("");
	attr.getString("consumable",consumable ,"false");
	item_data->m_consumable = (consumable == "true");
	attr.getFloat("consume_timer",item_data->m_consume_timer,0);
	attr.getInt("consume_timer_nr",item_data->m_consume_timer_nr,0);
				
	TiXmlNode* child;
	for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
	{
		if (child->Type()==TiXmlNode::TINYXML_ELEMENT) 
		{
			attr.parseElement(child->ToElement());
			
			if (!strcmp(child->Value(), "Image"))
			{
				std::string image ("");
				attr.getString("image", image);
				if (item_data->m_subtype != "")
				{
					DEBUGX("registered Item image %s %s",item_data->m_subtype.c_str(),image.c_str());
					ItemWindow::registerItemImage(item_data->m_subtype, image);
				}
			}
			else if (!strcmp(child->Value(), "Mesh"))
			{
				std::string file ("");
				attr.getString("file", file);
				if (item_data->m_subtype != "")
				{
					DEBUGX("registered Item mesh %s %s",item_data->m_subtype.c_str(),file.c_str());
					GraphicManager::registerGraphicMapping(item_data->m_subtype,file);
					
				}
			}
			else if (!strcmp(child->Value(), "RenderInfo"))
			{
				std::string file ("");
				attr.getString("name", file);
				if (item_data->m_subtype != "")
				{
					DEBUGX("registered Item mesh %s %s",item_data->m_subtype.c_str(),file.c_str());
					GraphicManager::registerGraphicMapping(item_data->m_subtype,file);
					
				}
			}
			else if (!strcmp(child->Value(), "Attribute"))
			{
				short levelreq =0;
				attr.getShort("level_requirement", levelreq,0);
				item_data ->m_level_req = levelreq;
				
				attr.getString("character_requirement",item_data->m_char_req ,"all");
						
				attr.getFloat("min_enchant",item_data->m_min_enchant,0);
				attr.getFloat("max_enchant",item_data->m_max_enchant,0);
				attr.getFloat("enchant_multiplier",item_data->m_enchant_multiplier,1.0);
				attr.getInt("price",item_data->m_price);
				
				attr.getFloat("modchance_health_mod",item_data->m_modchance[ItemFactory::HEALTH_MOD],0);
				attr.getFloat("modchance_strength_mod",item_data->m_modchance[ItemFactory::STRENGTH_MOD],0);
				attr.getFloat("modchance_willpower_mod",item_data->m_modchance[ItemFactory::WILLPOWER_MOD],0);
				attr.getFloat("modchance_dexterity_mod",item_data->m_modchance[ItemFactory::DEXTERITY_MOD],0);
				attr.getFloat("modchance_magic_power_mod",item_data->m_modchance[ItemFactory::MAGIC_POWER_MOD],0);
				attr.getFloat("modchance_armor_mod",item_data->m_modchance[ItemFactory::ARMOR_MOD],0);
				attr.getFloat("modchance_block_mod",item_data->m_modchance[ItemFactory::BLOCK_MOD],0);
				
				attr.getFloat("modchance_resist_phys_mod",item_data->m_modchance[ItemFactory::RESIST_PHYS_MOD],0);
				attr.getFloat("modchance_resist_fire_mod",item_data->m_modchance[ItemFactory::RESIST_FIRE_MOD],0);
				attr.getFloat("modchance_resist_ice_mod",item_data->m_modchance[ItemFactory::RESIST_ICE_MOD],0);
				attr.getFloat("modchance_resist_air_mod",item_data->m_modchance[ItemFactory::RESIST_AIR_MOD],0);
				
				attr.getFloat("modchance_damage_phys_mod",item_data->m_modchance[ItemFactory::DAMAGE_PHYS_MOD],0);
				attr.getFloat("modchance_damage_fire_mod",item_data->m_modchance[ItemFactory::DAMAGE_FIRE_MOD],0);
				attr.getFloat("modchance_damage_ice_mod",item_data->m_modchance[ItemFactory::DAMAGE_ICE_MOD],0);
				attr.getFloat("modchance_damage_air_mod",item_data->m_modchance[ItemFactory::DAMAGE_AIR_MOD],0);
				
				attr.getFloat("modchance_damage_mult_phys_mod",item_data->m_modchance[ItemFactory::DAMAGE_MULT_PHYS_MOD],0);
				attr.getFloat("modchance_damage_mult_fire_mod",item_data->m_modchance[ItemFactory::DAMAGE_MULT_FIRE_MOD],0);
				attr.getFloat("modchance_damage_mult_ice_mod",item_data->m_modchance[ItemFactory::DAMAGE_MULT_ICE_MOD],0);
				attr.getFloat("modchance_damage_mult_air_mod",item_data->m_modchance[ItemFactory::DAMAGE_MULT_AIR_MOD],0);
				
				attr.getFloat("modchance_attack_speed_mod",item_data->m_modchance[ItemFactory::ATTACK_SPEED_MOD],0);
				attr.getFloat("modchance_attack_mod",item_data->m_modchance[ItemFactory::ATTACK_MOD],0);
				attr.getFloat("modchance_power_mod",item_data->m_modchance[ItemFactory::POWER_MOD],0);

			}
			else if (!strcmp(child->Value(), "DropChance"))
			{
				
				int level;
				float prob;
				attr.getInt("level",level,0);
				attr.getFloat("probability",prob,0);
				item_data->m_drop_level = level;
				item_data->m_drop_probability = prob;
				ItemFactory::registerItemDrop(item_data->m_subtype, DropChance(level,prob,item_data->m_size));
			}
			else if (!strcmp(child->Value(), "UseupEffect"))
			{
				if (item_data->m_useup_effect ==0)
					item_data->m_useup_effect  = new CreatureDynAttrMod;
				
				attr.getFloat("dhealth", item_data->m_useup_effect->m_dhealth);
				attr.getFloat("dstatus_blind_immune_time", item_data->m_useup_effect->m_dstatus_mod_immune_time[Damage::BLIND]);
				attr.getFloat("dstatus_poison_immune_time", item_data->m_useup_effect->m_dstatus_mod_immune_time[Damage::POISONED]);
				attr.getFloat("dstatus_berserk_immune_time", item_data->m_useup_effect->m_dstatus_mod_immune_time[Damage::BERSERK]);
				attr.getFloat("dstatus_confuse_immune_time", item_data->m_useup_effect->m_dstatus_mod_immune_time[Damage::CONFUSED]);
				attr.getFloat("dstatus_mute_immune_time", item_data->m_useup_effect->m_dstatus_mod_immune_time[Damage::MUTE]);
				attr.getFloat("dstatus_paralyze_immune_time", item_data->m_useup_effect->m_dstatus_mod_immune_time[Damage::PARALYZED]);
				attr.getFloat("dstatus_frozen_immune_time", item_data->m_useup_effect->m_dstatus_mod_immune_time[Damage::FROZEN]);
				attr.getFloat("dstatus_burning_immune_time", item_data->m_useup_effect->m_dstatus_mod_immune_time[Damage::BURNING]);
				
			}
			else if (!strcmp(child->Value(), "EquipEffect"))
			{
				if (item_data->m_equip_effect == 0)
					item_data->m_equip_effect = new CreatureBaseAttrMod;
				
				attr.getShort("darmor",item_data->m_equip_effect->m_darmor);
				attr.getShort("dblock",item_data->m_equip_effect->m_dblock);
				attr.getFloat("dmax_health",item_data->m_equip_effect->m_dmax_health);
				attr.getShort("dattack",item_data->m_equip_effect->m_dattack);
				attr.getShort("dpower",item_data->m_equip_effect->m_dpower);
				attr.getShort("dstrength",item_data->m_equip_effect->m_dstrength);
				attr.getShort("ddexterity",item_data->m_equip_effect->m_ddexterity);
				attr.getShort("dwillpower",item_data->m_equip_effect->m_dwillpower);
				attr.getShort("dmagic_power",item_data->m_equip_effect->m_dmagic_power);
				attr.getString("flag",item_data->m_equip_effect->m_flag);
				
				attr.getShort("dwalk_speed",item_data->m_equip_effect->m_dwalk_speed);
				attr.getShort("dattack_speed",item_data->m_equip_effect->m_dattack_speed);
				
				attr.getShort("dresistances_physical",item_data->m_equip_effect->m_dresistances[Damage::PHYSICAL]);
				attr.getShort("dresistances_fire",item_data->m_equip_effect->m_dresistances[Damage::FIRE]);
				attr.getShort("dresistances_ice",item_data->m_equip_effect->m_dresistances[Damage::ICE]);
				attr.getShort("dresistances_air",item_data->m_equip_effect->m_dresistances[Damage::AIR]);
				
				attr.getShort("dresistances_cap_physical",item_data->m_equip_effect->m_dresistances_cap[Damage::PHYSICAL]);
				attr.getShort("dresistances_cap_fire",item_data->m_equip_effect->m_dresistances_cap[Damage::FIRE]);
				attr.getShort("dresistances_cap_ice",item_data->m_equip_effect->m_dresistances_cap[Damage::ICE]);
				attr.getShort("dresistances_cap_air",item_data->m_equip_effect->m_dresistances_cap[Damage::AIR]);
			}
			else if (!strcmp(child->Value(), "SpecialFlag"))
			{
				if (item_data->m_weapon_attr == 0)
					item_data->m_weapon_attr = new WeaponAttr;
				
				std::string type ("");
				attr.getString("type",type);

			}
			else if (!strcmp(child->Value(), "Immunity"))
			{
				if (item_data->m_equip_effect == 0)
					item_data->m_equip_effect = new CreatureBaseAttrMod;
				
				std::string type ("");
				attr.getString("type",type);
				
				if (type == "blind")
					item_data->m_equip_effect->m_ximmunity |= 1<<Damage::BLIND;
				else if (type ==  "poisoned")
					item_data->m_equip_effect->m_ximmunity |= 1<<Damage::POISONED;
				else if (type ==  "berserk")
					item_data->m_equip_effect->m_ximmunity |= 1<<Damage::BERSERK;
				else if (type ==  "confused")
					item_data->m_equip_effect->m_ximmunity |= 1<<Damage::CONFUSED;
				else if (type ==  "mute")
					item_data->m_equip_effect->m_ximmunity |= 1<<Damage::MUTE;
				else if (type ==  "paralyzed")
					item_data->m_equip_effect->m_ximmunity |= 1<<Damage::PARALYZED;
				else if (type ==  "frozen")
					item_data->m_equip_effect->m_ximmunity |= 1<<Damage::FROZEN;
				else if (type ==  "burning")
					item_data->m_equip_effect->m_ximmunity |= 1<<Damage::BURNING;
			}
			else if (!strcmp(child->Value(), "Ability"))
			{
				if (item_data->m_equip_effect == 0)
					item_data->m_equip_effect = new CreatureBaseAttrMod;
				
				std::string type ("");
				attr.getString("type",type);
				item_data->m_equip_effect->m_xabilities.insert(type);
			}
			else if (!strcmp(child->Value(), "WeaponAttribute"))
			{
				if (item_data->m_weapon_attr == 0)
					item_data->m_weapon_attr = new WeaponAttr;
				
				attr.getString("weapon_type",item_data->m_weapon_attr->m_weapon_type,"notype");
				
				Damage& dmg = item_data->m_weapon_attr->m_damage;
				attr.getFloat("damage_min_physical",dmg.m_min_damage[Damage::PHYSICAL]);
				attr.getFloat("damage_min_fire",dmg.m_min_damage[Damage::FIRE]);
				attr.getFloat("damage_min_ice",dmg.m_min_damage[Damage::ICE]);
				attr.getFloat("damage_min_air",dmg.m_min_damage[Damage::AIR]);
				
				attr.getFloat("damage_max_physical",dmg.m_max_damage[Damage::PHYSICAL]);
				attr.getFloat("damage_max_fire",dmg.m_max_damage[Damage::FIRE]);
				attr.getFloat("damage_max_ice",dmg.m_max_damage[Damage::ICE]);
				attr.getFloat("damage_max_air",dmg.m_max_damage[Damage::AIR]);
				
				attr.getFloat("multiplier_physical",dmg.m_multiplier[Damage::PHYSICAL],1);
				attr.getFloat("multiplier_fire",dmg.m_multiplier[Damage::FIRE],1);
				attr.getFloat("multiplier_ice",dmg.m_multiplier[Damage::ICE],1);
				attr.getFloat("multiplier_air",dmg.m_multiplier[Damage::AIR],1);
				
				attr.getFloat("damage_attack",dmg.m_attack);
				attr.getFloat("damage_power",dmg.m_power);
				
				attr.getShort("damage_blind_power", dmg.m_status_mod_power[Damage::BLIND]);
				attr.getShort("damage_poison_power", dmg.m_status_mod_power[Damage::POISONED]);
				attr.getShort("damage_berserk_power", dmg.m_status_mod_power[Damage::BERSERK]);
				attr.getShort("damage_confuse_power", dmg.m_status_mod_power[Damage::CONFUSED]);
				attr.getShort("damage_mute_power", dmg.m_status_mod_power[Damage::MUTE]);
				attr.getShort("damage_paralyze_power", dmg.m_status_mod_power[Damage::PARALYZED]);
				attr.getShort("damage_frozen_power", dmg.m_status_mod_power[Damage::FROZEN]);
				attr.getShort("damage_burning_power", dmg.m_status_mod_power[Damage::BURNING]);
				
				attr.getShort("dattack_speed", item_data->m_weapon_attr->m_dattack_speed);
				std::string twohanded ("");
				attr.getString("two_handed",twohanded);
				item_data->m_weapon_attr->m_two_handed = (twohanded == "yes");
				attr.getFloat("attack_range", item_data->m_weapon_attr->m_attack_range);
					
			}
			else if (child->Type()!=TiXmlNode::TINYXML_COMMENT)
			{
				SW_DEBUG("unexpected element of <Item>: %s",child->Value());
			}
		}
	}
	
	DEBUGX("registered Item %s",item_data->m_subtype.c_str());
	ItemFactory::registerItem(item_data->m_type, item_data->m_subtype, item_data, silent_replace);
	
	return item_data->m_subtype;
}

