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

#ifndef __SUMWARS_CORE_MINIMAP_H__
#define __SUMWARS_CORE_MINIMAP_H__

#include "geometry.h"
#include "charconv.h"
#include <vector>

/**
 * \class Minimap
 * \brief Speichert die Informationen, welche Bereiche einer Region schon durch die Spieler entdeckt wurden
 */
class Minimap
{
	public:
		/**
		 * \fn Minimap(short dimx, short dimy)
		 * \brief Erzeugt eine neue Minimap
		 * \param dimx Breite der Region
		 * \param dimy Laenge der Region
		 */
		Minimap(short dimx, short dimy);
		
		/**
		 * \fn ~Minimap()
		 * \brief Destruktor
		 */
		~Minimap()
		{
			
		}
		
		/**
		 * \fn void update(Vector playerpos)
		 * \brief Aktualisiert den Bereich, der als sichtbar markiert ist
		 * \param playerpos Position, an der ein Spieler steht
		 */
		void update(Vector playerpos);
		
		/**
		 * \fn void print()
		 * \brief (debugging)
		 */
		void print();
		
		/**
		 * \fn void merge(Minimap& other)
		 * \brief Vereinigt die sichtbaren Bereiche zweier Minimaps
		 */
		void merge(Minimap& other);
		
		/**
		 * \fn std::vector<char>& getData()
		 * \brief Gibt die Daten als char array aus
		 */
		std::vector<char>& getData()
		{
			return m_data;
		}
		
		/**
		 * \fn bool getData(short x, short y)
		 * \brief Gibt an, ob Gridunit bei x,y schon sichtbar ist
		 * \param x x
		 * \param y y
		 */
		bool getData(short x, short y)
		{
			return (m_data[(x+y*m_dimx)/8] & (1 << (x+y*m_dimx)%8));
		}
		
		/**
		 * \fn void setData(short x, short y)
		 * \brief Setzt das *sichtbar* Bit fuer die Gridunit bei x,y
		 * \param x x
		 * \param y y
		 */
		void setData(short x, short y)
		{
			m_data[(x+y*m_dimx)/8] |= (1 << (x+y*m_dimx)%8);
		}
		
		/**
		 * \fn toString(CharConv* cv)
		 * \brief Konvertiert das Objekt in einen String und schreibt ihn in der Puffer
		 * \param cv Ausgabepuffer
		 */
		void toString(CharConv* cv);


		/**
		 * \fn void fromString(CharConv* cv)
		 * \brief Erzeugt das Objekt aus einem String
		 * \param cv Eingabepuffer
		 */
		void fromString(CharConv* cv);
		
	protected:
		/**
		 * \var std::vector<char> m_data;
		 * \brief Daten kodiert als Bitfeld (1 = sichtbar) der Laenge dimx*dimy
		 */
		std::vector<char> m_data;
		
		/**
		 * \var short m_dimx
		 * \brief x-Dimension der Region
		 */
		short m_dimx;
		
		/**
		 * \var short m_dimy
		 * \brief y-Dimension der Region
		 */
		short m_dimy;
};



#endif // __SUMWARS_CORE_MINIMAP_H__

