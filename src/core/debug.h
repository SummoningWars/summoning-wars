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

#ifndef __SUMWARS_CORE_DEBUG_H__
#define __SUMWARS_CORE_DEBUG_H__


#include <stdio.h>
#include <string.h>
#include "timer.h"
#include "logger.h"
/**
 * \brief Gibt Informationen aus
 * \def LOGGER( level,format, ... )
 * \param level Ausgabelevel
 * \param format Format der Ausgabe
 * \param ... Parameter fuer die in format angegebenen Daten
 *
 * Fuer eine naehere Erklaerung der Parameter sollte die Dokumentation der Funktion printf
 * herangezogen werden.
 */
#define LOGGER( loglevel,format, ... ) \
	do { \
		char _format_[500] = "%s:%i %s > "; char _message_[500];  \
		strcat(_format_,format); \
		sprintf( _message_, _format_,__FILE__,__LINE__,__FUNCTION__ ,##__VA_ARGS__); \
		LogManager::instance().writeMessage(_message_,loglevel); \
	} while (false); \

/**
 * \def ERRORMSG( format, ... )
 * \brief Gibt Fehlermeldungen aus
 */
#define ERRORMSG( format, ... ) \
	LOGGER( Log::MSG_ERROR,format, ##__VA_ARGS__ )

/**
 * \def WARNING( format, ... )
 * \brief Gibt Warnungne aus
 */
#define WARNING( format, ... ) \
	LOGGER(Log::MSG_WARNING,format, ##__VA_ARGS__ )

/**
 * \def INFO( format, ... )
 * \brief Gibt unwichtige Informationen aus
 */
#define INFO( format, ... ) \
	LOGGER( Log::MSG_INFO,format, ##__VA_ARGS__ )

/**
 * \def DEBUG( format, ... )
 * \brief Gibt Debug-Informationen aus
 */
#define SW_DEBUG( format, ... ) \
	LOGGER( Log::MSG_DEBUG,format, ##__VA_ARGS__ )


#define DEBUGX( format, ... ) ;

extern "C"
{
	/**
	 * \fn void hexwrite(char* data,int len)
	 * \brief Schreibt String hexadezimal auf cout
	 * \param data String
	 * \param len Laenge des Strings
	 */
	void hexwrite(char* data,int len);


}


#endif // __SUMWARS_CORE_DEBUG_H__



