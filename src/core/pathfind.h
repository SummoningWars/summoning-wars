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


#ifndef __SUMWARS_CORE_PATHFIND_H__
#define __SUMWARS_CORE_PATHFIND_H__

#include "matrix2d.h"
#include "geometry.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <algorithm>


const float DIAG_PRIO=1.0;

const float penalty[10] = {1,1.2,1.5,2,3,5,10,20,40,200};

/**
 * \struct PathfindInfo
 * \brief Informationen zu einem Wegfindeauftrag
 */
struct PathfindInfo
{
	/**
	 * \var Matrix2d<float>* m_pot
	 * \brief Potentialfeld fuer die Wegfindung. Je hoeher das Potential desto groesser ist der Abstand zum Ziel
	 */
	Matrix2d<float>* m_pot;
	
	/**
	 * \var Matrix2d<char>* m_block
	 * \brief Array der blockierten Felder
	 * zulassig sind die Werte 0-9 sowie X . Felder mit Zahlen koennen ueberquert werden, allerdings steigen die Kosten von 0 nach 9 stark an. Felder mit X koennen nicht ueberquert werden
	 */
	Matrix2d<char>* m_block;
	
	/**
	 * \fn ~PathfindInfo()
	 * \brief gibt allokierten Speicher frei
	 */
	~PathfindInfo();
	
	/**
	 * \var Vector m_start;
	 * \brief Zielpunkt
	 * Da die Suche rueckwarts laeuft hier als Startpunkt bezeichnet
	 */
	Vector m_start;
	
	/**
	 * \var Vector m_center
	 * \brief Mittelpunkt des Suchbereiches
	 */
	Vector m_center;
	
	/**
	 * \var int m_dim
	 * \brief Laenge und Breite des Suchbereiches in Feldern
	 */
	int m_dim;
	
	/**
	 * \var short m_layer
	 * \brief Bitmaske der Ebenen in denen nach Hindernissen gesucht wird
	 */
	short m_layer;
	
	/**
	 * \var short m_region
	 * \brief Region in der die Suche stattfindet
	 */
	short m_region;
	
	/**
	 * \var float m_base_size
	 * \brief Durchmesser des Grundkreises fuer den die Suche stattfindet
	 * Die Groesse des Grundkreises bestimmt, durch welche engen Durchgaenge das Objekt passt
	 */
	float m_base_size;
	
	/**
	 * \var int m_quality
	 * \brief Qualitaet der Suche
	 * die Kantenlaenge eines Feldes bei der Suche errechnet sich als base_size/quality. Die Rechenzeit steigt quadratisch mit der Qualitaet
	 */
	int m_quality;
	
	/**
	 * \var int m_id
	 * \brief ID des Wegsuchenden Objektes
	 */
	int m_id;
	
	/**
	 * \var float m_timer
	 * \brief Timer fuer Debugging Zwecke
	 */
	float m_timer;
 
};

/**
 * \struct SearchFieldEntry
 * \brief Eintrag eines Suchfeldes
 */
struct SearchFieldEntry {
	
	/**
	 * \var float m_f
	 * \brief Entfernung des Suchfeldes von der Senke
	 */
	float m_f;
	
	/**
	 * \var m_x
	 * \brief x-Koordinate des aktuellen Feldes
	 */
	int m_x;
	/**
	 * \var m_y
	 * \brief x-Koordinate des aktuellen Feldes
	 */
	int m_y;
	
	/**
	 * \var int m_state
	 * \brief aktueller Zustand des Feldes (unentdeckt, entdeckt, abgearbeitet)
	 */
	int m_state;
	
	
	/**
	 * \var m_heapref
	 * \brief Verweis, wo das Element im Heap zu finden ist
	 */
	int m_heapref;

};

/**
 * \class SearchField
 * \brief Klasse fuer die Berechnung von Wegfindeauftraegen
 */
class SearchField
{
	public:
		/**
		* \fn SearchField(int max_distance)
		* \brief Konstruktor
		* \param max_distance maximale Anzahl Felder, die sich die Suche vom Startpunkt entfernen darf
		*
		* Legt ein neues Suchfeld an
		*/
			SearchField(int dim);
		/**
		* \fn ~SearchField()
		* \brief Destruktor
		*
		* Zerstört das Objekt und gibt allokierten Speicher frei
		*/
			~SearchField();
			
		/**
		 * \fn void init(Matrix2d<float>* pot, Matrix2d<char>* block)
		 * \brief initialisiert einen neuen Wegfindeauftrag
		 * \param pot Ausgabeparameter fuer das berechnete Potentialfeld
		 * \param block Matrix der blockierten Felder
		 */
		void init(Matrix2d<float>* pot, Matrix2d<char>* block);
		
		/**
		* \fn heapPush(SearchFieldEntry* fentry)
		* \brief Fügt ein Element dem Heap hinzu
		* \param fentry Feld, das auf den Heap abgelegt werden soll
		* Legt das Element auf dem Heap ab.
		*/
		inline void heapPush(SearchFieldEntry* fentry);
		
		/**
		* \fn SearchFieldEntry* heapPop()
		* \brief Gibt oberstes Element des Heaps zurück
		* \return SearchFieldEntry* Oberste Element des Heap
		*
		* Gibt das oberste Element des Heaps zurück. Das Element wird aus dem Heap gelöscht.
		*/
		inline SearchFieldEntry* heapPop();
		
		/**
		* \fn reOrder(SearchFieldEntry* fentry)
		* \brief Ordnet das Feld in den internen Datenstrukturen neu ein
		* \param fentry Feld, der neu eingeordnet werden soll
		*
		* Ordnet das Feld im Heap neu ein. Diese Funktion sollte immer dann aufgerufen werden, wenn die Priorität eines Felds sich geändert hat.
		*/
		inline void reOrder(SearchFieldEntry* fentry);
		
		
		/**
		* \fn SearchFieldEntry* getSearchFieldEntry(int x, int y)
		* \brief Gibt einen Zeiger auf den Feldeintrag an den gegebenen Koordinaten aus
		* \param x x-Koordinate
		* \param y y-Koordinate
		* \return Zeiger auf den Feldeintrag bei den gegebenen Koordinaten
		*/
		inline SearchFieldEntry* getSearchFieldEntry(int x, int y);
			
		/**
		 * \fn void createPotential(int start_x, int start_y)
		 * \brief Berechnet das Potentialfeld
		 * \param start_x x-Koordinate der Senke
		 * \param start_y y-Koordinate der Senke
		 */
		void createPotential(int start_x, int start_y);
		
		/**
		 * \fn  void createGradient(Matrix2d<float[2]>* grad)
		 * \brief Berechnet den Gradienten des Potentialfeldes
		 * \param grad Ausgabeparameter fuer den Gradienten
		 */
		void createGradient(Matrix2d<float[2]>* grad);
		
		/**
		 * \fn static void getGradient(PathfindInfo* pathinfo, Vector pos, Vector& dir)
		 * \brief Berechnet den Gradienten an einer gegebenen Stelle
		 * \param pinfo Wegfindeinformation
		 * \param pos Position an der die Richtung gesucht ist
		 * \param dir Ausgabeparameter fuer die Richtung
		 */
		static void getGradient(PathfindInfo* pathinfo,Vector pos, Vector& dir );

	private:
	
		/**
		* \var m_heap_dimension
		* \brief Gibt die Anzahl der Elemente im Heap an
		*/
		int m_heap_dimension;
		
		/**
		 * \var int m_dim
		 * \brief Dimension der Potentialfeld-, sowie Blockadematrix
		 */
		int m_dim;
		
		/**
		 * \var Matrix2d<float>* m_pot
		 * \brief Matrix fuer das Potentialfeld
		 */
		Matrix2d<float>* m_pot;
		
		/**
		 * \var Matrix2d<char>* m_block
		 * \brief Matrix fuer die blockierten Felder
		 */
		Matrix2d<char>* m_block;
		
		/**
		 * \var Matrix2d<float[2]>* m_grad
		 * \brief Matrix fuer den Gradienten des Potentialfeldes
		 */
		Matrix2d<float[2]>* m_grad;
		
		
		/**
		 * \var m_array
		 * \brief Datenarray, welches die Feldinformationen speichert.
		 */
		Matrix2d<SearchFieldEntry>* m_array ;
		
		/**
		 * \var m_heap
		 * \brief Heapstruktur, speichert Zeiger in das Datenarray
 		*/
		SearchFieldEntry** m_heap;
		
		
};

#endif // __SUMWARS_CORE_PATHFIND_H__

