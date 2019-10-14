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

#ifndef __SUMWARS_CORE_COMMAND_H__
#define __SUMWARS_CORE_COMMAND_H__



#include <string>
#include "action.h"


/**
 * \class Command
 * \brief Describes an action which an actor can receive. The execution will be split into multiple actions which will be executed sequentially
 */
class Command {
/**
 * Public stuff
 */
public:
	enum Flags
	{
		REPEAT = 0x1,
 		SECONDARY = 0x2,
	};
	/**
	 * Fields
	 */
	/**
	 * Constructors
	 */
	/**
	 * \fn Command(Action::ActionType type, Vector goal, int goal_object_id)
     * \brief Constructor
     * \param type Initializes the type of the command
     * \param goal Goal of the command
     * \param goal_object_id Initializes the ID of the goal object
     * Creates a new command object
	 */
	Command(Action::ActionType type, Vector goal, int goal_object_id)
	{
		m_type = type;
		m_goal = goal;
		m_goal_object_id= goal_object_id;
		m_damage_mult=1;
		m_range=1;
		m_flags =0;
	}

	/**
	 * \fn Command()
     * \brief Constructor
	 */
	Command()
	{
		m_type = "noaction";
		m_goal = Vector(0,0);
		m_goal_object_id= 0;
		m_damage_mult=1;
		m_range=1;
		m_flags =0;
	}

	/**
	 * \fn void toString(CharConv* cv)
     * \brief Converts the object into a string and writes it to the buffer
     * \param cv output buffer
	 */
	void toString(CharConv* cv);

	/**
	 * \fn void fromString(CharConv* cv)
     * \brief Creates the object from a string
     * \param cv input buffer
	 */
	void fromString(CharConv* cv);

	/**
	 * \fn bool operator!=(Command& other)
     * \brief not equals operator
	 */
	bool operator!=(Command& other);

	/**
	 * \fn void operator=(Command& other)
     * \brief assignment operator
	 */
	void operator=(Command& other);

	/**
	 * Fields
	 */
	/**
	 * \var m_type
     * \brief Type of the command
	 */
	 Action::ActionType m_type;

	/**
	 * \var Vector m_goal
     * \brief Goald of the command
	 */
	Vector m_goal;

	/**
	 * \var m_goal_object_id
     * \brief ID of the goal object
	 */
	int  m_goal_object_id;

	/**
	 * \var float m_range
     * \brief Range of the action
	 */
	float m_range;

	/**
	 * \var m_damage_mult;
     * \brief Multiplyer to the damage resulting from this action
	 */
	float m_damage_mult;

	/**
	 * \var char m_flags
     * \brief bit encoded flags
	 */
	char m_flags;
	/**
	 * Constructors
	 */
	/**
	 * Accessor Methods
	 */
	/**
	 * Operations
	 */
};
#endif // __SUMWARS_CORE_COMMAND_H__
