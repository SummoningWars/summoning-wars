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

#ifndef __SUMWARS_CORE_UNIONFINDTREE_H__
#define __SUMWARS_CORE_UNIONFINDTREE_H__

#include <list>
#include <vector>
#include <iostream>
#include <iomanip>
/**
 * \class UnionFindTree
 * \brief Implementation eines Unionfind Baumes
 */
class UnionFindTree
{
	public:
		/**
		 * \fn UnionFindTree(int dim)
		 * \brief Konstruktor, legt einen neuen Baum an, bestehend aus nur einelementigen Mengen
		 * \param dim Anzahl der Elemente
		 */
		UnionFindTree(int dim);
		
		/**
		 * \fn ~UnionFindTree()
		 * \brief Destruktor
		 */
		~UnionFindTree();
		
		/**
		 * \fn int find(int index)
		 * \brief Gibt die Wurzel des Teilbaumes, das Element index enthaelt aus
		 * \param index Element
		 */
		int find(int index);
		
		/**
		 * \fn void merge(int index1, index2)
		 * \brief Vereinigt die beiden Mengen die die beiden Elemente enthalten
		 * \param index1 Element der ersten Menge
		 * \param index2 Element der zweiten Menge
		 */
		void merge(int index1, int index2);
		
		/**
		 * \fn int getCardinality(int index)
		 * \brief Gibt die Groesse der Menge, die das Element index enthaelt aus
		 * \param index Element
		 */
		int getCardinality(int index);
		
		/**
		 * \fn void getElements(int index, std::list<int>& result)
		 * \brief Gibt die Elemente der Menge, die das Element index enthaelt aus
		 * \param index Element
		 * \param result Liste in die die Elemente eingefuegt werden
		 */
		void getElements(int index, std::list<int>& result);
		
		/*
		void print()
		{
			for (int i=0; i<m_dim; i++)
				std::cout << std::setw(3) << m_data[i];
			std::cout <<"\n";
			for (int i=0; i<m_dim; i++)
				std::cout << std::setw(3) << m_cardinality[i];
			std::cout <<"\n";
		}
		*/
		
	private:
		/**
		 * \var m_dim
		 * \brief Anzahl der Elemente
		 */
		int m_dim;
		
		/**
		 * \var int *m_data
		 * \brief Baum als Array
		 */
		int* m_data;
		
		/**
		 * \var int m_cardinality
		 * \brief Kardinalitaet der Teilmengen
		 */
		int* m_cardinality;
};
	









#endif // __SUMWARS_CORE_UNIONFINDTREE_H__

