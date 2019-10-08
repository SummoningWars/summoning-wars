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

#include "party.h"
#include "world.h"
#include "player.h"

void Party::init(int id)
{
	
	m_leader_id =0;
	m_id = id;
	m_relations[id] = Fraction::ALLIED;
}


Party::~Party()
{
	clear();
}

void Party::clear()
{
	m_members.clear();
	m_candidates.clear();
	m_relations.clear();
	
	m_leader_id =0;
	
	std::map<short,Minimap*>::iterator it;
	for (it = m_minimaps.begin(); it != m_minimaps.end(); ++it)
	{
		delete it->second;
	}
	m_minimaps.clear();

}

void Party::updateMinimaps()
{
	WorldObject* wo;
	Minimap* mm;
	std::set<int>::iterator it;

	int rid;
	
	for (it = m_members.begin(); it != m_members.end(); ++it)
	{
		wo = World::getWorld()->getPlayer(*it);
		// nur aktive Spieler
		if (wo !=0 && wo->getRegion()!=0 && wo->getState() == WorldObject::STATE_ACTIVE )
		{
			rid = wo->getRegion()->getId();
			mm = getMinimap(rid);
			
			mm->update(wo->getShape()->m_center);
		}
	}
}

Minimap* Party::getMinimap(short rid)
{
	DEBUGX("get Minimap for %i",rid);
	std::map<short,Minimap*>::iterator mt;
	
	mt = m_minimaps.find(rid);
			
	if (mt == m_minimaps.end())
	{
		// Minimap noch nicht vorhanden -> erzeugen
		mt = m_minimaps.insert(std::make_pair(rid,World::getWorld()->getRegion(rid)->createMinimap())).first;
	}
	return mt->second;
}

void Party::addMember(int id)
{
	Player* player = static_cast<Player*>(World::getWorld()->getPlayer(id));
	if (player ==0)
		return;
	
	DEBUGX("adding member %i to party %i",id, m_id);
	if (m_members.empty())
	{
		m_leader_id = id;
	}
	m_members.insert(id);
	
	Party* p = player->getParty();
	if (p!=0 && p!=this)
	{
		while (!p->getCandidates().empty())
		{
			p->removeCandidate(*(p->getCandidates().begin()));
		}
		
		// Sichtbereich des neuen Spielers mit dem der Party vereinigen 
		std::map<short,Minimap*>& othermmaps = p->getMinimaps();
		std::map<short,Minimap*>::iterator mt;
		Minimap* mmap;
		for (mt = othermmaps.begin(); mt != othermmaps.end(); ++mt)
		{
			
			mmap = getMinimap(mt->first);
			mmap->merge(*(mt->second));
		}
	}
	
	
	
	
	player->setFraction(getId());
	
	if (World::getWorld()->isServer())
	{
		
		NetEvent event;
		event.m_id = id;
		event.m_type = NetEvent::PLAYER_PARTY_CHANGED;
		
		World::getWorld()->insertNetEvent(event);
	}
	
	
}

void Party::addCandidate(int id)
{
	DEBUGX("adding candidate %i to party %i",id, m_id);
	Player* pl = static_cast<Player*>( World::getWorld()->getPlayer(id));
	if (pl !=0)
	{
		pl->setCandidateParty(getId());
	}
	m_candidates.insert(id);
	
	if (World::getWorld()->isServer())
	{
		
		NetEvent event;
		event.m_id = id;
		event.m_type = NetEvent::PLAYER_PARTY_CANDIDATE;
		
		World::getWorld()->insertNetEvent(event);
	}
}

void Party::removeMember(int id)
{
	DEBUGX("removing member %i from party %i",id, m_id);

	m_members.erase(id);
	DEBUGX("number of members %i",m_members.size());
	if (id == m_leader_id)
	{
		m_leader_id = *(m_members.begin());
	}
}

void Party::removeCandidate(int id)
{	
	DEBUGX("removing candidate %i from party %i",id, m_id);
	Player* pl = static_cast<Player*>( World::getWorld()->getPlayer(id));
	if (pl !=0)
	{
		pl->setCandidateParty(-1);
	}
	m_candidates.erase(id);
}


void Party::acceptCandidate(int id)
{
	removeCandidate(id);
	addMember(id);
}

void Party::setRelation(int id, Fraction::Relation rel)
{
	m_relations[id] = rel;
	
	if (World::getWorld()->isServer())
	{
		
		NetEvent event;
		event.m_id = id;
		event.m_data = getId();
		event.m_type = NetEvent::PARTY_RELATION_CHANGED;
		
		World::getWorld()->insertNetEvent(event);
	}
}

void Party::toString(CharConv* cv)
{
	
	cv->toBuffer(m_id);
	cv->toBuffer(static_cast<char>(getNrMembers()));
	
	std::set<int>::iterator it;
	for (it = m_members.begin(); it != m_members.end(); ++it)
	{
		cv->toBuffer(*it);
	}
	
	cv->toBuffer(static_cast<char>(getNrCandidates()));
	for (it = m_candidates.begin(); it != m_candidates.end(); ++it)
	{
		cv->toBuffer(*it);
	}
	
	cv->toBuffer(static_cast<char>(m_relations.size()));
	std::map<int, Fraction::Relation>::iterator jt;
	for (jt = m_relations.begin(); jt != m_relations.end(); ++jt)
	{
		cv->toBuffer(jt->first);
		cv->toBuffer(static_cast<char>(jt->second));
	}

}

void Party::fromString(CharConv* cv)
{
	cv->fromBuffer(m_id);
	char nr_member, nr_cand, nr_rel;
	int id;
	
	
	cv->fromBuffer(nr_member);
	std::set<int>::iterator it;
	for (int i=0; i<nr_member; i++)
	{
		cv->fromBuffer(id);
		addMember(id);
	}
	
	cv->fromBuffer(nr_cand);
	for (int i=0; i<nr_cand; i++)
	{
		cv->fromBuffer(id);
		addCandidate(id);
	}
	
	cv->fromBuffer(nr_rel);
	std::map<int, Fraction::Relation>::iterator jt;
	char rel;
	int pid;
	for (int i=0; i<nr_rel; i++)
	{
		cv->fromBuffer(pid);
		cv->fromBuffer(rel);
		m_relations[pid] = (Fraction::Relation) rel;
		
	}
}







