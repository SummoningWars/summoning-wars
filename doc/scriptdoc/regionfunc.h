/**
 * \file regionfunc.h
 * \brief Defines functions for querying and modifying region properties.
 */

/**
 * When used as a function parameter a Vector can be given in two different ways:
 * - a string that is a locationname
 * - a table of the form {float x,float y}
 * 
 * A function that returns a Vector will always return a table.
 */
class Vector {};

/**
 * When used as a function parameter an Area can be given in three different ways:
 * - a string that is an areaname
 * - a table of the form {"circle",{float center_x, float center_y}, float radius}
 * - a table of the form {"rect",{float center_x, float center_y}, {float extent_x, float extent_y}}
 * 
 * A function that returns an Area will always return a table.
 */
class Area {};

/**
 * 
 * Each event is located in a region, which is called the current region for that script. All modifications that are made to a region by the script are done to the current region implicitely. This function returns the name of this region.
 * 
 * \returns name of the current region.
 */
string getRegion();

/**
 * Checks if the \a point is inside the \a area.
 * 
 * \param point point
 * \param area area to check
 * \return Returns true, if the point is inside the area, false otherwise.
 */
bool pointIsInArea( Vector point, Area area);

/**
 * Creates a new location. After calling this function, the \a locationname can be used as argument to functions that require a vector and will be replaced by the position.
 * 
 * \param locname name of the location
 * \param position position of the location
 */
void addLocation(string locname, Vector position);

/**
 * Returns the position that corresponds to a locationname
 * 
 * \param locname name of the location
 * \return Position of the Location. Returns nil if the location does not exist.
 */
Vector getLocation(string locname);

/**
 * Creates a new area reference. After calling this function, the  \a areaname can be used as argument to functions that require an Area and will be replaced by the \a area.
 * \param areaname Name of the Area
 * \param area Values of the area
 */
void addArea(string areaname, Area area);

/**
 * Changes the lighting of the region. The lighting consists of three components: The ambient light, a cone light over the head of the hero and a directional light that corresponds to sun or moon. The light will change over the given time  or immediately if the time is zero.
 * \param lighttype Type of the light. Must be one of the strings "ambient","hero","directional"
 * \param value light value as {red,green,blue} table
 * \param time duration for the change between current lighting and given lighting
 */
void setLight(string lighttype, float value[3] ,time=0);
