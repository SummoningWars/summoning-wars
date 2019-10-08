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

#ifndef TIMER_H
#define TIMER_H

//#include <sys/time.h>
#include "OgreTimer.h"

/**
 * \class Timer
 * \brief Klasse zum Zeit messen
 */
class Timer
{
	
	public:
		
	/**
	 * \fn void start()
	 * \brief Startet den Timer
	 */
	void start()
	{
		mTimer = Ogre::Timer();
	}
		
	/**
	 * \fn float getTime()
	 * \brief Gibt vergangene Zeit in ms aus
	 */
	float getTime()
	{
        return (float)mTimer.getMilliseconds();
	}
	
	private:
	/**
	 * \var timeval m_tv
	 * \brief Speichert die Zeit zum Zeitpunkt des letzten Aufrufes von Start
	 */
	Ogre::Timer mTimer;
};


#endif

