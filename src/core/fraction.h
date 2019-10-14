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

#ifndef __SUMWARS_CORE_FRACTION_H__
#define __SUMWARS_CORE_FRACTION_H__

#include "debug.h"
#include "charconv.h"
#include <string>
#include <map>

/**
 * \class Fraction
 * \brief Fraktion, der Spieler und Monster angehoeren koennen
 */
class Fraction
{
	public:
		/**
		 * \enum Relation
		 * \brief Listet die verschiedenen Beziehungen zwischen Fraktionen auf
		 */
		enum Relation
		{
			NEUTRAL = 0,
			ALLIED = 1,
			HOSTILE =2
		};
		
		/**
		 * \enum SpecialFraction
		 * \brief einige besondere Werte fuer Fraktionids
		 */
		enum SpecialFraction
		{
			DEFAULT = -2,
			NOFRACTION = -1,
			PLAYER =0,
			NEUTRAL_TO_ALL= 9998,
			HOSTILE_TO_ALL = 9999,
			MONSTER = 10000,
		};
		
		typedef std::string Type;

		typedef short Id;
		
		/**
		 * \fn Fraction(Id id, Type type)
		 * \brief Konstruktor
		 */
		Fraction(Id id, Type type)
		{
			m_id = id;
			m_type = type;
			m_relations[DEFAULT] = NEUTRAL;
		}
		
		
		/**
		 * \fn int getId()
		 * \brief Gibt die ID aus
		 */
		int getId()
		{
			return m_id;
		}
		
		/**
		 * \fn Type getType()
		 * \brief Gibt den Typ aus
		 */
		Type getType()
		{
			return m_type;
		}
		
		/**
		 * \fn Relation getRelation(Id id)
		 * \brief Gibt Verhaeltnis zur Fraktion mit der ID id aus
		 * \param id ID einer anderen Fraktion
		 */
		Relation getRelation(Id id);
		
		/**
		 * \fn void setRelation(Id id, Relation relation)
		 * \brief Setzt des Verhaeltnis zu einer Fraktion
		 * \param id ID der Fraktion
		 * \param relation Verhaeltnis
		 */
		void setRelation(Id id, Relation relation)
		{
			m_relations[id] = relation;
		}
	
		/**
		 * \fn void toString(CharConv* cv)
		 * \brief Konvertiert das Objekt in einen String und schreibt ihn in der Puffer
		 * \param cv Ausgabepuffer
		 */
		void toString(CharConv* cv);


		/**
		 * \fn virtual void fromString(CharConv* cv)
		 * \brief Erzeugt das Objekt aus einem String
		 * \param cv Eingabepuffer
		 */
		void fromString(CharConv* cv);
		
	private:
		/**
		 * \var Id m_id
		 * \brief Nummer der Fraktion
		 */
		Id m_id;
		
		/**
		 * \var Type m_type
		 * \brief Type der Fraktion
		 */
		Type m_type;
		
		/**
		 * \var std::string m_name
		 * \brief Name der Fraktion
		 */
		std::string m_name;
		
		/**
		 * \var std::map<Id, Relation> m_relations
		 * \brief Gibt Verhaeltnis zu anderen Fraktionen an, gespeichert wird nur Verhaeltnis zu Fraktionen mit hoeherer ID
		 */
		std::map<Id, Relation> m_relations;
};

#endif // __SUMWARS_CORE_FRACTION_H__
