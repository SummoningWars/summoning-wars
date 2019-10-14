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

#ifndef __SUMWARS_CORE_WORLD_H__
#define __SUMWARS_CORE_WORLD_H__


#include <string>
#include <iostream>
#include <list>
#include <map>
#include <math.h>
#include <cstdlib>
#include <algorithm>
#include <queue>

#include "translatablestring.h"
#include "worldobject.h"
#include "projectile.h"
#include "servernetwork.h"
#include "clientnetwork.h"
#include "region.h"
#include "pathfind.h"
#include "party.h"
#include "quest.h"
#include "waypoint.h"
#include "fraction.h"



/**
 * \def WORLD_MAX_REGIONS
 * \brief Anzahl maximaler Regionen die gleichzeitig im Speicher sind
 * \def sqr(X)
 * \brief Quadratfunktion
 */
#define WORLD_MAX_REGIONS 100

#define sqr(X) ((X) * (X))

/**
 * \file world.h
 * \defgroup world \ Welt
 * \brief Deklaration der World
 * \author Hans Wulf
 * \version 1.0
 * \date 2007/05/28
 */

/**
 * Liste von Slotnommern auf denen neue Logins eingegangen sind
 */
typedef std::list<int> LoginList;

/**
 * \class World
 * \brief Hauptobjekt der Anwendung
 *
 * Dieses Objekt ist das Hauptobjekt der Anwendung. Die wesentliche Aufgabe liegt in der Verwaltung der WorldObject Objekte.
 */
class World {
//Public stuff
public:
	
	/**
	 * \brief Different groups of ingame data
	 */
	enum DataGroups
	{
		DATA_NONE = 0x0,
		DATA_PLAYERCLASSES = 0x1,
		DATA_MONSTERS = 0x2,
		DATA_OBJECTS = 0x4,
		
		DATA_ALL_CREATURES = 0xF,
		
		DATA_PROJECTILES = 0x10,
		DATA_ITEMS = 0x20,
		DATA_ABILITIES = 0x40,
		
		DATA_ALL_OBJECTS = 0xff,	
		
		DATA_LUACODE = 0x100,
		DATA_EVENTS = 0x400,
		DATA_LUAENVIRONMENT = 0x800,
		
		DATA_ALL_INGAME = 0x7ff,
		DATA_ALL_SCRIPT = 0x748,
		
		DATA_SOUND = 0x1000,
		DATA_MUSIC = 0x2000,
		DATA_RENDERINFO = 0x4000,
		
		DATA_ALL_CONTENT_INFO = 0xf000,
		
		DATA_MODELS = 0x10000,
		DATA_GUI = 0x20000,
		DATA_PARTICLESYSTEMS = 0x40000,
		DATA_IMAGES = 0x80000,
		
		DATA_ALL_CONTENT = 0xff000,
		DATA_ALL = 0xfff7ff,
		
	};
	
	
	//Fields
	//Constructors
	/**
	 * \fn World(bool server,  bool cooperative, int max_players)
	 * \param server Gibt an, ob der Rechner der Server ist
	 * \brief Konstruktor
	 * \param cooperative wenn auf true gesetzt, sind alle Spieler automatisch verbuendet
	 * \param max_players Maximale Spieleranzahl
	 * Legt ein neues World Objekt an
	 */
	World(bool server ,  bool cooperative, int max_players);


	/**
	* \fn ~World()
	 * \brief Destruktor
	 */
	~World();

	/**
	 * \brief Deletes data in global data structures and Factories
	 */
	void cleanup();

	/**
	 * \fn bool init(int port)
	 * \brief initialisiert die Welt
	 * \param port Port fuer den Netzverkehr
	 * \return true, wenn kein Fehler aufgetreten ist, sonst false
	 */
	bool init(int port );
	
	/**
	 * \brief Loads all the game Data from the XML files
	 * This function uses the \a m_data_reload_requests to determine which data to load and resets the bitmask afterwards
	 */
	bool loadGameData();
	
	/**
	 * \brief Deletes game data loaded by \ref loadGameData
	 * This function uses the \a m_data_reload_requests to determine which data delete
	 */
	void deleteGameData();

	/**
	 * \fn bool createRegion(short region)
	 * \brief Erzeugt die Region mit der angegebenen Nummer
	 * \param region Nummer der Region, die erzeugt werden soll
	 */
	bool createRegion(short region);


	/**
	 * \fn void acceptLogins()
	 * \brief Fuegt Spieler die sich neu eingeloggt haben ins Spiel ein
	 */
	void acceptLogins();

	/**
	 * \fn void updateLogins()
	 * \brief Aktualisiert die Liste der eingehenden Logins
	 */
	void updateLogins();

	//Accessor Methods
	/**
	 * \fn short insertRegion(Region* region, int rnr)
	 * \brief Fuegt eine neue Region in die Welt ein
	 * \param region Einzufuegende Region
	 * \param rnr ID der Region
	 */
	void insertRegion(Region* region, int rnr);

	/**
	 * \fn bool insertPlayerIntoRegion(WorldObject* player, short region, LocationName loc="")
	 * \param player der Spieler
	 * \param region ID der Region
	 * \param loc Ort an dem der Spieler eingefuegt wird
	 * \brief Versucht einen Spieler in eine Region einzufuegen
	 */
	bool insertPlayerIntoRegion(WorldObject* player, short region, LocationName loc ="");



	/**
	 * \fn bool insertPlayer(WorldObject* player, int slot = NOSLOT)
	 * \brief Fuegt einen neuen Spieler hinzu
	 */
	bool insertPlayer(WorldObject* player, int slot= NOSLOT);


	/**
	 * \fn bool calcBlockmat(PathfindInfo * pathinfo)
	 * \brief Berechnet die Matrix der blockierten Felder
	 * \param pathinfo Wegfindeinformation fuer die die Matrix benoetigt wird
	 */
	bool calcBlockmat(PathfindInfo * pathinfo);

	/**
	 * \fn bool calcPotential(PathfindInfo* pathinfo)
	 * \brief Berechnet auf Basis der Blockadematrix das Potentialfeld
	 * \param pathinfo Wegfindeinformation fuer die das Potential berechnet wird
	 */
	bool calcPotential(PathfindInfo* pathinfo);

	/**
	 * \fn void calcPathDirection(PathfindInfo* pathinfo, Vector pos, Vector& dir)
	 * \brief Berechnet die Bewegungsrichtung auf Basis der gegebenen Wegfindeinformation
	 * \param pathinfo verwendete Wegfindeinformation
	 * \param pos Standpunkt
	 * \param dir erhaltene Bewegungsrichtung
	 */
	void calcPathDirection(PathfindInfo* pathfinfo, Vector pos, Vector& dir);



	/**
	 * \fn getValidId()
	 * \brief Gibt eine zul&auml;ssige ID f&uuml;r ein WorldObject aus
	 * \return zul&auml;ssige ID f&uuml;r ein WorldObject
	 */
	int getValidId();

	/**
	 * \fn int getValidProjectileId()
	 * \brief Gibt eine zulaessige ID fuer ein Projektil zurueck
	 */
	int getValidProjectileId();


	/**
	 * \fn update(float time)
	 * \brief Laesst für alle Objekte in der Welt die angegebene Zeitspanne verstreichen
	 * \param time Zeit um die in der Welt vergeht in Millisekunden
	 */
	void update(float time);

	/**
	 * \fn void updatePlayers()
	 * \brief Aktualisiert die Spielerobjekte
	 */
	void updatePlayers();

	/**
	 * \fn bool writeNetEvent(Region* region, NetEvent* event, CharConv* cv)
	 * \brief Schreib die Daten zu dem NetEvent in den Bitstream
	 * \param region Region in der das NetEvent aufgetreten ist
	 * \param event NetEvent das geschrieben wird
	 * \param cv Bitstream zum Schreiben
	 */
	bool writeNetEvent(Region* region, NetEvent* event, CharConv* cv);

	/**
	 * \fn bool processNetEvent(Region* region,CharConv* cv)
	 * \brief Liest ein NetEvent aus dem Bitstream und fuehrt es aus
	 * \param region Region in der das NetEvent aufgetreten ist
	 * \param cv Bitstream
	 */
	bool processNetEvent(Region* region,CharConv* cv);

	/**
	 * \fn void calcBlockArray(PathfindInfo* p)
	 * \brief Berechnet fuer die durch p spezifizierte Wegfindeaufgabe das Feld mit den blockierten Feldern
	 * \param p Struktur mit den Daten zur Wegfindung
	 */
	void calcBlockArray(PathfindInfo* p);

	/**
	* \fn void setNetwork(ServerNetwork * network)
	* \param network Zeiger auf das Netzwerk-Objekt
	* \brief Setzt Zeiger auf das Netzwerk-Objekt
	*/

	void setNetwork(ServerNetwork * network)
	{
		m_network = network;
	};


	/**
	 * \fn 	Region* getRegion(int rid)
	 * \brief Gibt die Region aus
	 * \param rid ID der Region
	 */
	Region* getRegion(int rid);

	/**
	 * \fn int getRegion(std::string name)
	 * \brief Gibt die ID Region mit dem angegebenen Name aus
	 * \param name Name der Region
	 */
	int getRegionId(std::string name);

	/**
	 * \fn 	Region* getRegion(std::string name)
	 * \brief Gibt die Region aus
	 * \param name Name der Region
	 */
	Region* getRegion(std::string name)
	{
		return getRegion(getRegionId(name));
	}

	/**
	 * \fn Network* getNetwork()
	 *\return network Zeiger auf das Netzwerk-Objekt
	 * \brief Gibt Zeiger auf das Netzwerk-Objekt aus
	 */

	Network* getNetwork()
	{
		return m_network;
	}

	/**
	 * \fn Party* getParty(int id)
	 * \brief Gibt die Party mit der angegebenen ID
	 * \param id ID
	 */
	Party* getParty(int id);

	/**
	 * \fn WorldObject* getLocalPlayer()
	 * \brief Gibt den Spieler aus, der sich an dem Rechner befindet, auf dem diese Welt simuliert wird
	 */
	WorldObject* getLocalPlayer()
	{
		return m_local_player;
	}

	/**
	 * \fn Party* getEmptyParty()
	 * \brief Gibt eine Party aus, die bisher keine Mitglieder hat
	 */
	Party* getEmptyParty();

	/**
	 * \fn Party::Relation getRelation(Fraction::Id frac, WorldObject* wo)
	 * \brief Gibt die Beziehung eines Lebewesens zu einer Fraktion
	 * \param frac Fraktion
	 * \param wo Objekt
	 */
	Fraction::Relation getRelation(Fraction::Id frac, WorldObject* wo);

	/**
	 * \fn Fraction::Relation getRelation(Fraction::Id frac, Fraction::Id frac2)
	 * \brief Gibt die Beziehung zwischen zwei Fraktionen aus
	 * \param frac erste Fraktion
	 * \param frac2 zweite Fraktion
	 */
	Fraction::Relation getRelation(Fraction::Id frac, Fraction::Id frac2);

	/**
	 * \fn Fraction::Relation getRelation(Fraction::Type fractionname1, Fraction::Type fractionname2)
	 * \brief Gibt die Beziehung zwischen zwei Fraktionen aus
	 * \param fractionname1 Name von Fraktion 1
	 * \param fractionname2 Name von Fraktion 2
	 */
	Fraction::Relation getRelation(Fraction::Type fractionname1, Fraction::Type fractionname2);

	/**
	 * \fn void setRelation(Fraction::Id frac, Fraction::Id frac2, Fraction::Relation relation)
	 * \brief Setzt das Verhaeltnis zwischen zwei Fraktionen
	 * \param frac erste Fraktion
	 * \param frac2 zweite Fraktion
	 * \param relation Verhaeltnis zwischen den zwei Fraktionen
	 */
	void setRelation(Fraction::Id frac, Fraction::Id frac2, Fraction::Relation relation);

	/**
	 * \fn void setRelation(Fraction::Type fractionname1, Fraction::Type fractionname2, Fraction::Relation relation)
	 * \brief Setzt das Verhaeltnis zwischen zwei Fraktionen
	 * \param fractionname1 Name von Fraktion 1
	 * \param fractionname2 Name von Fraktion 2
	 * \param relation Verhaeltnis zwischen den zwei Fraktionen
	 */
	void setRelation(Fraction::Type fractionname1, Fraction::Type fractionname2, Fraction::Relation relation);


	/**
	 * \fn void handleSavegame(CharConv *cv, int slot=LOCAL_SLOT)
	 * \brief Behandelt den Empfang eines Savegames
	 * \param cv Eingabepuffer
	 * \param slot Slot ueber den das Savegame empfangen wurde. Wenn das Savegame nicht ueber das Netzwerk uebertragen wurde -1
	 */
	void handleSavegame(CharConv *cv , int slot=LOCAL_SLOT);

	/**
	 * \fn void handleCommand(ClientCommand* cmd, int slot=LOCAL_SLOT, float delay=0)
	 * \brief Behandelt ein erhaltenes Kommmando
	 * \param cmd Kommando
	 * \param slot Slot ueber den das Kommando empfangen wurde. Wenn das Kommando nicht ueber das Netzwerk uebertragen wurde -1
	 * \param delay Millisekunden die das Kommando verspaetet erhalten wurde
	 */
	void handleCommand(ClientCommand* cmd, int slot=LOCAL_SLOT, float delay=0);

	/**
	 * \brief Behandelt eine empfangene Nachricht
	 * \param msg Nachricht
	 * \param slot uebern den die Nachricht empfangen wurde
	 */
	void handleMessage(TranslatableString msg, int slot=LOCAL_SLOT);

	/**
	 * \fn void handleDataRequest(ClientDataRequest* request, int slot  = LOCAL_SLOT)
	 * \brief Behandelt eine Anfrage nach Daten vom Client
	 * \param request beschreibt welche Daten gefordert werden
	 * \param slot Ziel der Daten
	 */
	void handleDataRequest(ClientDataRequest* request, int slot  = LOCAL_SLOT);

	/**
	 * \fn void insertNetEvent(NetEvent &event)
	 * \brief Fuegt ein neues NetEvent in die NetEventliste ein
	 */
	void insertNetEvent(NetEvent &event);

	/**
	 * \fn WorldObject* getPlayer(int id)
	 * \brief Gibt Spieler mit der angegebenen ID aus
	 * \param id ID
	 */
	WorldObject* getPlayer(int id);

	/**
	 * \fn WorldObjectMap* getPlayers()
	 * \brief Gibt die Liste aller Spieler aus
	 */
	WorldObjectMap* getPlayers()
	{
		return m_players;
	}

	/**
	 * \fn Fraction* getFraction(Fraction::Id id)
	 * \brief Gibt die Fraktion mit der angegebenen ID aus
	 * \param id ID
	 */
	Fraction* getFraction(Fraction::Id id);

	/**
	 * \fn Fraction* getFraction(Fraction::Type fractionname)
	 * \brief Gibt Fraktion mit dem angegebenen Name aus
	 * \param fractionname Name einer Fraktion
	 */
	Fraction* getFraction(Fraction::Type fractionname)
	{
		return getFraction(getFractionId(fractionname));
	}

	/**
	 * \fn Fraction::Id getFractionId(Fraction::Type fractionname)
	 * \brief Gibt die ID der Fraktion mit dem angegebenen Name aus
	 * \param fractionname Name einer Fraktion
	 */
	Fraction::Id getFractionId(Fraction::Type fractionname);

	/**
	 * \fn void createFraction(Fraction::Type name)
	 * \brief Erzeugt eine neue Fraktion
	 * \param name Name der Fraktion
	 * \param human Gibt an, ob es sich um einer Fraktion von Spielern handelt
	 * \param id Id der Fraktion. Falls -1 angegeben wird, wird automatisch eine ID berechnet
	 */
	void createFraction(Fraction::Type name);

	/**
	 * \fn bool isServer()
	 * \brief gibt true aus, wenn die Welt der Server ist
	 */
	bool isServer()
	{
		return m_server;
	}

	/**
	 * \fn bool isCooperative()
	 * \brief Gibt aus ob die Welt im Modus kooperativ (alle Spieler sind verbuendet) ist
	 */
	bool isCooperative()
	{
		return m_cooperative;
	}

	/**
	 * \fn bool timerLimit(int i)
	 * \param i Nummer des Timers
	 * \brief Gibt true aus, wenn der Timer beim aktuellen update das Limit erreicht hat
	 */
	bool timerLimit(int i)
	{
		return m_timer_limit[i];
	}

	/**
	 * \fn void registerRegionData(RegionData* data, int id)
	 * \brief Registriert Daten fuer den Aufbau einer Region
	 * \param data Daten
	 * \param id ID der Region
	 */
	void registerRegionData(RegionData* data, int id);

	/**
	 * \fn void addEvent(RegionName rname, TriggerType type, Event* event)
	 * \brief Fuegt ein neues Event hinzu
	 * \param rname Name der Region
	 * \param trigger Typ des Triggers durch den das Event ausgeloest wird
	 * \param event Event
	 */
	void addEvent(RegionName rname, TriggerType trigger, Event* event);

	/**
	 * \fn void addQuest(std::string questname, Quest* quest)
	 * \brief Fuegt Quest hinzu
	 * \param questname Name des Quests
	 * \param quest Quest
	 */
	void addQuest(std::string questname, Quest* quest)
	{
		m_quests.insert(std::make_pair(questname,quest));
	}

	/**
	 * \fn void addWaypoint(short id, WaypointInfo& data)
	 * \brief Fuegt einen weiteren Wegpunkt hinzu
	 */
	void addWaypoint(short id, WaypointInfo& data)
	{
		m_waypoint_data.insert(std::make_pair(id,data));
	}

	/**
	 * \fn std::map<std::string, Quest*>& getQuests()
	 * \brief Gibt die Liste der Quests aus
	 */
	std::map<std::string, Quest*>& getQuests()
	{
		return m_quests;
	}

	/**
	 * \fn RegionLocation& getPlayerStartLocation()
	 * \brief Gibt Ort aus, an dem neu erzeugte Charaktere platziert werden
	 */
	RegionLocation& getPlayerStartLocation()
	{
		return m_player_start_location;
	}

	/**
	 * \fn std::map<short,WaypointInfo>& getWaypointData()
	 * \brief Gibt die Liste aller Wegpunkte aus
	 */
	std::map<short,WaypointInfo>& getWaypointData()
	{
		return m_waypoint_data;
	}

	/**
	 * \fn static World* getWorld()
	 * \brief Gibt den Zeiger auf das World Singleton Objekt zurueck
	 */
	static World* getWorld()
	{
		return m_world;
	}

	/**
	 * \fn static int getVersion()
	 * \brief Gibt die Version aus
	 */
	static int getVersion()
	{
		return m_version;
	}

	/**
	 * \fn static void createWorld(bool server, int port,  bool cooperative, int max_players)
	 * \brief Erzeugt die Spielwelt
	 * \param server auf true gesetzt, wenn der Rechner der Server ist
	 * \param cooperative wenn auf true gesetzt, sind alle Spieler automatisch verbuendet
	 * \param port Port fuer den Netzverkehr
	 *  \param max_players Maximale Spieleranzahl
	 */
	static void createWorld(bool server, int port, bool cooperative, int max_players);


	/**
	 * \fn static void deleteWorld()
	 * \brief entfernt die Spielwelt
	 */
	static void deleteWorld();

	/**
	 * \brief Requests reloading of the components contained in the bitmask
	 * \param bitmask Bitmask consisting of elements from \ref DataGroups
	 */
	void requestDataReload(int bitmask)
	{
		m_data_reload_requests |= bitmask;
	}
	
	/**
	 * \brief returns a bitmask that specified data to be reloaded in the next frame
	 */
	int getDataReloadRequests()
	{
		return m_data_reload_requests;
	}
	
	unsigned int getBaseRandomSeed()
	{
		return m_base_random_seed;
	}
		
//Private stuff
private:
	//Fields

	/**
	 * \var bool m_server
	 * \brief true, wenn der Rechner der Server ist
	 */
	bool m_server;

	/**
	 * \var bool m_cooperative
	 * \brief wenn auf true gesetzt, sind alle Spieler automatisch verbuendet. Nur im kooperative Modus koennen Aufgaben erledigt werden.
	 */
	bool m_cooperative;

	/**
	 * \var std::map<int, Region*> m_regions
	 * \brief Speichert die Spielwelt in Form von Regionen. Eine Region ist ein rechteckiger Ausschnitt der Spielwelt
	 */
	std::map<int, Region*> m_regions;

	/**
	 * \var std::map<std::string, int> m_region_name_id
	 * \brief Bildet den Name einer Region auf die ID ab
	 */
	std::map<std::string, int> m_region_name_id;

	/**
	 * \var std::map<int, RegionData*> m_region_data
	 * \brief Daten fuer die Erzeugung der Regionen sortiert nach ID
	 */
	std::map<int, RegionData*> m_region_data;


	/**
	 * \var std::vector<Party> m_parties
	 * \brief Liste alles Parties
	 */
	std::vector<Party> m_parties;

	/**
	 * \var std::map<Fraction::Id, Fraction*> m_fractions
	 * \brief Liste aller Fraktionen im Spiel
	 */
	std::map<Fraction::Id, Fraction*> m_fractions;

	/**
	 * \var m_player_slots
	 * \brief Liste der Spieler in der Welt mit ihren Slots
	 */
	WorldObjectMap* m_player_slots;

	/**
	 * \var WorldObjectMap* m_players
	 * \brief Liste der Spieler in der Welt sortiert nach ID
	 */
	WorldObjectMap* m_players;

	/**
	 * \var std::map<std::string, Quest*> m_quests
	 * \brief Liste der Quests
	 */
	std::map<std::string, Quest*> m_quests;

	/**
	 * \var std::map<short,WaypointInfo> m_waypoint_data
	 * \brief Daten zu Wegpunkten
	 */
	std::map<short,WaypointInfo> m_waypoint_data;


	/**
	 * \var int m_max_nr_players
	 * \brief Gibt die maximale Anzahl Spieler an
	 */
	int m_max_nr_players;

	/**
	 * \var Network* m_network
	 * \brief Netzwerkanbindung
	 */
	Network* m_network;

	/**
	 * \var WorldObject* m_local_player
	 * \brief lokaler Spieler
	 */
	WorldObject* m_local_player;

	/**
	 * \var LoginList m_logins
	 * \brief Liste der Spieler die sich gerade einloggen wollen
	 */
	LoginList m_logins;

	/**
	 * \var NetEventList m_events
	 * \brief Liste der globalen NetEvents beim aktuellen update
	 */
	NetEventList* m_events;

	/**
	 * \var float m_timer[6]
	 * \brief Timer a 250, 500 und 1000 ms
	 */
	float m_timer[6];

	/**
	 * \var  bool m_timer_limit[6]
	 * \brief ist true, wenn der betreffende Timer gerade in dem Tick abgelaufen ist
	 */
	bool m_timer_limit[6];

	/**
	 * \fn std::map<int,LocationName> m_region_enter_loc
	 * \brief Wenn ein Spieler nicht sofort in eine Region eingefuegt werden kann, wird hier der Ort zu einfuegen zwischengespeichert
	 */
	std::map<int,LocationName> m_region_enter_loc;

	/**
	 * \var RegionLocation m_player_start_location
	 * \brief Gibt den Ort an, an dem neu erzeugte Charaktere starten
	 */
	RegionLocation m_player_start_location;
	
	/**
	 * \brief Base number for random seed
	 * For debugging purpose, it might be set to a fixed value. Other options are storing it in savegame * or recreating it on every game start.
	 */
	unsigned int m_base_random_seed;

	/**
	 * \var  static World* m_world
	 * \brief Objekt fuer die Spielwelt
	 */
	static World* m_world;

	/**
	 * \var static int m_version
	 * \brief Versionsnummer
	 */
	static int m_version;
	
	/**
	 * \brief Bitmask assembled from elements of \ref DataGroups
	 * Initiates reloading of that data groupds contained in the bismask
	 */
	int m_data_reload_requests;
};
#endif // __SUMWARS_CORE_WORLD_H__
