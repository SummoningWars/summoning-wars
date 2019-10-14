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

#ifndef __SUMWARS_CORE_PARTY_H__
#define __SUMWARS_CORE_PARTY_H__

#include "debug.h"
#include "charconv.h"
#include "worldobject.h"
#include "minimap.h"
#include <set>
#include <map>

/**
 * \class Party
 * \brief Eine Party ist eine Gruppe von Lebewesen welche kooperieren
 */
class Party
{

	public:



	/**
	 * \fn Party()
	 * \brief Konstruktor
	 */
	Party()
	:	m_id(0),
		m_leader_id(0)
	{
		
	};


	/**
	 * \fn ~Party()
	 * \brief Destruktor
	 */
	virtual ~Party();

	/**
	 * \fn init( int id)
	 * \brief initialisiert das Objekt
	 * \param id ID
	 */
	void init( int id);


	/**
	 * \fn void clear()
	 * \brief Loescht alle aktuellen Mitglieder und Bewerber
	 */
	void clear();

	/**
	 * \fn void updateMinimaps()
	 * \brief aktualisiert die Minimaps
	 */
	void updateMinimaps();

	/**
	 * \fn std::set<int>& getMembers()
	 * \brief gibt Liste der Mitglieder zurueck
	 */
	std::set<int>& getMembers()
	{
		return m_members;
	}

	/**
	 * \fn std::set<int>&  getCandidates()
	 * \brief Gibt Liste der Kandidaten zurueck
	 */
	std::set<int>& getCandidates()
	{
		return m_candidates;
	}

	/**
	 * \fn std::map<int, Fraction::Relation>& getRelations()
	 * \brief Gibt Beziehungen zu den anderen Parties aus
	 */
	std::map<int, Fraction::Relation>& getRelations()
	{
		return m_relations;
	}
	
	/**
	 * \fn void setRelation(int id, Fraction::Relation rel)
	 * \brief Setzt die Beziehung zwischen zwei Parties
	 * \param id Id der Party
	 * \param rel Beziehung
	 */
	void setRelation(int id, Fraction::Relation rel);

	/**
	 * \fn int getId()
	 * \brief Gibt die ID aus
	 * \return ID
	 */
	int getId()
	{
		return m_id;
	}

	/**
	 * \fn int getNrMembers()
	 * \brief Gibt Anzahl der Mitglieder aus
	 */
	int getNrMembers()
	{
		return m_members.size();
	}

	/**
	 * \fn int getNrCandidates()
	 * \brief Gibt Anzahl der Bewerber aus
	 */
	int getNrCandidates()
	{
		return m_candidates.size();
	}
	
	/**
	 * \fn int getLeader()
	 * \brief Gibt die ID des Leaders aus
	 */
	int getLeader()
	{
		return m_leader_id;
	}
	
	/**
	 * \fn Minimap* getMinimap(short rid)
	 * \brief Gibt die Minimap zur angegebenen Region aus
	 * \param rid ID der Region
	 */
	Minimap* getMinimap(short rid);

	/**
	 * \fn void addMember(int id)
	 * \brief Fuegt Mitglied hinzu
	 * \param id ID des aufgenommenen Mitglieds
	 */
	void addMember(int id);

	/**
	 * \fn void removeMember(int id)
	 * \brief Entfernt Mitglied
	 * \param id ID des entfernten Mitglieds
	 */
	void removeMember(int id);

	/**
	 * \fn void addCandidate(int id)
	 * \brief Fuegt einen Bewerber hinzu
	 * \param id ID des neuen Bewerbers
	 */
	void addCandidate(int id);

	/**
	 * \fn void acceptCandidate(int id)
	 * \brief Nimmt Bewerber auf
	 * \param id ID des aufgenommenen Bewerbers
	 */
	void acceptCandidate(int id);

	/**
	 * \fn void removeCandidate(int id)
	 * \brief Entfernt einen Bewerber
	 * \param id ID des Bewerbers
	 */
	void removeCandidate(int id);

	
	/**
	 * \fn void toString(CharConv* cv)
	 * \brief Konvertiert das Objekt in einen String und schreibt ihn in der Puffer
	 * \param cv Ausgabepuffer
	 */
	virtual void toString(CharConv* cv);


	/**
	 * \fn void fromString(CharConv* cv)
	 * \brief Erzeugt das Objekt aus einem String
	 * \param cv Eingabepuffer
	 */
	virtual void fromString(CharConv* cv);

	/**
	 * \fn std::map<short,Minimap*>& getMinimaps()
	 * \brief Gibt alle Minimaps aus
	 */
	std::map<short,Minimap*>& getMinimaps()
	{
		return m_minimaps;
	}

	private:

	/**
	 * \var int m_id
	 * \brief ID
	 */
	int m_id;
	
	/**
	 * \var int m_leader_id
	 * \brief ID des Party Leiters
	 */
	int m_leader_id;

	/**
	 * \var std::set<int> m_members
	 * \brief Menge der IDs der Mitglieder
	 */
	std::set<int> m_members;

	/**
	 * \var std::set<int> m_candidates
	 * \brief Menge der IDs der Bewerber
	 */
	std::set<int> m_candidates;

	/**
	 * \var std::map<int, Fraction::Relation> m_relations
	 * \brief Beziehungen zu anderen Parties
	 */
	std::map<int, Fraction::Relation> m_relations;
	
	/**
	 * \var std::map<short,Minimap*> m_minimaps
	 * \brief Enthaelt die Minimaps fuer die von der Party erkundeten Regionen
	 */
	std::map<short,Minimap*> m_minimaps;

};


#endif // __SUMWARS_CORE_PARTY_H__

