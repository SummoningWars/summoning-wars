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

#ifndef MONSTER_BASE_H
#define MONSTER_BASE_H

#include "dropslot.h"
#include "worldobject.h"
#include "creaturestruct.h"

/**
 * \struct MonsterAIVars
 * \brief Sammlung von Variablen, die die KI steuern
 */
struct MonsterAIVars
{
	/**
	 * \var float m_ai_sight_range
	 * \brief Sichtweite der AI
	 */
	float m_sight_range;
	
	/**
	 * \var  float m_ai_shoot_range
	 * \brief Gibt an, ab welcher Entfernung Monster anfangen Fernangriffe zu nutzen
	 */
	float m_shoot_range;
	
	/**
	 * \var float m_action_range
	 * \brief Reichweite ab der die Monster anfangen Aktionen auszufuehren (Angriff erst innerhalb der Sichtweite)
	 */
	float m_action_range;
	
	/**
	 * \var float m_randaction_prob
	 * \brief Wahrscheinlichkeit eine zufaellige Aktion statt der optimalen auszufuehren
	 */
	float m_randaction_prob;
	
	/**
	 * \var float m_chase_distance
	 * \brief Gibt an, wie lange nach einem Angriff das Monster noch versucht den Angreifer zu stellen
	 */
	float m_chase_distance;
	
	/**
	 * \var float m_warn_radius
	 * \brief Radius in dem Verbuendete gewarnt werden, wenn eines der Monster angegriffen wird
	 */
	float m_warn_radius;
	
	/**
	 * \brief Writes the DataStructure to the XML node
	 * \param node XML node
	 * This function also deals with XML trees that are already written. In this case, only the difference is written.
	 */
	void writeToXML(TiXmlNode* node);
};

/**
 * \struct MonsterBasicData
 * \brief enthaelt alle Daten die zur Erzeugung eines Monsters noetig sind
 */
struct MonsterBasicData
{
	
	/**
	 * \var CreatureBaseAttr m_base_attr
	 * \brief Basisattribute
	 */
	CreatureBaseAttr m_base_attr;
	
	/**
	 * \var GameObject::Type m_type
	 * \brief Informationen zum Typ
	 */
	GameObject::Type m_type;
	
	/**
	 * \var GameObject::Type m_subtype
	 * \brief Informationen zum Subtyp
	 */
	GameObject::Type m_subtype;
	
	/**
	 * \var WorldObject::Race m_race
	 * \brief Rasse des Monsters
	 */
	WorldObject::Race m_race;

	/**
	 * \var std::string m_fraction_name
	 * \brief Fraktion des Objektes
	 */
	std::string m_fraction_name;
	
	/**
	 * \var DropSlot m_drop_slots[4]
	 * \brief Beschreibung der Items die das Monster droppen kann, wenn es stirbt
	 */
	DropSlot m_drop_slots[4];
	
	/**
	 * \var MonsterAIVars m_ai_vars
	 * \brief Einstellunge fuer die AI
	 */
	MonsterAIVars m_ai_vars;
	
	/**
	 * \var m_radius
	 * \brief Radius des Grundkreises
	 */
	float m_radius;
	
	/**
	 * \var short m_layer
	 * \brief Ebene in der das Monster sich befindet
	 */
	short m_layer;
	
	/**
	 * \brief Name des Monsters
	 */
	TranslatableString m_name;
	
	/**
	 * \var  std::string m_emotion_set
	 * \brief Name des Satzes der Emotionsbilder
	 */
	std::string m_emotion_set;
	
	/**
	 * \brief assignment operator
	 * \param other assigned value
	 */
	void operator=(MonsterBasicData& other);
	
	/**
	 * \brief Writes the DataStructure to the XML node
	 * \param node XML node
	 * This function also deals with XML trees that are already written. In this case, only the difference is written.
	 */
	void writeToXML(TiXmlNode* node);
};

/**
 * Name fuer eine Gruppe von Monstern die erzeugt wird
 **/ 
typedef std::string MonsterGroupName;

/**
 * \struct MonsterGroup
 * \brief Struktur fuer eine Gruppe von Monstern
 */
class MonsterGroup
{
public:
	/**
	 * \struct SubGroup
	 * \brief Untergruppe der Monster bei denen alle Monster den selben Typ haben
	 */
	struct SubGroup
	{
		/**
		 * \var GameObject::Subtype m_subtype
		 * \brief Subtype der erzeugten Monster
		 */
		GameObject::Subtype m_subtype;
		
		/**
		 * \var float m_prob
		 * \brief Wahrscheinlichkeit mit der ein Monster erzeugt wird
		 */
		float m_prob;
		
		/**
		 * \var int m_number
		 * \brief Anzahl der Monster die maximal erzeugt werden. Jedes Monster wird gemaess der Wahrscheinlichkeit prob erzeugt. Im Erwartungswert also prob * number Monster
	 	 */
		int m_number;
	};
	
		/**
		 * \fn void addMonsterGroup(GameObject::Subtype subtype, int number =1, float prob = 1.0)
		 * \brief Fuegt der Monstergruppe eine Subgruppe hinzu
		 * \param subtype Typ der Monster
		 * \param number Anzahl der Monster
		 * \param prob Wahrscheinlichkeit mit der ein Monster erzeugt wird
		 */
		void addMonsters(GameObject::Subtype subtype, int number =1, float prob = 1.0)
		{
			SubGroup sg;
			sg.m_subtype = subtype;
			sg.m_number = number;
			sg.m_prob = prob;
			m_monsters.push_back(sg);
		}
		
		/**
		 * \fn std::list<SubGroup> m_monsters
		 * \brief die Monster die zu der Gruppe gehoeren
		 */
		std::list<SubGroup> m_monsters;
};

#endif

