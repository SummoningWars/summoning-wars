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

#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include "creature.h"
#include "itemlist.h"
#include "dropitem.h"
#include "networkstruct.h"
#include "servernetwork.h"
#include "item.h"

#include "playerbase.h"

/**
 * \class PlayerCamera
 * \brief Zeigt an, aus welchem Blickwinkel der Spieler die Szene betrachtet
 */
class PlayerCamera
{
	public:

		/**
		 * \fn PlayerCamera()
		 * \brief Konstruktor
		 */
		PlayerCamera();

		/**
		 * \fn void moveTo(float distance, float theta, float phi, float time=0)
		 * \brief Bewegt die Kamera
		 * \param distance Zielabstand
		 * \param theta Zielwinkel zum Boden
		 * \param phi Zielwinkel um die z-Achse
		 * \param time Zeit in der die Aenderung vollzogen wird
		 */
		void moveTo(float distance, float theta, float phi, float time=0);

		/**
		 * \fn void moveRelative(float ddistance, float dtheta, float dphi, float time=0)
		 * \brief Relative Bewegung der Kamera
		 * \param ddistance Aenderung des Abstandes
		 * \param dtheta Aenderung des Winkels zum Boden
		 * \param dphi Aenderung des Winkels um die z-Achse
		 * \param time Zeit in der die Aenderung vollzogen wird
		 */
		void moveRelative(float ddistance, float dtheta, float dphi, float time=0)
		{
			moveTo(m_distance + ddistance, m_theta + dtheta, m_phi + dphi, time);
		}

		/**
		 * \fn void update(float time)
		 * \brief aktualisiert die Kamerastellung nachdem eine gewisse Zeit vergangen ist
		 * \param time Zeit in ms
		 */
		void update(float time);

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
		 * \var float m_goal_distance
		 * \brief Fuer Bewegungen der Kamera: Zielabstand zum Spieler
		 */
		float m_goal_distance;

		/**
		 * \var float m_goal_theta
		 * \brief Fuer Bewegungen der Kamera: Zielwinkel zum Boden
		 */
		float m_goal_theta;

		/**
		 * \var float m_goal_phi
		 * \brief Fuer Bewegungen der Kamera: Zieldrehwinkel um die z-Achse
		 */
		float m_goal_phi;

		/**
		 * \var float m_time
		 * \brief Zeit die zum Erreichen des Zielzustandes bleibt
		 */
		float m_time;

};

/**
 * \class Player
 * \brief Klasse fuer Spielercharactere
 */
class Player : public Creature {
//Public stuff
public:
	//Fields
	//Constructors
	/**
	 * \fn Player(int id, Subtype subtype = "")
	 * \brief Konstruktor
	 * \param id Objekt-ID
	 * \param subtype Spielerklasse
	 *
	 * Legt ein neues Player Objekt an.
	 */
	Player( int id=0, Subtype subtype = "");



	/**
	 * \fn ~Player()
	 * \brief Destruktor
	 *
	 * Gibt den Allokierten Speicher wieder frei
	 */
	~Player();






	//Operations
	/**
	 * \fn virtual bool init()
	 * \brief Initialisierung des Nutzergesteuerten Spielers
	 * \return bool Gibt an ob die Initialisierung erfolgreich war
	 */
	virtual bool init();

	/**
	 * \fn virtual bool destroy()
	 * \brief Zerstoert das Objekt
	 * \return bool der angibt, ob die Zerst&ouml;rung erfolgreich war
	 */
	virtual  bool  destroy();

	/**
	 * \fn void revive()
	 * \brief Erweckt den Spieler wieder zum Leben
	 */
	void revive();

	/**
	 * \fn virtual bool update (float time)
	 * \brief Updatefunktion des Players
	 * \param time Verstrichene Zeit
	 * \return bool, Erfolgreiches Update?
	 *
	 * Die Updatefunktion des Players wird in regelm&auml;ssigen Abst&auml;nden von der World aufgerufen und ben&ouml;tigt die Angabe, wie viel Zeit f&uuml;r das Update zur Verf&uuml;gung gestellt wird. Der R&uuml;ckgabewert gibt an ob die Funktion erfolgreich ausgef&uuml;hrt wurde. Da die Funktion hier virtuell definiert wird, wird sie erst in den abgeleiteten Klassen implementiert.
	 */
	virtual bool update(float time);

	/**
	 * \fn virtual void recalcDamage();
	 * \brief Berechnet den Schaden, der in internen Damage Objekten angezeigt wird neu
	 */
	virtual void recalcDamage();

	/**
	 * \fn virtual void calcBaseDamage(std::string impl,Damage& dmg)
	 * \param impl String der bestimmt, welche Implementation verwendet wird
	 * \param dmg Schaden der Aktion
	 * \brief Berechnet den Basisschaden einer Aktion
	 */
	virtual void calcBaseDamage(std::string impl,Damage& dmg);

	/**
	 * \fn virtual void calcBaseAttrMod()
	 * \brief Die modifizierten Basisattribute werden neu berechnet. Verwendet die Basisattribute, verwendete Items, sowie temporaere Modifikationen.
	 */
	virtual void calcBaseAttrMod();
	
	/**
	 * \fn virtual void initAction()
	 * \brief initialisiert die aktuell gesetzte Aktion
	 */
	virtual void initAction();

	/**
	 * \fn Item* getWeapon()
	 * \brief Gibt die aktuell genutzte Waffe zurueck
	 */
	Item* getWeapon();

	/**
	 * \fn Item* getShield()
	 * \brief Gibt das aktuell genutzte Schild zurueck
	 */
	Item* getShield();

	/**
	 * \fn Party* getParty()
	 * \brief Gibt die Party aus, der der Spieler angehoert
	 */
	Party* getParty()
	{
		return World::getWorld()->getParty(getFraction());
	}

	/**
	 * \fn bool onClientCommand( ClientCommand* command, float delay =0)
	 * \brief  Reaktion auf eine Anweisung des Nutzers
	 * \param command Kommando des Nutzers
	 * \param delay Millisekunden die das Kommando verspaetet erhalten wurde
	 * \return bool gibt an, ob die Behandlung fehlerfrei verlaufen ist
	 */
	bool onClientCommand( ClientCommand* command, float delay =0);

	/**
	 * \fn bool onGamefieldClick(ClientCommand* command)
	 * \brief Reaktion auf einen Klick ins das Spielfeld
	 * \param command Kommando des Nutzers
	 * \return gibt an, ob die Behandlung fehlerfrei verlaufen ist
	 */
	bool onGamefieldClick(ClientCommand* command);

	/**
	 * \fn bool onItemClick(ClientCommand* command)
	 * \brief Reaktion auf einen Klick auf ein Item
	 * \param command Kommando des Nutzers
	 * \return gibt an, ob die Behandlung fehlerfrei verlaufen ist
	 */
	bool onItemClick(ClientCommand* command);

	/**
	 * \fn bool insertItem(Item* itm, bool use_equip = true, bool emit_event=true)
	 * \brief Fuegt ein Items in das Inventar ein
	 * \param itm einzufuegendes Item
	 * \param use_equip wenn auf true gesetzt, wird das Item falls moeglich als Ausruestungsgegenstand verwendet
	 * \param emit_event Wenn auf true gesetzt, wird ein Event erzeugt, dass auf das Item hinweist
	 * \return Position an der das Item eingefuegt wurde
	 */
	short insertItem(Item* itm, bool use_equip = true, bool emit_event=true);

	/**
	 * \fn bool checkItemRequirements(Item* itm)
	 * \brief prueft, ob der Spieler das Item verwenden kann
	 */
    ItemRequirementsMet checkItemRequirements(Item* itm);

	/**
	 * \fn void abortAction()
	 * \brief Bricht die aktuell angefangene Aktion ab
	 */
	void abortAction();


	/**
	 * \fn virtual void toString(CharConv* cv)
	 * \brief Konvertiert das Objekt in einen String und schreibt es in der Puffer
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
	 * \fn void readItem(CharConv* cv)
	 * \brief Liest ein Item aus dem Bitstream und fuegt es ins Inventar ein
	 */
	void readItem(CharConv* cv);

	/**
	 * \fn void readItem(CharConv* cv)
	 * \brief Liest ein Item mit allen Details aus dem Bitstream und fuegt es ins Inventar ein
	 */
	void readItemComplete(CharConv* cv);

	/**
	 * \fn void writeEquipement(CharConv* cv)
	 * \brief Schreibt des Inventar in den Puffer
	 * \param cv Ausgabepuffer
	 */
	void writeEquipement(CharConv* cv);

	/**
	 * \fn void loadEquipement(CharConv* cv)
	 * \brief Laedt das Inventar aus dem Puffer
	 * \param cv Eingabepuffer
	 */
	void loadEquipement(CharConv* cv);

	/**
	 * \fn void toSavegame(CharConv* cv)
	 * \brief Erzeugt ein binaeres Savegame und schreibt es in den Puffer
	 * \param cv Ausgabepuffer
	 */
	void toSavegame(CharConv* cv);


	/**
	 * \fn void fromSavegame(CharConv* cv, bool local=true)
	 * \brief Laedt die Daten aus einem Savegame
	 * \param cv Eingabepuffer
	 * \param local Gibt an, ob es sich um einen lokalen Spieler handelt
	 */
	void fromSavegame(CharConv* cv, bool local=true);

	/**
	 * \fn void increaseAttribute(CreatureBaseAttr::Attribute attr)
	 * \brief Erhoeht das angegebene Attribut
	 */
	void increaseAttribute(CreatureBaseAttr::Attribute attr);

	/**
	 * \fn bool checkRole(std::string role)
	 * \brief Prueft ob der Spieler fuer eine bestimmte Rolle geeignet ist
	 * \param role Rolle
	 */
	bool checkRole(std::string role);


	/**
	 * \fn bool checkAbilityLearnable(Action::ActionType at)
	 * \brief Testet ob eine Faehigkeit erlernbar ist
	 * \param at Faehigkeit
	 * \return true, wenn die Faehigkeit erlernbar ist
	 */
	bool checkAbilityLearnable(Action::ActionType at);

	/**
	 * \fn virtual Action::ActionEquip getActionEquip()
	 * \brief Gibt aus, ob die Aktion einhaendig oder zweihaendig ausgefuehrt wird
	 */
	virtual Action::ActionEquip getActionEquip();

	/**
	 * \fn Action::ActionType getRightAction()
	 * \brief Gibt Aktion, die auf die rechte Maustaste gelegt ist, aus
	 */
	Action::ActionType getRightAction()
	{
		return m_right_action;
	}
	
	/**
	 * \brief Returns the alternate action for the right mouse skill
	 */
	Action::ActionType getRightAlternateAction()
	{
		return m_right_alternate_action;
	}

	/**
	 * \fn Action::ActionType getLeftAction()
	 * \brief Gibt Aktion, die auf die linke Maustaste gelegt ist, aus
	 */
	Action::ActionType getLeftAction()
	{
		return m_left_action;
	}

	/**
	 * \fn short getAttributePoints()
	 * \brief Gibt die Anzahl der zu verteilenden Attributpunkte aus
	 */
	short getAttributePoints()
	{
		return m_attribute_points;
	}

	/**
	 * \fn short getSkillPoints()
	 * \brief Gibt die Anzahl der zu verteilenden Skillpunkte aus
	 */
	short getSkillPoints()
	{
		return m_skill_points;
	}

	/**
	 * \fn Damage& getBaseDamage()
	 * \brief Gibt Basisschaden aus
	 */
	Damage& getBaseDamage()
	{
		return m_base_damage;
	}

	/**
	 * \fn Damage& getLeftDamage()
	 * \brief Gibt Schaden der mit der Aktion auf der linken Maustaste angerichtet wird aus
	 */
	Damage& getLeftDamage()
	{
		return m_left_damage;
	}

	/**
	 * \fn Damage& getRightDamage()
	 * \brief Gibt Schaden der mit der Aktion auf der rechten Maustaste angerichtet wird aus
	 */
	Damage& getRightDamage()
	{
		return m_right_damage;
	}

	/**
	 * \fn std::string& getMessages()
	 * \brief Gibt die Nachrichten des Spielers aus
	 */
	std::string& getMessages()
	{
		return m_messages;
	}

	/**
	 * \fn void addMessage(std::string msg)
	 * \brief Fuegt eine neue Nachricht fuer den Spieler hinzu
	 * \param msg Nachricht
	 */
	void addMessage(std::string msg);

	/**
	 * \fn int getCandidateParty()
	 * \brief Gibt die Nummer der Party aus, bei der sich der Spieler bewirbt
	 */
	int getCandidateParty()
	{
		return m_candidate_party;
	}

	/**
	 * \fn void setCandidateParty(int id)
	 * \brief Setzt die Nummer der Party, bei der sich der Spieler bewirbt
	 * \param id ID der party
	 */
	void setCandidateParty(int id)
	{
		m_candidate_party  = id;
	}

	/**
	 * \fn virtual int getValue(std::string valname)
	 * \brief Schiebt den gewuenschten Attributwert eines Objektes auf den Lua Stack
	 * \param valname Name des Wertes
	 */
	virtual int getValue(std::string valname);

	/**
	 * \fn virtual bool setValue(std::string valname)
	 * \brief Setzt den gewuenschten Attributwert eines Objektes
	 * \param valname Name des Wertes
	 */
	virtual bool setValue(std::string valname);


	/**
	 * \fn virtual void writeNetEvent(NetEvent* event, CharConv* cv)
	 * \brief Schreibt die Daten zu einem NetEvent in den Bitstream
	 * \param event NetEvent das beschrieben wird
	 * \param cv Bitstream
	 */
	virtual void writeNetEvent(NetEvent* event, CharConv* cv);

	/**
	 * \fn virtual void processNetEvent(NetEvent* event, CharConv* cv)
	 * \brief Fuehrt die Wirkung eines NetEvents auf das Objekt aus. Weitere Daten werden aus dem Bitstream gelesen
	 */
	virtual void processNetEvent(NetEvent* event, CharConv* cv);

	/**
	 * \fn PlayerCamera& getCamera()
	 * \brief Gibt die Kameraeinstellung des Spielers aus
	 */
	PlayerCamera& getCamera()
	{
		return m_camera;
	}

	/**
	 * \fn PlayerLook& getPlayerLook()
	 * \brief Gibt Aussehen des Spielers aus
	 */
	PlayerLook& getPlayerLook()
	{
		return m_look;
	}



	/**
	 * \fn RegionLocation getRevivePosition()
	 * \brief Gibt die Position aus, an der der Spieler das Spiel betritt bzw wiederbelebt wird
	 */
	RegionLocation getRevivePosition()
	{
		return m_revive_position;
	}

	/**
	 * \fn bool isUsingSecondaryEquip()
	 * \brief Gibt an, ob der Spieler die Zweiausruestung verwendet
	 */
	bool isUsingSecondaryEquip()
	{
		return m_secondary_equip;
	}

	/**
	 * \fn void setUsingSecondaryEquip(bool sec)
	 * \brief Setzt Verwendung der Zweitausruestung
	 * \param sec Gibt an, ob die Zweitausruestung verwendet wird
	 */
	void setUsingSecondaryEquip(bool sec)
	{
		m_secondary_equip = sec;
		calcBaseAttrMod();
	}


	/**
	 * \fn void setRevivePosition(RegionLocation regloc)
	 * \brief Setzt die Position, an der der Spieler wiederbelebt wird
	 */
	void setRevivePosition(RegionLocation regloc);

	/**
	 * \fn void addWaypoint(short id, bool check_party=false)
	 * \brief Fuegt dem Spieler einen Wegpunkt hinzu
	 * \param id ID der Region
	 * \param check_party Wenn auf true gesetzt dann wird der Wegpunkt bei der kompletten Party hinzugefuegt, falls er noch nicht aktiv war
	 */
	void addWaypoint(short id, bool check_party=false);

	/**
	 * \fn bool checkWaypoint(short id)
	 * \brief Prueft, ob der Spieler sich zu einem Wegpunkt teleportieren darf
	 * \param id ID der Region
	 */
	bool checkWaypoint(short id);

	/**
	 * \fn bool isUsingWaypoint()
	 * \brief Gibt aus, ob der Spieler gerade einen Wegpunkt verwendet
	 */
	bool isUsingWaypoint()
	{
		return m_using_waypoint;
	}

	/**
	 * \fn void setUsingWaypoint(bool val)
	 * \brief Setzt, ob der Spieler gerade einen Wegpunkt verwendet
	 * \param val bool
	 */
	void setUsingWaypoint(bool val);

	/**
	 * \fn virtual bool canBeAttacked()
	 * \brief Gibt an, ob die Kreatur gerade angegriffen werden darf
	 */
	virtual bool canBeAttacked();

	/**
	 * \fn void updateMessageTimer(float time)
	 * \brief aktualisiert den Nachrichten Timer
	 */
	void updateMessageTimer(float time);

	/**
	 * \fn std::string getActionString()
	 * \brief Gibt die aktuelle Aktion als String aus
	 */
	virtual std::string getActionString();

	/**
	 * \fn std::string getActionWeaponSuffix()
	 * \brief Gibt die durch die Waffe bestimmte Erweiterung des Aktionsname aus
	 */
	std::string getActionWeaponSuffix();

	/**
	 * \fn Gender getGender()
	 * \brief Gibt das Geschlecht aus
	 */
	Gender getGender()
	{
		return m_look.m_gender;
	}

	/**
	 * \fn void setGender(Gender gender)
	 * \brief Setzt das Geschlecht
	 */
	void setGender(Gender gender)
	{
		m_look.m_gender = gender;
	}

	/**
	 * \fn LearnableAbilityMap& getLearnableAbilities()
	 * \brief Gibt alle erlernbaren Faehigkeiten aus
	 */
	LearnableAbilityMap& getLearnableAbilities()
	{
		return m_learnable_abilities;
	}

	/**
	 * \fn RegionLocation& getPortalPosition()
	 * \brief Gibt die Position aus, an die der Spieler per Portal zurueck gebracht wird
	 */
	RegionLocation& getPortalPosition()
	{
		return m_portal_position;
	}
	
	/**
	 * \brief returns the Stash of the player
	 */
	Equipement* getStash()
	{
		return m_stash;
	}

	/**
	 * \fn void setPortalPosition(RegionLocation regloc)
	 * \brief Setzt die Position des Town Portals
	 * \param regloc Region und Ort, an dem das Portal steht
	 */
	void setPortalPosition(RegionLocation regloc);

	/**
	 * \fn void insertLearnableAbility(Action::ActionType type, Vector position, int tab)
	 * \brief Fuegt eine erlernbare Faehigkeit hinzu
	 * \param type Typ der Faehigkeit
	 * \param position Position im Skilltree
	 * \param tab Tab in dem dei Faehigkeit auftaucht
	 */
	void insertLearnableAbility(Action::ActionType type, Vector position, int tab);

	/**
	 * \fn void insertLearnableAbility(LearnableAbility& ability)
	 * \brief Fuegt eine erlernbare Faehigkeit hinzu
	 * \param ability erlernbare Faehigkeit
	 */
	void insertLearnableAbility(LearnableAbility& ability);

//Protected stuff
protected:
	//Fields
	//Constructors
	//Accessor Methods
	//Operations

	/**
 * \fn virtual void gainLevel()
 * \brief Wird aufgerufen, wenn das Lebewesen ein Levelup erhält. Passt Erfahrung und alle Basisattribute an.
	 */
	virtual void gainLevel();


	/**
	 * \fn virtual void performActionCritPart(Vector goal, WorldObject* goalobj)
	 * \brief Fuehrt den entscheidenden Part einer Action (Schaden austeilen, Status veraendern usw aus
	 * \param goal Zielpunkt
	 * \param goalobj Zeiger auf der Zielobjekt, ist NULL wenn kein Zielobjekt existiert
	 */
	virtual void performActionCritPart(Vector goal, WorldObject* goalobj);
	
	/**
	 * \brief Swaps the content of the two inventory positions, equiping the second Item. 
	 * \param pos1 Position of Item1. Must not be an equiped item
	 * \param pos2 Position of Item2
	 * This function requires, that Position1 is a not equiped item and pos2 is an equipment item slot
	 */
	void swapEquipItems(Equipement::Position pos1, Equipement::Position pos2);


	/**
	 * \var short m_attribute_points
	 * \brief Anzahl noch zu verteilender Attributspunkte
	 */
	short m_attribute_points;

	/**
	 * \var short m_skill_points
	 * \brief Anzahl noch zu verteilender Skillpunkte
	 */
	short m_skill_points;

	/**
	 * \var bool m_secondary_equip
	 * \brief Zeigt an, ob die zweite Ausruestung benutzt wird
	 */
	bool m_secondary_equip;

	/**
	 * \var Damage m_base_damage
	 * \brief Schaden der mit der Basisattacke ausgeteilt wird
	 */
	Damage m_base_damage;

	/**
	 * \var Damage m_left_damage
	 * \brief Schaden der mit der Attacke auf der linken Maustaste ausgeteilt wird
	 */
	Damage m_left_damage;

	/**
	 * \var Damage m_right_damage
	 * \brief Schaden der mit der Attacke auf der rechten Maustaste ausgeteilt wird
	 */
	Damage m_right_damage;

	/**
	 * \var Action::ActionType m_left_action
	 * \brief Aktion die auf mit linke Maustaste ausgeloest wird
	 */
	Action::ActionType m_left_action;

	/**
	 * \var Action::ActionType m_right_action
	 * \brief Aktion die auf mit rechten Maustaste ausgeloest wird
	 */
	Action::ActionType m_right_action;
	
	/**
	 * \brief alternative action for the right mouse button
	 */
	Action::ActionType m_right_alternate_action;

	/**
	 * \var std::string m_messages
	 * \brief Nachrichten die dieser Spieler erhalten hat
	 */
	std::string m_messages;

	/**
	 * \fn int m_candidate_party
	 * \brief Nummer der Party bei der sich der Spieler bewirbt
	 */
	int m_candidate_party;

	/**
	 * \fn PlayerCamera m_camera
	 * \brief Kameraposition durch die dieser Spieler die Szene sieht
	 */
	PlayerCamera m_camera;

	/**
	 * \var PlayerLook m_look
	 * \brief bestimmt das Aussehen des Spielers
	 */
	PlayerLook m_look;

	/**
	 * \var bool m_using_waypoint
	 * \brief auf true gesetzt, waehrend ein Spieler einen Wegpunkt verwendet
	 */
	bool m_using_waypoint;


	/**
	 * \var RegionLocation m_revive_position
	 * \brief Ort an dem der Spieler wiedererweckt wird
	 */
	RegionLocation m_revive_position;

	/**
	 * \var RegionLocation m_portal_position
	 * \brief Ort an den der Spieler per TownPortal zurueckgebracht wird
	 */
	RegionLocation m_portal_position;

	/**
	 * \var std::set<short> m_waypoints
	 * \brief Wegpunkte die der Spieler aktiviert hat
	 */
	std::set<short> m_waypoints;

	/**
	 * \var std::list<std::string> m_lua_instructions
	 * \brief Speichert die Lua Anweisungen zwischen, die die Questvariablen des Spielers setzen
	 */
	std::list<std::string> m_lua_instructions;

	/**
	 * \var float m_message_clear_timer
	 * \brief Timer, der fuer das entfernen von Nachrichten zustaendig ist
	 */
	float m_message_clear_timer;

	/**
	 * \var LearnableAbilityMap m_learnable_abilities
	 * \brief Faehigkeiten, die der Spieler lernen kann
	 */
	LearnableAbilityMap m_learnable_abilities;
	
	/**
	 * \var m_stash
	 * \brief The content of the stash of the player
	 */
	Equipement* m_stash;

	//Constructors
	//Accessor Methods
	//Operations
};
#endif //PLAYER_H
