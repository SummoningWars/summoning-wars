#ifndef DEBUG_H
#define DEBUG_H

/*
	Ein kleines Rollenspiel
	Copyright (C) 2007 Alexander Boehm

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/**
 * \file debug.h
 * \defgroup Debuging \ Debugfunktion
 * \author Alexander Boehm
 * \version 1.0
 * \date 2007-05-31
 * \brief Verwaltung von Debug-Informationen und Ausgaben
 */

#include <stdio.h>
#include "timer.h"

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
#define LOGGER( level,format, ... ) \
	if( level <= DEBUG_LEVEL ) { \
		printf( "%s:%i %s > ",__FILE__,__LINE__,__FUNCTION__ ); \
		printf( format,##__VA_ARGS__ ); \
		printf( "\n" ); \
	}

/**
 * \def ERRORMSG( format, ... )
 * \brief Gibt Fehlermeldungen aus
 */
#define ERRORMSG( format, ... ) \
	LOGGER( 0,format, ##__VA_ARGS__ )

/**
 * \def WARNING( format, ... )
 * \brief Gibt Warnungne aus
 */
#define WARNING( format, ... ) \
	LOGGER( 1,format, ##__VA_ARGS__ )

/**
 * \def INFO( format, ... )
 * \brief Gibt unwichtige Informationen aus
 */
#define INFO( format, ... ) \
	LOGGER( 2,format, ##__VA_ARGS__ )

/**
 * \def DEBUG( format, ... )
 * \brief Gibt Debug-Informationen aus
 */
#define DEBUG( format, ... ) \
	LOGGER( 3,format, ##__VA_ARGS__ )

/**
 * \def DEBUG1( format, ... )
 * \brief Gibt Debug-Informationen aus
 */
#define DEBUG1( format, ... ) \
	LOGGER( 4,format, ##__VA_ARGS__ )

/**
 * \def DEBUG2( format, ... )
 * \brief Gibt Debug-Informationen aus
 */
#define DEBUG2( format, ... ) \
	LOGGER( 5,format, ##__VA_ARGS__ )

/**
 * \def DEBUG3( format, ... )
 * \brief Gibt Debug-Informationen aus
 */
#define DEBUG3( format, ... ) \
	LOGGER( 6,format, ##__VA_ARGS__ )

/**
 * \def DEBUG4( format, ... )
 * \brief Gibt Debug-Informationen aus
 */
#define DEBUG4( format, ... ) \
	LOGGER( 7,format, ##__VA_ARGS__ )

/**
 * \def DEBUG5( format, ... )
 * \brief Gibt Debug-Informationen aus
 */
#define DEBUG5( format, ... ) \
	LOGGER( 8,format, ##__VA_ARGS__ )

/**
 * \def DBG( format, ... )
 * \brief Abkuerzung fuer DEBUG
 */
#define DBG( format, ... ) \
	DEBUG( format, ##__VA_ARGS )


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


#endif



