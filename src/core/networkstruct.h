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

#ifndef NETWORKSTRUCT_H
#define NETWORKSTRUCT_H

#include "network.h"
#include "action.h"
#include "geometry.h"

#include "charconv.h"
#include <list>

/**
 * \enum PackageType
 * \brief Zaehlt die verschiedenen Arten von Netzwerkpaketen auf
 */
enum PackageType
{
	/**
	 * \brief Anfrage des Clients an den Server
	 */
	//PTYPE_C2S_REQUEST =ID_USER_PACKET_ENUM,
	PTYPE_C2S_REQUEST =200,

 /**
  * \brief Antwort des Servers auf eine Anfrage des Clients
  */
	PTYPE_S2C_RESPONSE ,

 /**
  * \brief Savegame vom Server an den Client
  */
	PTYPE_S2C_SAVEGAME ,

 	/**
  	* \brief Savegame vom Client an den Server
  	*/
 	PTYPE_C2S_SAVEGAME ,

 /**
  * \brief Normale Daten vom Server zum Client
  */
	PTYPE_S2C_EVENT ,

 /**
  * \brief Kommando vom Client zum Server
  */
	PTYPE_C2S_COMMAND,

 	/**
	 * \brief Anfrage nach Daten die beim Client fehlen / inkorrekt sind
	 */
	PTYPE_C2S_DATA_REQUEST,

	/**
	* \brief Daten zu einer Region (feste Objekte und Untergrund)
	*/
	PTYPE_S2C_REGION ,

	/**
	 * \brief Daten ueber Spieler
	 */
 	PTYPE_S2C_PLAYER,

  	/**
	 * \brief Informationen zur Initialisierung
	 */
	PTYPE_S2C_INITIALISATION,

	/**
	 * \brief Information, dass beim Server der Spieler die Region gewechselt hat
	 */
	PTYPE_S2C_REGION_CHANGED,

	/**
	* \brief Nachricht vom Server an den Client
	*/
 	PTYPE_S2C_MESSAGE,

	/**
	* \brief Nachricht vom Client an den Server
	*/
 	PTYPE_C2S_MESSAGE,

	/**
	* \brief Partyinformationen vom Server an den Client
	*/
	PTYPE_S2C_PARTY,

 	/**
  	* \brief Lua Code vom Server an den Client
  	*/
 	PTYPE_S2C_LUA_CHUNK,

  	/**
   	* \brief Position der Wegpunkte
   	*/
  	PTYPE_S2C_WAYPOINTS,

   /**
    * \brief Daten zum Pruefung der Synchronisation
    */
   PTYPE_S2C_DATA_CHECK,

   /**
    * \brief Daten der Quests
    */
   PTYPE_S2C_QUEST,

    /**
	* \brief Daten der Fraktionen
	 */
   PTYPE_S2C_FRACTION,
};

/**
 * \struct ClientDataRequest
 * \brief Beschreibt Anfrage des Client nach Daten vom Server
 */
struct ClientDataRequest
{
	/**
	 * \enum Data
	 * \brief Zaehlt verschiedene Datenanfragen auf
	 */
	enum Data
	{
		REGION_STATIC=1,
		REGION_NONSTATIC=2,
		REGION_ALL=3,
		ITEM = 0x10,
		PLAYERS = 0x20,
		OBJECT = 0x40,
 		PROJECTILE = 0x80,
	};

	/**
	 * \var Data m_data
	 * \brief Art der geforderten Daten
	 */
	Data m_data;

	/**
	 * \var int m_id
	 * \brief ID des geforderten Objektes
	 */
	int m_id;

	/**
	 * \var short m_region_id
	 * \brief Id der Region
	*/
	short m_region_id;

	/**
	 * \fn void toString(CharConv* cv)
	 * \brief Konvertiert das Objekt in einen String und schreibt ihn in der Puffer
	 * \param cv Ausgabepuffer
	 */
	void toString(CharConv* cv)
	{
		cv->toBuffer((char) m_data);
		cv->toBuffer(m_id);
		cv->toBuffer(m_region_id);
	}


	/**
	 * \fn void fromString(CharConv* cv)
	 * \brief Erzeugt das Objekt aus einem String
	 * \param cv Eingabepuffer
	 */
	void fromString(CharConv* cv)
	{
		char tmp;
		cv->fromBuffer(tmp);
		m_data = (Data) tmp;
		cv->fromBuffer(m_id);
		cv->fromBuffer(m_region_id);
	}
};


/**
 * \enum Button
 * \brief Zählt alle Möglichkeiten auf, wie der Anwender durch Mausklicks oder das Drücken von Buttons Aktionen auslösen kann
 */
enum Button
{
	/**
	 * \brief Eingabe durch Klicken mit linker Maustaste ins Spielfeld
	 */
	LEFT_MOUSE_BUTTON =0,

 	/**
 	 * \brief Eingabe durch Klicken mit linker Maustaste und Shift ins Spielfeld
 	 */
 	LEFT_SHIFT_MOUSE_BUTTON =1,

 	 /**
	* \brief Eingabe durch Klicken mit rechter Maustaste und Shift ins Spielfeld
	*/
  	RIGHT_MOUSE_BUTTON =2,


	/**
	 * \brief Eingabe durch Speichern+Beenden Schaltfläche oder Schliessen-Kreuz
	 */
	BUTTON_SAVE_QUIT =3,

	BUTTON_SET_LEFT_ACTION =6,

	BUTTON_SET_RIGHT_ACTION=7,

	BUTTON_ITEM_LEFT=8,

	BUTTON_ITEM_RIGHT=9,

	BUTTON_SWAP_EQUIP = 10,

	REQUEST_DETAILED_ITEM = 11,

	REQUEST_ABILITY_DAMAGE=12,

	BUTTON_INCREASE_ATTRIBUTE=13,

	BUTTON_LEARN_ABILITY = 14,
	
	BUTTON_SET_ALTERNATE_RIGHT_ACTION=15,
	
	BUTTON_SWAP_RIGHT_ACTION=16,

	DROP_ITEM = 20,

	BUTTON_ANSWER=21,

	BUTTON_MEMBER_ACCEPT = 30,

 	BUTTON_MEMBER_REJECT = 31,

	BUTTON_APPLY = 32,

	BUTTON_KICK = 33,

	BUTTON_PEACE = 34,

	BUTTON_WAR = 35,

	BUTTON_LEAVE = 36,

	BUTTON_TRADE_ITEM_LEFT=37,

	BUTTON_TRADE_ITEM_RIGHT=38,

	BUTTON_TRADE_SELL=40,

	BUTTON_TRADE_END = 39,

 	BUTTON_WAYPOINT_CHOSE = 45,

	BUTTON_SKIP_DIALOGUE_TEXT = 50,

	DEBUG_SIGNAL=100,
};


/**
 * \struct ClientCommand
 * \brief Datenpaket, das ein Kommando des Anwenders enthält (Struktur)
 *
 * Sämtliche Anweisungen des Nutzers werden auf diese Weise vom Client zum Server übertragen.
 */
struct ClientCommand
{
	/**
	 * \var m_button;
	 * \brief Gibt den angelickten Button an
	 */
	enum Button m_button;

	/**
	 * \var Action::ActionType m_action
	 * \brief Action die ausgefuehrt werden soll
	 */

	Action::ActionType m_action;

	/**
	 * \var Vector m_goal
	 * \brief Zielpunkt der Aktion
	 */
	Vector m_goal;

	/**
	 * \var m_id;
	 * \brief Gibt Typ des Gegenstands an, falls die Aktionen einen Gegenstand betrifft Gibt Id des ZielObjekts an, wenn die Aktion ein Objekt betrifft
	 */
	int m_id;
	/**
	 * \var m_number;
		 * \brief Gibt Anzahl der Gegenst&auml;nde an, falls die Aktionen einen Gegenstand betrifft
	 */
	int m_number;
	/**
	 * \var m_direction;
	 * \brief Gibt an, in welche Richtung ein Gegenstand verschoben wird, wenn die Aktion einen Handel betrifft
	 */
	short m_direction;

	/**
	 * \fn void toString(CharConv* cv)
	 * \brief Konvertiert das Objekt in einen String und schreibt ihn in der Puffer
	 * \param cv Ausgabepuffer
	 */
	void toString(CharConv* cv);


	/**
	 * \fn void fromString(CharConv*  buf)
	 * \brief Erzeugt das Objekt aus einem String
	 * \param cv Eingabepuffer
	 */
	 void fromString(CharConv*  cv);
};

/**
 * \enum ItemLocation
 * \brief Zählt alle möglichen Positionen auf, an denen sich ein beim Client angezeigter Gegenstand befinden kann: Im Inventar des Spielers, im Angebot des Spielers für einen Handel, im Inventar des Handelspartners, im Angebot des Handelspartner, sowie in der Hand beziehungsweise am Körper, wenn der Gegenstand gerade benutzt wird.
 */
enum ItemLocation
{
	LOCATION_INVENTORY=1,
	LOCATION_OFFER=2,
	LOCATION_TRADER_INVENTORY=3,
	LOCATION_TRADER_OFFER=4,
	LOCATION_RIGHT_HAND=5,
	LOCATION_BODY_LOCATION=6,
	LOCATION_LEFT_HAND=7
};



/**
 * \struct PackageHeader
 * \brief Struktur fuer den Header jedes gesendeten Paketes
 */
struct PackageHeader
{
	/**
	 * \var PackageType m_content
	 * \brief Art der Nachricht
	 */
	PackageType m_content;

	/**
	 * \var short m_number
	 * \brief Zaehler fuer die Art der gleichartigen Objekte
	 */
	short m_number;

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
 * \struct NetEvent
 * \brief beschreibt ein Ereignis in der Spielwelt das ueber das Netzwerk mitgeteilt wird
 */
struct NetEvent
{

	/**
	 * \enum EventType
	 * \brief verschiedene Eventarten
	 */
	enum EventType
	{
		NOEVENT =0,
		OBJECT_CREATED =1,
		OBJECT_DESTROYED = 2,
		OBJECT_STAT_CHANGED =3,

		PROJECTILE_CREATED = 5,
		PROJECTILE_DESTROYED = 6,
		PROJECTILE_STAT_CHANGED = 7,

		PLAYER_JOINED = 10,
		PLAYER_QUIT = 11,
		PLAYER_CHANGED_REGION = 12,

		PLAYER_WAYPOINT_DISCOVERED = 15,

		PLAYER_ITEM_INSERT = 20,
		PLAYER_ITEM_REMOVE = 21,
		PLAYER_ITEM_SWAP = 22,

		PLAYER_PARTY_CHANGED = 24,
		PLAYER_PARTY_CANDIDATE = 25,

		PARTY_RELATION_CHANGED = 28,
		FRACTION_RELATION_CHANGED = 29,

		ITEM_DROPPED = 30,
		ITEM_REMOVED = 31,

		TRADER_INVENTORY_REFRESH = 50,
		TRADER_ITEM_BUY = 51,
		TRADER_ITEM_SELL = 52,

		REGION_CUTSCENE = 40,
		REGION_CAMERA = 41,
		REGION_LIGHT = 42,

		DIALOGUE_CREATED =60,
		DIALOGUE_DESTROYED =61,
  		DIALOGUE_STAT_CHANGED =62,

		MUSIC_CHANGED = 70,

		DAMAGE_VISUALIZER_CREATED = 80,
	};

	/**
	 * \var EventType m_type
	 * \brief Gibt die Art des Events an
	 */
	EventType m_type;

	enum GameObjectData
	{
		DATA_TYPE = 0x1,
		DATA_SHAPE = 0x2,
		DATA_STATE = 0x4,
		DATA_SPEED = 0x8,
		DATA_TIMER = 0x10,
	};

	/**
	 * \enum ObjectData
	 * \brief Objektdaten die mit dem Event versendet werden
	 */
	enum WorldObjectData
	{
		DATA_COMMAND =0x100,
		DATA_ACTION =0x200,
		DATA_HP = 0x400,
		DATA_EFFECTS = 0x800,
		DATA_STATUS_MODS = 0x1000,

		DATA_ATTACK_WALK_SPEED = 0x4000,
		DATA_NEXT_COMMAND = 0x8000,
		DATA_ABILITIES = 0x10000,
		DATA_ATTRIBUTES_LEVEL = 0x20000,
		DATA_FLAGS = 0x40000,
		DATA_EXPERIENCE=0x80000,
		DATA_TRADE_INFO = 0x100000,
		DATA_DIALOGUE = 0x200000,
		DATA_WAYPOINT = 0x400000,
		DATA_FIGHT_STAT= 0x800000,
		DATA_SPEAK_TEXT = 0x1000000,
		DATA_REVIVE_LOCATION = 0x2000000,
		DATA_SKILL_ATTR_POINTS = 0x4000000,
		DATA_NAME = 0x8000000,
		DATA_AI = 0x10000000,
	};

	/**
	 * \enum ProjectileData
	 * \brief Projektildaten, die mit dem Event versendet werden
	 */
	enum ProjectileData
	{
		DATA_GOAL_OBJECT = 0x100,
		DATA_MAX_RADIUS = 0x200,
	};

	/**
	 * \var int m_data
	 * \brief Bitmaske die angibt welche Daten gesendet werden
	 */
	int m_data;

	/**
	 * \var int m_id
	 * \brief ID des Objektes, das das Ereignis erzeugt hat
	 */
	int m_id;



	/**
	 * \fn void toString(CharConv* cv)
	 * \brief Konvertiert das Objekt in einen String und schreibt ihn in der Puffer
	 * \param cv Ausgabepuffer
	 */
	void toString(CharConv* cv)
	{
		cv->toBuffer((char) m_type);
		cv->toBuffer(m_data);
		cv->toBuffer(m_id);
	}

	/**
	 * \fn void fromString(CharConv* cv)
	 * \brief Erzeugt das Objekt aus einem String
	 * \param cv Eingabepuffer
	 */
	void fromString(CharConv* cv)
	{
		char ctmp;
		cv->fromBuffer(ctmp);
		m_type = (EventType) ctmp;
		cv->fromBuffer(m_data);
		cv->fromBuffer(m_id);
	}

	/**
	 * \fn NetEvent()
	 * \brief Konstruktor
	 */
	NetEvent()
	{
		m_id =0;
		m_data =0;
		m_type = NOEVENT;
	}

};

typedef std::list<NetEvent> NetEventList;


#endif //NETWORKSTRUCT_H
