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

#ifndef REGION_H
#define REGION_H

#include "event.h"
#include "matrix2d.h"
#include "worldobject.h"
#include "dropitem.h"
#include "networkstruct.h"
#include "projectile.h"
#include "objectfactory.h"
#include "dialogue.h"
#include "minimap.h"



#include <string>
#include <list>
#include <map>
#include <set>
#include <math.h>
#include <cstdlib>
#include <algorithm>

struct Gridunit;
class Region;

typedef std::string RegionName;

typedef std::string MusicTrack;

typedef std::pair<RegionName, LocationName> RegionLocation;

/**
 * \struct FloatingText
 * \brief Objekt fuer creating colored one-line text overlays, mainly for damage visualization
 */
struct FloatingText
{
	/**
	 * \brief Size of the text
	 */
	enum Size
	{
		SMALL = 1,
		NORMAL = 2,
		BIG = 3,
	};
	
	/**
	 * \brief displayed Text
	 */
	TranslatableString m_text;
	
	/**
	 * \brief remaining time the text will stay visible
	 */
	float m_time;
	
	/**
	 * \brief overall text display time
	 */
	float m_maxtime;
	
	/**
	 * \brief maximal offset of the  text when floating upwards 
	 */
	float m_float_offset;
	
	/**
	 * \brief size of the text
	 */
	Size m_size;
	
	/**
	 * \brief Dargestellter Ort
	 */
	Vector m_position;
	
	/**
	 * \brief Colour of the text as hexadecimal AARRGGBB
	 */
	std::string m_colour;
};

/**
 * \brief Structure for a sound played in the region
 */
struct PlayedSound
{
	/**
	 * \brief Name of the sound
	 */
	std::string m_soundname;
	
	/**
	 * \brief Position where the sound should be played
	 */
	Vector m_position;
	
	/**
	 * \brief Volume of the sound
	 */
	float m_volume;
	
	/**
	 * \brief if set to true, this sound will be played on all clients
	 */
	bool m_global;
};


/**
 * \enum ExitDirection
 * \brief Verschiedene Richtungen in denen man die Region verlassen kann
 */
enum ExitDirection
{
	SOUTH = 3,
	NORTH = 2,
	WEST = 0, 
	EAST = 1,
};

/**
 * \struct RegionExit
 * \brief Struktur die einen Ausgang aus der Region beschreibt
 */
struct RegionExit
{
	/**
	 * \var Shape m_shape
	 * \brief Form der Flaeche, in der der Spieler stehen muss um die Region zu verlassen
	 */
	Shape m_shape;
	
	/**
	 * \var LocationName m_exit_name
	 * \brief Name des Ortes, an dem sich der Ausgang befindet
	 */
	LocationName m_exit_name;
	
	/**
	 * \var std::string m_destination_region
	 * \brief Name der Zielregion
	 */
	std::string m_destination_region;
	
	/**
	 * \var LocationName m_destination_location
	 * \brief Name des Zielpunktes in der Zielregion
	 */
	LocationName m_destination_location;
};

/**
 * \class RegionData
 * \brief enthaelt alle Daten die noetig sind um die Region zu erstellen
 */
class RegionData
{
	public:
		
		/**
		 * \struct ObjectGroupSet
		 * \brief Struktur fuer eine Gruppe von Objekten die mehrmals in die Region eingefuegt werden soll
		 */
		struct ObjectGroupSet
		{
			/**
			 * \var ObjectGroupName m_group_name
			 * \brief Name der Gruppe
			 */
			ObjectGroupName m_group_name;
			
			/**
			 * \var int m_number
			 * \brief Anzahl wie oft man versuchen soll, eine Gruppe eingefuegen
			 */
			int m_number;
			
			/**
			 * \var float m_probability
			 * \brief Wahrscheinlichkeit mit der eine Gruppe eingefuegt wird
			 * es werden m_number viele Orte ausgewaehlt, aber fuer jeden nur mit dieser Warscheinlichkeit wirklich die Gruppe eingefuegt
			 */
			float m_probability;
			
			/**
			 * \var bool m_decoration
			 * \brief Gibt an, ob es sich um ein rein dekoratives Template handelt. Auf diesen Templates koennen auch Monstergruppen platziert werden
			 */
			bool m_decoration;
		};
		
		/**
		 * \fn struct NamedObjectGroup
		 * \brief Struktur fuer eine Objektgruppe deren Instanz mit einem Namen versehen werden soll
		 */
		struct NamedObjectGroup
		{
			/**
			 * \var ObjectGroupName m_group_name
			 * \brief Name der Gruppe
			 */
			ObjectGroupName m_group_name;
			
			/**
			 * \var std::string m_name
			 * \brief Name der Gruppe
			 */
			std::string m_name;
			
			/**
			 * \var float m_angle
			 * \brief Winkel mit dem das Template eingefuegt wird
			 */
			float m_angle;
		};
		
		/**
		 * \fn struct SpawnGroup
		 * \brief Gruppe von Monstern die in der Region auftaucht
		 */
		struct SpawnGroup
		{
			/**
			 * \var MonsterGroupName m_monsters
			 * \brief Name der Monstergruppe
			 */
			MonsterGroupName m_monsters;
			
			/**
			 * \var int m_number
			 * \brief Anzahl wie oft die Gruppe auftaucht
			 */
			int m_number;
			
		};
		
		/**
		 * \fn RegionData()
		 * \brief Konstruktor
		 */
		RegionData();
		
		/**
		 * \fn ~RegionData()
		 * \brief Destruktor
		 */
		~RegionData();
		
		/**
		 * \fn void addObjectGroup(ObjectGroupName m_group_name, int prio, int number =1, float probability=1.0, bool decoration=false)
		 * \brief Fuegt eine neue Objektgruppe ein
		 * \param group_name Name der Gruppe
		 * \param prio Prioritaet der Gruppe
		 * \param number Anzahl wie oft die Gruppe eingefuegt wird
		 * \param probability Wahrscheinlichkeit mit der die Gruppe eingefuegt wird
		 * \param decoration Gibt an, ob es sich um ein rein dekoratives Template handelt
		 */
		void addObjectGroup(ObjectGroupName group_name, int prio=0, int number =1, float probability=1.0, bool decoration=false);
		
		/**
		 * \fn void  addNamedObjectGroup(ObjectGroupName group_name, std::string name,float angle=0, int prio=0)
		 * \brief Fuegt eine neue Objektgruppe ein
		 * \param group_name Name der Gruppe
		 * \param name Name unter dem die Gruppe eingefuegt wird
		 * \param angle Winkel unter dem das Template eingefuegt wird
		 * \param prio Prioritaet der Gruppe
		 */
		void addNamedObjectGroup(ObjectGroupName group_name, std::string name, float angle=0, int prio=0)
		{
			NamedObjectGroup og;
			og.m_name = name;
			og.m_group_name = group_name;
			og.m_angle = angle;
			m_named_object_groups.insert( std::make_pair( prio, og));
		}
		
		/**
		 * \fn void addSpawnGroup(MonsterGroupName group_name, int number =1)
		 * \brief Fuegt eine neue Monstergruppe zu den in der Region auftauchenden Monstern hinzu
		 * \param group_name Name der Monstergruppe
		 * \param number Gibt an wie oft die Gruppe auftauchen soll
		 */
		void addSpawnGroup(MonsterGroupName group_name, int number =1)
		{
			SpawnGroup sg;
			sg.m_monsters = group_name;
			sg.m_number =number;
			m_monsters.push_back(sg);
		}
		
		/**
		 * \fn void addEnvironment(float maxheight, EnvironmentName env)
		 * \brief Fuegt eine neue Umgebung fuer die Region ein
		 * \param maxheight maximale Hoehe bis zu der diese Umgebung verwendet wird
		 * \param env Name der Umgebung
		 */
		void addEnvironment(float maxheight, EnvironmentName env)
		{
			m_environments.push_back(std::make_pair(maxheight,env));
		}
		
		/**
		 * \fn void addExit(RegionExit exit)
		 * \brief Fuegt einen Ausgang hinzu
		 * \param exit Ausgang
		 */
		void addExit(RegionExit exit)
		{
			m_exits.push_back(exit);
		}
		
		/**
		 * \fn void addEvent(TriggerType type, Event* event)
		 * \brief Fuegt ein neues Event hinzu
		 * \param trigger Typ des Triggers durch den das Event ausgeloest wird
		 * \param event Event
		 */
		void addEvent(TriggerType trigger, Event* event);
		
		/**
		 * \var short m_id
		 * \brief Nummer der Region
		 */
		short m_id;
		
		/**
		 * \var std::string m_name
		 * \brief Name der Region
		 */
		std::string m_name;
		
		/**
		 * \var m_dimx
		 * \brief Ausdehnung der Region in x-Richtung
		 */
		short m_dimx;
	
		/**
		 * \var m_dimy
		 * \brief Ausdehnung der Region in y-Richtung
		 */
		short m_dimy;
		
		/**
		 * \var float m_area_percent
		 * \brief Gibt Anteil der Flaeche an, die tatsaechlich zugaenglich sein soll
		 */
		float m_area_percent;
		
		/**
		 * \var float m_complexity
		 * \brief Komplexitaet der Berandung (Wert zwischen 0 und 1)
		 */
		float m_complexity;
		
		/**
		 * \var int m_granularity
		 * \brief Groesse der groessten Strukturen - muss eine Zweierpotenz sein
		 */
		int m_granularity;
		
		/**
		 * \var bool m_exit_directions[4]
		 * \brief Richtungen in denen ein Ausgang aus der Region erstellt wird
		 */
		bool m_exit_directions[4];
		
		/**
		 * \var std::list<RegionExit> m_region_exits
		 * \brief Liste der Ausgaenge aus der Region
		 */
		std::list<RegionExit> m_exits;
		
		/**
		 * \var std::multimap<int,ObjectGroupSet> m_object_groups
		 * \brief die Patterns sortiert nach einer Prioritaet
		 */
		std::multimap<int,ObjectGroupSet> m_object_groups;
		
		/**
		 * \var std::multimap<int, NamedObjectGroup > m_named_object_groups
		 * \brief Objektgruppen mit einem Namen. Diese Objektgruppen werden immer zuerst eingefuegt und sind verpflichtend
		 */
		std::multimap<int, NamedObjectGroup > m_named_object_groups;
		
		/**
		 * \var std::list<std::pair<float, EnvironmentName> > m_environments
		 * \brief Liste von moeglichen Umgebungen mit ihren Hoehenlinien
		 */
		std::list<std::pair<float, EnvironmentName> > m_environments;
		
		
		/**
		 * \var list<SpawnGroup> m_monsters
		 * \brief Liste der Monster die in der Region auftauchen
		 */
		std::list<SpawnGroup> m_monsters;
		
		
		/**
		 * \var LocationName m_revive_location
		 * \brief Ort an dem in dieser Region gestorbene Helden wiedererweckt werden
		 */
		LocationName m_revive_location;
		
		/**
		 * \var std::multimap<TriggerType, Event*> m_events
		 * \brief Events der Region
		 */
		std::multimap<TriggerType, Event*> m_events;
		
		/**
		 * \var std::string m_ground_material
		 * \brief Material mit dem der Boden gerendert wird
		 */
		std::string m_ground_material;
		
		/**
		 * \var ObjectGroupName m_region_template
		 * \brief Ist gesetzt, wenn die komplette Region aus einem Template besteht
		 */
		ObjectGroupName m_region_template;
		
		/**
		 * \var float m_ambient_light[3]
		 * \brief Werte fuer ambientes Licht
		 */
		float m_ambient_light[3];
		
		/**
		 * \var float m_hero_light[3]
		 * \brief Werte fuer das Spotlicht des Helden
		 */
		float m_hero_light[3];
		
		/**
		 * \var float m_directional_light[3]
		 * \brief Werte fuer das Licht von Sonne/Mond
		 */
		float m_directional_light[3];
		
		/**
		 * \var bool m_has_waypoint
		 * \brief auf true gesetzt, wenn die Region einen Wegpunkt hat
		 */
		bool m_has_waypoint;
		
		/**
		 * \brief Liste von moeglichen Musikstuecken fuer die Region
		 */
		std::list<MusicTrack> m_music_tracks;
};

/**
 * \class RegionCamera
 * \brief Zeigt an, aus welcher Position die Szene bei Zwischenszenen betrachtet wird
 */
class RegionCamera
{
	public:
		/**
		* \struct Position
		* \brief Speichert ein Position der Kamera
		*/
		struct Position
		{
			/**
			* \var float m_distance
			* \brief Abstand zum Spieler
			*/
			float m_distance;
			
			/**
			* \var float m_theta
			* \brief Winkel zum Boden
			*/
			float m_theta;
			
			/**
			* \var float m_phi
			* \brief Drehwinkel um die z-Achse
			*/
			float m_phi;
		
			/**
			* \var Vector m_focus
			* \brief Vector, auf den die Kamera fokussiert ist
			*/
			Vector m_focus;
		};
	
		/**
		 * \fn void addPosition(Position& pos, float time)
		 * \brief Fuegt zur Kamerabewegung einen weiteren Punkt hinzu
		 * \param pos Position die erreicht werden soll
		 * \param time Zeit in ms, die vergeht, bis ausgehend von der vorhergehenden Position die angegebenen Position erreicht wird
		 */
		void addPosition(Position& pos, float time);
		
	
		/**
		 * \fn void update(float time)
		 * \brief aktualisiert die Kamerastellung nachdem eine gewisse Zeit vergangen ist
		 * \param time Zeit in ms
		 */
		void update(float time);
		
		/**
		 * \fn void toString(CharConv* cv)
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
		
		
		/**
		 * \var Position m_position
		 * \brief aktuelle Position
		 */
		Position m_position;
		
		/**
		 * \var std::list<std::pair<Position,float> > m_next_positions
		 * \brief Positionen, die als naechstes eingenommen werden
		 */
		std::list<std::pair<Position,float> > m_next_positions;
		
		/**
		 * \var  Region* m_region
		 * \brief Region zu der die Kamera gehoert
		 */
		Region* m_region;
};

class RegionLight
{
	public:
	
		/**
		 * \brief Konstruktor
		 * \param region Zeiger auf die Region zu der das Licht gehoert
		 */
		RegionLight(Region* region)
		{
			m_region = region;
		}
		
		/**
		 * \brief Struktur fuer eine einzelne Lichtart
		 */
		struct Light
		{
			/**
			 * \brief Konstruktor
			 */
			Light()
			{
				m_timer =0;
			}
			
			/**
			 * \brief aktualisiert das Objekt nachdem eine gewissen Zeit vergangen ist
			 * \param time Zeit in Millisekunden
			 */
			void update(float time);
			
			/**
			* \brief Farbe des Lichts in RGB
			*/
			float m_value[3];
			
			/**
			* \brief Zielfarbe des Lichts in RGB
			*/
			float m_goal_value[3];
			
			/**
			* \brief Millisekunden bis die Zielfarbe erreicht wird
			*/
			float m_timer;
			
			/**
			 * \fn void toString(CharConv* cv)
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
		};
		
		/**
		 * \brief aktualisiert das Objekt nachdem eine gewissen Zeit vergangen ist
		 * \param time Zeit in Millisekunden
		 */
		void update(float time)
		{
			m_ambient_light.update(time);
			m_hero_light.update(time);
			m_directional_light.update(time);
		}
		
		/**
		* \fn float* getAmbientLight()
		* \brief Gibt ambiente Lichtstaerke aus
		*/
		float* getAmbientLight()
		{
			return m_ambient_light.m_value;
		}
			
		/**
		* \fn float* getHeroLight()
		* \brief Gibt Lichtstaerke des Heldeslichtes aus
		*/
		float* getHeroLight()
		{
			return m_hero_light.m_value;
		}
			
		/**
		* \fn float* getDirectionalLight()
		* \brief Gibt Lichtstaerke des gerichteten Lichts aus
		*/
		float* getDirectionalLight()
		{
			return m_directional_light.m_value;
		}
		
		/**
		 * \brief initialisiert die Werte
		 * \param ambient Werte fuer ambientes Licht
		 * \param herolight Werte fuer Heldenlicht
		 * \param directional Werte fuer gerichtetes Licht
		 */
		void init(float* ambient, float* herolight, float* directional);
		
		/**
		 * \brief Setzt die Werte fuer Lichtart, an die sich die Lichtstaerke annaehern soll
		 * \param lighttype Typ des Lichts, muss "ambient", "hero", oder "directional" sein
		 * \param value Werte des Lichts als RGB
		 * \param time Zeitdauer bis die angegebene Lichtstaerke erreicht wird
		 */
		void setLight(std::string lighttype, float* value, float time);
		
		/**
		 * \fn void toString(CharConv* cv)
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
	
	private:
		/**
		 * \brief Werte fuer ambientes Licht
		 */
		Light m_ambient_light;
		
		/**
		 * \brief Werte fuer das Spotlicht des Helden
		 */
		Light m_hero_light;
		
		/**
		 * \var float m_directional_light
		 * \brief Werte fuer das Licht von Sonne/Mond
		 */
		Light m_directional_light;
		
		/**
		 * \brief Zeiger auf die Region
		 */
		Region* m_region;
};

	
/**
 * \class Region
 * \brief Eine Region ist ein rechteckiges Teilstueck der Spielwelt. Monster und Geschosse koennen nicht zwischen Regionen wechseln
 */
class Region
{
	public:
		/**
	 * \fn Region(short dimx, short dimy, short id, std::string name, RegionData* data=0)
		 * \brief Konstruktor
		 * \param dimx Ausdehnung in x-Richtung
	 	 * \param dimy Ausdehnung in y-Richtung
		 * \param id ID der Region
		 * \param name Name der Region
		 * \param data Daten zur Initialisierung
	 	 */
		Region(short dimx, short dimy, short id, std::string name, RegionData* data=0);


		/**
		 * \fn ~Region()
		 * \brief Destruktor
		 */
		~Region();

		/**
		 * \fn insertObject (WorldObject* object, Vector pos, float angle=0)
		 * \brief Fuegt ein bereits existierendes WorldObject ein
		 * \param object Zeiger auf das Objekt, welches eingefuegt werden soll
		 * \param pos Position an der das Objekt eingefuegt wird
		 * \param angle Drehwinkel des Objektes
		 * \return bool, der angibt, ob die Operation erfolgreich war
		 */
		bool insertObject (WorldObject* object, Vector pos, float angle=0);
		
		/**
		 * \fn int createObject(ObjectTemplateType generictype, Vector pos, float angle=0, float height=0, WorldObject::State state = WorldObject::STATE_ACTIVE)
		 * \brief Erstellt ein neues Objekt und fuegt es an der angegebenen Stelle ein
		 * \param type Typ des neuen Objekts
		 * \param generictype Subtyp des neuen Objekts, kann generisch sein
		 * \param pos Position an der das Objekt eingefuegt wird
		 * \param angle Drehwinkel des Objektes
		 * \param state Zustand in dem das Objekt erzeugt wird
		 * \param height Hoehe des Objekts
		 * \return ID des neu erstellten Objekts
		 */
		int createObject(ObjectTemplateType generictype, Vector pos, float angle=0, float height=0, WorldObject::State state = WorldObject::STATE_NONE);
		
		/**
		 * \fn void createObjectGroup(ObjectGroupName templname, Vector position, float angle=0, std::string name="",WorldObject::State state = WorldObject::STATE_NONE)
		 * \brief erzeugt eine Gruppe von Objekten und fuegt sie in die Region ein
		 * \param templname Name des Templates aus dem die Gruppe erzeugt wird
		 * \param position Ort an dem die Gruppe eingefuegt wird
		 * \param angle Drehwinkel mit dem die Gruppe eingefuegt wird
		 * \param Name des Ortes
		 * \param state Zustand in dem die Objekte erzeugt werden
		 */
		void createObjectGroup(ObjectGroupName templname, Vector position, float angle=0, std::string name ="",WorldObject::State state = WorldObject::STATE_NONE );
		
		/**
		 * \fn void createMonsterGroup(MonsterGroupName mgname, Vector position, float radius=3, std::list<int>* monsters=0)
		 * \brief erzeugt an der angegebenen Stelle eine Monstergruppe
		 * \param mgname Name der Monstergruppe
		 * \param position Ort an dem die Gruppe erzeugt wird
		 * \param radius Umkreis in dem die Monster verteilt werden
		 * \param monsters Wenn eine Liste uebergeben wird, so werden die IDs der erzeugten Monster in die Liste eingefuegt
		 */
		void createMonsterGroup(MonsterGroupName mgname, Vector position, float radius=3, std::list<int>* monsters=0);

		/**
		 * \fn bool getObjectsInShape( Shape* shape,  WorldObjectList* result,short layer=WorldObject::LAYER_ALL, short group = WorldObject::GROUP_ALL, WorldObject* omit=0, bool empty_test = false );
		 * \brief Sucht WorldObjects innerhalb eines Gebietes
		 * \param shape Form des Gebietes
		 * \param layer Ebene in der gesucht wird
		 * \param result Liste, an die die gefundenen Objekte angehangen werden
		 * \param omit Objekt, das ausgelassen wird
		 * \param empty_test wenn true, wird nach dem ersten gefundenen Objekt abgebrochen
		 * \param group Gruppen die durchsucht werden sollen
		 * \return bool, der angibt, ob die Operation erfolgreich war
		 *
 		 */
		bool getObjectsInShape( Shape* shape,  WorldObjectList* result,short layer=WorldObject::LAYER_ALL, short group = WorldObject::GROUP_ALL, WorldObject* omit=0, bool empty_test = false );

		/**
		 * \fn bool getFreePlace(Shape* shape, short layer, Vector& pos, WorldObject* omit=0)
		 * \brief Sucht nach einem freien Platz fuer ein Objekt moeglichst nahe an den angegebenen Koordinaten
		 * \param shape Form des Objekts
		 * \param layer Ebene des Objekts
		 * \param pos Eingabe: Zielpunkt, Ausgabe, tatsaechlich gefundener Ort
		 * \param omit Objekt, das ausgelassen wird
		 * \return true, wenn ein freier Platz gefunden wurde, sonst false
		 */
		bool getFreePlace(Shape* shape, short layer, Vector& pos, WorldObject* omit=0);

		/**
		 * \fn bool addObjectsInShapeFromGridunit(Shape* shape, Gridunit* gu, WorldObjectList* result, short layer=WorldObject::LAYER_ALL, short group = WorldObject::GROUP_ALL,WorldObject* omit=0, bool empty_test = false )
		 * \brief Fuegt alle Objekte aus der Gridunit, die sich mit dem Gebiet befinden zu der Liste hinzu
		 * \param layer Ebene in der gesucht wird
		 * \param result Liste, an die die gefundenen Objekte angehangen werden
		 * \param omit Objekt, das ausgelassen wird
		 * \param empty_test wenn true, wird nach dem ersten gefundenen Objekt abgebrochen
		 * \param group Gruppen die durchsucht werden sollen
		 * \param shape Es werden nur Objekte hinzugefuegt die diese Flaeche beruehren
		 * \param gu Gridunit aus der Objekte hinzugefuegt werden
		 * \return bool, der angibt, ob die Operation erfolgreich war
		 **/
		bool addObjectsInShapeFromGridunit(Shape* shape, Gridunit* gu, WorldObjectList* result, short layer=WorldObject::LAYER_ALL, short group = WorldObject::GROUP_ALL,WorldObject* omit=0, bool empty_test = false );

		/**
		 * \fn bool addObjectsOnLineFromGridunit(Line& line, Gridunit* gu, WorldObjectList* result, short layer=WorldObject::LAYER_ALL, short group = WorldObject::GROUP_ALL,WorldObject* omit=0, bool empty_test = false )
		 * \brief Fuegt alle Objekte aus der Gridunit, die sich in einer Flaeche befinden zu der Liste hinzu (intern)
		 *  \param layer Ebene in der gesucht wird
		 * \param result Liste, an die die gefundenen Objekte angehangen werden
		 * \param omit Objekt, das ausgelassen wird
		 * \param empty_test wenn true, wird nach dem ersten gefundenen Objekt abgebrochen
		 * \param group Gruppen die durchsucht werden sollen
		 * \param line Linie
		 * \param gu Gridunit aus der Objekte hinzugefuegt werden
		 * \return bool, der angibt, ob die Operation erfolgreich war
		 **/
		bool addObjectsOnLineFromGridunit(struct Line& line, Gridunit* gu, WorldObjectList* result, short layer=WorldObject::LAYER_ALL, short group = WorldObject::GROUP_ALL,WorldObject* omit=0, bool empty_test = false );

		/**
		 * \fn bool changeObjectGroup(WorldObject* object,WorldObject::Group group )
		 * \brief Veraendert die Gruppe des Objektes
		 * \param object Objekt fuer das die Gruppe geaendert werden soll
		 * \param group neue Gruppe des Objektes
		 */
		bool changeObjectGroup(WorldObject* object,WorldObject::Group group );
		
		/**
		 * \fn bool changeObjectLayer(WorldObject* object,WorldObject::Layer layer)
		 * \brief Aendert die Ebene eines Objektes
		 * \param object Object fuer das die Ebene geaendert werden soll
		 * \param layer neue Ebene des Objektes
		 */
		bool changeObjectLayer(WorldObject* object,WorldObject::Layer layer);

		/**
		 * \fn WorldObject* getObject ( int id)
		 * \brief Sucht Objekt anhand seiner ID heraus
		 * \param id ID des Objekts
		 * \return Objekt, Nullzeiger wenn das Objekt nicht existiert
		 */
		WorldObject* getObject ( int id);

		/**
		 * \fn WorldObject* getObjectAt(Vector pos, short layer=WorldObject::LAYER_ALL, short group = WorldObject::GROUP_ALL);
		 * \brief Sucht WorldObject an den gegebenen Koordinaten
		 * \param pos Ort an dem gesucht wird
		 * \param layer Ebene in der gesucht wird
		 * \param group Gruppen die durchsucht werden sollen
		 * \return Zeiger auf das WorldObject, wenn sich an dem Punkt eines befindet, sonst NULL
		 */
		WorldObject* getObjectAt(Vector pos, short layer=WorldObject::LAYER_ALL, short group = WorldObject::GROUP_ALL);

		/**
		 * \fn void getObjectsOnLine( Line & line,  WorldObjectList* result,short layer=WorldObject::Geometry::LAYER_ALL, short group = WorldObject::GROUP_ALL, WorldObject* omit=0 )
		 * \brief Sucht alle Objekte die auf der angegebenen Linie liegen
		 * \param line Linie auf der gesucht wird
		 * \param result Liste, an die die gefundenen Objekte angehangen werden
		 * \param layer Ebene in der gesucht wird
		 * \param group Gruppen die durchsucht werden sollen
		 * \param omit Objekt, das ausgelassen wird
		 */
		void getObjectsOnLine(struct Line& line,  WorldObjectList* result,short layer=WorldObject::LAYER_ALL, short group = WorldObject::GROUP_ALL, WorldObject* omit=0 );


		/**
		 * \fn bool  insertProjectile(Projectile* object, Vector pos)
		 * \brief Fuegt ein Projektil ein
		 * \param object Zeiger auf das Objekt, welches eingefuegt werden soll
		 * \param pos Position an der das Objekt eingefuegt wird
		 * \return bool, der angibt, ob die Operation erfolgreich war
		 *
		 */
		bool  insertProjectile(Projectile* object, Vector pos);

		/**
		 * \fn Projectile* getProjectile(int id)
		 * \brief Gibt das Projektil mit der angegebenen ID aus
		 */
		Projectile* getProjectile(int id);

		/**
		 * \fn deleteObject(WorldObject* object)
		 * \brief L&ouml;scht WorldObject
		 * \param object Zeiger auf das Objekt, welches gel&ouml;scht werden soll
		 * \return bool, der angibt, ob die Operation erfolgreich war
		 *
		 * L&ouml;scht das WorldObject aus den internen Datenstrukturen. Das Objekt selbst wird nicht gel&ouml;scht, es wird nur die Einbindung in die Datenstrukturen entfernt. Wenn das L&ouml;schen erfolgreich war, so wird true zur&uuml;ckgegeben, sonst false.
		 * Wenn ein NULL-Zeiger &uuml;bergeben wird, so wird false ausgegeben.
		 */
		bool  deleteObject (WorldObject* object);

		/**
		 * \fn bool deleteObject(int id)
		 * \brief Loescht das Objekt mit der angegebenen ID
		 */
		bool deleteObject(int id);
		
		 /**
		 * \fn moveObject(WorldObject* object, Vector newpos)
		 * \brief verschiebt ein WorldObject an den Punkt (x,y)
		 * \param object Zeiger auf das Objekt, welches verschoben werden soll
		 * \param newpos neue Koordinaten des Objekts
		 * \return bool, welcher angibt, ob das Verschieben erfolgreich war
		  */
		bool moveObject(WorldObject* object, Vector newpos);

		/**
		 * \fn void createObjectFromString(CharConv* cv, WorldObjectMap* players)
		 * \brief liest die Daten zu einem Objekt aus dem Bitstream und erzeugt es
		 * \param cv Bitstream aus dem die Daten gelesen werden
		 * \param players Spielerdaten (Spieler werden nicht neu erzeugt sondern nur in die Region verschoben)
		 */
		void createObjectFromString(CharConv* cv, WorldObjectMap* players);

		/**
		 * \fn void createProjectileFromString(CharConv* cv)
		 * \brief liest die Daten zu einem Projektil aus dem Bitstream und erzeugt es
		 */
		void createProjectileFromString(CharConv* cv);

		/**
		 * \fn void createItemFromString(CharConv* cv)
		 * \brief liest die Daten zu einem Gegenstand aus dem Bitstream und erzeugt es
		 */
		void createItemFromString(CharConv* cv);
		
		/**
		 * \fn void createDialogueFromString(CharConv* cv)
		 * \brief Liest einen Dialog aus dem Bitstream und erzeugt ihn
		 */
		void createDialogueFromString(CharConv* cv);

		/**
		 * \fn void deleteProjectile(Projectile* proj)
		 * \brief Entfernt das Projektil aus der Region
		 */
		void deleteProjectile(Projectile* proj);

		/**
		 * \fn update(float time)
		 * \brief Lässt für alle Objekte in der Welt die angegebene Zeitspanne verstreichen
		 * \param time Zeit um die in der Welt vergeht in Millisekunden
		 */
		void update(float time);

		/**
		 * \fn bool dropItem(Item* item, Vector pos)
		 * \brief Laesst Item in der Region fallen
		 * \param item Gegenstand
		 * \param pos Ort an dem der Gegenstand fallen gelassen wird
		 */
		bool dropItem(Item* item, Vector pos);

		/**
		 * \fn bool dropItem(Item::Subtype subtype, Vector pos,int magic_power=0)
		 * \brief Erzeugt ein Item mit dem angegebenen Typ und laesst es fallen
		 * \param subtype Typ des Gegenstandes
		 * \param pos Position an der der Gegenstand fallen gelassen wird
		 * \param magic_power Verzauberung des Gegenstandes
		 */
		bool dropItem(Item::Subtype subtype, Vector pos, int magic_power=0);
		
		/**
		 * \fn Item* getItemAt(Vector pos)
		 * \param pos Ort an dem gesucht wird
		 * \brief Gibt Item an der angegebenen Position aus. Gibt NULL aus, wenn dort kein Item ist
		 */
		Item* getItemAt(Vector pos);

		/**
		 * \fn bool deleteItem(int id, bool delitem)
		 * \brief Entfernt Item aus der Region
		 * \param id ID des items
		 * \param delitem Wenn true, wird das Item geloescht, sonst nur aus der Region entfernt
		 */
		bool deleteItem(int id, bool delitem=false);

		/**
		 * \fn Item* getItem(int id)
		 * \brief Gibt Item aus. Gibt NULL aus, wenn dort kein Item ist
		 */
		Item* getItem(int id);

		/**
		 * \fn Item* getDropItem(int id)
		 * \brief Gibt Item mit den Information wo es liegt aus. Gibt NULL aus, wenn dort kein Item ist
		 */
		DropItem* getDropItem(int id);

		/**
		 * \fn GameObject* getGameObject(int id)
		 * \brief Gibt das Objekt mit der angegebenen Id  aus
		 * \param id ID
		 */
		GameObject* getGameObject(int id);
		
		/**
		 * \fn void getRegionData(CharConv* cv)
		 * \brief Schreibt alle Objekte, Projektile, Items und Tiles in einen String
		 * \param cv Eingabepuffer
		 * \return Zeiger hinter den beschriebenen Bereich
		 */
		void getRegionData(CharConv* cv);

		/**
		 * \fn void setRegionData(CharConv* cv, WorldObjectMap* players)
		 * \brief Liest die Objekte, Projektile, Items und Tiles aus einem String ein
		 * \param cv Eingabepuffer
		 * \param players Liste der Spieler (die Spieler werden nicht neu erzeugt, sondern nur eingefuegt)
		 */
		void setRegionData(CharConv* cv, WorldObjectMap* players);

		/**
		 * \fn void getRegionCheckData(CharConv* cv)
		 * \brief Gibt die Daten aus, mit denen sich das Inventar der Welt ueberpruefen laesst
		 * \param cv Eingabepuffer
		 */
		void getRegionCheckData(CharConv* cv);
		
		/**
		 * \fn void checkRegionData(CharConv* cv)
		 * \brief Prueft die Daten der Region mit Hilfe der im Puffer vorliegenden Daten
		 * \param cv Ausgabepuffer
		 */
		void checkRegionData(CharConv* cv);
		

		/**
		 * \fn Minimap* createMinimap()
		 * \brief Erzeugt eine Minimap fuer die Region
		 */
		Minimap* createMinimap()
		{
			return new Minimap(m_dimx,m_dimy);
		}
		
		/**
		 * \fn WorldObjectMap* getPlayers()
		 * \brief Gibt alle Spieler in der Region aus
		 */
		WorldObjectMap* getPlayers()
		{
			return m_players;
		}


		/**
		 * \fn DropItemMap* getDropItems()
		 * \brief Gibt alle auf dem Boden liegenden Items aus
		 */
		DropItemMap* getDropItems()
		{
			return m_drop_items;
		}

		/**
		 * \fn ProjectileMap* getProjectiles()
		 * \return Gibt alle Projektile in der Region aus
		 */
		ProjectileMap* getProjectiles()
		{
			return m_projectiles;
		}
		
		/**
		 * \fn GameObjectMap& getGameObjects()
		 * \brief Gibt die Datenstruktur mit allen Objekten aus
		 */
		GameObjectMap& getGameObjects()
		{
			return m_game_objects;
		}
		
		/**
		 * \fn WorldObjectMap* getStaticObjects()
		 * \brief Gibt die statischen Objekte aus
		 */
		WorldObjectMap* getStaticObjects()
		{
			return m_static_objects;
		}

		/**
		 * \fn void insertNetEvent(NetEvent &event)
		 * \brief Fuegt ein neues NetEvent in die NetEventliste ein
		 */
		void insertNetEvent(NetEvent &event)
		{
			m_netevents->push_back(event);
		}
		
		/**
		 * \fn void insertEnvironment(float maxheight, EnvironmentName env)
		 * \brief Fuegt eine neue Umgebung in die Region ein
		 * \param maxheight maximale Hoehe bis zu der diese Umgebung verwendet wird
		 * \param env Name der Umgebung
		 */
		void insertEnvironment(float maxheight, EnvironmentName env)
		{
			m_environments.insert(std::make_pair(maxheight,env));
		}

		/**
		 * \fn NetEventList* getNetEvents()
		 * \brief Gibt die Liste der NetEvents aus
		 */
		NetEventList* getNetEvents()
		{
			return m_netevents;
		}
		
		/**
		 * \fn Matrix2d<float>* getHeight()
		 * \brief Gibt die Hoehenkarte aus
		 */
		Matrix2d<float>* getHeight()
		{
			return m_height;
		}
		
		/**
		 * \fn EnvironmentName getEnvironment(Vector pos)
		 * \brief Gibt die Umgebung aus, die an der angegebenen Stelle vorherrscht
		 * \param pos Ort
		 */
		EnvironmentName getEnvironment(Vector pos);
		
		/**
		 * \fn void addLocation(LocationName name, Vector pos)
		 * \brief Fuegt einen neuen Ort ein
		 * \param name Name des Ortes
		 * \param pos Koordinaten des Ortes
		 */
		void addLocation(LocationName name, Vector pos)
		{
			m_locations.insert(std::make_pair(name,pos));
		}
		
		/**
		 * \fn void setLocation(LocationName name, Vector pos)
		 * \brief set the Position of a location
		 * \param name name of the location
		 * \param pos coordinates of the location
		 * In contrast to \a addLocation this function overwrites existing locations
		 */
		void setLocation(LocationName name, Vector pos)
		{
			m_locations[name] = pos ;
		}
		
		/**
		 * \fn bool hasLocation(LocationName loc)
		 * \brief Gibt an, ob die Region einen Ort mit dem angegebenen Namen hat
		 * \param loc Ort
		 */
		bool hasLocation(LocationName loc)
		{
			return (m_locations.count(loc) > 0);
		}
		
		/**
		 * \fn Vector getLocation(LocationName name)
		 * \brief Gibt die Position eines Ortes aus
		 * \param name Name des Ortes
		 */
		Vector getLocation(LocationName name);
		
		/**
		 * \fn void addArea(AreaName name, Shape area)
		 * \brief Fuegt eine neue Flaeche ein
		 */
		void addArea(AreaName name, Shape area)
		{
			m_areas.insert(std::make_pair(name,area));	
		}
		
		/**
		 * \fn Shape getArea(AreaName name)
		 * \brief Gibt die Flaeche mit dem angegebenen Name aus
		 */
		Shape getArea(AreaName name);
		
		/**
		 * \fn std::string getName()
		 * \brief Gibt den Name der Region aus
		 */
		std::string getName()
		{
			return m_name;
		}
		
		/**
		 * \fn void addExit(RegionExit exit)
		 * \brief Fuegt einen Ausgang hinzu
		 * \param exit Ausgang
		 */
		void addExit(RegionExit exit);
		
		/**
		 * \fn short getId()
		 * \brief Gibt die ID der Region aus
		 */
		short getId()
		{
			return m_id;
		}
		
		/**
		 * \fn void setReviveLocation(LocationName loc)
		 * \brief Setzt den Ort an dem in dieser Region gestorbene Helden wiederbelebt werden
		 * \param loc Name des Ortes
		 */
		void setReviveLocation(LocationName loc)
		{
			m_revive_location = loc;
			
		}
		
		/**
		 * \fn void insertTrigger(Trigger* trigger)
		 * \brief Fuegt einen neuen Trigger in die Liste der aktuell aktivierten Trigger ein
		 * \param trigger eingefuegter Trigger
		 */
		void insertTrigger(Trigger* trigger);
		
		/**
		 * \fn void insertTimedTrigger(Trigger* trigger)
		 * \brief Fuegt einen Trigger ein, der zeitverzoegert aktiv wird
		 * \param trigger eingefuegter Trigger
		 * \param time Zeit
		 */
		void insertTimedTrigger(Trigger* trigger, float time);
		
		/**
		 * \fn void addEvent(TriggerType type, Event* event)
		 * \brief Fuegt ein neues Event hinzu
		 * \param trigger Typ des Triggers durch den das Event ausgeloest wird
		 * \param event Event
		 */
		void addEvent(TriggerType trigger, Event* event);
		
		/**
		 * \brief Deletes all Events that were copied from RegionData* structures
		 */
		void deleteCopiedEvents();
		
		/**
		 * \brief Copies the events from a \ref RegionData structure
		 * \rdata RegionData to copy events from
		 */
		void copyEventsFromRegionData(RegionData* rdata);
		
		/**
		 * \fn Damage& getDamageObject(std::string name)
		 * \brief Gibt das Schadensobjekt mit dem angegebenen Name aus. Wenn das Objekt noch nicht existierte wird es angelegt
		 * \param name Name
		 */
		Damage& getDamageObject(std::string name)
		{
			return m_damage_objects[name];
		}
		
		/**
		 * \fn void setCutsceneMode(bool mode)
		 * \brief Setzt den Cutscene Modus
		 * \param mode Modus
		 */
		void setCutsceneMode(bool mode);
		
		/**
		 * \fn bool getCutsceneMode()
		 * \brief Gibt aus, ob die sich die Region aktuell im Cutscene Modus befindet
		 */
		bool getCutsceneMode()
		{
			return m_cutscene_mode;
		}
		
		/**
		 * \fn std::multimap<TriggerType, Event*>& getEvents()
		 * \brief Gibt Referenz auf die Eventliste aus
		 */
		std::multimap<TriggerType, Event*>& getEvents()
		{
			return m_events;
		}
		
		/**
		 * \fn RegionCamera& getCamera()
		 * \brief Gibt die Kamera aus
		 */
		RegionCamera& getCamera()
		{
			return m_camera;
		}
		
		/**
		 * \brief Gibt Beleuchtungsinformationen aus
		 */
		RegionLight& getLight()
		{
			return m_light;
		}
		
		/**
		 * \fn Dialogue* getDialogue(int id)
		 * \brief Gibt zu der die ID den Dialog aus
		 * \param id ID
		 */
		Dialogue* getDialogue(int id);
		
		/**
		 * \fn void insertDialogue(Dialogue* dia)
		 * \brief Fuegt einen neuen Dialog ein
		 * \param dia Dialog
		 */
		void insertDialogue(Dialogue* dia);
		
		/**
		 * \fn void deleteDialogue(Dialogue* dia)
		 * \brief Loescht einen Dialog
		 * \param dia Dialog
		 */
		void deleteDialogue(Dialogue* dia);
		
		/**
		 * \fn short getDimX()
		 * \brief Dimension in x-Richtung
		 */
		short getDimX()
		{
			return m_dimx;
		}
		
		/**
		 * \fn short getDimY()
		 * \brief Dimension in y-Richtung
		 */
		short getDimY()
		{
			return m_dimy;
		}
		
		/**
		 * \fn void setGroundMaterial(std::string material)
		 * \brief Setzt das Material des Bodens
		 * \param material Material
		 */
		void setGroundMaterial(std::string material)
		{
			m_ground_material = material;
		}
		
		/**
		 * \fn std::string getGroundMaterial()
		 * \brief Gibt das Material fuer den Boden aus
		 */
		std::string getGroundMaterial()
		{
			return m_ground_material;
		}
		
		/**
		 * \fn Vector& getWaypointLocation()
		 * \brief Gibt den Ort des Wegpunktes aus
		 */
		Vector& getWaypointLocation()
		{
			return m_waypoint_location;
		}
		
		/**
		 * \fn void insertPlayerTeleport(int playerid, RegionLocation regionloc)
		 * \brief Fuegt einen Spieler ein, der sich aus der Region per Wegpunkt entfernt
		 */
		void insertPlayerTeleport(int playerid, RegionLocation regionloc)
		{
			m_teleport_players.insert(std::make_pair(playerid,regionloc));
		}
		
		/**
		 * \brief Gibt die Schadensanzegier aus
		 */
		std::map<int,FloatingText*>& getFloatingTexts()
		{
			return m_floating_texts;
		}
		
		/**
		 * \brief Gibt die Liste der Musikstuecke aus
		 */
		const std::list<MusicTrack>& getMusicTracks()
		{
			return m_music_tracks;
		}
		
		/**
		 * \brief Loescht alle aktuell gesetzten Musikstuecke der Region
		 */
		void clearMusicTracks();
		
		/**
		 * \brief Fuegt ein Musikstueck hinzu
		 * \param track ogg-Datei
		 */
		void addMusicTrack(MusicTrack track);
		
		
		/**
		 * \brief Schreibt alle Musikstuecken in den Stream
		 * \param cv Stream
		 */
		void writeMusicTracksToString(CharConv* cv);
		
		/**
		 * \brief Liest alle Musicstuecken aus dem Stream
		 * \param cv Stream
		 */
		void readMusicTracksFromString(CharConv* cv);
		
		/**
		 * \fn void visualizeDamage(int number, Vector position, short size=1)
		 * \brief Fuegt Schadenanzeige hinzu
		 * \param number Dargestellte Zahl
		 * \param position Position an der der Schaden dargestellt wird
		 * \param size Schriftgroesse
		 */
		void visualizeDamage(int number, Vector position, FloatingText::Size size = FloatingText::NORMAL);
		
		/**
		 * \brief Create a floating text overlay
		 * \param text the text displayed
		 * \param position position of the text 
		 * \param size Size of the text
		 * \param  colour colour of the text as hexadecimal AARRGGBB
		 * \param time how long the text is displayed
		 * \param float_offset distance the text floats up (1 is screen height)
		 */
		void createFloatingText(TranslatableString text,  Vector position, FloatingText::Size size = FloatingText::NORMAL, std::string colour="FFFF5555", float time = 1000, float float_offset = 0.1);
		
		/**
		 * \brief Plays a Sound in this regionloc
		 * \param soundname Name of the sound
		 * \param position Position where the Sound should be played
		 * \param volume Volume of the sound
		 * \param global if true, the sound will be played on all clients, too
		 */
		void playSound(std::string soundname, Vector position, float volume = 1.0, bool global= true);
		
		/** 
		 * \brief Returns list of sounds played during current update
		 */
		const std::list<PlayedSound*>& getPlayedSounds()
		{
			return m_played_sounds;
		}
		
		/**
		 * \fn void setNamedId(std::string name, int id)
		 * \brief Speichert Id unter einem bestimmten Name
		 */
		void setNamedId(std::string name, int id);
		
		/**
		 * \fn int getIdByName(std::string name)
		 * \brief Gibt die ID mit dem angegebenen Name aus
		 */
		int getIdByName(std::string name);

		/**
		 * \fn std::string getIdString()
		 * \brief Gibt den ID String aus
		 */
		std::string getIdString()
		{
			return m_name;
		}
	private:
		/**
		* \var m_dimx
		* \brief Ausdehnung der Region in x-Richtung
		*/
		short m_dimx;
	
		/**
			* \var m_dimy
			* \brief Ausdehnung der Region in y-Richtung
			*/
		short m_dimy;
	
		/**
			* \var m_data_grid
			* \brief Das Array, welches die Daten der Region enthaelt. Jeder Eintrag im Array entspricht einem 4*4 Quadrat
			*/
		Matrix2d<Gridunit>* m_data_grid;
	
		
		/**
		 * \var Matrix2d<float>* m_height
		 * \brief Hoehenkarte in 4x4 Kaestchen
		 */
		Matrix2d<float>* m_height;
		
		/**
		 * \var std::map<float, EnvironmentName> m_environments
		 * \brief Liste von moeglichen Umgebungen mit ihren Hoehenlinien (Obergrenzen)
		 */
		std::map<float, EnvironmentName> m_environments;
	
		/**
		* \var m_players
		* Liste der Spieler in der Region
		*/
		WorldObjectMap* m_players;
	
		/**
		* \var ProjectileMap* m_projectiles
		* \brief Liste aller Projektile in der Region
		*/
		ProjectileMap* m_projectiles;
	
		/**
		* \var WorldObjectMap* m_objects
		* \brief organisiert alle nicht statischen Objekte in einem Bin&auml;rbaum
		*/
		WorldObjectMap* m_objects;
	
		/**
		 * \var WorldObjectMap* m_static_objects;
		 * \brief organisiert statische Objekte in einem Bin&auml;rbaum
		 */
		WorldObjectMap* m_static_objects;
		
		/**
		 * \var WorldObjectMap m_large_objects
		 * \brief Speichert alle Objekte, die so gross sind, dass sie fuer die Kollisionsbehandlung gesondert behandelt werden muessen
		 */
		WorldObjectMap m_large_objects;
	
		/**
		* \var DropItemMap* m_drop_items
		* \brief Liste der Gegenstaende, die auf dem Boden liegen
		*/
		DropItemMap* m_drop_items;
	
		/**
		* \var DropItemMap* m_drop_item_locations
		* \brief Orte der Gegenstaende, die auf dem Boden liegen
		*/
		DropItemMap* m_drop_item_locations;
	
		/**
		 * \var GameObjectMap m_game_objects
		 * \brief Datenstruktur mit allen (nicht statischen) Spielobjekten der Region
		 */
		GameObjectMap m_game_objects;
	
		/**
		* \var short m_id
		* \brief Nummer der Region
		*/
		short m_id;
		
		/**
		 * \var std::string m_name
		 * \brief Name der Region
		 */
		std::string m_name;
		
		/**
		 * \var bool m_cutscene_mode
		 * \brief Gibt an, ob die Region sich aktuell im Cutszene Modus befindet. In diesem Modus werden nur per Script gesetzte Aktionen ausgefuehrt
		 */
		bool m_cutscene_mode;
	
		/**
		* \var NetEventList* m_netevents
		* \brief Liste der lokalen NetEvents beim aktuellen update
		*/
		NetEventList* m_netevents;
		
		/**
		 * \var std::map<LocationName, Vector> m_locations
		 * \brief Liste der Orte
		 */
		std::map<LocationName, Vector> m_locations;
		
		/**
		 * \var std::map<AreaName, Shape> m_areas
		 * \brief Liste der Orte
		 */
		std::map<AreaName, Shape> m_areas;
		
		/**
		 * \var std::list<RegionExit> m_region_exits
		 * \brief Liste der Ausgaenge aus der Region
		 */
		std::list<RegionExit> m_exits;
		
		/**
		 * \var LocationName m_revive_location
		 * \brief Ort an dem in dieser Region gestorbene Helden wiedererweckt werden
		 */
		LocationName m_revive_location;
		
		/**
		 * \var std::list<Trigger*> m_triggers
		 * \brief Trigger die waehrend dem aktuellen Event ausgeloest wurden
		 */
		std::list<Trigger*> m_triggers;
		
		/**
		 * \var std::list<std::pair<float, Trigger*> > m_timed_trigger
		 * \brief Liste der Trigger die mit Zeitverzoegerung ausgeloest werden
		 */
		std::list<std::pair<float, Trigger*> > m_timed_trigger;
		
		/**
		 * \var std::multimap<TriggerType, Event*> m_events
		 * \brief Liste der registrierten Events, aufgelistet nach dem Trigger durch den sie ausgeloest werden
		 */
		std::multimap<TriggerType, Event*> m_events;
		
		/**
		 * \var std::map<std::string,Damage> m_damage_objects
		 * \brief Liste der Schadensobjekte
		 */
		std::map<std::string,Damage> m_damage_objects;
		
		/**
		 * \var std::map<int, Dialogue*> m_dialogues
		 * \brief Liste der Dialoge
		 */
		std::map<int, Dialogue*> m_dialogues;
		
		/**
		 * \var std::map<int,RegionLocation>  m_teleport_players;
		 * \brief Liste der Spieler die sich aus der Region per Wegpunkt entfernen
		 */
		std::map<int,RegionLocation>  m_teleport_players;
		
		/**
		 * \var std::map<std::string,int> m_name_ids
		 * \brief Ids benamter Objekte
		 */
		std::map<std::string,int> m_name_ids;
		
		/**
		 * \brief floating text overlays
		 */
		std::map<int,FloatingText*> m_floating_texts;
		
		/**
		 * \brief floating text overlays
		 */
		std::list<PlayedSound*> m_played_sounds;
		
		/**
		 * \var RegionCamera m_camera
		 * \brief Kameraposition von der aus die Region bei Zwischenszenen gesehen wird
		 */
		RegionCamera m_camera;
		
		/**
		 * \brief Enthaelt alle Daten zur Beleuchtung der Szene
		 */
		RegionLight m_light;
		
		/**
		 * \var std::string m_ground_material
		 * \brief Material mit dem der Boden gerendert wird
		 */
		std::string m_ground_material;
		
		/**
		 * \var Vector m_waypoint_location
		 * \brief Position des Wegpunktes
		 */
		Vector m_waypoint_location;
		
		/**
		 * \brief Liste von moeglichen Musikstuecken fuer die Region
		 */
		std::list<MusicTrack> m_music_tracks;

};

#include "gridunit.h"

#endif
