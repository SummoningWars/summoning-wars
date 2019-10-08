/**
 * \file eventfunc.h
 * \brief Defines functions creating new events and triggering existing ones.
 */

/**
 * This trigger is inserted directly after the creation of the region. It is always the first trigger that is activated in a region.
 */
struct create_region
{
	
};

/**
 * This trigger is inserted if a players enters a region.\n
 * The trigger membervariables can be accessed by trigger.variablename in the condition and effect lua codes (trigger.player for example).
 */
struct enter_region
{
	/**
	 * ID of the player
	 */
	int player;
}

/**
 * This trigger is inserted if a players leaves a region. Be aware that the player is not an object in the current region and various functions will be invalid. \n
 * The trigger membervariables can be accessed by trigger.variablename in the condition and effect lua codes (trigger.player for example).
 */
struct leave_region
{
	/**
	 * ID of the player
	 */
	int player;
};

/**
 *  This trigger is inserted if a objectgroup or objectgroup template that is either named or has at least one location is created. When creating a region, these triggers are inserted directly after the create_region trigger for all object groups that are placed by the mapgenerator. \n
 * The trigger membervariables can be accessed by trigger.variablename in the condition and effect lua codes (trigger.position for example). \n
 * In addition to the members listes below a member variable of type Vector containing the coordinates is added for each location in the objectgroup. The name of the variable matches the locationname.\n
 * 
 */
struct create_template
{
	/**
	 * Type of the objectgroup
	 */
	string templname;
	
	/**
	 * Name of the objectgroup that is defined in the Region XML file
	 */
	string name;
	
	/**
	 * Coordinates of the center of the object group
	 */
	Vector position;
	
	/**
	 * angle that was used when inserting the object group
	 */
	float angle;
};

/**
 * This trigger is inserted if a unit is created. \n
 * The trigger membervariables can be accessed by trigger.variablename in the condition and effect lua codes (trigger.unit for example).
 */
struct create_unit
{
	/**
	 * ID of the unit
	 */
	int unit;
};

/**
 * This trigger is created when a player has finished a step.
 * The trigger membervariables can be accessed by trigger.variablename in the condition and effect lua codes (trigger.player for example).
 */
struct player_moved
{
	/**
	 * ID of the player
	 */
	int player;
};

/**
 * This trigger is created when a unit has finished a step. The trigger is also created if the unit is a player (in addition to the player_moved trigger). \n
 * This should be used sparingly, especially in crowded regions, as it is created very often.  \n
 * The trigger membervariables can be accessed by trigger.variablename in the condition and effect lua codes (trigger.unit for example).
 */
struct unit_moved
{
	/**
	 * ID of the unit
	 */
	int unit;
};

/**
 * This trigger is created if a unit takes damage (or suffers other negative effects like status changes) from another unit. There is no trigger created if the attack was blocked. \n
 * The trigger membervariables can be accessed by trigger.variablename in the condition and effect lua codes (trigger.defender for example). \n
 * At the moment, the attacker ID is always valid. Script functions that allow dealing damage to units by script might be implemented in future. In such a case when there is no attacking unit the attacker ID is 0.
 */
struct unit_hit
{
	/**
	 * ID of the attacking unit
	 */
	int attacker;
	
	/**
	 * ID of the defending unit
	 */
	int defender;
	
	/**
	 * Damage dealt
	 */
	float damage;
};

/**
 * This trigger is created if an object is hit  by a damage dealing attack. This is also created if a unit was hit, in this case unit_hit is created additionally. \n
 * The trigger membervariables can be accessed by trigger.variablename in the condition and effect lua codes (trigger.defender for example).
 */
struct object_hit
{
	/**
	 * ID of the attacking unit
	 */
	int attacker;
	
	/**
	 * ID of the defending unit
	 */
	int defender;
	
	/**
	 * ID of the used object. This member variable allows ScriptObject events use this trigger.
	 */
	int _id;
};

/**
 * This trigger is created when a player uses an object. \n
 * The trigger membervariables can be accessed by trigger.variablename in the condition and effect lua codes (trigger.used_object for example).
 */
struct object_use
{
	/**
	 * ID of the used object
	 */
	int used_object;
	
	/**
	 * ID of the player using the object
	 */
	int user;
	
	/**
	 * ID of the used object. This member variable allows ScriptObject events use this trigger.
	 */
	int _id;
};

/**
 * This trigger is created when a units health drops below 0. The unit still exists and is performing the die action. So accessing and modifying the units properties is valid. This includes reviving it by raising its health above 0. \n
 * The trigger membervariables can be accessed by trigger.variablename in the condition and effect lua codes (trigger.unit for example).
 */
struct unit_die
{
	/**
	 * ID of the unit
	 */
	int unit;
};


/**
 * This trigger is created when a dead unit has completed its die action and will be deleted. Accessing the units properties is not allowed. The only valid (and useful) operation using the unit member variable of this trigger is comparison with other IDs. \n
 * The trigger membervariables can be accessed by trigger.variablename in the condition and effect lua codes (trigger.unit for example).
 */
struct unit_dead
{
	/**
	 * ID of the unit
	 */
	int unit;
};


/**
 * This trigger is created when a unit has completed a script generated command. The trigger is also created when the command was aborted because the target was not found or was unreachable. \n
 * The trigger membervariables can be accessed by trigger.variablename in the condition and effect lua codes (trigger.unit for example).
 */
struct command_complete
{
	/**
	 * ID of the unit
	 */
	int unit;
	
	/**
	 * Command that was completed
	 */
	string command;
	
	/**
	 * true if the command was completed with success. Commands may be aborted if the target is unreachable or does no longer exist.
	 */
	bool success;
};

/**
 * This trigger is created when a unit has completed a script generated command and the command queue of that object is empty now. The trigger is also created when the command was aborted because the target was not found or was unreachable. \n
 * The trigger membervariables can be accessed by trigger.variablename in the condition and effect lua codes (trigger.unit for example).
 */
struct all_commands_complete
{
	/**
	 * ID of the unit
	 */
	int unit;
	
	/**
	 * Command that was completed
	 */
	string command;
	
	/**
	 * true if the command was completed with success. Commands may be aborted if the target is unreachable or does no longer exist.
	 */
	bool success;
};


/**
 * This trigger is created when a camera move in cutscenemode is complete. The trigger is not created if the duration of the move was 0. This usually applies to the first call to addCameraPosition that defines a starting position instead of a move. 
 */
struct camera_move_complete
{
	
};

/**
 * This trigger is created when a camera move in cutscenemode is complete and the camera position queue ist empty afterwards.
 */
struct all_camera_moves_complete
{
	
};


/**
 * This trigger is created in addition to the enter_region trigger if a player enters a region and his complete party is in the region after that. The main purpose is when assembling the party by teleportPlayer calls to detect when the last teleport is complete
 */
struct party_complete
{
	
};

 
/**
 * Creates a new Event that catches the trigger named \a triggername. The effect and (optionally) the condition are set by a subsequent setEffect and steCondition call. This calls have to be made in the same script as the createEvent call.
 * 
 * \param triggername Name of the trigger that is caught by the new Event
 * \param once If set to true, the event will be deleted after the first successful execution
 * \param regionname Name of the region in which the event will be creates. By default, the event will be created in the current region.
 * 
 * \sa setEffect setCondition
 */

void createEvent(string triggername ,bool once = false ,string regionname= current_region);

/**
 * Sets the condition of a scriptcreated event. The event has to be created by createEvent or createScriptObjectEvent before. The condition code is always executed when the corresponding event catches a trigger. The code must be a valid lua script that returns true or false. \n
 * Be aware that the \a luacode is a new lua script that does not share any local variables with the script calling the addCondition function. You can access the member variables of the trigger by the trigger.membername as usual.  
 * 
 * \param luacode Code that does a condition check for the event
 * 
 * \sa createEvent createScriptObjectEvent
 */
void setCondition(string luacode);

/**
 * Sets the effect of a scriptcreated event. The event has to be created by createEvent or createScriptObjectEvent before. \n
 * This code is execute when the corresponding event catches a trigger and the condition code has returned true.  If no condition is set, the event will be executed always. If the event has the \a once flag set, it will be deleted afterwards. The code must be a valid lua script. \n
 * Be aware that the \a luacode is a new lua script that does not share any local variables with the script calling the addEffect function. You can access the member variables of the trigger by the trigger.membername as usual.
 * 
 * \param luacode Code that performs the effect of the event
 */
void setEffect(string luacode);

/**
 * Creates a new trigger. The trigger is inserted into the region immediately. All events in the region, that react on the trigger \a  triggername are checked by their condition and in case of success executed. \n
 * The \a triggername can be an arbitrary string, but names of predefined triggernames (like unit_die) are valid as well. In this case it is very advisable to add all the trigger member variables that are included in systemgenerated triggers of the same name. \n
 * Membervariables may be added to the trigger by using the addTriggerVariable function.
 * 
 * \param triggername Name of the trigger
 * \param regionname Name of the region in which the trigger is inserted. Default value is the current region
 * 
 * \sa addTriggerVariable
 **/
void insertTrigger(string triggername, string regionname = current_region);

/**
 * Creates a trigger with a certain delay time. The trigger is inserted into the current region. All other properties are equivalent to triggers created by insertTrigger.
 * 
 * \param triggername Name of the trigger
 * \param time delay time in milliseconds
 * 
 * \sa insertTrigger addTriggerVariable
 */
void startTimer(string triggername, float time);

/**
 * Adds a member variable to a trigger. The trigger has to be created by insertTrigger or startTimer before. The member variable can be accessed by trigger.varname in condition and effect lua scripts. \n
 * Supported value types are number, string and Vector at the moment. Arbitrary tables are not supported.
 * 
 * The \a varname "_id" has a special meaning: Triggers with such a member will trigger ScriptObject member events if a ScriptObject with the corresponding ID exists.
 * 
 * \param varname name of the member variable
 * \param value value of the member variable
 * 
 * \sa insertTrigger startTimer
 */
void addTriggerVariable(string varname, various value);

/**
 * Creates a new ScriptObject. These are objects which do need to be defined in a data file. The whole behavior can implemented by script during runtime. To create predefined scriptobjects use the createObject function. \n
 * The object is inserted into the current region at the center of the Area given by the \a shape parameter.
 * 
 * \param subtype arbitrary string that will be stored to the subtype property
 * \param renderinfo name of the renderinfo that is used to generate the visual representation of this object
 * \param shape defines the position and the size of the object
 * \param layer is used to initialize the layer of the object
 * \return Returns the ID of the created object. Returns 0 if creation was not successful.
 * 
 * \sa createObject getObjectValue createScriptObjectEvent
 */
int createScriptObject( string subtype, string renderinfo, Area shape, string layer ="normal");

/**
 * Creates a new ScriptObject member event. This event can be started by triggers that have a member variable named "_id" if the value matches the ID of the object. Condition and Effect can be set by setCondition and setEffect.
 * 
 * \param objectid ID of a ScriptObject
 * \param triggername Name of the Trigger that can start the Event
 * \param once If set to true, the event will be deleted after the first successful execution
 * 
 * \sa setEffect setCondition addTriggerVariable
 */
void createScriptObjectEvent(int objectid, string triggername ,bool once = false);




