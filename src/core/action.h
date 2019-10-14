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

#ifndef __SUMWARS_CORE_ACTION_H__
#define __SUMWARS_CORE_ACTION_H__

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
 * \brief Describes an action which an actor can perform. An action is the smallest unit of an act.
 */
struct Action {
//Public stuff

public:

	/**
	 * \enum ActionType
     * \brief Enum of different actions
	 */

	typedef  std::string ActionType;

	/**
	 * \enum TargetType
     * \brief Enum of different target types for actions
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
     * \brief Enum of special properties of actions
	 */
	enum Flags
	{
		MUTE_AFFECTED= 1,
	};

	/**
	 * \struct ActionInfo
     * \brief Holds basic information for an action
	 */
	struct ActionInfo
	{
		/**
		 * \var int m_timer_nr
         * \brief Number of the timer which will be started with the action. Prossible values are 1, 2 ,3
		 */
		int m_timer_nr;

		/**
		 * \var float m_standard_time
         * \brief Time the action lasts in ms, not affected by any modifications
		 */
		float m_standard_time;

		/**
		 * \var float m_timer
         * \brief Current running time since start of the action
		 */
		float m_timer;

		/**
		 * \var ActionType m_base_action
         * \brief Holds the action that will be performed instead when the timer is running
		 */
		ActionType m_base_action;

		/**
		 * \var float m_critial_perc
         * \brief Percentage of completion when the actual effect will be executed
		 */
		float m_critical_perc;

		/**
		 * \var TargetType m_target_type
         * \brief Type of the target object
		 */
		TargetType m_target_type;

		/**
		 * \var char m_flags
         * \brief different properties of the action in bit encoded
		 */
		char m_flags;

		/**
		 * \var std::string m_name
         * \brief Name of the action
		 */
		std::string m_name;

		/**
		 * \var std::string m_description
         * \brief Shortcut of the description as a string
		 **/
		std::string m_description;

		/**
		 * \var float m_radius
         * \brief Effective radius of the action. Exact value is dependand of the implementation.
		 */
		float m_radius;


		/**
		 * \var std::string m_projectile_type
         * \brief Type of the created projectile.
		 */
		std::string m_projectile_type;

		/**
		 * \var float m_projectile_speed
         * \brief Speed of the projectile
		 */
		float m_projectile_speed;

		/**
		 * \var char m_projectile_flags
         * \brief Flags of the projectile
		 */
		char m_projectile_flags;

		/**
		 * \var int m_projectile_counter
         * \brief Counter of the projectile. Exact usage is dependand on the implementation.
		 */
		int m_projectile_counter;

		/**
		 * \var float m_projectile_radius
         * \brief Radius of the projectile
		 */
		float m_projectile_radius;

		/**
		 * \var HybridImplementation m_effect
         * \brief Effect of the action
		 */
		HybridImplementation m_effect;

		/**
		 * \var HybridImplementation m_damage
         * \brief Damage of the action
		 */
		HybridImplementation m_damage;

		/**
		 * \var HybridImplementation m_base_mod
         * \brief Base modifications of the action
		 */
		HybridImplementation m_base_mod;

		/**
		 * \var HybridImplementation m_dyn_mod
         * \brief Modification of dynamic attributes through this action.
		 */
		HybridImplementation m_dyn_mod;
	};

	/**
	 * \fn enum ActionEquip
     * \brief Enum which equipment items will be used for the action.
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
     * \brief Constructs a new action object
     * \param type Initializes the action type
	 */
	Action(ActionType type = "noaction"):
		m_elapsed_time(0),
		m_time(0),
		m_action_equip(NO_WEAPON),
		m_goal(0, 0),
		m_goal_object_id(0)
	{}

	/**
	 * \fn static void init()
     * \brief Initializes the static variables
	 */
	static void init();

	/**
	 * \fn static void cleanup()
     * \brief Deletes all data
	 */
	static void cleanup();

	/**
	 * \fn static ActionInfo* getActionInfo(ActionType type)
     * \brief Returns the base information of an action
     * \param type Type of the action
     * \return Pointer to the action
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
     * \brief Returns the name of the action
     * \param type Type of the action
     * \return Name of the action
	 */
	static std::string getName(ActionType type);

	/**
	 * \fn static std::string getDescription(ActionType type)
     * \brief Returns the description of an action
     * \param type Type of the action
     * \return Description of the action
	 */
	static std::string getDescription(ActionType type);

	/**
	 * \fn static ActionType getType(std::string name)
     * \brief Returns the enum value to an action name
     * \param name Name of the action
	 */
	static ActionType getActionType(std::string name);

	/**
	 * \fn void toString(CharConv* cv)
     * \brief Converts the object into a string and writes it to the buffer
     * \param cv Output buffer
	 */
	void toString(CharConv* cv);


	/**
	 * \fn void fromString(CharConv* cv)
     * \brief Creates the object from a string
     * \param cv input buffer
	 */
	void fromString(CharConv* cv);

	/**
	 * \fn static void loadAbilities(TiXmlNode* node)
     * \brief Loads the abilities for an action for an XML document
     * \param node XML document
	 */
	static void loadAbilities(TiXmlNode* node);

	/**
	 * \fn 	static void loadAbility(TiXmlNode* node)
     * \brief Loads the abilities from an XML node
     * \param node XML node
	 */
	static void loadAbility(TiXmlNode* node);

	/**
	 * \fn loadAbilityData(const char* pFilename);
     * \brief Loads data for an ability from an XML file
     * \param pFilename Name of the XML file
	 */
	static bool loadAbilityData(const char* pFilename);

	/**
	 * \fn static bool loadHybridImplementation(TiXmlNode* node, HybridImplementation& impl)
     * \brief Loads data for a hybrid implementation
     * \param node XML node
     * \param impl Input structure in which the data will be loaded
	 */
	static bool loadHybridImplementation(TiXmlNode* node, HybridImplementation& impl);

	static void toxml();

	//Fields
	/**
	 * \var m_type;
     * \brief Type of the action
	 */
	ActionType m_type;

	/**
	 * \var m_elapsed_time;
     * \brief Elapsed time
	*/
	float m_elapsed_time;

	/**
	 * \var m_time;
     * \brief Finish time of the action
	 */
	float m_time;

	/**
	 * \var ActionEquip m_action_equip
     * \brief Equipment type with which the action will be executed
	 */
	ActionEquip m_action_equip;


	/**
	 * \var Vector m_goal
     * \brief Goal of the action
	 */
	Vector m_goal;

	/**
	 * \var m_goal_object_id;
     * \brief ID of the target object
	 */
	int  m_goal_object_id;

	/**
	 * \var static std::map<ActionType,ActionInfo> m_action_info
     * \brief Holds base information for all actions
	 */
	static std::map<ActionType,ActionInfo> m_action_info;

};



#endif // __SUMWARS_CORE_ACTION_H__
