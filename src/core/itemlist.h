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

#ifndef ITEMLIST_H
#define ITEMLIST_H

#include <string>
#include <cstdlib>
#include <algorithm>
#include "debug.h"

#include "item.h"


/**
 * \class ItemList
 *
 * \brief Klasse fuer eine Liste von Gegenstaenden
 */
class ItemList {
//Public stuff
public:
	//Fields
	//Constructors
   /**
	 * \fn ItemList(short max_small, short max_medium, short max_big)
	 * \brief Konstruktor
	 *
	 * Legt ein neues ItemList Objekt an
	 * \param max_small maximale Anzahl kleiner Objekte in der Liste
	 * \param max_medium maximale Anzahl mittelgroßer Objekte in der Liste
	 * \param max_big maximale Anzahl großer Objekte in der Liste
	 */
	ItemList(short max_small, short max_medium, short max_big);

	/**
	 * \fn ItemList(Itemlist* itemlist)
	 * \brief Kopierkonstruktor, legt eine flache Kopie an, die referenzierten Items sind also die gleichen!
	 * \param itemlist Kopiervorlage
	 */
	ItemList(ItemList* itemlist);

   /**
	 * \fn ~ItemList()
	 * \brief Destruktor
	 *
	 * Löscht das ItemList Objekt
	 */
	~ItemList();

	/**
	 * \fn void clear()
	 * \brief Loescht alle enthaltenen Items
	 */
	void clear();


	/**
	 * \fn Item* getItem(Item::Size size, int index);
	 * \brief Gibt einen Zeiger auf das Item der Groesse m_size an der Position index zurueck. Gibt NULL zurueck, wenn der Index die Listengroesse ueberschreitet.
	 * \param size Groesse des Items
	 * \param index Index
	 */
	Item* getItem(Item::Size size, int index);

	/**
	 * \fn int getFreePlace(Item::Size size, bool useup_possible = false)
	 * \brief Gibt den Indes  des ersten freien Platzes fuer ein Item der angegebenen Groesse aus.
	 * \param size Groesse des Items
	 * \return Index fuer einen freien Platz
	 */
	int getFreePlace(Item::Size size);

	/**
	 * \fn  swapItem(Item* &item,Item::Size size, int index)
	 * \brief vertauscht den angegebene Gegenstand mit dem gleichgrossen an Position index. Ein Gegenstand wird aus der Liste entfernt, indem man ihn mit einem leeren Gegenstand austauscht. Ein Gegenstand wird in die Liste eingefuegt, indem man einen leeren Platz der richtigen Groesse sucht und damit austauscht.
	 * \param item Zeiger auf den zu tauschenden Gegenstand
	 * \param size Groesse des Items
	 * \param index Position des Gegenstandes in der Liste, mit dem getauscht werden soll
	 */
	void swapItem(Item* &item,Item::Size size, int index);

	/**
	 * \fn short getMaxSmall()
	 * \brief Gibt maximale Anzahl kleiner Gegenstaende aus
	 */
	short getMaxSmall()
	{
		return m_max_small;
	}

	/**
	 * \fn short getMaxMedium()
	 * \brief Gibt maximale Anzahl mittlere Gegenstaende aus
	 */
	short getMaxMedium()
	{
		return m_max_medium;
	}

	/**
	 * \fn short getMaxBig()
	 * \brief Gibt maximale Anzahl grosser Gegenstaende aus
	 */
	short getMaxBig()
	{
		return m_max_big;
	}

	/**
	 * \fn int getGold()
	 * \brief Gibt die Menge Gold aus
	 * \return Gold
	 */
	int getGold()
	{
		return m_gold;
	}

	/**
	 * \fn void setGold(int gold)
	 * \brief Setzt die Menge Gold
	 * \param gold Gold
	 */
	void setGold(int gold)
	{
		m_gold = gold;
	}


//Private stuff
private:
	//Fields

	/**
	 * \var m_max_small
	 * \brief Gibt die maximale Anzahl kleiner Gegenstaende in der Liste an
	 */
	short m_max_small;

	/**
	 * \var m_max_medium
	 * \brief Gibt die maximale Anzahl mittelgrosser Gegenstaende in der Liste an
	 */
	short m_max_medium;

	/**
	 * \var m_max_big
	 * \brief Gibt die maximale Anzahl grosser Gegenstaende in der Liste an
	 */
	short m_max_big;


	/**
	 * \var m_small_items;
	 * \brief Liste der kleinen Gegenst&auml;nde
	 */
	Item**  m_small_items;

	/**
	 * \var m_medium_items;
	 * \brief Liste der mittelgrossen Gegenst&auml;nde
	 */
	Item**  m_medium_items;

	/**
	 * \var m_big_items;
	 * \brief Liste der kleinen Gegenst&auml;nde
	 */
	Item**  m_big_items;

	/**
	 * \var m_gold
	 * \brief Menge des Goldes
	 */
	int m_gold;


};


/**
 * \class Equipement
 * \brief Klasse fuer die Ausruestung eines Spielers
 */
class Equipement
{
	public:

	/**
	 * \enum Position
	 * \brief Zaehlt Positionen auf, an denen sich ein Gegenstand im Inventar eines Spielers befinden kann
	 */
	enum Position
	{
		NONE = 0,
		ARMOR  =1,
		HELMET =2,
		GLOVES =3,
		WEAPON =4,
		SHIELD =5,
		RING_LEFT=6,
		RING_RIGHT =7,
		AMULET= 8,
		WEAPON2=9,
		SHIELD2=10,
		CURSOR_ITEM=11,
		NR_BODY_ITEMS=12,
		BIG_ITEMS=1000,
		MEDIUM_ITEMS=2000,
		SMALL_ITEMS = 3000,
		BELT_ITEMS = 3500,
		//HACK: Belt should be separated from Inventory
		//BELT_ITEMS_END = 3510,
		BELT_ITEMS_END = 3505,
		GOLD = 4000
	};

	/**
	 * \fn Equipement(short max_small, short max_medium, short max_big)
	 * \brief Konstruktor
	 * \param max_small maximale Anzahl kleiner Items
	 * \param max_medium maximale Anzahl mittlegrosser Items
	 * \param max_big maximale Anzahl grosser Items
	 */
	Equipement(short max_small, short max_medium, short max_big);

	/**
	 * \fn ~Equipement()
	 * Destruktor
	 */
	virtual ~Equipement();

	/**
	 * \fn Item* getItem(int pos)
	 * \param pos Position des Items
	 * \brief Gibt Item an der angegebenen Position aus
	 */
	Item* getItem(int pos);

	/**
	 * \fn short insertItem(Item* item)
	 * \brief Fuegt Gegenstand ins Inventar ein
	 * \param item Zeiger auf den Gegenstand
	 * \param use_belt  Bei der Suche nach einem Platz wird auch der Guertel verwendet
	 * \param use_equip Bei der Suche nach einem Platz werden auch die Ausruestungsslots verwendet
	 * \param use_secondary Ruestungen und Schilde werden in den zweiten Slot eingefuegt
	 * \return Gibt an, wo das item eingefuegt wurde
	 */
	short insertItem(Item* item, bool use_belt=false, bool use_equip= false, bool use_secondary=false);

	/**
	 * \fn short findItem(Item::Subtype subtype, short startpos = 0)
	 * \param subtype Itemtyp
	 * \param startpos Es werden nur Position groesser oder gleich startpos durchsucht
	 * \brief Gibt die erste Position aus, auf der sich ein Item mit dem angegebenen Typ befindet. Gibt 0 aus, wenn keine Position gefunden wurde
	 */
	short findItem(Item::Subtype subtype, short startpos = 0);

	/**
	 * \fn short stringToPosition(std::string posstr, bool secondary = false)
	 * \brief Konvertiert Positionsstring in eine position
	 * \param posstr String der eine Position bezeichnet
	 * \param secondary Wenn auf true gesetzt, wird die sekundaere Ausruestung ausgegeben
	 */
	static short stringToPosition(std::string posstr, bool secondary = false);

	/**
	 * \fn bool swapCursorItem(int pos)
	 * \param pos Position des Items
	 * \brief Tauscht das aktuelle Cursoritem mit dem an der angegebenen Position
	 */
	bool swapCursorItem(int pos);

	/**
	 * \fn bool swapItem(Item* &item, int pos)
	 * \param pos Position des Items
	 * \param item Zeiger auf das Item das getauscht wird. Nach beenden der Funktion Zeiger auf das Item das aus dem Inventar herrausgetauscht wurde
	 * \brief Tauscht das angegebene Item mit dem an der angegebenen Position
	 */
	bool swapItem(Item* &item, int pos);


	/**
	 * \fn int getNumberItems()
	 * \brief Gibt aus die Anzahl der Gegenstaende in der Ausruestung zurueck
	 */
	int getNumberItems();

	/**
	 * \fn int getNumberItems(Item::Size size)
	 * \brief Gibt die Nummer des letzten belegten Platzes einer bestimmten Groesse aus
	 */
	int getMaxItemNumber(Item::Size size);
	
	/**
	 * \brief Returns the number of small objects with the specified subtype
	 * \param subtype subtype of the items to look for
	 * \return number of items found
	 */
	int getNumSmallItemsOfType(Item::Subtype subtype);
	
	/**
	 * \brief returns the number of object slots in the belt
	 */
	static int getMaxBeltItemNumber()
	{
		return BELT_ITEMS_END - BELT_ITEMS;
	}

	/**
	 * \fn virtual void toString(CharConv* cv)
	 * \brief Konvertiert das Objekt in einen String und schreibt ihn in der Puffer
	 * \param cv Ausgabepuffer
	 */
	virtual void toString(CharConv* cv);


	/**
	 * \fn virtual void fromString(CharConv* cv)
	 * \brief Erzeugt das Objekt aus einem String
	 * \param cv Eingabepuffer
	 */
	virtual void fromString(CharConv* cv);

	/**
	 * \fn virtual void toStringComplete(CharConv* cv)
	 * \brief Konvertiert das Objekt in einen String und schreibt ihn in der Puffer
	 * \param cv Ausgabepuffer
	 */
	virtual void toStringComplete(CharConv* cv);


	/**
	 * \fn virtual void fromStringComplete(CharConv* cv)
	 * \brief Erzeugt das Objekt aus einem String
	 * \param cv Eingabepuffer
	 */
	virtual void fromStringComplete(CharConv* cv);

	/**
	 * \fn void clear()
	 * \brief Loescht alle enthaltenen Item
	 */
	void clear();

	/**
	 * \fn int getGold()
	 * \brief Gibt die Menge Gold aus
	 */
	int getGold()
	{
		return m_gold;
	}

	/**
	 * \fn void setGold(int gold)
	 * \brief Setzt die Menge Gold
	 * \param gold Gold
	 */
	void setGold(int gold)
	{
		m_gold = gold;
	}
	
	/**
	 * \brief Returns the position where the Item shold be placed as equip position. The proposed position is prefered, if more than one position is possible (for rings)
	 * \param item The item for that the position should be determined
	 * \param proposed Position, that should be used if more than one position is possible
	 * \param secondary_equip If set to true, secondary equip slots will be returned for weapon and shield
	 * \return position where this items could be equiped
	 */
	Position getItemEquipPosition(Item* item, bool secondary_equip, Position proposed = NONE);

	private:

	/**
	 * \brief am Koeper getragene Gegenstaende
	 */
	Item* m_body_items[12];
	
	/**
	 * \brief Items placed in the Belt
	 */
	Item* m_belt_items[BELT_ITEMS_END - BELT_ITEMS];

	/**
	 * \var ItemList m_inventory
	 * \brief das Inventar
	 */
	ItemList m_inventory;

	/**
	 * \var int m_gold
	 * \brief Gold
	 */
	int m_gold;

};

#endif //ITEMLIST_H
