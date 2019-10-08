/**
 * \file itemfunc.h
 * 
 * \brief Defines several function for creation, modification and deletion of items
 */

/**
 * Creates a new item with the requested \a itemtype. The item is stored in a current item variable and can be dropped into the region or inserted into a players inventory afterwards. \n
 * 
 * \param itemtype type of the item
 * \param magic_power If a magic power above the minimal enchantment of the item is given, enchantments are added. The item is marked as magical if not "rare" or "unique" are requested explicitely
 * \param rarity Valid values are "normal", "magical", "rare" and "unique". The item is given the property, even if there are no magical enchantments.
 * 
 * \sa dropItem insertPlayerItem
 */
void createItem(string itemtype, float magic_power=0,string rarity="normal");

/**
 * Creates a randomized item. The item is stored in a current item variable and can be dropped into the region or inserted into a players inventory afterwards. \n
 * 
 * \param size_prob Table of the form {float prob_big, float prob_medium, float pro_small, float prob_gold}. Determines the probability for the different item sizes. The sum of the table elements must be less or equal than 1. If the sum is less than one, there is a chance that no Item is created.
 * \param level Table of the {int min_level, int max_level}. The created Item has an item level in the given bounds.
 * \param gold_amount Table of the form {int min_gold, int max_gold}. If gold is created, the amount of gold is inside the interval.
 * \param magic_prob Probability to create a magical item
 * \param magic_power If the created item is magical, its magical enchantment power is chosen at random between 0.25*magic_power and magic_power
 * 
 * \sa dropItem insertPlayerItem
 */
void createRandomItem(table size_prob,table level,table gold_amount, float magic_prob=0, float magic_power=0);

/**
 * Drops the current Item into the current Region. The item must not be in a players inventory, in this case remove it from the players inventory first. A new created item can be dropped immediately.
 * 
 * \param position Position where the Item is dropped
 * \sa createItem createRandomItem removePlayerItem 
 */
void dropItem(Vector position);

/**
 * Returns a named property \a valname of the current item. The call is invalid if there is no current item. An exception is the \a valname \b "type" that will return "noitem" if there is no current item.\n
 * The type of the return value depends on the requested value.
 * 
 * Accepted strings for \a valname:
 * - \b "type" Returns the base type of the item. Return value is one of: "armor", "helmet", "gloves", "weapon", "shield", "potion", "ring", "amulet", "gold", "noitem" \n Return type: string
 * - \b "subtype" Returns a more precise type of the item.
 *   \n Return type: string
 * - \b "size" Returns one of: "big", "medium", "small", "gold"
 *   \n Return type: string
 * - \b "value" Return type: int
 * - \b "magic_power" Return type: int
 * - \b "char_requirement" Returns a comma separated list of player classes that can use this item. Returns "all" if there is no restriction
 *   \n Return type: string
 * - \b "level_requirement" Return type: int
 * - \b "rarity" Returns one of: "normal", "magical", "rare", "unique"
 *   \n Return type: string
 * 
 * If the current item is a weapon these strings are valid additionally:
 * - \b "weapon_type" Return type: string
 * - \b "attack_range" Returns the attack range of the weapon. Note that this is only used for melee weapons.
 *   \n Return type: float
 * - \b "two_handed" Return type: bool
 * - \b "attack_speed" Returns the modification of the attack speed by the weapon. This is not the resulting attack speed when using this weapon! Also note, that attack speed is multiplied with 1000, so 2 attacks per second correspond to attackspeed 2000.
 *   \n Return type: int
 * - all \a valname strings that are valid for the getDamageValue function
 * 
 * If the current item is an item that can be equiped (including weapons) all \a valname strings that can be used with getBaseModValue are valid.
 * 
 * If the current item can be used up all \a valname strings that can be used with getDynModValue are valid.
 * 
 * \param valname name of an item property
 * 
 * \sa setItemValue getDamageValue getBaseModValue getDynModValue
 */
various getItemValue(string valname);

/**
 * Sets the named property \a valname of the current item to \a value. This call is always invalid if there is no current item. \n The required type for \a value is the same that is returned by the corresponding getObjectValue call. \n
 * All property names, that are valid for getItemValue are valid for setting as well. The required type for \a value is the same that is returned by the corresponding getItemValue call.
 * 
 * \param valname name of a property
 * \param value new value of the property
 * 
 * \sa getItemValue
 */
void setItemValue(string valname, various value);

/**
 * Deletes the current item. The item must not be in a players inventory, in this case remove it from the players inventory first.
 */
void deleteItem();

/**
 * Returns an integers that can be mapped to the players inventory slots. This integer is used to specify the item position.
 * 
 * \param playerid ID of the player
 * \param position Must be one of these: "armor", "helmet", "gloves", "shield", "weapon", "ring_left", "ring_right", "amulet", "big_items", "medium_items", "small_items"
 * 
 * \sa searchPlayerItem removePlayerItem  getPlayerItem
 */
int getInventoryPosition(int playerid, string position);

/**
 * Searches a players inventory for an item with the requested \a subtype and returns the item position. The item position is an integer that is mapped to the players inventory slots. \n
 * The function does not overwrite the current item. This can be done with a subsequent getPlayerItem call.\n
 * 
 * \param playerid ID of the player
 * \param subtype subtype of the item to search for
 * \param startpos starting position of the search. A value of 0 results in a complete search of the inventory. After a successful search the return value +1 can be used to search the rest of the inventory. \n Instead of an integer a string can be given as well. It is converted using the getInventoryPosition function.
 * 
 * \sa getInventoryPosition getPlayerItem
 */
int searchPlayerItem(int playerid, string subtype, int startpos=0);

/**
 * Searches a players inventory for an item with the requested \a subtype and sets it as current item. If no item was found the current item is set to none. \n
 * The item is not removed from the players inventory and can not deleted or dropped. 
 * 
 * \param playerid ID of the player
 * \param subtype of the item to search for
 * 
 * \sa removePlayerItem
 */
void getPlayerItem(int playerid, string subtype);

/**
 * Sets the item at the given position as current item. If there is no item at this position, the current item is set to none. \n
 * The item is not removed from the players inventory and can not deleted or dropped. 
 * 
 * \param playerid ID of the player
 * \param itempos position of the item. \n Instead of an integer a string can be given as well. It is converted using the getInventoryPosition function.
 * 
 * \sa getInventoryPosition removePlayerItem
 */
void getPlayerItem(int playerid, int itempos);

/**
 * Searches a players inventory for an item with the requested \a subtype sets it as current item and removes it from the players inventory. The item is not deleted and can be dropped or reinserted into a players inventory. If no item was found the current item is set to none.
 * 
 * \param playerid ID of the player
 * \param subtype of the item to search for
 * 
 * \sa deleteItem
 */
void removePlayerItem(int playerid, string subtype);

/**
 * Sets the item at the given position as current item and removes it from the players inventory. The item is not deleted and can be dropped or reinserted into a players inventory. If there is no item at this position, the current item is set to none.
 * 
 * \param playerid ID of the player
 * \param itempos position of the item. \n Instead of an integer a string can be given as well. It is converted using the getInventoryPosition function.
 * 
 * \sa getInventoryPosition deleteItem
 */
void removePlayerItem(int playerid, int itempos);

/**
 * Inserts the current item into a players inventory. The item must not be in a players inventory or already dropped. \n
 * If the players inventory is full, the item will drop at the players position.
 * 
 * \param playerid ID of the player
 * \param equip if set to true, the item can be equiped
 * 
 * \sa createItem createRandomItem
 */
void insertPlayerItem(int playerid, bool equip= true);

/**
 * Returns the value of the named property of the current damage object. The current damage object depends on the context of the script. 
 * 
 * Accepted strings for \a valname are listed below. The type of the return value depends on the requested value:
 * - \b phys_dmg Returns the range of physical damage as a {min,max} table
 *   \n Return type: Vector
 * - \b fire_dmg Returns the range of fire damage as a {min,max} table
 *   \n Return type: Vector
 * - \b ice_dmg Returns the range of ice damage as a {min,max} table
 *   \n Return type: Vector
 * - \b air_dmg Returns the range of air damage as a {min,max} table
 *   \n Return type: Vector
 * - \b phys_mult Returns the multiplier that is applied to the physical damage
 *   \n Return type: float
 * - \b fire_mult Returns the multiplier that is applied to the fire damage
 *   \n Return type: float
 * - \b ice_mult Returns the multiplier that is applied to the ice damage
 *   \n Return type: float
 * - \b air_mult Returns the multiplier that is applied to the air damage
 *   \n Return type: float
 * - \b attack Returns the attack value of the damage that influences blocking chance. Note that only attacks with nonzero physical damage may be blocked. Also be aware that damage with an attack value of zero will always be blocked.
 *   \n Return type: float
 * - \b power Returns the power value of the damage, that influences physical damage reduction by armor. Note that a power value of zero will result in zero physical damage-
 *   \n Return type: float
 * - \b crit_chance Returns the probability of a critical hit as a number between 0 and 1
 *   \n Return type: float
 * - \b blockable Returns false, if the unblockable flag is set, true otherwise. If the unblockable flag is set, the damage will never be blocked regardless of the attack value of the attacker and the block value of the defender.
 *   \n Return type: bool
 * - \b ignore_armor Returns if the ignore_armor flag is set. If this flag is set, the attack will always deal the full physical damage ignoring the defenders armor.
 *   \n Return type: bool
 * - \b attacker Return the ID of the attack that deals the damage. If there is no specific attacker, the ID is zero. The damage is only inflicted if the attacker and the defender are hostile to each other.
 *   \n Return type: int
 * - \b visualize Returns, if the inflicted damage will be visualized by a number.
 *   \n Return type: bool
 * - \b visualize_small Returns true, if the damage number is printed with the normal (small) font. Big numbers are usually used for special attacks.
 *   \n Return type: bool
 * - \b extra_dmg_race Returns a race that will receive triple damage. This is used to create attacks that are especially efficient against undead for instance. If there is no such race an empty string is returned.
 *   \n Return type: string
 * - \b blind Returns the strenght of the blind status modification on the attack. The strength influences the probability, that the victim will receive the status mod as well as the duration.
 *   \n Return type: int
 * - \b mute Returns the strenght of the mute status modification on the attack. The strength influences the probability, that the victim will receive the status mod as well as the duration.
 *   \n Return type: int
 * - \b paralyzed Returns the strenght of the paralyzed status modification on the attack. The strength influences the probability, that the victim will receive the status mod as well as the duration.
 *   \n Return type: int
 * - \b berserk Returns the strenght of the berserk status modification on the attack. The strength influences the probability, that the victim will receive the status mod as well as the duration.
 *   \n Return type: int
 * - \b frozen Returns the strenght of the frozen status modification on the attack. The strength influences the probability, that the victim will receive the status mod as well as the duration.
 *   \n Return type: int
 * - \b poisoned Returns the strenght of the poisoned status modification on the attack. The strength influences the probability, that the victim will receive the status mod as well as the duration.
 *   \n Return type: int
 * - \b confused Returns the strenght of the confused status modification on the attack. The strength influences the probability, that the victim will receive the status mod as well as the duration.
 *   \n Return type: int
 * - \b burning Returns the strenght of the burning status modification on the attack. The strength influences the probability, that the victim will receive the status mod as well as the duration.
 *   \n Return type: int
 */
various getDamageValue(string valname);


void setDamageValue(string valname, various value);

