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

#include "itemfactory.h"


#include <iostream>

std::map<Item::Subtype, DropChance> ItemFactory::m_item_probabilities;

std::map<Item::Subtype, Item::Type> ItemFactory::m_item_types;

std::map<Item::Subtype,ItemBasicData*> ItemFactory::m_item_data;


ItemFactory::ItemFactory()
{
}

ItemBasicData* ItemFactory::getItemBasicData(Item::Subtype subtype)
{
	std::map<Item::Subtype,ItemBasicData*>::iterator it;
	it = m_item_data.find(subtype);
	if (it != m_item_data.end())
		return it->second;
	
	return 0;
}


Item* ItemFactory::createItem(Item::Type type, Item::Subtype subtype, int id, float magic_power, Item::Rarity rarity)
{
	
	if (type == Item::NOITEM)
		return 0;
	
	DEBUGX("creating item %i / %s",type, subtype.c_str());
	
	if (type == Item::GOLD_TYPE)
	{
		return createGold(0,id);
	}
	
	std::map<Item::Subtype,ItemBasicData*>::iterator it;
	it = m_item_data.find(subtype);
	Item * item;

	if (it != m_item_data.end())
	{
		ItemBasicData* idata = it->second;
		item = new Item(*idata,id);
		item->m_rarity = rarity;
		
		item->m_magic_power =0;
		createMagicMods(item,idata->m_modchance,magic_power, idata->m_min_enchant, idata->m_max_enchant, idata->m_enchant_multiplier);

		// Preis ausrechnen
		item->calcPrice();

		return item;
	}
	else
	{
		WARNING("Item type %s unknown",subtype.c_str());
		item = new Item();
		item->m_id = id;
		item->m_type = type;
		item->m_subtype = subtype;
		
		return item;
	}
	return 0;

}

Item* ItemFactory::createGold(int value, int id)
{
	Item* item = new Item;
	if (id ==0)
	{
		id = World::getWorld()->getValidId();
	}
	
	item->m_size = Item::GOLD;
	item->m_type = Item::GOLD_TYPE;
	item->m_id = id;
	item->m_subtype = "gold";
	item->m_price = value;
	
	return item;
}


int ItemFactory::createMagicMods(Item* item, float* modchance, float magic_power, float min_enchant, float max_enchant, float enchant_multiplier, bool duplicate_enchant, int max_enchant_number)
{
	DEBUGX("magic power %f min_enchant %f",magic_power, min_enchant);
	if (magic_power < min_enchant && magic_power != 0)
	{
		if (item->m_type == Item::RING || item->m_type == Item::AMULET)
		{
			magic_power = min_enchant+1;
		}
		else
		{
			return 0;
		}
	}
	
	// Modifikationen des Items auswuerfeln
	DEBUGX("mods auswuerfeln");
	
	// bisher zugeteilte Staerke der Modifikation
	float mod_power[NUM_MAGIC_MODS];
	float modprob[NUM_MAGIC_MODS];
	for (int i=0; i<NUM_MAGIC_MODS; i++)
	{
		if (duplicate_enchant || !item->m_magic_mods.test(i))
		{
			modprob[i] = modchance[i];
		}
		else
		{
			modprob[i] = 0;
		}
	}
	
	memset(mod_power,0,NUM_MAGIC_MODS*sizeof(float));

	float sum =0;
	int i;
	for (i=0;i<NUM_MAGIC_MODS;i++)
	{
		sum += modprob[i];
	}
	
	// Staerke der aktuellen Verzauberung
	float mp, mpbase;
	float sqrtmp;
	float logmp;

	float dmgdiff,dmgavg;

		// durch Verzauberungen benoetigtes Level
	int levelreq =0;

		// aktuelle Verzauberung
	int mod;

		// Anzahl verschiedene Verzauberungen
	int num_mods=0;
	while (magic_power>=min_enchant && num_mods<max_enchant_number)
	{
		
		if (sum < 0.001)
			break;
		
		// Staerke auswuerfeln
		mp = Random::randrangef(min_enchant,max_enchant);
		mp = MathHelper::Min(mp, magic_power);
		item->m_magic_power += mp;
		mpbase = mp;
		
		magic_power -= mp;
		
		mp *= enchant_multiplier;
		sqrtmp = sqrt(mp);
		logmp = log(mp);
		
		
		// Modifikation auswuerfeln
		mod = Random::randDiscrete(modprob,NUM_MAGIC_MODS,sum);
		DEBUGX("ausgewuerfelt: Starke der Verzauberung: %f",mp);
		DEBUGX("Art der Verzauberung: %i",mod);

		num_mods++;

		levelreq = MathHelper::Max(levelreq,(int) (mpbase*0.09-5));
		levelreq = MathHelper::Min(80,levelreq);

		dmgavg = mp*0.06;
		
		// bei Ringen und Amuletten koennen die Strukturen noch fehlen
		if (mod <=RESIST_ALL_MOD && item->m_equip_effect==0)
		{
			item->m_equip_effect = new CreatureBaseAttrMod;
		}
		else if (mod > RESIST_ALL_MOD && item->m_weapon_attr ==0)
		{
			item->m_weapon_attr = new WeaponAttr;
			item->m_weapon_attr->m_weapon_type = "notype";
		}

			// Wirkung der Modifikation
		switch (mod)
		{
			case HEALTH_MOD:
				item->m_equip_effect->m_dmax_health += ceil(mp*0.15);
				break;

			case ARMOR_MOD:
				item->m_equip_effect->m_darmor += (short) ceil(mp*0.08);
				break;

			case BLOCK_MOD:
				item->m_equip_effect->m_dblock += (short) ceil(mp*0.08);
				break;

			case STRENGTH_MOD:
				item->m_equip_effect->m_dstrength += (short) ceil(mp*0.03);
				break;

			case DEXTERITY_MOD:
				item->m_equip_effect->m_ddexterity += (short) ceil(mp*0.03);
				break;

			case WILLPOWER_MOD:
				item->m_equip_effect->m_dwillpower += (short) ceil(mp*0.03);
				break;

			case MAGIC_POWER_MOD:
				item->m_equip_effect->m_dmagic_power += (short) ceil(mp*0.03);
				break;

			case RESIST_PHYS_MOD:
				item->m_equip_effect->m_dresistances[Damage::PHYSICAL] += (short) ceil(sqrtmp*0.3);
				break;

			case RESIST_FIRE_MOD:
				item->m_equip_effect->m_dresistances[Damage::FIRE] += (short) ceil(sqrtmp*0.6);
				break;

			case RESIST_ICE_MOD:
				item->m_equip_effect->m_dresistances[Damage::ICE] += (short) ceil(sqrtmp*0.6);
				break;

			case RESIST_AIR_MOD:
				item->m_equip_effect->m_dresistances[Damage::AIR] += (short) ceil(sqrtmp*0.6);
				break;

			case RESIST_ALL_MOD:
				item->m_equip_effect->m_dresistances[Damage::FIRE] += (short) ceil(sqrtmp*0.3);
				item->m_equip_effect->m_dresistances[Damage::ICE] += (short) ceil(sqrtmp*0.3);
				item->m_equip_effect->m_dresistances[Damage::AIR] += (short) ceil(sqrtmp*0.3);
				break;

			case DAMAGE_PHYS_MOD:
				dmgavg *=0.6;
				dmgdiff = Random::randrangef(dmgavg*0.2,dmgavg*0.6);
				item->m_weapon_attr->m_damage.m_min_damage[Damage::PHYSICAL] += ceil(dmgavg-dmgdiff);
				item->m_weapon_attr->m_damage.m_max_damage[Damage::PHYSICAL] += ceil(dmgavg+dmgdiff);
				break;

			case DAMAGE_FIRE_MOD:
				dmgdiff = Random::randrangef(dmgavg*0.2,dmgavg*0.5);
				item->m_weapon_attr->m_damage.m_min_damage[Damage::FIRE] += ceil(dmgavg-dmgdiff);
				item->m_weapon_attr->m_damage.m_max_damage[Damage::FIRE] += ceil(dmgavg+dmgdiff);
				break;

			case DAMAGE_ICE_MOD:
				dmgdiff = Random::randrangef(dmgavg*0.1,dmgavg*0.3);
				item->m_weapon_attr->m_damage.m_min_damage[Damage::ICE] += ceil(dmgavg-dmgdiff);
				item->m_weapon_attr->m_damage.m_max_damage[Damage::ICE] += ceil(dmgavg+dmgdiff);
				break;

			case DAMAGE_AIR_MOD:
				dmgdiff = Random::randrangef(dmgavg*0.4,dmgavg*0.8);
				item->m_weapon_attr->m_damage.m_min_damage[Damage::AIR] += ceil(dmgavg-dmgdiff);
				item->m_weapon_attr->m_damage.m_max_damage[Damage::AIR] += ceil(dmgavg+dmgdiff);
				break;

			case DAMAGE_MULT_PHYS_MOD:
				item->m_weapon_attr->m_damage.m_multiplier[Damage::PHYSICAL] *= (1+sqrtmp*0.006);
				break;

			case DAMAGE_MULT_FIRE_MOD:
				item->m_weapon_attr->m_damage.m_multiplier[Damage::FIRE] *= (1+sqrtmp*0.006);
				break;

			case DAMAGE_MULT_ICE_MOD:
				item->m_weapon_attr->m_damage.m_multiplier[Damage::ICE] *= (1+sqrtmp*0.006);
				break;

			case DAMAGE_MULT_AIR_MOD:
				item->m_weapon_attr->m_damage.m_multiplier[Damage::AIR] *= (1+sqrtmp*0.006);
				break;

			case ATTACK_SPEED_MOD:
				item->m_equip_effect->m_dattack_speed += (short)  (sqrtmp*15);
				break;

			case ATTACK_MOD:
				item->m_weapon_attr->m_damage.m_attack += ceil(mp*0.4);
				break;

			case POWER_MOD:
				item->m_weapon_attr->m_damage.m_power += ceil(mp*0.4);
				break;

		}
		
		if (item->m_rarity == Item::NORMAL)
		{
			item->m_rarity = Item::MAGICAL;
		}

		// jede Modifikation darf nur einmal vorkommen, entfernen
		item->m_magic_mods.set(mod,true);
		sum -= modprob[mod];
		modprob[mod]=0;
	}

	item->m_level_req = MathHelper::Max(item->m_level_req,(char) levelreq);
	DEBUGX("level req %i",item->m_level_req);
	
	return num_mods;
}

Item::Type  ItemFactory::getBaseType(Item::Subtype subtype)
{
	std::map<Item::Subtype,Item::Type>::iterator it;
	it = m_item_types.find(subtype);
	
	if (it != m_item_types.end())
	{
		return it->second;
	}
	
	return Item::NOITEM;
}

std::string ItemFactory::getItemName(Item::Subtype subtype)
{
	std::map<Item::Subtype,ItemBasicData*>::iterator it;
	it = m_item_data.find(subtype);
	
	if (it== m_item_data.end())
	{
		return subtype;
	}
	return it->second->m_name;
}

void ItemFactory::registerItemDrop(Item::Subtype subtype, DropChance chance)
{
	m_item_probabilities.insert(make_pair(subtype, chance));
}

void ItemFactory::registerItem(Item::Type type,Item::Subtype subtype, ItemBasicData* data, bool silent_replace)
{
	DEBUGX("registered item %s %p",subtype.c_str(), data->m_weapon_attr);
	if (m_item_data.count(subtype) != 0)
	{
		if (!silent_replace)
		{
			WARNING("Duplicate item data: %s",subtype.c_str());
		}
		delete m_item_data[subtype];
	}
	m_item_data[subtype] = data;
	m_item_types[subtype] = type;
}

void ItemFactory::init()
{

}

void ItemFactory::cleanup()
{
	DEBUG("cleanup");

	std::map<Item::Subtype,ItemBasicData*>::iterator it;
	for (it = m_item_data.begin(); it != m_item_data.end(); ++it)
	{
		DEBUGX("deleting item data %s",it->first.c_str());
		delete it->second;
	}
	m_item_data.clear();
	m_item_types.clear();
	m_item_probabilities.clear();
	
}

Item* ItemFactory::createItem(DropSlot &slot, float factor )
{
	DEBUGX("get item by dropslot");
	// wenn maximales Level unter 0 liegt kein Item ausgeben
	if (slot.m_max_level<0)
		return 0;

	DEBUGX("drop item %i %i",slot.m_min_level, slot.m_max_level);
	
	float probs[5];
	probs[Item::SIZE_NONE] =1;
	for (int i=0; i<4; i++)
	{
		probs[i] = slot.m_size_probability[i]* factor;
		probs[Item::SIZE_NONE] -= slot.m_size_probability[i]* factor;
	}
	
	if (probs[Item::SIZE_NONE] < 0)
	{
		float div = 1-probs[Item::SIZE_NONE];
		for (int i=0; i<4; i++)
		{
			probs[i] /= div;
		}
	}
		
	int size = Random::randDiscrete(probs,5);

	// kein Item ausgewurfelt
	if (size ==4)
		return 0;

	
	
	DEBUGX("item size: %i",size);
	Item* item =0;
	if (size  == Item::GOLD)
	{
		
		// Gold gedroppt
		item = createGold(Random::randrangei(slot.m_min_gold, slot.m_max_gold));

		return item;
	}
	else
	{
		// *richtiges* Item gedroppt

		// Vector fuer die moeglichen Items
		std::vector<Item::Subtype> types;

		// Vector fuer die Wahrscheinlickeit
		std::vector<float> prob;

		float p;

		std::map<Item::Subtype, DropChance>::iterator i;
		for (i= m_item_probabilities.begin();i!=m_item_probabilities.end();++i)
		{
			// Testen ob die Groesse passt
			if (i->second.m_size != size)
				continue;

			// Testen ob Level ausreicht
			if (fabs(float(i->second.m_level)) > slot.m_max_level || (i->second.m_level>0 && i->second.m_level <slot.m_min_level))
				continue;

			// Wahrscheinlichkeit fuer Drop berechnen
			p = i->second.m_probability;

			if (p>0)
			{
				DEBUGX("possible item subtype %s prob %f",i->first.c_str(),p);
				types.push_back(i->first);
				prob.push_back(p);

			}


		}

		if (prob.empty())
			return 0;

		// Item auswuerfeln
		int res = Random::randDiscrete(prob);

		// Typ, Subtyp bestimmen
		Item::Subtype subtype= types[res];
		Item::Type type = m_item_types.find(subtype)->second;

		// Magiestaerke berechnen
		float magic =0;
		if (Random::random() < slot.m_magic_probability || type == Item::RING  || type == Item::AMULET)
		{
			magic = Random::randrangef(slot.m_magic_power/4,slot.m_magic_power);
		}
		

		item = createItem(type,subtype,0,magic);
		DEBUGX("item type %i  subtype %s level %i magic %f",type,subtype.c_str(), item->m_level_req,magic);
		return item;
	}
	return 0;

}

