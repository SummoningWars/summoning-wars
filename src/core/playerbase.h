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

#ifndef PLAYER_BASE_H
#define PLAYER_BASE_H

#include "worldobject.h"
#include "creaturestruct.h"

enum Gender
{
	MALE =1,
 FEMALE = 2,
};

/**
 * \struct PlayerLook
 * \brief Enthaelt grundlegende Informationen fuer die Darstellung des Spielers
 */
struct PlayerLook
{
	/**
	 * \var std::string m_name
	 * \brief Geschlecht des Spielers
	 */
	Gender m_gender;
	
	/**
	 * \var std::string m_name
	 * \brief Name des Looks
	 */
	std::string m_name;
	
	/**
	 * \var std::string m_render_info
	 * \brief Name des RenderInfo
	 */
	std::string m_render_info;
	
	/**
	 * \var std::string m_item_suffix
	 * \brief Suffix, das an die Itemnamen angehaengt wird
	 * Das ermoeglicht verschiedenes Aussehen der Items bei verschiedenen Spielern
	 */
	std::string m_item_suffix;
	
	/**
	 * \var std::string m_hair
	 * \brief Renderinfo fuer das Haar
	 */
	std::string m_hair;
	
	/**
	 * \var std::string m_emotion_set
	 * \brief Name des Satzes von Emotionen des Spielers
	 */
	std::string m_emotion_set;
	
	/**
	 * \fn void operator=(PlayerLook other)
	 * \brief Zuweisungsoperator
	 */
	void operator=(PlayerLook other);
	
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
 * \struct LearnableAbility
 * \brief Struktur fuer die Beschreibung einer erlernbaren Faehigkeit
 */
struct LearnableAbility
{
	/**
	 * \var Action::ActionType m_type
	 * \brief Name der Aktion
	 */
	Action::ActionType m_type;
	
	/**
	 * \var Vector m_skilltree_position
	 * \brief Position des Bilds im Skilltree (relativ zur Groesse des Fensters)
	 */
	Vector m_skilltree_position;
	
	/**
	 * \var int m_id
	 * \brief interne ID
	 */
	int m_id;
	
	/**
	 * \var int m_skilltree_tab
	 * \brief Nummer des Tabs auf dem die Faehigkeit dargestellt wird (1,2,3)
	 */
	int m_skilltree_tab;
	
	/**
	 * \var std::string m_image
	 * \brief Name des Bildes
	 */
	std::string m_image;
	
	/**
	 * \var short m_req_level
	 * \brief Erforderliches Level
	 */
	short m_req_level;
	
	/**
	 * \var std::list<Action::ActionType> m_req_abilities
	 * \brief Vorher zu erlernende Faehigkeiten
	 */
	std::list<Action::ActionType> m_req_abilities;
	
	/**
	 * \var LearnableAbility& operator=(LearnableAbility other)
	 * \brief Zuweisungsoperator
	 */
	LearnableAbility& operator=(LearnableAbility other)
	{
		m_type = other.m_type;
		m_skilltree_position = other.m_skilltree_position;
		m_id = other.m_id;
		m_skilltree_tab = other.m_skilltree_tab;
		m_req_level = other.m_req_level;
		m_req_abilities = other.m_req_abilities;
		
		return *this;
	}
};

typedef  std::map<int,LearnableAbility> LearnableAbilityMap;

/**
 * \struct PlayerBasicData
 * \brief Enthaelt alle Daten, die fuer die Initialisierung eines neuen Spielers notwendig sind
 */
struct PlayerBasicData
{
	/**
	 * \var CreatureBaseAttr m_base_attr
	 * \brief Basisattribute
	 */
	CreatureBaseAttr m_base_attr;
	
	/**
	 * \var GameObject::Type m_subtype
	 * \brief Informationen zum Subtyp
	 */
	GameObject::Type m_subtype;
	
	/**
	 * \var std::string m_name
	 * \brief Name der Spielerklasse
	 */
	std::string m_name;
	
	/**
	 * \var Action::ActionType m_base_ability
	 * \brief Basisfaehigkeit der Spielerklasse
	 */
	Action::ActionType m_base_ability;
	
	/**
	 * \var LearnableAbilityMap m_learnable_abilities
	 * \brief erlernbare Faehigkeiten
	 */
	LearnableAbilityMap m_learnable_abilities;
	
	/**
	 * \var std::string m_tabnames[3]
	 * \brief Namen der Skilltreetabs
	 */
	std::string m_tabnames[3];
	
	/**
	 * \var std::list<std::string> m_item_req_tags
	 * \brief Liste von Tags, wenn im Itemrequirement eines Items eines der Tags auftaucht, kann das Item verwendet werden
	 */
	std::list<std::string> m_item_req_tags;
	
	/**
	 * \var std::list<GameObject::Subtype> m_start_items
	 * \brief Liste der Items, mit denen ein Spieler in das Spiel startet
	 */
	std::list<GameObject::Subtype> m_start_items;
};


#endif

