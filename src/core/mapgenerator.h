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

#ifndef __SUMWARS_CORE_MAPGENERATOR_H__
#define __SUMWARS_CORE_MAPGENERATOR_H__

#include "matrix2d.h"
#include "unionfindtree.h"
#include "objectfactory.h"
#include "spawnpoint.h"
#include <iomanip>

#include "region.h"
#include "random.h"
#include <queue>

#ifndef PI
#define PI 3.14159
#endif

class TemplateMap
{
	public:
		/**
		 * \brief Konstruktor
		 * \param size Groesse der Region in
		 * \param dimx Groesse in y-Richtung in 4x4 Feldern
		 * \param dimy Groesse in y-Richtung in 4x4 Feldern
		 */
		TemplateMap(int dimx, int dimy);
		
		/**
		 * initialisiert die Karte
		 */
		void init(Matrix2d<char>* base_map);
		
		/**
		* \brief ermittelt einen Ort fuer das Template
		* \param shape Grundflaeche des Templates
		* \param place Rueckgabewert fuer den Ort
		* \param deco wenn auf true gesetzt, dann wird ein Platz fuer ein dekoratives Template gesucht. Auf solchen Templates koennen Spawnpunkte platziert werden
		* \return gibt true aus, wenn ein Ort gefunden wurde, sonst false
		*/
		bool getTemplatePlace( Shape* shape, Vector & place);
	
	private:
		
		/**
		 * \brief Berechnet die Groesse des groessten Templates an Position i,j neu
		 */
		void recalcMapElement(int i, int j);
		
		/**
		 * \brief Setzt Groesse des groessten Templates an Position i,j auf template_max_size
		 */
		void setMapElement(int i, int j, int template_max_size);
		
		void print();
		
		/**
		 * \brief Orte an denen Templates platziert werden koennen
		 * Map von Groesse des Templates auf einen Vector mit Positionen
		 */
		std::map< int, std::vector< std::pair<int, int> > > m_template_places;
		
		/**
		 * \var Matrix2d<char> m_template_map 
		 * \brief Karte die fuer jede Position angibt, wie gross das groesste Template ist, das an diese Position passt
		 */
		Matrix2d<int> m_template_map;
		
		/**
		 * \var Matrix2d<int>* m_template_index_map
		 * \brief enthaelt die Indizee der Punkte in der m_template_places Datenstruktur
		 */
		Matrix2d<int> m_template_index_map;
		
		/**
		 * \brief Groesse in x-Richtung in 4x4 Feldern
		 */
		int m_dimx;
		
		/**
		 * \brief Groesse in y-Richtung in 4x4 Feldern
		 */
		int m_dimy;
};

/**
 * \class MapGenerator
 * \brief Klasse zum Karten generieren
 */
class MapGenerator
{
	
	
	public:
	/**
	 * \struct MapData
	 * \brief alle Daten die waehrend des Aufbaus der Karte notwendig sind
	 */
	struct MapData
	{
		/**
		 * \var Matrix2d<char>* m_base_map
		 * \brief die grundlegende Karte. Freie Felder sind mit 1 markiert, blockierte mit -1
		 */
		Matrix2d<char>* m_base_map;

		/**
		 * \var Matrix2d<float>* m_environment
		 * \brief Karte fuer die Art der Umgebung
		 */
		Matrix2d<float>* m_environment;
		
		/**
		 * \var std::list<std::pair<int,int> > m_border
		 * \brief Menge der Punkte, die den Rand des betretbaren Gebietes bilden
		 */
		std::list<std::pair<int,int> > m_border;
		

		/**
		 * \var Region* m_region
		 * \brief die Region die erstellt wird
		 */
		Region* m_region;
		 
	};
	

	/**
	 * \fn static void  createMapData(MapData* mdata, RegionData* rdata)
	 * \brief allokiert den Speicher fuer die Karte
	 * \param rdata Daten fuer die Region die erstellt werden soll
	 * \param mdata temporaere Daten zum Aufbau der Region
	 */
	static void  createMapData(MapData* mdata, RegionData* rdata);
	
	/**
	 * \fn static void createBaseMap(MapData* mdata, RegionData* rdata)
	 * \brief erstellt die grundlegende Karte
	 * \param rdata Daten fuer die Region die erstellt werden soll
	 * \param mdata temporaere Daten zum Aufbau der Region
	 */
	static void createBaseMap(MapData* mdata, RegionData* rdata);
	
	/**
	 * \fn static bool insertGroupTemplates(MapData* mdata, RegionData* rdata)
	 * \brief Fuegt die Objektgruppen in die Region ein
	 * \param rdata Daten fuer die Region die erstellt werden soll
	 * \param mdata temporaere Daten zum Aufbau der Region
	 * \return Gibt true zurueck, wenn alle obligatorischen Objektgruppen erfolgreich eingefuegt wurden
	 */
	static bool insertGroupTemplates(MapData* mdata, RegionData* rdata);
	
	/**
	 * \brief Fuegt einen Bereich ein, in dem keine Monster platziert werden
	 * \param s blockierte Flaeche
	 */
	static void insertBlockedArea(MapData* mdata, Shape s);
	
	/**
	 * \fn static void createBorder(MapData* mdata, RegionData* rdata)
	 * \brief erstellt die Berandung
	 * \param rdata Daten fuer die Region die erstellt werden soll
	 * \param mdata temporaere Daten zum Aufbau der Region
	 */
	static void createBorder(MapData* mdata, RegionData* rdata);
	
	/**
	 * \fn static void insertSpawnpoints(MapData* mdata, RegionData* rdata)
	 * \brief Fuegt der Region die Spawnpoints hinzu
	 * \param rdata Daten fuer die Region die erstellt werden soll
	 * \param mdata temporaere Daten zum Aufbau der Region
	 */
	static void insertSpawnpoints(MapData* mdata, RegionData* rdata);
	
	/**
	 * \fn static void createExits(MapData* mdata, RegionData* rdata)
	 * \brief Fuegt der Region die Ausgaenge hinzu
	 * \param rdata Daten fuer die Region die erstellt werden soll
	 * \param mdata temporaere Daten zum Aufbau der Region
	 */
	static void createExits(MapData* mdata, RegionData* rdata);
	
	/**
	 * \fn static Region* createRegion(RegionData* rdata)
	 * \brief erzeugt aus den angegebenen Daten eine Region
	 * \param rdata Daten zur Erzeugung der Region
	 */
	static Region* createRegion(RegionData* rdata);
	
	/**
	 * \fn createPerlinNoise(Matrix2d<float> *data, int dimx, int dimy,int startfreq,float persistance, bool bounds)
	 * \brief erzeugt Zufallskarte mit dem Perlin Noise Algorithmus
	 * \param data Ausgabeparameter
	 * \param dimx Ausdehnung der Karte in x-Richtung
	 * \param dimy Ausdehnung der Karte in y-Richtung
	 * \param startfreq Startfrequenz fuer den Algorithmus. Beeinflusst die Groesse der Strukturen. Muss eine Potenz von 2 sein
	 * \param persistance Beeinflusst die Komplexitaet der Strukturen
	 * \param bounds bestimmt, ob die Karte eine Begrenzung hat
	 */
	static void createPerlinNoise(Matrix2d<float> *data, int dimx, int dimy,int startfreq, float persistance, bool bounds);
};

struct WeightedLocation
{
	
	int m_x;
	int m_y;
	
	float m_value;
	
};

extern "C"
{
	bool operator<(WeightedLocation first, WeightedLocation second);
}



#endif // __SUMWARS_CORE_MAPGENERATOR_H__

