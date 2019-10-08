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

#include "music.h"

#include <iostream>
#include "debug.h"

void MusicManager::play(std::string file)
{
	try
	{
		m_ogg_stream.open(file);
		if(!m_ogg_stream.playback())
		{
			ERRORMSG("Ogg refused to play file %s.",file.c_str());
			return;
		}
		
		m_musicfile = file;
		m_is_playing = true;
	}
	catch (std::string error)
	{
		ERRORMSG("Ogg errormessage: %s",error.c_str());
	}
}

void MusicManager::update()
{
	if (!m_is_playing)
		return;
	
	bool active = m_ogg_stream.update();
			
	if (active)
	{
		if(!m_ogg_stream.playing())
		{
			if(!m_ogg_stream.playback())
			{
				stop();
				ERRORMSG("Ogg abruptly stopped playing file %s",m_musicfile.c_str());
			}
			else
			{
				stop();
				ERRORMSG("Ogg stream playing file %s was interrupted",m_musicfile.c_str());
			}
		}
	}
	
	if (!active && m_musicfile != "")
	{
		stop();
	}
	
}

void MusicManager::stop()
{
	if (!m_is_playing)
		return;
	
	try
	{
		m_is_playing = false;
		m_ogg_stream.release();
	}
	catch (std::string msg)
	{
		
	}
}
