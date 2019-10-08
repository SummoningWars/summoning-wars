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

#ifndef DROP_ITEM_H
#define DROP_ITEM_H

#include "item.h"
#include "itemlist.h"
#include <sstream>
#include <map>
#include "random.h"
#include "gameobject.h"

/**
 * \struct DropItem
 * \brief Wrapper Struktur fuer Gegenstaende die auf dem Boden liegen
 */
class DropItem : public GameObject
{
	public:
		/**
		* \fn DropItem(int id=0)
		* \brief Konstruktor
		 * \param id ID des Objektes
		*/
		DropItem(int id=0);
		
		/**
		 * \fn DropItem(Item* item)
		 * \brief Konstruktor
		 * \param item Item innerhalb des Wrappers
		 */
		DropItem(Item* item);
		
		/**
		 * \fn ~DropItem()
		 * \brief Destruktor
		 */
		~DropItem();
		
		/**
		 * \fn void init()
		 * \brief Initialisierung
		 */
		void init();
		
		/**
		 * \fn Item* getItem()
		 * \brief Gibt das zugrundeliegende Item aus
		 */
		Item* getItem()
		{
			return m_item;
		}
		
		/**
		 * \fn float getAngleX()
		 * \brief Gibt den Drehwinkel um die X-Achse aus
		 */
		float getAngleX()
		{
			return m_angle_x;
		}
		
		/**
		 * \fn float getTimer()
		 * \brief Gibt an, wie lange das Item bereits am Boden liegt
		 */
		float getTimer()
		{
			return m_time;
		}
		
		/**
		 * \fn virtual void toString(CharConv* cv)
		* \brief Konvertiert das Objekt in einen String und schreibt ihn in der Puffer
		* \param cv Ausgabepuffer
		*/
		virtual void toString(CharConv* cv);
				
				
		/**
		* \fn void fromString(CharConv* cv)
		* \brief Erzeugt das Objekt aus einem String
		* \param cv Eingabepuffer
		*/
		virtual void fromString(CharConv* cv);
		
		
		/**
		* \brief aktualisiert das Dropitem, nach eine bestimmte Zeitspanne vergangen ist
		* \param time Zeitspanne
		*/
		virtual bool update(float time);
		
		/**
		 * \fn int getLocationId()
		 * \brief Gibt die ID des Ortes aus (wird aus der Position berechnet)
		 */
		int getLocationId();
		
		/**
		 * \fn std::string getActionString()
		 * \brief Gibt die aktuelle Aktion als String aus
		 */
		virtual std::string getActionString();
	
		
		/**
		 * \fn virtual float getActionPercent()
		 * \brief Gibt den Prozentsatz, zu dem die aktuelle Aktion fortgeschritten ist aus
		 */
		virtual float getActionPercent();
		
	private:
		
		/**
	 	* \var Item* m_item
		 * \brief Zeiger auf das Item
		 */
		Item* m_item;
		
		/**
		 * \var float m_angle_x
		 * \brief Drehwinkel um die x Achse (beim fallen)
		 */	
		float m_angle_x;
		
		/**
		 * \var float m_time
		 * \brief Zeit die der Gegenstand schon auf dem Boden liegt
		 */
		float m_time;
		
		/**
		* \var float m_drop_speed
		* \brief Fallgeschwindigkeit
		*/
		float m_drop_speed;
	
		
		
};

typedef std::map<int,DropItem*> DropItemMap;

#endif
