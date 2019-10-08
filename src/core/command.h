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

#ifndef COMMAND_H
#define COMMAND_H



#include <string>
#include "action.h"


/**
 * \class Command
 * \brief Beschreibt ein Kommando, welches eine Figur erhalten kann. Zur Ausf&uuml;hrung des Kommandos wird die Anweisung in mehrere Aktionen unterteilt, die hintereinander ausgefuehrt werden.
 */
class Command {
/**
 * Public stuff
 */
public:
	enum Flags
	{
		REPEAT = 0x1,
 		SECONDARY = 0x2,
	};
	/**
	 * Fields
	 */
	/**
	 * Constructors
	 */
	/**
	 * \fn Command(Action::ActionType type, Vector goal, int goal_object_id)
	 * \brief Konstruktor
	 * \param type initialisiert den Typ des Kommandos
	 * \param goal Zielpunkt der Aktion
	 * \param goal_object_id initialisiert ID des Zielobjekts
	 * Legt ein neues Command Objekt an
	 */
	Command(Action::ActionType type, Vector goal, int goal_object_id)
	{
		m_type = type;
		m_goal = goal;
		m_goal_object_id= goal_object_id;
		m_damage_mult=1;
		m_range=1;
		m_flags =0;
	}

	/**
	 * \fn Command()
	 * \brief Konstruktor
	 */
	Command()
	{
		m_type = "noaction";
		m_goal = Vector(0,0);
		m_goal_object_id= 0;
		m_damage_mult=1;
		m_range=1;
		m_flags =0;
	}

	/**
	 * \fn void toString(CharConv* cv)
	 * \brief Konvertiert das Objekt in einen String und schreibt ihn in der Puffer
	 * \param cv Eingabepuffer
	 */
	void toString(CharConv* cv);

	/**
	 * \fn void fromString(CharConv* cv)
	 * \brief Erzeugt das Objekt aus einem String
	 * \param cv Ausgabepuffer
	 */
	void fromString(CharConv* cv);

	/**
	 * \fn bool operator!=(Command& other)
	 * \brief Vergleichsoperator auf Ungleichheit
	 */
	bool operator!=(Command& other);

	/**
	 * \fn void operator=(Command& other)
	 * \brief Zuweisungsoperator
	 */
	void operator=(Command& other);

	/**
	 * Fields
	 */
	/**
	 * \var m_type
	 * \brief Typ des Kommandos
	 */
	 Action::ActionType m_type;

	/**
	 * \var Vector m_goal
	 * \brief Ziel des Kommandos
	 */
	Vector m_goal;

	/**
	 * \var m_goal_object_id
	 * \brief ID des Zielobjekts
	 */
	int  m_goal_object_id;

	/**
	 * \var float m_range
	 * \brief Reichweite der Aktion
	 */
	float m_range;

	/**
	 * \var m_damage_mult;
	 * \brief Multiplikator auf den Schaden der die Aktion abschließt
	 */
	float m_damage_mult;

	/**
	 * \var char m_flags
	 * \brief bitkodierte Flags
	 */
	char m_flags;
	/**
	 * Constructors
	 */
	/**
	 * Accessor Methods
	 */
	/**
	 * Operations
	 */
};
#endif //COMMAND_H
