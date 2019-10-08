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

#include "itemlist.h"

#include "itemfactory.h"

//Constructors/Destructors

ItemList::ItemList(short max_small, short max_medium, short max_big)
{
	m_max_small = max_small;
	m_max_medium = max_medium;
	m_max_big = max_big;

	m_small_items = new Item*[max_small];
	m_medium_items = new Item*[max_medium];
	m_big_items = new Item*[max_big];

	 int i;
	 for (i=0; i<max_small;i++)
	 {
		 m_small_items[i]=0;
	 }

	 for (i=0; i<max_medium;i++)
	 {
		 m_medium_items[i]=0;
	 }

	 for (i=0; i<max_big;i++)
	 {
		 m_big_items[i]=0;
	 }
	 m_gold =0;
}

ItemList::ItemList(ItemList* itemlist)
{
	m_max_small = itemlist->m_max_small;
	m_max_medium = itemlist->m_max_medium;
	m_max_big = itemlist->m_max_big;

	m_small_items = new Item*[m_max_small];

	m_medium_items = new Item*[m_max_medium];

	m_big_items = new Item*[m_max_big];

	int i;
	for (i=0; i<m_max_small;i++)
	{
		m_small_items[i]=itemlist->m_small_items[i];
	}

	for (i=0; i<m_max_medium;i++)
	{
		m_medium_items[i]=itemlist->m_medium_items[i];
	}

	for (i=0; i<m_max_big;i++)
	{
		m_big_items[i]=itemlist->m_big_items[i];
	}
	m_gold =itemlist->m_gold;
}


ItemList::~ItemList()
{
	delete[] m_small_items;
	delete[] m_medium_items;
	delete[] m_big_items;
}

void ItemList::clear()
{
	int i;
	for (i=0; i<m_max_small;i++)
	{
		if (m_small_items[i]!=0)
			delete m_small_items[i];
		m_small_items[i]=0;
	}

	for (i=0; i<m_max_medium;i++)
	{
		if (m_medium_items[i])
		{
			delete m_medium_items[i];
		}
		m_medium_items[i]=0;
	}

	for (i=0; i<m_max_big;i++)
	{
		if (m_big_items[i])
			delete m_big_items[i];
		m_big_items[i]=0;
	}
	m_gold =0;
}


Item* ItemList::getItem(Item::Size m_size, int index)
{
	if (m_size == Item::SMALL && index < m_max_small)
		return m_small_items[index];

	if (m_size == Item::MEDIUM && index < m_max_medium)
		return m_medium_items[index];

	if (m_size == Item::BIG && index < m_max_big)
		return m_big_items[index];

	return 0;
}

int ItemList::getFreePlace(Item::Size size)
{
	Item** arr = m_small_items;
	short k = m_max_small;

	if (size == Item::MEDIUM)
	{
		arr = m_medium_items;
		k = m_max_medium;
	}

	if (size == Item::BIG)
	{
		arr = m_big_items;
		k = m_max_big;
	}

	for (int i=0 ;i<k;i++)
	{
		if (arr[i]==0)
			return i;
	}
	return -1;
}

void ItemList::swapItem(Item* &item,Item::Size size, int index)
{
	DEBUGX("swapping %p of size %i at %i",item,size,index);
	Item** arr = m_small_items;
	short k = m_max_small;

	if (size == Item::MEDIUM)
	{
		arr = m_medium_items;
		k = m_max_medium;
	}

	if (size == Item::BIG)
	{
		arr = m_big_items;
		k = m_max_big;
	}

	if (index >=k)
		return;

	Item* t;
	t=arr[index];
	arr[index]=item;
	item = t;
}


Equipement::Equipement(short max_small, short max_medium, short max_big)
	: m_inventory(max_big,max_medium,max_small)
{
	for (int i=0; i<12; i++)
	{
		m_body_items[i] =0;
	}
	
	for (int i=0; i< getMaxBeltItemNumber(); i++)
	{
		m_belt_items[i] = 0;
	}

	m_gold = 0;
};

Equipement::~Equipement()
{
	clear();
}

void Equipement::clear()
{
	for (int i=0; i<NR_BODY_ITEMS; i++)
	{
		if (m_body_items[i] !=0)
		{
			delete m_body_items[i];
			m_body_items[i] =0;
		}
	}
	
	for (int i=0; i< getMaxBeltItemNumber(); i++)
	{
		if (m_belt_items[i] != 0)
		{
			delete m_belt_items[i];
			m_belt_items[i] = 0;
		}
	}

	DEBUGX("clearing equipement");
	m_inventory.clear();
	DEBUGX("done");
}

Item* Equipement::getItem(int pos)
{
	if (pos>0 && pos <NR_BODY_ITEMS)
	{
		return m_body_items[pos];
	}
	
	if (pos >=BELT_ITEMS && pos < BELT_ITEMS_END)
	{
		return m_belt_items[pos - BELT_ITEMS];
	}
	
	if (pos>=BIG_ITEMS && pos < MEDIUM_ITEMS)
	{
		return m_inventory.getItem(Item::BIG,pos-BIG_ITEMS);
	}

	if (pos>=MEDIUM_ITEMS && pos < SMALL_ITEMS)
	{
		return m_inventory.getItem(Item::MEDIUM,pos-MEDIUM_ITEMS);
	}

	if (pos>=SMALL_ITEMS)
	{
		return m_inventory.getItem(Item::SMALL,pos-SMALL_ITEMS);
	}

	return 0;

}



bool Equipement::swapItem(Item* &item,int pos)
{
	if (pos>= ARMOR && pos<=CURSOR_ITEM )
	{
		// Tausch mit einem Ausruestungsgegenstand
		std::swap(item, m_body_items[pos]);
		return true;
	}
	else if (pos >=BELT_ITEMS && pos < BELT_ITEMS_END)
	{
		// swap into belt
		std::swap(item, m_belt_items[pos - BELT_ITEMS]);
		return true;
	}
	else
	{
		// Index berechnen bei dem getauscht werden soll
		int idx = pos-BIG_ITEMS;
		Item::Size size= Item::BIG;

		if (pos>= MEDIUM_ITEMS)
		{
			idx = pos - MEDIUM_ITEMS;
			size = Item::MEDIUM;
		}
		if (pos>=SMALL_ITEMS)
		{
			idx = pos - SMALL_ITEMS;
			size = Item::SMALL;
		}

		// Tausch mit Gegenstand im Inventar
		m_inventory.swapItem(item,size,idx);
		DEBUGX("cursor item %p %p",getItem(Equipement::CURSOR_ITEM), &(m_body_items[CURSOR_ITEM ]));

		return true;
	}
}

bool Equipement::swapCursorItem(int pos)
{
	bool ret = swapItem(m_body_items[CURSOR_ITEM],pos);
	return ret;
}

short  Equipement::insertItem(Item* item, bool use_belt, bool use_equip, bool use_secondary)
{
	if (item ==0)
		return NONE;

	if (item->m_size == Item::GOLD)
	{
		m_gold += item->m_price;
		delete item;
		return GOLD;
	}

	if (use_equip)
	{
		if (item->m_type == Item::WEAPON && !use_secondary && m_body_items[WEAPON] == 0 && (item->m_weapon_attr->m_two_handed == false || m_body_items[SHIELD] == 0))
		{
			swapItem(item,WEAPON);
			return WEAPON;
		}
		else if (item->m_type == Item::WEAPON && use_secondary && m_body_items[WEAPON2] == 0  && (item->m_weapon_attr->m_two_handed == false || m_body_items[SHIELD2] == 0))
		{
			swapItem(item,WEAPON2);
			return WEAPON2;
		}
		else if (item->m_type == Item::SHIELD && !use_secondary && m_body_items[SHIELD] == 0 && (m_body_items[WEAPON] == 0 || m_body_items[WEAPON]->m_weapon_attr->m_two_handed == false))
		{
			swapItem(item,SHIELD);
			return SHIELD;
		}
		else if (item->m_type == Item::SHIELD && use_secondary && m_body_items[SHIELD2] == 0 && (m_body_items[WEAPON2] == 0 || m_body_items[WEAPON2]->m_weapon_attr->m_two_handed == false))
		{
			swapItem(item,SHIELD2);
			return SHIELD2;
		}
		else if (item->m_type == Item::ARMOR && m_body_items[ARMOR] == 0)
		{
			swapItem(item,ARMOR);
			return ARMOR;
		}
		else if (item->m_type == Item::GLOVES && m_body_items[GLOVES] == 0)
		{
			swapItem(item,GLOVES);
			return GLOVES;
		}
		else if (item->m_type == Item::HELMET && m_body_items[HELMET] == 0)
		{
			swapItem(item,HELMET);
			return HELMET;
		}
		else if (item->m_type == Item::RING && m_body_items[RING_LEFT] == 0)
		{
			swapItem(item,RING_LEFT);
			return RING_LEFT;
		}
		else if (item->m_type == Item::RING && m_body_items[RING_RIGHT] == 0)
		{
			swapItem(item,RING_RIGHT);
			return RING_RIGHT;
		}
		else if (item->m_type == Item::AMULET && m_body_items[AMULET] == 0)
		{
			swapItem(item,AMULET);
			return AMULET;
		}
	}

	Item* itm = item;
	int pos = -1;
	bool useup = (item->m_useup_effect != 0);
	
	if (useup && use_belt)
	{
		// might be placed in belt
		// check if this object is not placed in the belt yet
		for (int i=0; i< getMaxBeltItemNumber(); i++)
		{
			if (pos == -1 && m_belt_items[i] == 0)
			{
				pos = i+ BELT_ITEMS;
			}
			else if (m_belt_items[i] != 0
				&& m_belt_items[i]->m_subtype == item->m_subtype)
			{
				// item of this type is already placed in the belt
				pos = -1;
				break;
			}
		}
		
		if (pos != -1)
		{
			std::swap(item, m_belt_items[pos - BELT_ITEMS]);
			return true;
		}
	}
	
	pos = m_inventory.getFreePlace(item->m_size);
	DEBUGX("free pos: %i",pos);


	if (pos ==-1)
	{
		return NONE;
	}

	if (item->m_size == Item::SMALL)
	{
		swapItem(itm,pos+SMALL_ITEMS);
		return pos+SMALL_ITEMS;
	}
	if (item->m_size == Item::MEDIUM)
	{
		swapItem(itm,pos+MEDIUM_ITEMS);
		return pos+MEDIUM_ITEMS;
	}
	if (item->m_size == Item::BIG)
	{
		swapItem(itm,pos+BIG_ITEMS);
		return pos+BIG_ITEMS;
	}

	ERRORMSG("unknown item size item type %i subtype %s",item->m_type,item->m_subtype.c_str());
	return NONE;

}

short Equipement::findItem(Item::Subtype subtype,short startpos)
{
	int start = startpos;
	for (int i=MathHelper::Max(0,start); i<CURSOR_ITEM; i++)
	{
		if (m_body_items[i] !=0 && m_body_items[i]->m_subtype == subtype)
			return i;
	}

	Item* item;
	for (int i= MathHelper::Max(int(BIG_ITEMS),start); i< BIG_ITEMS+m_inventory.getMaxBig(); i++)
	{
		item = m_inventory.getItem(Item::BIG, i - BIG_ITEMS);
		if (item != 0 && item->m_subtype == subtype)
			return i;
	}

	for (int i= MathHelper::Max(int(BIG_ITEMS),start); i< BIG_ITEMS+m_inventory.getMaxBig(); i++)
	{
		item = m_inventory.getItem(Item::BIG, i - BIG_ITEMS);
		if (item != 0 && item->m_subtype == subtype)
			return i;
	}

	for (int i= MathHelper::Max(int(MEDIUM_ITEMS),start); i< MEDIUM_ITEMS+m_inventory.getMaxMedium(); i++)
	{
		item = m_inventory.getItem(Item::MEDIUM, i - MEDIUM_ITEMS);
		if (item != 0 && item->m_subtype == subtype)
			return i;
	}

	for (int i= MathHelper::Max(int(SMALL_ITEMS),start); i< SMALL_ITEMS+m_inventory.getMaxSmall(); i++)
	{
		item = m_inventory.getItem(Item::SMALL, i - SMALL_ITEMS);
		if (item != 0 && item->m_subtype == subtype)
			return i;
	}
	
	for (int i= MathHelper::Max(int(BELT_ITEMS),start); i< BELT_ITEMS_END; i++)
	{
		item = m_belt_items[i - BELT_ITEMS];
		if (item != 0 && item->m_subtype == subtype)
			return i;
	}

	return NONE;
}

short Equipement::stringToPosition(std::string posstr, bool secondary)
{
	if (posstr == "armor")
		return ARMOR;
	else if (posstr == "helmet")
		return HELMET;
	else if (posstr == "gloves")
		return GLOVES;
	else if (posstr == "shield")
	{
		if (!secondary)
			return SHIELD;
		else
			return SHIELD2;
	}
	else if (posstr == "weapon")
	{
		if (!secondary)
			return WEAPON;
		else
			return WEAPON2;
	}
	else if (posstr == "ring_left")
		return RING_LEFT;
	else if (posstr == "ring_right")
		return RING_RIGHT;
	else if (posstr == "amulet")
		return AMULET;
	else if (posstr == "big_items")
		return BIG_ITEMS;
	else if (posstr == "medium_items")
		return MEDIUM_ITEMS;
	else if (posstr == "small_items")
		return SMALL_ITEMS;

 	return NONE;
}

int Equipement::getNumberItems()
{
	int nr =0;

	for (int i=0; i<NR_BODY_ITEMS; i++)
	{
		if (m_body_items[i] !=0)
		{
			nr ++;
		}
	}

	int i;
	for (i=0;i<m_inventory.getMaxBig();i++)
	{
		if (m_inventory.getItem(Item::BIG,i))
			nr++;
	}

	for (i=0;i<m_inventory.getMaxMedium();i++)
	{
		if (m_inventory.getItem(Item::MEDIUM,i))
			nr++;
	}

	for (i=0;i<m_inventory.getMaxSmall();i++)
	{
		if (m_inventory.getItem(Item::SMALL,i))
			nr++;
	}

	for (i=0;i<getMaxBeltItemNumber();i++)
	{
		if (m_belt_items[i])
			nr++;
	}

	return nr;
}

int Equipement::getMaxItemNumber(Item::Size size)
{
	int pos =0;
	if (size == Item::BIG)
	{
		int i;
		for (i=0;i<m_inventory.getMaxBig();i++)
		{
			if (m_inventory.getItem(Item::BIG,i))
				pos = i;
		}
	}
	else if(size == Item::MEDIUM)
	{
		int i;
		for (i=0;i<m_inventory.getMaxMedium();i++)
		{
			if (m_inventory.getItem(Item::MEDIUM,i))
				pos = i;
		}
	}
	else if(size == Item::SMALL)
	{
		int i;
		for (i=0;i<m_inventory.getMaxSmall();i++)
		{
			if (m_inventory.getItem(Item::SMALL,i))
				pos = i;
		}
	}
	return pos;
}

int Equipement::getNumSmallItemsOfType(Item::Subtype subtype)
{
	int ret = 0;
	for (int i=0;i<m_inventory.getMaxSmall();i++)
	{
		Item* it = m_inventory.getItem(Item::SMALL,i);
		if (it!=0 && it->m_subtype == subtype)
		{
			ret++;
		}
	}
	
	for (int i=0;i<getMaxBeltItemNumber();i++)
	{
		Item* it = m_belt_items[i];
		if (it!=0 && it->m_subtype == subtype)
		{
			ret++;
		}
	}
	
	for (int i=0; i<=CURSOR_ITEM; i++)
	{
		Item* it = m_body_items[i];
		if (it!=0 && it->m_subtype == subtype)
		{
			ret++;
		}
	}
	return ret;
}

Equipement::Position Equipement::getItemEquipPosition(Item* item, bool secondary_equip, Equipement::Position proposed)
{
	Position pos = proposed;
	
	if (item->m_type == Item::WEAPON && !secondary_equip)
		pos = Equipement::WEAPON;
	else if (item->m_type == Item::WEAPON && secondary_equip)
		pos = Equipement::WEAPON2;
	else if (item->m_type == Item::SHIELD && !secondary_equip)
		pos = Equipement::SHIELD;
	else if (item->m_type == Item::SHIELD && secondary_equip)
		pos = Equipement::SHIELD2;
	else if (item->m_type == Item::HELMET)
		pos = Equipement::HELMET;
	else if (item->m_type == Item::GLOVES)
		pos = Equipement::GLOVES;
	else if (item->m_type == Item::ARMOR)
		pos = Equipement::ARMOR;
	else if (item->m_type == Item::AMULET)
		pos = Equipement::AMULET;
	else if (item->m_type == Item::RING)
	{
		if (pos != Equipement::RING_LEFT && pos != Equipement::RING_RIGHT)
		{
			pos = Equipement::RING_LEFT;
			if (getItem(pos) != 0)
				pos = Equipement::RING_RIGHT;
		}
	}
	
	return pos;
}

void Equipement::toString(CharConv* cv)
{

	short nr = getNumberItems();
	cv->toBuffer(nr);

	for (int i=0; i<NR_BODY_ITEMS; i++)
	{
		if (m_body_items[i] !=0)
		{
			cv->printNewline();
			cv->toBuffer(i);
			m_body_items[i]->toString(cv);
		}
	}


	// grosse Items
	Item* it;
	for (int i=0;i<m_inventory.getMaxBig();i++)
	{
		it = m_inventory.getItem(Item::BIG,i);
		if (it!=0)
		{
			cv->printNewline();
			cv->toBuffer(static_cast<short>(BIG_ITEMS+i));
			it->toString(cv);
			nr++;
		}
	}

	// mittlere Items
	for (int i=0;i<m_inventory.getMaxMedium();i++)
	{
		it = m_inventory.getItem(Item::MEDIUM,i);
		if (it!=0)
		{
			cv->printNewline();
			cv->toBuffer(static_cast<short>(MEDIUM_ITEMS+i));
			it->toString(cv);
			nr++;
		}
	}

	// kleine Items
	for (int i=0;i<m_inventory.getMaxSmall();i++)
	{
		it = m_inventory.getItem(Item::SMALL,i);
		if (it!=0)
		{
			cv->printNewline();
			cv->toBuffer(static_cast<short>(SMALL_ITEMS+i));
			it->toString(cv);
			nr++;
		}
	}

	// belt
	for (int i=0;i<getMaxBeltItemNumber();i++)
	{
		it = m_belt_items[i];
		if (it!=0)
		{
			cv->printNewline();
			cv->toBuffer(static_cast<short>(BELT_ITEMS+i));
			it->toString(cv);
			nr++;
		}
	}

}

void Equipement::fromString(CharConv* cv)
{
	clear();
	short nr;
	cv->fromBuffer(nr);

	int i;
	short pos;
	Item* it;

	std::string subtype;
	char type;
	int id;

	for (i=0;i<nr;i++)
	{
		cv->fromBuffer(pos);
		cv->fromBuffer(type);
		cv->fromBuffer(subtype);
		cv->fromBuffer(id);

		it = ItemFactory::createItem((Item::Type) type, std::string(subtype),id);

		// Datenfelder des Items belegen
		it->fromString(cv);

		DEBUGX("creating item %p",it);

		// Item ins Inventar tauschen
		swapItem(it,pos);
	}

}

void Equipement::toStringComplete(CharConv* cv)
{

	short nr = getNumberItems();
	cv->toBuffer(nr);

	for (int i=0; i<NR_BODY_ITEMS; i++)
	{
		if (m_body_items[i] !=0)
		{
			cv->printNewline();
			cv->toBuffer(static_cast<short>(i));
			m_body_items[i]->toStringComplete(cv);
		}
	}


	// grosse Items
	Item* it;
	for (int i=0;i<m_inventory.getMaxBig();i++)
	{
		it = m_inventory.getItem(Item::BIG,i);
		if (it!=0)
		{
			cv->printNewline();
			cv->toBuffer(static_cast<short>(BIG_ITEMS+i));
			it->toStringComplete(cv);
			nr++;
		}
	}

	// mittlere Items
	for (int i=0;i<m_inventory.getMaxMedium();i++)
	{
		it = m_inventory.getItem(Item::MEDIUM,i);
		if (it!=0)
		{
			cv->printNewline();
			cv->toBuffer(static_cast<short>(MEDIUM_ITEMS+i));
			it->toStringComplete(cv);
			nr++;
		}
	}

	// kleine Items
	for (int i=0;i<m_inventory.getMaxSmall();i++)
	{
		it = m_inventory.getItem(Item::SMALL,i);
		if (it!=0)
		{
			cv->printNewline();
			cv->toBuffer(static_cast<short>(SMALL_ITEMS+i));
			it->toStringComplete(cv);
			nr++;
		}
	}
	
		// belt
	for (int i=0;i<getMaxBeltItemNumber();i++)
	{
		it = m_belt_items[i];
		if (it!=0)
		{
			cv->printNewline();
			cv->toBuffer(static_cast<short>(BELT_ITEMS+i));
			it->toStringComplete(cv);
			nr++;
		}
	}
	
	cv->printNewline();
	cv->toBuffer(m_gold);

}

void Equipement::fromStringComplete(CharConv* cv)
{
	clear();
	short nr;
	cv->fromBuffer(nr);

	int i;
	short pos;
	Item* it;
	std::string subtype;
	char type;
	int id;

	for (i=0;i<nr;i++)
	{
		cv->fromBuffer(pos);
		cv->fromBuffer(type);
		cv->fromBuffer(subtype);
		cv->fromBuffer(id);
		if (World::getWorld() !=0 && World::getWorld()->isServer())
			id =0;
		DEBUGX("pos %i type %i subtype %s",pos,type, subtype.c_str());

		it = ItemFactory::createItem((Item::Type) type, std::string(subtype),id);

		// Datenfelder des Items belegen
		it->fromStringComplete(cv);

		DEBUGX("creating item %p",it);

		// Item ins Inventar tauschen
		swapItem(it,pos);
	}

	cv->fromBuffer(m_gold);

}










