//   This file is part of the guslib library, licensed under the terms of the MIT License.
//
//   The MIT License
//   Copyright (C) 2010-2011  Augustin Preda (thegusty999@gmail.com)
//
//   Permission is hereby granted, free of charge, to any person obtaining a copy
//   of this software and associated documentation files (the "Software"), to deal
//   in the Software without restriction, including without limitation the rights
//   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//   copies of the Software, and to permit persons to whom the Software is
//   furnished to do so, subject to the following conditions:
//   
//   The above copyright notice and this permission notice shall be included in
//   all copies or substantial portions of the Software.
//   
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//   THE SOFTWARE.
//
//   Timer utility.
//
//	 Last change:	$LastChangedDate: 2013-06-20 22:36:33 +0200 (J, 20 iun. 2013) $
//	 Revision:		$Revision: 573 $


#include <gussoundutil/timer.h>

#if GUSLIB_FLAG_MULTITHREAD
#pragma message("Timer: Multithreaded synchronization enabled!")
#else
#pragma message("Timer: Multithreaded synchronization disabled!")
#endif


#include <sstream>

#ifdef _WINDOWS
#include <windows.h>
#include <mmsystem.h>	//	(Requires winmm.lib added to the link options.)
#pragma comment(lib,"winmm.lib")

#else
#include <time.h>

#endif //_WINDOWS

namespace guslib
{
	AbstractTimer::AbstractTimer()
		: latestTime(0)
	{
	}


	AbstractTimer::~AbstractTimer()
	{
	}


	void AbstractTimer::reset()
	{
		// call to abstract function. this must not be called in the abstract class' constructor.
		latestTime = getCurrentTimeUnits();
	}


	TimeUnits AbstractTimer::elapsed()
	{
		TimeUnits curr = getCurrentTimeUnits();
		TimeUnits tu = curr - latestTime;
		return tu;
	}

	TimeUnits AbstractTimer::renew()
	{
		TimeUnits curr = getCurrentTimeUnits();
		TimeUnits tu = curr - latestTime;
		latestTime = curr;
		return tu;
	}


	// --------------------- the Ogre timer --------------------------
	GusLibOgreTimer::GusLibOgreTimer ()
	{
		reset ();
	}

	GusLibOgreTimer::~GusLibOgreTimer ()
	{
	}


	TimeUnits GusLibOgreTimer::getCurrentTimeUnits ()
	{
		unsigned long currentMillis = timer_.getMilliseconds ();
		return static_cast<TimeUnits> (currentMillis);
	}

	TimeUnits GusLibOgreTimer::getTimeSinceMidnight () const
	{
		TimeUnits returnValue (0);

#ifdef _WINDOWS
		SYSTEMTIME localTime;
		GetLocalTime (&localTime);

		returnValue = localTime.wHour;
		returnValue = returnValue * 60 + localTime.wMinute;
		returnValue = returnValue * 60 + localTime.wSecond;
		returnValue = returnValue * 1000 + localTime.wMilliseconds;
#else
		struct tm *newtime;

		time_t long_time;

		time( &long_time );
		newtime = localtime( &long_time ); // Convert to local time.

		returnValue = newtime->tm_hour;
		returnValue = returnValue * 60 + newtime->tm_min;
		returnValue = returnValue * 60 + newtime->tm_sec;
		returnValue = returnValue * 1000 + 0; // no millis
#endif
		return returnValue;
	}

	void GusLibOgreTimer::reset ()
	{
		timer_.reset ();
	}

	// --------------------- the windows timer --------------------------

#ifdef _WINDOWS

	WinTimer::WinTimer()
	{
		reset();
	}


	WinTimer::~WinTimer()
	{
	}


	TimeUnits WinTimer::getCurrentTimeUnits()
	{
		return timeGetTime();
	}

	TimeUnits WinTimer::getTimeSinceMidnight () const
	{
		TimeUnits returnValue (0);

#ifdef _WINDOWS
		SYSTEMTIME localTime;
		GetLocalTime (&localTime);

		returnValue = localTime.wHour;
		returnValue = returnValue * 60 + localTime.wMinute;
		returnValue = returnValue * 60 + localTime.wSecond;
		returnValue = returnValue * 1000 + localTime.wMilliseconds;
#else
		struct tm *newtime;

		time_t long_time;

		time( &long_time );
		newtime = localtime( &long_time ); // Convert to local time.

		returnValue = newtime->tm_hour;
		returnValue = returnValue * 60 + newtime->tm_min;
		returnValue = returnValue * 60 + newtime->tm_sec;
		returnValue = returnValue * 1000 + 0; // no millis
#endif
		return returnValue;
	}

#endif


	// ------------------------- app clock ---------------------------

	ApplicationClockUtil::ApplicationClockUtil ()
		: Timer ()
	{
		initialTime_ = getCurrentTimeUnits();
	}

	ApplicationClockUtil::~ApplicationClockUtil ()
	{
	}

	std::string ApplicationClockUtil::getTimeAsString ()
	{
		TimeUnits temp (getTimeFromStart ());
		std::string returnValue;

		int nMillis = temp % 1000;
		temp = (temp - nMillis) / 1000;

		int nSec = temp % 60;
		temp = (temp - nSec) / 60;

		int nMin = temp % 60;
		temp = (temp - nMin) / 60;

		int nHour = temp % 24;
		temp = (temp - nHour) / 24;

		std::stringstream ss;
		ss << nHour << ":" << nMin << ":" << nSec << "." << nMillis;
		returnValue = ss.str ();

		return returnValue;
	}

	TimeUnits ApplicationClockUtil::getTimeFromStart () 
	{
		TimeUnits curr = getCurrentTimeUnits();
		TimeUnits tu = curr - initialTime_;
		return tu;
	}


	// ------------------------- sys clock ---------------------------

	SystemClockUtil::SystemClockUtil ()
	{
		initialTime_ = getTimeSinceMidnight ();

		TimeUnits timeSinceStartup = getCurrentTimeUnits ();

		// since we will always get the time based on the time since the startup, we must change this initial time.
		// so that it has an offset.
		initialTime_ = timeSinceStartup - initialTime_;
	}


	TimeUnits SystemClockUtil::getTimeFromStart () 
	{
		TimeUnits curr = getCurrentTimeUnits();
		TimeUnits tu = curr - initialTime_;
		return tu;
	}

}