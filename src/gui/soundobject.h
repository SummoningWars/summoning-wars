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

#ifndef __SUMWARS_GUI_SOUNDOBJECT_H__
#define __SUMWARS_GUI_SOUNDOBJECT_H__

//
// Includes
//

// use std::string
#include <string>

// Geometry utility functions built into sumwars.
#include "geometry.h"


///
/// Class used to map 3D objects in the game to sounds that need to be played.
/// This means in effect storing the sources of the sounds. Keep in mind though that these sources are taken from a pool of available
/// sources in the lower level libraries. As a result, they will not be available for play all at the same time, running into
/// platform specific limitations.
/// The sounds are played only as needed, according to the location settings provided in the sound object.
///
class SoundObject
{
public:
	/**
	 * @fn SoundObject (const std::string& name, Vector pos = Vector (0, 0))
	 * @brief Constructor.
	 * @param name Name used to identify this sound object. // TODO: check if needed!!!
	 * @param pos Position of the sound.
	 */
	SoundObject (const std::string& name, Vector pos = Vector (0, 0));

	// Default constructor
	SoundObject ();

	/**
	 * @fn ~SoundObject()
	 * @brief Destructor. Nothing special.
	 */
	~SoundObject();


	/**
	 * @fn void setPosition(Vector pos)
	 * @brief Set the position (source) of the sound
	 * @param pos new position to use.
	 */
	void setPosition (Vector pos);
	
	/**
	 * @brief Sets the position relative to the listener
	 * @param pos relative Position
	 */
	//void setPositionRelative (Vector pos);


	/**
	 * \fn Vector getPosition()
	 * \brief Gibt die Position der Schallquelle aus
	 */
	Vector getPosition();
		
	/**
	 * \fn void setSoundData(SoundName sname)
	 * \brief Setzt den abgespielten Sound
	 * \param sname Name des Sounds
	 */
	void setSoundData (const std::string& data);

	/**
	 * \fn std::string getName()
	 * \brief gibt den Name aus
	 */
	std::string getName()
	{
		return m_name;
	}

	/**
	 * \fn void play()
	 * \brief Startet das Abspielen des eingestellten Sounds
	 */
	void play();

protected:
	std::string m_name;

	std::string m_soundData;

	Vector m_pos;
};


#endif // __SUMWARS_GUI_SOUNDOBJECT_H__

