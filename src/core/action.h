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

#ifndef ACTION_H
#define ACTION_H

#include <string>
#include "gettext.h"
#include "charconv.h"
#include "debug.h"
#include "geometry.h"
#include <map>
#include <vector>
#include <list>
#include <tinyxml.h>
#include "hybridimplementation.h"


/**
 * \struct Action
 * \brief Beschreibt eine Aktion, welche eine Figur ausf&uuml;hren kann. Eine Aktion ist dabei die kleinste Einheit einer Handlung.
 */
struct Action {
//Public stuff

public:

	/**
	 * \enum ActionType
	 * \brief Aufzaehlung verschiedener Aktionen
	 */

	typedef  std::string ActionType;

	/**
	 * \enum TargetType
	 * \brief Aufzaehlung verschiedener Zieltypen fuer Aktionen
	 */
	enum TargetType
	{
		NONE=0,
		MELEE = 1,
		RANGED = 2,
		PASSIVE = 4,
		SELF = 8,
 		PARTY =16,
		PARTY_MULTI = 32,
		CIRCLE = 64,
	};

	/**
	 * \enum Flags
	 * \brief Aufzaehlung von Sondereigenschaften von Aktionen
	 */
	enum Flags
	{
		MUTE_AFFECTED= 1,
	};

	/**
	 * \struct ActionInfo
	 * \brief Enthaelt einige grundlegende Informationen zu einer Aktion
	 */
	struct ActionInfo
	{
		/**
		 * \var int m_timer_nr
		 * \brief Nummer des Timers, der bei benutzen der Aktion gestartet wird. Moegliche Werte sind 0,1,2
		 */
		int m_timer_nr;

		/**
		 * \var float m_standard_time
		 * \brief Zeit in ms, die die Aktion ohne Modifikatoren in Anspruch nimmt
		 */
		float m_standard_time;

		/**
		 * \var float m_timer
		 * \brief Gibt die Zeitdauer in ms an, die der Timer laeuft, der beim Benutzen dieser Aktion gestartet wird.
		 */
		float m_timer;

		/**
		 * \var ActionType m_base_action
		 * \brief Gibt die Aktion an, die stattdessen ausgefuehrt wird, wenn die Aktion selbst nicht benutzt werden kann weil der Timer noch lauft
		 */
		ActionType m_base_action;

		/**
		 * \var float m_critial_perc
		 * \brief Prozentsatz zu dem die Aktion abgeschlossen ist, wenn der eigentliche Effekt der Aktion ausgeloest wird
		 */
		float m_critical_perc;

		/**
		 * \var TargetType m_target_type
		 * \brief Gibt die Art des Zielobjekts an
		 */
		TargetType m_target_type;

		/**
		 * \var char m_flags
		 * \brief verschiedene Eigenschaften der Aktion in Bitkodierung
		 */
		char m_flags;

		/**
		 * \var std::string m_name
		 * \brief Name der Aktion
		 */
		std::string m_name;

		/**
		 * \var std::string m_description
		 * \brief Abkuerzung der Beschreibung als String
		 **/
		std::string m_description;

		/**
		 * \var float m_radius
		 * \brief Wirkungsradius der Aktion. Genaue Verwendung des Wertes haengt von der Implementation der Aktion ab
		 */
		float m_radius;


		/**
		 * \var std::string m_projectile_type
		 * \brief Typ eines erzeugten Geschosses
		 */
		std::string m_projectile_type;

		/**
		 * \var float m_projectile_speed
		 * \brief Geschwindigkeit des Geschosses
		 */
		float m_projectile_speed;

		/**
		 * \var char m_projectile_flags
		 * \brief Flags des Geschosses
		 */
		char m_projectile_flags;

		/**
		 * \var int m_projectile_counter
		 * \brief Zaehler fuer das Geschoss. Genaue Verwendung haengt vom Geschosstyp ab
		 */
		int m_projectile_counter;

		/**
		 * \var float m_projectile_radius
		 * \brief Radius eines Geschosses
		 */
		float m_projectile_radius;

		/**
		 * \var HybridImplementation m_effect
		 * \brief Effekt der Aktion
		 */
		HybridImplementation m_effect;

		/**
		 * \var HybridImplementation m_damage
		 * \brief Schaden der Aktion
		 */
		HybridImplementation m_damage;

		/**
		 * \var HybridImplementation m_base_mod
		 * \brief BasisModifikation der Aktion
		 */
		HybridImplementation m_base_mod;

		/**
		 * \var HybridImplementation m_dyn_mod
		 * \brief Modifikation dynamischer Attribute durch die Aktion
		 */
		HybridImplementation m_dyn_mod;
	};

	/**
	 * \fn enum ActionEquip
	 * \brief Aufzaehlung, welche Ausruestungsgegenstaende fuer die Aktion verwendet werden
	 */
	enum ActionEquip
	{
		NO_WEAPON=0,
		ONE_HANDED =1,
		TWO_HANDED =2,
	};

	//Fields
	//Constructors
	/**
	 * \fn Action(ActionType type)
	 * \brief Legt ein neues Action Objekt an
	 * \param type initialisiert den Typ der Aktion
	 */
	Action(ActionType type)
	{
		m_type = type;
		m_time =0;
		m_elapsed_time =0;
	}


	/**
	 * \fn Action()
	 * \brief Legt eine leere Aktion an
	 */
	Action()
	{
		m_type = "noaction";
		m_time =0;
		m_elapsed_time =0;
	}

	/**
	 * \fn static void init()
	 * \brief initialisiert die statischen Variablen
	 */
	static void init();

	/**
	 * \fn static void cleanup()
	 * \brief loescht alle Daten
	 */
	static void cleanup();

	/**
	 * \fn static ActionInfo* getActionInfo(ActionType type)
	 * \brief Gibt zu einer Aktion die Basisinformationen aus
	 * \param type die Aktion
	 * \return Zeiger auf die Daten
	 */
	static ActionInfo* getActionInfo(ActionType type)
	{
		std::map<ActionType,ActionInfo>::iterator it;
		it = m_action_info.find(type);
		if (it != m_action_info.end())
			return &(it->second);

		return 0;
	}

	/**
	 * \fn static std::string getName(ActionType type)
	 * \brief Gibt zu einer Aktion den Namen aus
	 * \param type die Aktion
	 * \return Name der Aktion
	 */
	static std::string getName(ActionType type);

	/**
	 * \fn static std::string getDescription(ActionType type)
	 * \brief Gibt zu einer Aktion die Beschreibung aus
	 * \param type die Aktion
	 * \return Beschreibung der Aktion
	 */
	static std::string getDescription(ActionType type);

	/**
	 * \fn static ActionType getType(std::string name)
	 * \brief Gibt zu einem Name einer Aktion den enum Wert aus
	 * \param name Name der Aktion
	 */
	static ActionType getActionType(std::string name);

	/**
	 * \fn void toString(CharConv* cv)
	 * \brief Konvertiert das Objekt in einen String und schreibt ihn in der Puffer
	 * \param cvAusgabepuffer
	 */
	void toString(CharConv* cv);


	/**
	 * \fn void fromString(CharConv* cv)
	 * \brief Erzeugt das Objekt aus einem String
	 * \param cv Eingabepuffer
	 */
	void fromString(CharConv* cv);

	/**
	 * \fn static void loadAbilities(TiXmlNode* node)
	 * \brief Laedt die Faehigkeiten aus dem angegebenen XML Dokument
	 * \param node XML Dokument
	 */
	static void loadAbilities(TiXmlNode* node);

	/**
	 * \fn 	static void loadAbility(TiXmlNode* node)
	 * \brief Laedt eine Faehigkeit aus dem angegebenen XML Knoten
	 * \param node XML Knoten
	 */
	static void loadAbility(TiXmlNode* node);

	/**
	 * \fn loadAbilityData(const char* pFilename);
	 * \brief Liest die Daten zu einer Faehigkeit aus einer XML Datei
	 * \param pFilename Name der XML Datei
	 */
	static bool loadAbilityData(const char* pFilename);

	/**
	 * \fn static bool loadHybridImplementation(TiXmlNode* node, HybridImplementation& impl)
	 * \brief laedt Daten fuer eine hybride Implementation
	 * \param node XML Knoten
	 * \param impl Struktur in die die Daten geladen werden
	 */
	static bool loadHybridImplementation(TiXmlNode* node, HybridImplementation& impl);

	static void toxml();

	//Fields
	/**
	 * \var m_type;
	 * \brief Typ der Aktion
	 */
	ActionType m_type;

	/**
	 * \var m_elapsed_time;
	 * \brief bereits vergangene Zeit der Aktion
	*/
	float m_elapsed_time;

	/**
	 * \var m_time;
	 * \brief Gesamtdauer der Aktion
	 */
	float m_time;

	/**
	 * \var ActionEquip m_action_equip
	 * \brief Ausruestung mit der die Aktion ausgefuehrt wird
	 */
	ActionEquip m_action_equip;


	/**
	 * \var Vector m_goal
	 * \brief Ziel der Aktion
	 */
	Vector m_goal;

	/**
	 * \var m_goal_object_id;
	 * \brief ID des Zielobjekts
	 */
	int  m_goal_object_id;

	/**
	 * \var static std::map<ActionType,ActionInfo> m_action_info
	 * \brief Enthaelt Basisinformationen fuer alle Aktionen
	 */
	static std::map<ActionType,ActionInfo> m_action_info;

};



#endif //ACTION_H
