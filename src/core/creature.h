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

#ifndef __SUMWARS_CORE_CREATURE_H__
#define __SUMWARS_CORE_CREATURE_H__

#include <string>
#include <list>
#include <algorithm>
#include "world.h"
#include "worldobject.h"
#include "action.h"
#include "command.h"
#include "damage.h"
#include "itemlist.h"
#include "creaturestruct.h"
#include "dialogue.h"
#include "action.h"
// debugging
#include <iostream>



/**
 * \class Creature
 *
 * \brief Base class for living objects like monsters, players and npcs
 *
 */


class Creature : public WorldObject
 {
//Public stuff
public:
	//Fields
	//Constructors
	/**
	 * \brief Constructor^
	 * \param id id of the object
	 */
	Creature(int id);

	/**
	 * \brief Destructor
	 */
	~Creature();
	//Accessor Methods




	/**
	 * \brief returns the current action of the object
	 * \return action pointer
	 * Avoid modifying the data structure from the outside using the pointer - its only non-const for historical reason. Changes made with this pointer will not be propagated via network.
	 */
	Action* getAction()
	{
		return &m_action;
	}

	/**
	 * \brief Sets the current action of the object
	 * \param act action object to be copied
	 * Deep copies the provided \a act parameters. This function also propagates the changes via network.
	 **/
	void setAction(Action& act)
	{
		m_action = act;
		addToNetEventMask(NetEvent::DATA_ACTION);
	}

	/**
	 * \brief returns the current command of the object
	 * \return command pointer
	 * Avoid modifying the data structure from the outside using the pointer - its only non-const for historical reason. Changes made with this pointer will not be propagated via network.
	 */
	Command* getCommand()
	{
		return &m_command;
	}

	/**
	 * \brief returns the next command of the object
	 * \return command pointer
	 * The next command replaces the current command after completion of the current action. Different behavior may result if specific flags are set. See \ref Command for further reference.
	 */
	Command* getNextCommand()
	{
		return &m_next_command;
	}



	/**
	 * \brief returns the damage associated with the current command
	 * \return current damage object
	 * The returned object usually specifies the damage dealt with the commands final action.
	 */
	Damage* getDamage()
	{
		return &m_damage;
	}

	/**
	 * \brief returns the struct containing base attributes of the creature
	 * \sa getBaseAttrMod
	 * 
	 */
	CreatureBaseAttr* getBaseAttr()
	{
		return &m_base_attr;

	}

	/**
	 * \brief returns the modified base attributes of the creature
	 * The modified attributes include the effects of equipement as well as temporary boosts or inflictions.
	 * \sa getBaseAttr
	 * \sa calcBaseAttrMod
	 */
	CreatureBaseAttr* getBaseAttrMod()
	{
		return &m_base_attr_mod;

	}

	

	/**
	 * \brief returns the struct containing dynamic attributs of the creature
	 */
	CreatureDynAttr* getDynAttr()
	{
		return &m_dyn_attr;
	}


	/**
	 * \brief returns which share of the timer's delay is expired
	 * \param timer Number of the timer. Valid values are 1 to NR_TIMERS
	 * Timers are used to realize the cooldown time for abilities. 
	 * A value of 0.0 signals that the timer is not ticking and hence an ability requiring this timer may be used.
	 */
	float getTimerPercent(int timer);


	//Operations
	/**
	 * \brief Initializes all internal data structures
	 * \return true on success, false otherwise
	 */
	virtual bool init ();

	/**
	 * \brief clears all internal data structures
	 * \return true on success, false otherwise
	 */
	virtual bool destroy();


	/**
	 * \brief central update routing
	 * \param time update time in milliseconds
	 * \return true on success, false otherwise
	 *
	 * The update routine is called each frame with the duration of the previous frame as argument. It updates the whole internal status of the object, especially the progress of action and the status of temporary effects.
	 */
	virtual bool  update (float time);



	/**
	 * \brief increases the experience value by the specified amount
	 * \param exp gained experience
	 */
	virtual void gainExperience (float exp);


	/**
	 * \brief Applies a damage object on the creature making it take damage
	 * \param damage damage object
	 * This function calculates the actual damage based on several creature properties like armor, resists and others. The possibility of miss of block is also taken into account. In addition to direct damage, status changes can be inflicted. The effects of the damage object are propagated via network.
	 */
	virtual bool takeDamage(Damage* damage);

	/**
	 * \brief Applies a modification to the base attributes
	 * \param mod modification object
	 * \param add true, if the modification is newly added. Only set to false in internal recalculation functions.
	 * \sa getBaseAttrMod
	 * The modification is added to the modified base attributes. If the time set in the \a mod structure is inequal to 0, the modification is treated as temporary and will be removed after the specified amount of milliseconds. 
	 */
	void applyBaseAttrMod(CreatureBaseAttrMod* mod, bool add = true);

	/**
	 * \brief Applies a modification to the dynamic attributes
	 * \param mod modification object
	 */
	void applyDynAttrMod(CreatureDynAttrMod* mod);

	/**
	 * \brief Removes a modification applied to the base attributes
	 * \param mod modification object
	 * \return true, if a complete recalculation of the modified base attributes is necessary
	 */
	bool removeBaseAttrMod(CreatureBaseAttrMod* mod);

	/**
	 * \brief Removes all modifications to the base attributs
	 */
	void removeAllBaseAttrMod();

	/**
	 * \brief returns a direction to get to this creature
	 * \param pos starting point
	 * \param base_size radius of the base circle of the other object searching for a path
	 * \param layer layer used for collision testing (used to differentiate flying and walking)
	 * \param region region id of the objects
	 * \param dir return value: direction to get to this creature from \a pos
	 * The pathfinding algorithm uses a gradient array approach. For efficiency reason, the gradient is calculated once and stored inside the creature. All other objects seeking a path to this object use this function to evaluate the gradient field.
	 */
	void getPathDirection(Vector pos,short region, float base_size, short layer,  Vector& dir);


	/**
	 * \brief Calculates the walk direction of this object to another object
	 * \param goal target position, only used if the target object is NULL
	 * \param goalobj target object
	 * This function calculates a movement direction for this object. Note difference to \ref getPathDirection which returns a direction for another object leading to this creature.
	 */
	void calcWalkDir(Vector goal,WorldObject* goalobj);

	/**
	 * \brief Ends the current command
	 * \param success specifies if the command ended successfully
	 * \param norepeat if true, a command with repeat flag set will be cancelled completely (not just the current instance)
	 */
	virtual void clearCommand(bool success, bool norepeat = true);

	/**
	 * \brief Serializes the object to the specified buffer
	 * \param cv output buffer
	 */
	virtual void toString(CharConv* cv);

	/**
	 * \brief Deserializes the object from the specified buffer
	 * \param cv input buffer
	 */
	virtual void fromString(CharConv* cv);

	/**
	 * \brief Checks if the creature can perform a specific ability
	 * \param at ability
	 * \return true, if the ability is available
	 */
	bool checkAbility(Action::ActionType at);


	/**
	 * \brief Calculates the damage associated with an action that may be performed by this creature
	 * \param act action type
	 * \param dmg return value: damage object
	 * The returned damage object is usually the damage inflicted on the targets if the action is carried out.
	 */
	virtual void calcDamage(Action::ActionType act,Damage& dmg);

	/**
	 * \brief Calculates the attribute modification associated with an action  that may be performed by this creature
	 * \param act action type
	 * \param bmod modifications to base attributes
	 * \param dmod modifications to dynamic attributes
	 * The modifiers can be applied on other creatures or on itselfs, depending on the type of the action.
	 */
	virtual void calcActionAttrMod(Action::ActionType act,CreatureBaseAttrMod & bmod, CreatureDynAttrMod& dmod);

	/**
	 * \brief Returns if the action is carried out unarmed, with a one- or two-hand weapon
	 * This information is required for graphic representation only.
	 */
	virtual Action::ActionEquip getActionEquip()
	{
		return Action::ONE_HANDED;
	}

	/**
	 * \brief Returns whether the object is allowed as attack target
	 * There are several reasons for disallowing attacks including already dead, involved in a dialog, invincible by script and others.
	 */
	virtual bool canBeAttacked();

	/**
	 * \brief Pushes the  value with the required name to the lua stack
	 * \param valname Name of the value
	 * This function serves as a universal get function for the lua script interface. Lua interface code uses this to get data from the C++ to the lua stack. Refer to the scripting documentation for valid value names.
	 */
	virtual int getValue(std::string valname);

	/**
	 * \brief Sets a member taking the value from the lua stack
	 * \param valname Name of the value
	 * This function serves as a universal set function for the lua script interface. Lua interface code uses this to get data from the lua stack to C++. The value is popped from the stack afterwards. Refer to the scripting documentation for valid value names.
	 */
	virtual bool setValue(std::string valname);

	/**
	 * \brief Adds a command which was set by script
	 * \param cmd command
	 * \param time time limit for the command
	 * Commands created by script are added to a special queue which is completed using FIFO regime. If a command can not completed within the provided \a time, it is aborted. Scripted commands superseed other command generation mechanism like the monster AI.
	 */
	virtual void insertScriptCommand(Command &cmd, float time=50000);

	/**
	 * \brief Clears the queue of scripted commands, ending scripted behavior of the creature
	 */
	virtual void clearScriptCommands();

	/**
	 * \brief Returns, whether the creature is steered by script right now
	 * \return true, if there are scripted commands in the queue
	 */
	bool hasScriptCommand();

	/**
	 * \brief sells an item specified by an inventory position
	 * \param position position of the item
	 * \param item return value: pointer to the item removed from this creatures inventory
	 * \param gold reference to the gold value of the buying creature
	 * This function removes the item from this objects inventory and returns the pointer in the \a item parameter. Other functions are used to insert the item into the buyers inventory. The price of the items is substracted from the \a gold amount. It is also checks if the gold amount is sufficient.
	 * This function is used (on the trader) if the trader is the seller and the player is the buyer.
	 */
	void sellItem(short position, Item* &item, int& gold);

	/**
	 * \brief buys an item
	 * \param item item buyed. It is inserted to the inventory and the pointer is set to NULL
	 * \param gold reference to the gold value of the selling creature
	 *
	 * This function adds the object to this creatures inventory and resets the \a item parameter. The price of the item is added to the \a gold amount. 
	 * This function is used (on the trader) if the trader is the buyer and the player is the seller.
	 */
	void buyItem(Item* &item, int& gold);


	/**
	 * \brief Sets the text spoken by the creature right now
	 * \param text spoken text
	 * This is mainly used in dialogs, but may be used for speech bubbles as well.
	 **/
	void speakText(CreatureSpeakText& text);

	/**
	 * \brief updates the combat information displayed in the attack tooltips
	 * \param blockchance chance to hit the enemy
	 * \param armorperc damage reduction by the armor of the enemy
	 * \param attacker name of the enemy
	 */
	void updateFightStat(float hitchance, float armorperc, TranslatableString attacked);


	/**
	 * \brief Returns the currently spoken text
	 * \return currently spoken text
	 */
	CreatureSpeakText& getSpeakText()
	{
		return m_speak_text;
	}

	/**
	 * \brief Returns the struct containing information on the trade status
	 * \return trade status
	 */
	CreatureTradeInfo& getTradeInfo()
	{
		return m_trade_info;
	}

	/**
     * \brief Clears the spoken text
	 */
	void clearSpeakText();

	/**
	 * \brief Returns the current dialogue of the creature
	 * \return dialogue
	 * Returns NULL if the creature is not involved in a dialogue right now.
	 */
	Dialogue* getDialogue()
	{
		return getRegion()->getDialogue(m_dialogue_id);
	}

	/**
	 * \brief Returns the ID of the current dialogue of the creature
	 * \return dialogue id
	 * Returns 0 if the creature is not involved in a dialogue right now.
	 */
	int getDialogueId()
	{
		return m_dialogue_id;
	}

	/**
	 * \brief Assigns a dialogue to the creature
	 * \param id ID of the dialogue
	 */
	void setDialogue(int id);

	/**
	 * \brief Sets the reference name
	 * \param name reference name
	 * The reference name is used to is used to mark creatures as specific persons occuring in storyline.  Dialogue options and spoken text are usually connected to reference names. This name is also displayed (potentially translated) in the dialogue windows and in the health bar.
	 */
	void setRefName(std::string name)
	{
		m_refname = name;
		addToNetEventMask(NetEvent::DATA_NAME);
	}
	
	/**
	 * \brief Returns the reference name of the creature
	 * return  reference name
	 * \sa setRefName
	 */
	virtual std::string getRefName()
	{
		if (m_refname =="")
			return getName().getRawText();

		return m_refname;
	}



	/**
	 * \brief Returns the translatable name of the creature
	 * This is the name intented for display in the GUI. If a reference name is set, this is returned. Otherwise, the object type is used.
	 */
	virtual TranslatableString  getName()
	{
		if (m_name.getRawText() == "" && m_refname != "")
			return TranslatableString(m_refname,"sumwars");

		return WorldObject::getName();
	}

	/**
	 * \brief Returns the equipement of the creature
	 * \return equipement
	 */
	Equipement* getEquipement()
	{
		return m_equipement;
	}

	/**
	 * \brief Sets the equipement
	 * \param equ equipement
	 * This function does a mere pointer copy  and is intended for internal use only.
	 */
	void setEquipement(Equipement* equ)
	{
		m_equipement = equ;
	}

	/**
	 * \brief Returns the current trade partner
	 * \return trade partner
	 * Returns NULL if the creature is not involved in a trade right now.
	 */
	Creature* getTradePartner();

	/**
	 * \brief Sets the trade partner
	 * \param id id of the tradepartner
	 */
	void setTradePartner(int id)
	{
		m_trade_info.m_trade_partner = id;
		m_trade_info.m_last_sold_item = 0;
		addToNetEventMask(NetEvent::DATA_TRADE_INFO);
	}

	/**
	 * \brief Returns statistic combat data
	 * \return combat data
	 */
	FightStatistic& getFightStatistic()
	{
		return m_fight_statistic;
	}

	/**
	 * \brief Returns the image for a specified emotion
	 * \param emotion emotion name
	 * \return emotion image name
	 * Emotion images are displayed in dialogues. Refer to the scripting documentation for valid emotion names.
	 */
	std::string getEmotionImage(std::string emotion);
	
	
	/**
	 * \brief Calculates the time requires to perform an action
	 * \param action action name
	 * \return action execution time in milliseconds
	 */
	float getActionTime(Action::ActionType action);

protected:

	/**
	* \brief Called on level-up - increases several attributes and does a full-heal.
	*/
	virtual void gainLevel();

	/**
	 * \brief Recalculates the modified base attributes
	 * Starts with the base attributes and adds all modifications by equipement, skills and temporary modifications.
	 */
	virtual void calcBaseAttrMod();



	/**
	 * \brief Recalculates the damage objects associated with the currently selected actions
	 */
	virtual void recalcDamage();

	/**
	 * \param impl implementation used for calculation the damage
	 * \param dmg input/output value: damage object
	 * \brief Calculates the damage based on the creatures attributes
	 * The calculated damage is added to the provided \a dmg object. The \a impl parameter is mainly used to differentiate the character and attack typ dependant formulas. Refer to the scripting documentation for a list of valid implementations.
	 */
	virtual void calcBaseDamage(std::string impl,Damage& dmg);

	/**
	 * \param impl implementation used for calculation the damagemplementation used for calculation the damage
	 * \param dmg input/output value: damage object
	 * \brief Calculates the damage of an action
	 * The calculated damage is added to the provided \a dmg object. The damage is added to the amount calculated by the \ref calcBaseDamage function. This function is used for those damage calculation which can not be done in Lua only.
	 */
	virtual void calcAbilityDamage(std::string impl,Damage& dmg);



	/**
	 * \brief Calculates the next action based on the current command
	 */
	virtual void calcAction();

	/**
	 * \brief Updates the current command
	 * This function checks if the current command is complete and might replace is with the next command or a command from the script queue.
	 */
	virtual void updateCommand();

	/**
	 * \brief Special routine which generates commands based on status modifications like berserk or confused.
	 * Does not generate any command if none of the relevant status modifications is active.
	 */
	void calcStatusModCommand();
	
	/**
	 * \brief Calculates the action that is used for berserk
	 * Does not generate any command if no berserk mod is inflicted.
	 */
	Action::ActionType getBerserkAction();

	/**
	 * \brief Initializes the current action.
	 * Usually called after \ref calcAction.
	 */
	virtual void initAction();

	/**
	 * \brief Performs the current action for the provided time.
	 * \param time available time in milliseconds
	 * \return remaining time if the action was completed, 0 otherwise
	 * Updates the action completion status and performs the critical part of the action, if the critical point of the action was passed. If the action needs less that the available \a time to complete, the remaining time is stored to the \a time parameter.
	 */
	virtual void performAction(float &time);

	/**
	 * \brief Performs the critical part of the action
	 * \param goal target point
	 * \param goalobj Target object, might be NULL if no target object is set
	 * The critical part of the action might be dishing out damage, creation of arrows or missiles, application of modifiers and various others. The specific implementation is stored in the \ref Action database.
	 */
	virtual void performActionCritPart(Vector goal, WorldObject* goalobj);

	/**
	 * \brief Called on death of the creature
	*/
	virtual void die();



	/**
	 * \brief Handles the collision with another object
	 * \param s shape of the colliding object
	 * Updates the movement direction and might abort the command if the creature got stuck.
	 */
	bool handleCollision(Shape* s);

	/**
	 * \brief Tries to detect and avoid collisions that might occur in future
	 * \param time time to look forward in milliseconds
	 */
	void collisionDetection(float time);


	/**
	 * \brief Writes the data for a NetEvent concerning this creature to the buffer
	 * \param event netevent
	 * \param cv output buffer
	 * A NetEvent is any change to the internal data of the creature that must be transferred via network. This functions writes all the data needed on other clients to the buffer.
	 * \sa processNetEvent
	 */
	virtual void writeNetEvent(NetEvent* event, CharConv* cv);

	/**
	 * \brief Reads the data for a NetEvent concerning this creature from the buffer and applies the changes
	 * \param event netevent
	 * \param cv input buffer
	 * \sa writeNetEvent
	 * Reads a NetEvent written by \ref writeNetEvent and updates the internal data accordingly. This function also takes the delay of the data transfer into account. In addition, the game may approach the new status instead of setting it instantly. Required prediction and interpolation is done by this function.
	 */
	virtual void processNetEvent(NetEvent* event, CharConv* cv);

	/**
	 * \brief Returns the current action name as string
	 * \return action name
	 */
	virtual std::string getActionString();

	/**
	 * \brief Returns the progress of the current action
	 * \return progress as value in the interval 0-1
	 */
	virtual float getActionPercent();

	/**
	 * \brief Returns the list of flags set
	 * \param flags return value: list of flags
	 * Flags might be set by game logic, but also by script. This list of flags is directly connected to the graphical display of object statuus.
	 */
	virtual void getFlags(std::set<std::string>& flags);

	/**
	 * \brief clears all flags
	 */
	virtual void clearFlags();

	/**
	 * \brief Returns the timer required for an action
	 * \param action action name
	 * \return number of the timer required for this action.
	 * \sa getTimer
	 * \sa getTimerPercent
	 * Actions may only be started if their timer is not running. A return value of 0 signals, that no timer is required.
	 */
	int getTimerNr(Action::ActionType action);

	/**
	 * \brief Returns the remaining cooldown time of the timer required for an action
	 * \param action action name
	 * \return remaining timer time. A value of 0 signals that the timer is not running and the action may be started
	 */
	float getTimer(Action::ActionType action);




//Private stuff
private:
	//Fields




	/**
	 * \brief action currently performed
	 */
	Action m_action;

	/**
	 * \brief current command
	 */
	Command m_command;

	/**
	 * \brief next command
	 */
	Command m_next_command;

	/**
	 * \brief List of commands added by script
	 * Scripted commands superseed the normal commands. Script commands are executed in a FIFO regime. The float value specifies a time limit how long the command may take before it is aborted.
	 **/
	std::list<std::pair<Command,float> > m_script_commands;

	/**
	 * \brief Text currently spoken by the creature
	 */
	CreatureSpeakText m_speak_text;

	/**
	 * \brief Information on the current trade status
	 */
	CreatureTradeInfo m_trade_info;

	/**
	 * \brief ID of the current dialogue partner
	 */
	int m_speak_id;

	/**
	 * \brief Reference name of the creature
	 * The reference name is used to bind ingame creature objects to objects referenced in script code. It determines the dialogue options and speech roles.
	 */
	std::string m_refname;

	/**
	 * \brief Remaining time before a scripted command is aborted
	 */
	float m_script_command_timer;


	/**
	 * \brief ID of the current trade
	 * Is 0 if the creature is not involved in a trade right now.
	 */
	int m_trade_id;

	/**
	 * \brief Base attributes
	 */
	CreatureBaseAttr m_base_attr;

	/**
	 * \brief Base attributes with modification by equipment and temporary effects
	 */
	CreatureBaseAttr m_base_attr_mod;


	/**
	 * \brief dynamic attributes
	 */
	CreatureDynAttr m_dyn_attr;

	/**
	 * \brief Damage associated with the current action
	 */
	Damage m_damage;

	protected:


		/**
		* \brief ID of the current dialogue
		* Is 0 if the creature is not involved in a dialogue right now.
		*/
		int m_dialogue_id;


		/**
		* \brief Cooldown times for the different timers in milliseconds
		* As long as a timer is running, action requiring this timer can not be used. 
		* If a timer is not running, the value is set to 0. 
		*/
		float m_timers[NR_TIMERS];

	
		/**
		* \brief Timer values set when starting a timer
		* Used to calculate how much of the cooldown time has elapsed in percent.
		*/
		float m_timers_max[NR_TIMERS];

		/**
		 * \brief name of the emotionset
		 * The emotionset is used to map emotion names to emotion images.
		 */
		std::string m_emotion_set;

		/**
		* \brief Name of the base action
		* The base action is started if the intended action is not available due to cooldown timer or status modifications like mute.
		*/
		Action::ActionType m_base_action;

		/**
		* \brief Equipement
		*/
		Equipement* m_equipement;

		/**
		* \brief statistic combat data
		*/
		FightStatistic m_fight_statistic;

		/**
		* \brief Gradient field leading small objects to this creature
		*/
		PathfindInfo* m_small_path_info;

		/**
		* \brief Gradient field leading small flying objects to this creature
		*/
		PathfindInfo* m_small_flying_path_info;


		/**
		* \brief Gradient field leading medium objects to this creature
		*/
		PathfindInfo* m_medium_path_info;


		/**
		* \brief Gradient field leading big objects to this creature
		*/
		PathfindInfo* m_big_path_info;


		/**
		* \brief Gradient field used for non-object target points
		*/
		PathfindInfo* m_path_info;

		/**
		 * \brief counter used to detect if the creature gets stuck while searching for a path
		 */
		float m_pathfind_counter;
};

#endif // __SUMWARS_CORE_CREATURE_H__

