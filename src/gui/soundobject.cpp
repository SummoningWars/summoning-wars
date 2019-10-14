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

//
// Includes
//

// Own header
#include "soundobject.h"

// Helper for sound operations
#include "soundhelper.h"


// Sound library
#include "gussound.h"


// Allow a shorter name for the sound manager.
using gussound::SoundManager;


SoundObject::SoundObject ()
{
}


SoundObject::SoundObject (const std::string& name, Vector pos)
	: m_name (name)
	, m_pos (pos)
	, m_soundData ("")
{
}


SoundObject::~SoundObject ()
{
}


void SoundObject::setPosition (Vector pos)
{
	m_pos = pos;
}


Vector SoundObject::getPosition ()
{
	return m_pos;
}

void SoundObject::setSoundData (const std::string& data)
{
	SW_DEBUG ("Setting sound data for [%s] to : [%s]", m_name.c_str (), data.c_str ());
	m_soundData = data;
}

void SoundObject::play ()
{
	SoundHelper::playSoundGroupAtPosition (m_soundData, m_pos.m_x, m_pos.m_y, 0);
}
