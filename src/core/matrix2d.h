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

#ifndef MATRIX2D_H
#define MATRIX2D_H

#include "debug.h"
#include <stdio.h>
#include <string.h>

/**
 * \class Matrix2d
 * \brief Klasse fuer ein zweidimensionales Feld
 */
template <class Element>
class Matrix2d
{

	private:
		/**
 		 * \var int m_dimx
 		 * \brief Anzahl Zeilen
 		 */
		int m_dimx;
		
        /**
         * \var int m_dimy
         * \brief Anzahl Spalten
         */
		int m_dimy;
		
		/**
 		 * \var Element* m_data
 		 * \brief Datenfeld
 		 */
		Element* m_data;
		
	public:
	/**
 	 * fn Matrix2D(int dimx, int dimy)
 	 * \brief Konstruktor
 	 * \param dimx Breite des Feldes
 	 * \param dimy Laenge des Feldes
	 */
	Matrix2d(int dimx, int dimy)
	{
		m_dimx = dimx,m_dimy = dimy;
		m_data = new Element[dimx*dimy];
		DEBUGX("creating Matrix2d at %p",m_data);
	}
	
	
	/**
 	 * \fn ~Matrix2d()
 	 * \brief Destruktor
 	 */
	~Matrix2d()
	{
		delete[] m_data;
	}
	
	
	/**
	 * \fn Element* ind(int x, int y)
	 * \brief Gibt Referenz auf das Element an der angegebenen Position aus
	 * \param x Index1
	 * \param y Index2
	 */
	Element* ind(int x, int y)
	{
		return &(m_data[x*m_dimy +y]);
	}
	
	/**
	 * \fn Element* operator[](int x)
	 * \brief Indexoperator
	 * \param x Index
	 */
	Element* operator[](int x)
	{
		return &(m_data[x*m_dimy]);
	}
	
	/**
	 * \fn void clear()
	 * \brief Schreibt 0 in alle Elemente des Feldes
	 */
	void clear()
	{
		memset(m_data,0,sizeof(Element)*m_dimx*m_dimy);
	}
	
			
};
	

#endif
