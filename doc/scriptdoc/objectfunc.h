/**
 * \file objectfunc.h
 * \brief Defines several functions involve object interaction.
 */


/**
 * Returns the value of a named property of the object. The object is referenced by its ID and must be located in the current region. Players are an exception from this rule, a player ID is always valid.
 * 
 * Accepted strings for \a valname are listed below. The type of the return value depends on the requested value:
 * 
 * Strings for \a valname that are always valid:
 * - \b "region" Returns the ID of the region in which the object is located. If the given object ID does not refer to a player, this will always return the name of the current region. 
 *   \n Return type: string
 * - \b "type" Returns the basic type of the object. Possible return values are "FIXED_OBJECT", "MONSTER", "PLAYER", "SCRIPTOBJECT" or "TREASURE"
 *   \n Return type: string
 * - \b "subtype" Returns a more exact type of the object.
 *   \n Return type: string
 * - \b "position" Returns the location of the object.
 *   \n Return type: Vector
 * - \b "angle" Returns the rotation angle of the object in degree. An angle of zero corresponts to the direction of (1,0).
 * - \b "height" Returns the height of the object in meter. This height is a pure visual property and has no influence on collision detection.
 *   \n Return type: float
 * - \b "speed" Returns speed of the objects in meter per millisecond.
 *   \n  Return type: Vector
 * - \b "layer" Returns the layer of the object. The layer determines which objects can collide with each other. Possible values are:
 *     -# "normal" collides with "normal", "base" and "air". This the default value.
 *     -# "air"  collides with "normal" and "air" but not with "base". This is usually used for flying objects like arrows.
 *     -# "base" collides with "normal" and "base" but not with "air". This is usually used for pits and other objects that should let flying objects pass
 *     -# "nocollision" collides with nothing (not even other objects in "nocollision" layer)
 *     -# "dead" special layer reserved for dead monsters
 *   \n Return type: string
 * - \b "name" Returns the name of the objects. Per default, this depends on the subtype only, but different names can be set. If ID refers to a player, the player's name is returned of course.
 *   \n Return type: string
 * 
 * Strings for \a valname that are valid if the ID refers to a unit (player or monster)
 *  - \b "fraction" Returns the name of the fraction the unit. The fraction determines whether two units are hostile, neutral or allied to each other.
 *    \n Return type: string
 *  - \b "level"  Return type: int
 *  - \b "strength" Return type: int
 *  - \b "dexterity" Return type: int
 *  - \b "willpower" Return type: int
 *  - \b "magic_power" Return type: int
 *  - \b "max_health" Return type: int
 *  - \b "block" Return type: int
 *  - \b "attack" Return type: int
 *  - \b "armor" Return type: int
 *  - \b "resist_fire" Return type: int
 *  - \b "resist_ice" Return type: int
 *  - \b "resist_air" Return type: int
 *  - \b "resist_phys" Return type: int
 *  - \b "max_resist_ice" Return type: int
 *  - \b "max_resist_air" Return type: int
 *  - \b "max_resist_fire" Return type: int
 *  - \b "max_resist_phys" Return type: int
 *  - \b "attack_speed" Returns the number of attack per seconds. The number is multiplied with 1000, so one attack per second equals to attack speed 1000. \n Return type: int
 *  - \b "walk_speed" Returns the number of steps per seconds. The number is multiplied with 1000, so one step per second equals to walk speed 1000. Note that the real speed (in meters per seconds) also depends on the length of a step
 *  - \b "health" Return type: float
 *  - \b "experience" Return type: float
 *  - \b "emotionset" Returns the name of the emotionset. The emotionset determines, which picture are shown for this unit in the dialogue window. 
 *    \n Return type: string
 *  - \b "timer1" Returns the remaining time of timer1. The player can not use any abilities, that use this timer during this time.
 *    \n Return type: float
 *  - \b "timer2" Return type: float
 *  - \b "ignored_by_ai" When this option is set, the unit is completely ignored by the ai. Returns the current value.
 *    \n Return type: bool
 * 
 * The returned value is always the current value, that may be influenced by equipement, boosters, curses etc. To get the base value prepend "base:" to the string ("base:strength" for example"). This is valid for all items in the list from "strength" to "walk_speed".
 * 
 *  Strings for \a valname that are valid if the ID refers to a monster:
 *   - \b "ai_state" Returns "active" or "inactive". If ai state is set to inactive, the ai will not generate any commands for this unit.
 *    \n Return type: string
 * 
 * Strings for \a valname that are valid if the ID refers to a player:
 *  - \b "attribute_points" Returns the number of attribute points that the player has left to distribute
 *    \n Return type: int
 *  - \b "skill_points" Returns the number of skill points that the player has left to distribute
 *    \n Return type: int
 *  - \b "gold"
 * 
 * \param objectid ID of an object
 * \param valname name of a property
 * \return Returns the value of the property. Returns nil if the \a objectid or \a valname are invalid.
 * 
 * For ScriptObjects, there is a table scriptobjectvars[ objectid ] that can be used to store properties of these objects. If ID refers to a ScriptObject, each \a valname is valid and the function will return scriptobjectvars[ objectid ][ valname ].
 */
various getObjectValue(int objectid, string valname);

/**
 * This function is equivalent to getObjectValue(int objectid, string valname). 
 * 
 * \param objectid ID of an object
 * \param valname name of a property
 * \return Returns the value of the property. Returns nil if the \a objectid or \a valname are invalid.
 * 
 * \sa getObjectValue
 */
various get(int objectid, string valname);

/**
 * Sets the value of a named property of an object. The object is referenced by its ID and must be located in the current region. Players are an exception from this rule, a player ID is always valid.
 * 
 * In general, all property names, that are valid for getObjectValue are valid for setting as well. Exceptions are "region", "type", "subtype", "position" and "speed". The required type for \a value is the same that is returned by the corresponding getObjectValue call. \n
 * \b Note: To set the position of an object use moveObject.
 * 
 * \param objectid ID of an object
 * \param valname name of a property
 * \param value new value of the property
 * 
 * For ScriptObjects, there is a table scriptobjectvars[ objectid ] that can be used to store properties of these objects. If ID refers to a ScriptObject, each \a valname is valid and the function will modify scriptobjectvars[ objectid ][ valname ]. This functions does not support arbitrary lua tables so writing directly to this table may be preferable.
 * 
 *  \sa getObjectValue moveObject
 */
void setObjectValue(int objectid, string valname, various value);

/**
 * This function is equivalent to setObjectValue(int objectid, string valname). 
 * 
 * \param objectid ID of an object
 * \param valname name of a property
 * \param value new value of the property
 * 
 * \sa setObjectValue
 */
void set(int objectid, string valname, various value);

/**
 * Moves the object references by \a objectid to the position specified by \a pos.  The object must be in the currect region. If \a collision_check is set, the object is moved as close as possible to the desired position, but it is ensured, that it doesnt collide with any other objects. \n
 * Note that in contrast to the getObjectValue function, player IDs that are not in the current region are invalid.
 * 
 * \param objectid ID of an object
 * \param pos new position of the object
 * \param collision_check if set, object is slightly moved if neccesary in order to avoid collisions
 */
void moveObject(int objectid, Vector pos ,bool collision_check=true);

/**
 * \param objectid ID of an object
 * \return Returns true, if the object is in the current region, false otherwise.
 */
bool objectIsInRegion(int objectid);

/**
 * Creates a named reference to an object. The can be retrieved by getObjectByNameRef later.
 * 
 * \param objectid ID of an object
 * \param name Name of the reference
 * 
 * \sa getObjectByNameRef
 */
void setObjectNameRef(int objectid, string name);

/**
 * Returns the ID of an object that is given by a reference name.
 * 
 * \param  name Name of an object reference
 * \return Returns the ID of the object. Returns 0 if no reference with the given name exists
 * 
 * \sa setObjectNameRef
 */
int getObjectByNameRef(string name);

/**
 * Creates a new object with the given \a subtype and inserts it at the given \a position into the current region. The object is always inserted with collision checking, so the final position may differ from the requested position slightly. If exact positioning is required, move the object afterwards. \n
 * If an angle is given, the object is rotated accordingly. If height is given, the object is displayed with a height offset. This is a pure visual modification, that does not influence gameplay! \n
 * The \a subtype parameter may be an object template name. In this case the subtype of the object is determined on basis of the environment in the region. Note that no object is created, if the object template has explicitely no object for the currect environment.
 * 
 * \param subtype subtype of the new object
 * \param position position of the new object
 * \param angle angle of the new object
 * \param height height offset for rendering the object
 * \return Returns ID of the new object. Returns 0 if creation was not successful.
 */
int createObject(string subtype, Vector position, float angle=0, float height =0);

/**
 * Creates a new Projectile with the given \a subtype and inserts it at the given \a position into the current region. If a \a target vector is given, the projectile will fly from the starting position to the target with the given \a speed. The speed is multiplied with 1000 to match the walk speed values, so 10000 equals a speed of 10 meter per second. The default speed is 0 if there is no target and 10000 if there is a target. If a \a margin is given, the projectile will not start exactly at \a position, instead it is created with an offset of margin towards the target. This is useful if the projectile should start at the border of a circular object instead of its center.
 * \param subtype projectile type
 * \param position position where the projectile is created
 * \param target position there projectile flies to
 * \param speed speed in 1000*meter/second, default is 10000 if a target is given
 * \param margin offset of starting position
 */
void createProjectile(string subtype, Vector position, Vector target=position, float speed=0, float margin=0);

/**
 * Creates a new objectgroup with the given \a type and places it in the current region with center at the given \a position.
 * If an \a angle is given, the objectgroup is rotate by this angle as a whole.
 * The \a type parameter may be an objectgroup template name. In this case the type of the objectgroup is determined on basis of the environment in the region.
 * \param type type of the objectgroup
 * \param position center of the objectgroup
 * \param angle rotation of the objectgroup
 */
void createObjectGroup( string type, Vector position , float angle=0);

/**
 * Checks if the object references by the \a objectid is inside the \a area. The object is considered as inside, if its base shape intersects the area.
 * 
 * \param objectid ID of an object
 * \param area area to check
 * \return Returns true if the object is in the area, false otherwise. Returns nil if input is invalid.
 */
bool objectIsInArea( int objectid, Area area);

/**
 * Creates the monstergroup with the given name at the given \a position in the current region. The monsters are placed randomly inside a circle with the specified \a radius.
 * 
 * \param monstergroup Name of a monstergroup
 * \param position position of the monstergroup
 * \param radius radius of the circle in which the monsters are spawned
 * 
 * \return Returns a list of created monsters
 */
list<int> createMonsterGroup( string monstergroup, Vector position, float radius=3);

/**
 * Deletes the object with the given ID.
 * 
 * \param objectid ID of an object
 */
void deleteObject( int objectid);

/**
 * Adds a command to the units command queue. The enqueue commands will be executed in the order of insertion. Commands created with this function have a higher priority than AI generated commands. No AI command will be executed as long as the unit has scripted commands.
 * \param unitid ID of a unit
 * \param command name of the command. All actions that can be performed by the unit are valid command names. 
 * \param target_id ID of the target of the command
 * \param range defines the distance to the target that should be reached. This is normaly computed internally. If the range value is 0, the computed value is used. The main purpose of this parameter is to set how close units should walk to a certain position.
 * \param flags List of flag strings separated "|". Accepted flags are:
 *    - \b "repeat" The command will be renewed after completion. Note that this completely blocks the execution of all other commands.
 *    - \b "secondary" Reduces the priority of the command to be below AI generated commands. This command will be executed only if the AI does not generate any commands at the moment.
 * \param time upper time limit for the completion of the command in milliseconds. If this time limit is exceeded, the command is canceled.
 * 
 * \sa clearUnitCommands
 */
void addUnitCommand(int unitid, string command,  int target_id , float range=0, string flags="", float time=50000);

/**
 * Clears a units command queue. Especially, this also deletes commands created with the repeat flag
 * 
 * \param unitid ID of a unit
 */
void clearUnitCommands(int unitid);

/**
 * Sets the Animation that is shown by an object. This animation is shown only if the object does not do any action at the moment, that requires a different animation.
 * 
 * \param objectid ID of an object
 * \param animation Animation that should be shown by the object
 * \param time Length of the animation in millisecond
 * \param repeat If set to true, the animation will loop
 */
void setObjectAnimation(int objectid, string animation, float time, bool repeat=false);

/**
 * Sets the flag with name \a flagname of the object to the given \a value. 
 * The flagname is an arbitrary string. These flags influence visual appearance only.
 * 
 * \param objectid ID of an object
 * \param flagname Name of the Flag to change
 * \param value new value
 * 
 * \sa getObjectFlag
 */
void setObjectFlag(int objectid, string flagname ,bool value=true);

/**
 * Returns the value of the flag with name \a flagname of the object.
 * 
 * \param objectid ID of an object
 * \param flagname Name of the Flag to query
 * \return true if the flag is set, false otherwise
 */
bool getObjectFlag(int objectid, string flagname);

/**
 * Returns the object, that is located at the given \a position.
 * 
 * \param position Position to check
 * \return Returns the ID of the object at the given \a position. Returns 0 if no objects was found.
 */
int getObjectAt(Vector position);

/**
 * Returns all Objects that are inside the given \a area. The objects may be filtered by specifying a group or a layer.
 * 
 * \param area Area to search
 * \param objectgroup applies a filter to the list. Valid values are:
 *   - \b "all" Do not filter
 *   - \b "unit" Return units only
 *   - \b "monster" Return monsters only
 *   - \b "player" Return players only
 *   - \b "fixed" Return fixed objects only
 * \param layer Only objects that collide with the given layer will be returns
 * \return List of all matching objects found
 */
list<int> getObjectsInArea(Area area, string objectgroup="all", string layer="normal");

/**
 * Teleports a player into a new region. If the player is already in the target region, he will be moved to the target location. Note that the target position must be given by a location name in this case. \n
 * The teleport will complete immediately after the call of this function. Instead, the enter_region should be used to detect when the teleport is complete.
 * 
 * \param playerid ID of a player
 * \param regionname Name of the target region
 * \param locationname Name of the target location
 */
void teleportPlayer(int playerid, string regionname, string locationname);

/**
 * Returns the whether two fraction are hostile, neutral oder allied to each other.
 * 
 * \param fraction1 name of the first fraction 
 * \param fraction2 name of the second fraction
 * \return "hostile", "neutral" or "allied"
 */
string getRelation(string fraction1, string fraction2);

/**
 * Returns the relation of an object and a fraction.
 * 
 * \param objectid ID of an object
 * \param fraction name of a fraction
 * \return "hostile", "neutral" or "allied"
 * 
 */
string getRelation(int objectid, string fraction);

/**
 * Return the relation of two objects.
 * 
 * \param objectid1 ID of the first object
 * \param objectid2 ID of the second object
 * \return "hostile", "neutral" or "allied"
 */ 
string getRelation(int objectid1, int objectid2);

/**
 * Modifies the relation of two fractions.
 * 
 * \param fraction1 name of the first fraction 
 * \param fraction2 name of the second fraction
 * \param relation new relation, valid values are "hostile", "neutral" or "allied"
 */
void setRelation(string fraction1, string fraction2, string relation);


