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

#include "dialogue.h"
#include "world.h"
#include "region.h"
#include "creature.h"
#include "itemfactory.h"

std::map<std::string , TopicList > Dialogue::m_topics;

std::map<std::string, NPCTrade > Dialogue::m_npc_trades;


TopicList::~TopicList()
{
	std::map < std::string, Event*>::iterator it;
	for (it = m_topics.begin(); it != m_topics.end(); ++it)
	{
		delete it->second;
	}
}

void TopicList::addTopic(std::string topic, Event* speech)
{
	if (m_topics.count(topic) != 0)
	{
			WARNING("duplicate topic %s",topic.c_str());
			delete m_topics[topic];
			m_topics.erase(topic);
	}
	
	m_topics.insert(std::make_pair(topic,speech));

}

void TopicList::addStartTopic(TranslatableString text, std::string topic)
{
	m_start_topics.push_back(std::make_pair(text,topic));
}

Event* TopicList::getSpeakTopic(std::string topic)
{
	std::map < std::string, Event*>::iterator it;

	it = m_topics.find(topic);
	if (it == m_topics.end())
		return 0;

	return it->second;
}



void NPCTrade::TradeObject::operator=(TradeObject& other)
{
	m_subtype = other.m_subtype;
	m_number = other.m_number;
	m_number_magical = other.m_number_magical;
	m_min_enchant = other.m_min_enchant;
	m_max_enchant = other.m_max_enchant;
	m_probability = other.m_probability;
}

NPCTrade::NPCTrade()
	:	m_trade_objects()
{
	m_cost_multiplier= 1.0;
	m_refresh_time = 36000000; // 10 min
	m_refresh_timer.start();
}

bool NPCTrade::checkRefresh(Equipement* &equ)
{
	// update, wenn das Inventar noch garnicht existiert oder zu alt ist
	if (equ ==0 || m_refresh_timer.getTime() > m_refresh_time)
	{
		if (equ == 0)
		{
			equ = new Equipement(100,100,100);
		}
		equ->clear();

		Item* itm=0;
		Item::Type type;
		std::list<TradeObject>::iterator it;
		// Schleife ueber alle handelbaren Objekte
		for (it = m_trade_objects.begin(); it != m_trade_objects.end(); ++it)
		{
			// nicht magische Objekte erzeugen
			for (int i=0; i< it->m_number; ++i)
			{
				if (Random::random() > it->m_probability)
					continue;
				
				type = ItemFactory::getBaseType(it->m_subtype);
				itm = ItemFactory::createItem(type,it->m_subtype);
				if (itm != 0)
				{
					itm->m_price = (int) (itm->m_price * m_cost_multiplier);
	
					// einfuegen und erfolg testen
					if (equ->insertItem(itm,false,false) == Equipement::NONE)
					{
						delete itm;
					}
				}
				else
				{
					ERRORMSG("could not create Item %s",it->m_subtype.c_str());
				}
			}

			// magische Objekte erzeugen
			float magic;
			for (int i=0; i< it->m_number_magical; ++i)
			{
				if (Random::random() > it->m_probability)
					continue;
				
				type = ItemFactory::getBaseType(it->m_subtype);
				magic = Random::randrangef(it->m_min_enchant, it->m_max_enchant);
				itm = ItemFactory::createItem(type,it->m_subtype,0,magic,Item::MAGICAL);
				itm->m_price = (int) (itm->m_price * m_cost_multiplier);

				// einfuegen und erfolg testen
				if (equ->insertItem(itm,false,false) == Equipement::NONE)
				{
					delete itm;
				}
			}
		}

		return true;
	}
	return false;
}



Dialogue::Dialogue(Region* region, std::string topic_base,int id)
{
	if (id ==0)
	{
		m_id = World::getWorld()->getValidId();
	}
	else
	{
		m_id = id;
	}
	m_region = region;
	m_topic_base = topic_base;
	m_main_player_id =0;
	m_finished = false;

	m_started = true;
	m_trade = false;
	m_nr_players =0;
	
	for (int i=0; i<4; i++)
	{
		m_active_speaker[i] =0;
	}
	m_active = true;
	m_question = 0;
}

Dialogue::~Dialogue()
{
	std::map<std::string, int>::iterator it;

	WorldObject* wo;
	Creature* cr;
	for (it = m_speaker.begin(); it != m_speaker.end(); ++it)
	{
		wo = m_region->getObject(it->second);
		if (wo !=0 && wo->isCreature())
		{
			cr = static_cast<Creature*>(wo);
			cr ->clearSpeakText();
			cr ->setDialogue(0);
		}
	}
	
	if (m_question != 0)
	{
		delete m_question;
	}
}

void Dialogue::init()
{
}

void Dialogue::cleanup()
{
	m_topics.clear();
	m_npc_trades.clear();
}

void Dialogue::addSpeaker(int id, std::string refname, bool force)
{
	if (id ==0)
		return;
	
	// Pruefen, ob die Kreatur existiert
	WorldObject* wo =0;
	wo = m_region->getObject(id);
	if (wo ==0 || !wo->isCreature())
	{
		ERRORMSG("cant add %i: %s to dialogue: not a creature", id, refname.c_str());
		return;
	}

	Creature *cr =  static_cast<Creature*>(wo);
	if (refname =="")
	{
		refname = cr->getRefName();
	}

	// Pruefen, ob die Kreatur nicht schon einen Dialog hat
	if (cr->getDialogueId() != 0)
	{
		if (cr->getDialogueId()!= m_id)
		{
			if (!force)
			{
				ERRORMSG("cant add %i: %s to dialogue: has already dialogue",id, refname.c_str());
				return;
			}
			else
			{
				cr->getDialogue()->removeSpeaker(cr->getId());
				cr->setDialogue(m_id);
			}
		}
	}
	else
	{
		cr->setDialogue(m_id);
	}

	m_speaker.insert(std::make_pair(refname,id));

	if (cr->getType() == "PLAYER")
	{
		if (m_main_player_id ==0 )
		{
			m_main_player_id = cr->getId();
			m_speaker["main_player"] = m_main_player_id;
		}
	}
	
	// ggf Spielerstatus erstellen
	std::map<int,SpeakerState>::iterator it;
	it = m_speaker_state.find(id);
	if (it == m_speaker_state.end())
	{
		m_event_mask =1;
		
		SpeakerState& state = m_speaker_state[id];
		state.m_id = id;
		state.m_emotion = "";
		state.m_position = UNKNOWN;
		state.m_visible = false;
		state.m_text_visible = false;
		
		cr->getSpeakText().m_emotion="";
		if (cr->getType() == "PLAYER")
		{
			m_nr_players ++;
		}
	}

}

void Dialogue::removeSpeaker(int id)
{
	DEBUGX("remove %i",id);
	WorldObject* wo =0;
	wo = m_region->getObject(id);
	Creature* cr = dynamic_cast<Creature*>(wo);
	if (cr == 0 || cr->getDialogueId() != m_id)
		return;
	
	setSpeakerPosition(id, NONE);
	
	if (cr->getType() == "PLAYER")
	{
		m_nr_players --;
		if (m_trade)
		{
			m_finished = true;
			m_trade = false;
			cr->setTradePartner(0);
		}
	}
	
	
	// alle Sprecherrollen des Spielers loeschen
	std::map<std::string, int>::iterator it;
	for (it = m_speaker.begin(); it != m_speaker.end(); )
	{
		if (it->second == id)
		{
			m_speaker.erase(it++);
		}
		else
		{
			++it;
		}
	}
	
	// Status des Spielers loeschen
	m_speaker_state.erase(id);
	
	cr->setDialogue(0);
	cr->clearSpeakText();
}

void Dialogue::speak(std::string refname, TranslatableString text, std::string emotion,float time)
{
	CreatureSpeakText txt;
	txt.m_text = text;
	txt.m_time = time;
	txt.m_emotion = emotion;
	txt.m_in_dialogue = true;
	
	m_speech.push_back(std::make_pair(refname,txt));
	
}

void Dialogue::addQuestion(TranslatableString text, std::string asked_player)
{
	if (m_question != 0)
		delete m_question;
	
	m_question = new Question;
	m_question->m_text = text;
	m_question->m_asked_player = asked_player;
}

void Dialogue::addAnswer(TranslatableString text, std::string topic)
{
	if (m_question == 0)
	{
		ERRORMSG("cannot add answer: no question asked");
		return;
	}

	if (checkTopic(topic))
	{
		
		m_question->m_answers.push_back(std::make_pair(text,topic));
	}
}

void Dialogue::setTopicBase(std::string topic_base)
{
	m_topic_base = topic_base;
}

bool Dialogue::checkTopic(std::string topic)
{
	if (topic == "start" || topic == "end" || topic == "abort" || topic =="#change_topic#" || topic =="#jump_topic#")
		return true;

	// Events werden nicht geprueft!
	if (m_topic_base == "global")
		return true;
	
	Event* st = m_topics[m_topic_base].getSpeakTopic(topic);

	if (st !=0)
	{
		DEBUGX("checking Topic %s",topic.c_str());
		return st->checkCondition();
	}

	return false;
}

void Dialogue::changeTopic(std::string topic, int answer_id)
{
	if (topic =="abort")
	{
		if (m_topic == "start")
		{
			topic = "end";
		}
		else
		{
			return;
		}
	}
	
	
	if (topic == "end")
	{
		// Gespraech beenden
		m_finished = true;

		if (m_trade)
		{
			Creature* player = dynamic_cast<Creature*>( m_region->getObject(m_main_player_id));
			if (player != 0)
			{
				player->setTradePartner(0);
			}
		}
		return ;
	}

	m_speech.clear();
	m_started = true;

	m_topic = topic;
	
	Event* st;

	if (topic == "start")
	{
		Creature* npc = dynamic_cast<Creature*>( m_region->getObject( getSpeaker(m_topic_base) ) );
		if (npc ==0)
		{
			addQuestion(TranslatableString("Which Topic"));
		}
		else
		{
			TranslatableString tr = npc->getName();
			addQuestion(tr);
		}

		// Alle Antworten hinzufuegen, deren Themen freigeschaltet sind
		bool noanswer = true;
		std::list< std::pair<TranslatableString, std::string> >& lst = m_topics[m_topic_base].getStartTopics();
		std::list< std::pair<TranslatableString, std::string> >::iterator jt;
		for (jt = lst.begin(); jt != lst.end(); ++jt)
		{
			if (jt->second == "start_dialogue")
			{
				st = m_topics[m_topic_base].getSpeakTopic(jt->second);
				if (st->checkCondition())
				{
					EventSystem::setRegion(m_region);
					st->doEffect();
				}
				continue;
			}
			if (checkTopic(jt->second))
			{
				DEBUGX("added answer %s %s",jt->first.c_str(),jt->second.c_str());
				addAnswer(jt->first, jt->second);
				noanswer = false;
			}
		}
		
		if (noanswer)
		{
			m_finished = true;
		}
		return;
	}

	if (topic == "trade")
	{
		// Pruefen, dass der NPC wirklich handeln kann
		if (!canTrade(m_topic_base))
		{
			m_finished = true;
			return ;
		}

		// Handelspartner und Basisinformationen einholen
		NPCTrade& tradeinfo = getNPCTrade(m_topic_base);
		Creature* npc = dynamic_cast<Creature*>( m_region->getObject( getSpeaker(m_topic_base) ) );
		Creature* player = dynamic_cast<Creature*>( m_region->getObject(m_main_player_id));

		if (npc==0 || player ==0)
		{
			m_finished = true;
			return ;
		}

		// evtl Inventar auffrischen
		Equipement* equ = npc->getEquipement();
		tradeinfo.checkRefresh(equ);

		if (npc->getEquipement() ==0)
		{
			npc->setEquipement(equ);
		}

		m_trade = true;


		NetEvent event;
		event.m_type =  NetEvent::TRADER_INVENTORY_REFRESH;
		event.m_data = 0;
		event.m_id = npc->getId();

		m_region->insertNetEvent(event);


		// Handelspartner setzen
		npc->setTradePartner(player->getId());
		npc->getTradeInfo().m_price_factor = tradeinfo.m_cost_multiplier;
		player->setTradePartner(npc->getId());
	}
	
	if (topic == "stash")
	{
		Creature* player = dynamic_cast<Creature*>( m_region->getObject(m_main_player_id));
		
		if (player != 0)
		{
			// Spieler handelt *mit sich selbst*
			player->setTradePartner(player->getId());
			m_finished = true;
		}
	}

	// globale Dialoge loesen Events aus
	// lokale Topics
	
	
	if (m_topic_base == "global")
	{
		Trigger* tr = new Trigger(topic);
		tr->addVariable("_dialogue",getId());
		if (answer_id != -1)
		{
			tr->addVariable("answer_id",answer_id);
		}
		m_region->insertTrigger(tr);
	}
	else
	{
		st = m_topics[m_topic_base].getSpeakTopic(topic);
	
		if (st ==0)
		{
			ERRORMSG("topic %s:%s existiert nicht",m_topic_base.c_str(),topic.c_str());
			m_finished = true;
			return ;
		}
	
		if (st->checkCondition())
		{
			EventSystem::setRegion(m_region);
			
			if (answer_id != -1)
			{
				Trigger tr("");
				tr. addVariable("answer_id",answer_id);
				EventSystem::doString((char*) tr.getLuaVariables().c_str());
			}
			
			st->doEffect();
		}
	}

}

void Dialogue::chooseAnswer(int playerid, int answer_nr)
{
	if (!playerCanAnswer(playerid))
	{
		return;
	}
	
	m_event_mask =1;
	
	m_question->m_players_answered.insert(playerid);
	float weight = 1;
	if (playerid == getSpeaker("main_player"))
	{
		weight = 1.1;
	}
	
	if (m_question->m_weighted_answers.count(answer_nr) == 0)
	{
		m_question->m_weighted_answers[answer_nr] = weight;
	}
	else
	{
		m_question->m_weighted_answers[answer_nr] += weight;
	}
	
	if (m_question->m_asked_player != "all" || (int) m_question->m_players_answered.size() == m_nr_players)
	{	
		// Antwort mit den meisten Stimmen ermitteln
		float maxweight = -1;
		int answernr=0;
		std::map<int, float>::iterator it;
		
		for (it = m_question->m_weighted_answers.begin(); it != m_question->m_weighted_answers.end(); ++it)
		{
			if (it->second > maxweight)
			{
				maxweight = it->second;
				answernr = it->first;
			}
		}
		
		int answer_id = answernr +1;
		
		std::list < std::pair<TranslatableString, std::string> >::iterator jt;
		jt = m_question->m_answers.begin();
		while (jt !=m_question->m_answers.end() && answernr >0)
		{
			answernr--;
			++jt;
		}
		
		std::string newtopic = "end";
		if (jt !=m_question->m_answers.end())
		{
			newtopic = jt->second;
		}
				
		delete m_question;
		m_question = 0;
		
		changeTopic(newtopic, answer_id);
	}
}

bool Dialogue::playerCanAnswer(int playerid)
{
	if (m_question == 0 || m_question->m_active == false)
		return false;
		
	if (m_question->m_asked_player == "all")
	{
		if (m_question->m_players_answered.count(playerid) > 0)
		{
			return false;
		}
		return true;
	}
	else if (getSpeaker(m_question->m_asked_player) == playerid)
	{
		return true;
	}
	
	return false;
	
}

int Dialogue::getSpeaker(std::string refname)
{
	std::map<std::string, int>::iterator it;
	it = m_speaker.find(refname);
	if (it == m_speaker.end())
		return 0;

	return it->second;
}

Dialogue::Position Dialogue::calcSpeakerPosition(int id)
{
	WorldObject* wo =0;
	wo = m_region->getObject(id);
	if (wo ==0 || !wo->isCreature())
	{
		ERRORMSG("Speaker %i is not a creature", id);
		return Dialogue::UNKNOWN;
	}
		
	if (wo->getType() == "PLAYER")
	{
		return LEFT;
	}
	
	return RIGHT;
	
	
}

void Dialogue::setSpeakerPosition(int id, Position pos)
{
	if (pos<NONE || pos>AUTOMATIC || pos == UNKNOWN || m_speaker_state.count(id) == 0)
		return;
	
	m_event_mask =1;
	
	if (pos == AUTOMATIC)
		pos = calcSpeakerPosition(id);
	
	// Spieler entfernen
	// bisherige Position ermitteln:
	Position oldpos = m_speaker_state[id].m_position;
	if (oldpos != UNKNOWN && pos != oldpos)
	{
		m_active_speaker[oldpos] =0;
	}
	
	// Sprecher der bisher auf der Position pos war verdraengen
	if (pos != NONE)
	{
		int prev_speaker = m_active_speaker[pos];
		if (prev_speaker != 0 && prev_speaker != id)
		{
			m_speaker_state[prev_speaker].m_visible = false;
			m_speaker_state[prev_speaker].m_text_visible = false;
		}
					
		// neuen Sprecher aktivieren
		m_speaker_state[id].m_visible = true;
		m_speaker_state[id].m_position = pos;
		m_active_speaker[pos] = id;
	}
	else
	{
		m_speaker_state[id].m_visible = false;
		m_speaker_state[id].m_text_visible = false;
	}
	
	DEBUGX("set speaker %i at position %i",id,pos);
	
}

Dialogue::SpeakerState* Dialogue::getSpeakerState(Position pos)
{
	if (pos<0 || pos >=NR_POSITIONS)
		return 0;
	
	if (m_active_speaker[pos] == 0)
		return 0;
	
	return &(m_speaker_state[m_active_speaker[pos]]);
}

void Dialogue::update(float time)
{

	if (m_finished)
	{

		return;
	}

	float stime = time;
	CreatureSpeakText* cst;

	// Zeitverlauf berechnen
	while (stime >0 && !m_speech.empty())
	{
		cst = &(m_speech.front().second);

		// Fragen bleiben generell stehen
		// Handel ebenso
		if (!m_started && m_question != 0 && m_question->m_active || m_trade)
		{
			stime =0;
			break;
		}

		if (!m_started && cst->m_time > stime)
		{
			cst->m_time -= stime;
			stime =0;
		}
		else
		{
			
			// bei inaktivem Dialog keine neuen Saetze erzeugen
			if (!m_active)
				return;
			
			m_player_skips.clear();
			
			// Aenderung eingetreten
			WorldObject* wo=0;
			Creature* cr=0;
			if (!m_started)
			{
				stime -= cst->m_time;

				// aktuellen Sprecher ermitteln
				wo = m_region->getObject( getSpeaker(m_speech.front().first));

				// fuer diesen Specher aktuellen Text zuruecksetzen
				if (wo !=0 && wo->isCreature())
				{
					cr = static_cast<Creature*>(wo);
					cr->clearSpeakText();
				}

				m_speech.pop_front();
			}
			m_started = false;

			// Naechsten zulaessigen Text suchen
			cst =0;
			int id;
			Position pos=UNKNOWN;
			while (cst==0 && !m_speech.empty())
			{
				DEBUGX("speak %s",m_speech.front().second.m_text.c_str());
				
				// Change Topic braucht keinen Sprecher
				if (m_speech.front().second.m_text == "#change_topic#")
				{
					cst = &(m_speech.front().second);
					break;
				}
				
				if (m_speech.front().second.m_text == "#execute#")
				{
					EventSystem::setRegion(m_region);
					EventSystem::doString(m_speech.front().second.m_emotion.c_str());
					m_speech.pop_front();
					continue;
				}
				
				if (m_speech.front().second.m_text == "#add_speaker#")
				{
					// ID wird als String uebergeben
					std::stringstream stream;
					stream.str(m_speech.front().first);
					int id;
					stream >> id;
					
					// Refname steckt in Emotion
					std::string refname = m_speech.front().second.m_emotion;
					addSpeaker(id,refname,true);
					
					m_speech.pop_front();
					continue;
				}
				
				id = getSpeaker(m_speech.front().first);
				wo = m_region->getObject(id );
				cr = static_cast<Creature*>(wo);
				
				

				if (cr ==0 && m_speech.front().first!="nobody")
				{
					DEBUGX("cant speak text %s %s",m_speech.front().first.c_str(),m_speech.front().second.m_text.c_str());
					m_speech.pop_front();
					continue;
				}
				
				cst = &(m_speech.front().second);
				
				if (cst->m_text == "#emotion#")
				{
					cr->getSpeakText().m_emotion = cst->m_emotion;
					m_speaker_state[id].m_emotion = cst->m_emotion;
					m_event_mask =1;
					m_speech.pop_front();
					cst =0;
					continue;
				}
				
				if (cst->m_text == "#animation#" || cst->m_text == "#animation_r#")
				{
					cr->setAnimation(cst->m_emotion, cst->m_time, cst->m_text == "#animation_r#");
					m_event_mask =1;
					m_speech.pop_front();
					cst =0;
					continue;
				}
				
				if (cst->m_text == "#position#")
				{
 					std::string posstr = cst->m_emotion;
					Position pos = UNKNOWN;
					if (posstr == "none")
						pos = NONE;
					else if (posstr == "left")
						pos = LEFT;
					else if (posstr == "right")
						pos = RIGHT;
					else
					{
						WARNING("Invalid value %s as argument to setSpeakerPosition",posstr.c_str());
					}
					
					if (pos != UNKNOWN)
					{
						setSpeakerPosition(id,pos);
					}
					
					m_event_mask =1;
					cst =0;
					m_speech.pop_front();
					continue;
				}
				
				// Springt ein Topic an, setzt danach aber mit dem alten fort
				if (cst->m_text == "#jump_topic#")
				{
					Event* st;
				
					st = m_topics[m_topic_base].getSpeakTopic(cst->m_emotion);
					if (st->checkCondition())
					{
						EventSystem::setRegion(m_region);
						st->doEffect();
					}
				
					m_speech.pop_front();
					cst =0;
					continue;
				}
				
				if (cst->m_text == "#remove_speaker#")
				{
					removeSpeaker(id);
					m_speech.pop_front();
					cst =0;
					continue;
				}
				
				

				
				// Falls die Position des Spielers im Dialog noch unbekannt: Position berechnen
				pos = m_speaker_state[id].m_position;
				if (pos == UNKNOWN)
				{
					pos = calcSpeakerPosition(id);
				}
				
				if (pos == UNKNOWN)
				{
					continue;
				}
				
				
			}

			if (cst ==0 || m_speech.empty())
			{
				if (m_question != 0)
				{
					if (m_question->m_active == false)
					{
						m_question->m_active = true;
						m_event_mask =1;
					}
				}
				else
				{
					m_finished = true;
				}
				return;
			}

			// geht direkt zum naechsten Topic
			if (cst->m_text == "#change_topic#")
			{
				changeTopic(cst->m_emotion);
				return;
			}

			// naechsten Text sprechen
			DEBUGX("spoken text %s",cst->m_text.c_str());

			if (cr !=0)
			{
				cr->speakText(*cst);
				
				setSpeakerPosition(id,pos);
				for (int i=0; i<4; i++)
				{
					if (m_active_speaker[i] != 0)
					{
						if (i== pos)
						{
							m_speaker_state[m_active_speaker[i]].m_text_visible = true;
						}
						else
						{
							m_speaker_state[m_active_speaker[i]].m_text_visible = false;
						}
					}
				}
				
				// Emotion setzen
				if (cst->m_emotion != "")
				{
					m_speaker_state[id].m_emotion = cst->m_emotion;
				}
				
				cr->getSpeakText().m_emotion = m_speaker_state[id].m_emotion;
				m_event_mask =1;
			}

		}

	}

	if (m_speech.empty() && !m_trade)
	{
		if (m_question != 0)
		{
			if (m_question->m_active == false)
			{
				m_question->m_active = true;
				m_event_mask =1;
			}
		}
		else
		{
			m_finished = true;
		}
	}
}

void Dialogue::toString(CharConv* cv)
{
	// nur die relevanten Daten werden geschrieben: die aktuell sichtbaren 4 Spieler
	cv->toBuffer(m_id);
	for (int i=0; i<4; i++)
	{
		cv->toBuffer(m_active_speaker[i]);
		if (m_active_speaker[i] != 0)
		{
			SpeakerState& state = m_speaker_state[m_active_speaker[i]];
			cv->toBuffer(state.m_id);
			cv->toBuffer(state.m_emotion);
			cv->toBuffer(static_cast<short>(state.m_position));
			cv->toBuffer(state.m_visible);
			cv->toBuffer(state.m_text_visible);
		}
	}
	
	std::map<std::string, int>::iterator st;
	cv->toBuffer((int) m_speaker.size());
	for (st = m_speaker.begin(); st != m_speaker.end(); ++st)
	{
		cv->toBuffer(st->first);
		cv->toBuffer(st->second);
	}
	
	if (m_question ==0 || !m_question->m_active)
	{
		cv->toBuffer(static_cast<int>(0));
	}
	else
	{
		cv->toBuffer(static_cast<int>(1));
		
		m_question->m_text.toString(cv);
		cv->toBuffer(m_question->m_active);
		cv->toBuffer(m_question->m_asked_player);
		
		std::list < std::pair<TranslatableString, std::string> >::iterator it;
		cv->toBuffer((int) m_question->m_answers.size());
		for (it = m_question->m_answers.begin(); it != m_question->m_answers.end(); ++it)
		{
			it->first.toString(cv);
			cv->toBuffer(it->second);
		}
		
		std::set<int>::iterator jt;
		cv->toBuffer((int) m_question->m_players_answered.size());
		for (jt = m_question->m_players_answered.begin(); jt != m_question->m_players_answered.end(); ++jt)
		{
			cv->toBuffer(*jt);
		}
		
		std::map<int, float>::iterator kt;
		cv->toBuffer((int) m_question->m_weighted_answers.size());
		for (kt = m_question->m_weighted_answers.begin(); kt != m_question->m_weighted_answers.end(); ++kt)
		{
			cv->toBuffer(kt->first);
			cv->toBuffer(kt->second);
		}
		
	}
	
}

void Dialogue::fromString(CharConv* cv)
{
	for (int i=0; i<4; i++)
	{
		cv->fromBuffer(m_active_speaker[i]);
		if (m_active_speaker[i] != 0)
		{
			short tmp;
			SpeakerState& state = m_speaker_state[m_active_speaker[i]];
			cv->fromBuffer(state.m_id);
			cv->fromBuffer(state.m_emotion);
			cv->fromBuffer(tmp);
			state.m_position = (Position) tmp;
			cv->fromBuffer(state.m_visible);
			cv->fromBuffer(state.m_text_visible);
		}
	}
	
	int size;
	cv->fromBuffer(size);
	std::string name;
	int id;
	m_speaker.clear();
	for (int i=0; i<size; i++)
	{
		cv->fromBuffer(name);
		cv->fromBuffer(id);
		m_speaker[name] = id;
	}
	
	int ques;
	cv->fromBuffer(ques);
	if (m_question != 0)
	{
		delete m_question;
		m_question = 0;
	}
	
	if (ques == 1)
	{	
		m_question = new Question;
		
		m_question->m_text.fromString(cv);
		cv->fromBuffer(m_question->m_active);
		cv->fromBuffer(m_question->m_asked_player);
		
		cv->fromBuffer(size);
		m_question->m_answers.clear();
		std::string topic;
		TranslatableString answr;
		for (int i=0; i<size; i++)
		{
			answr.fromString(cv);
			cv->fromBuffer(topic);
			m_question->m_answers.push_back(std::make_pair(answr,topic));
		}
		
		cv->fromBuffer(size);
		m_question->m_players_answered.clear();
		int plid;
		for (int i=0; i<size; i++)
		{
			cv->fromBuffer(plid);
			m_question->m_players_answered.insert(plid);
		}
		
		cv->fromBuffer(size);
		int answer;
		float weight;
		m_question->m_weighted_answers.clear();
		for (int i=0; i<size; i++)
		{
			cv->fromBuffer(answer);
			cv->fromBuffer(weight);
			m_question->m_weighted_answers[answer] = weight;
		}
		
	}
}

void Dialogue::skipText(int id)
{
	m_player_skips.insert(id);
	DEBUGX("players %i skips %i",m_nr_players, m_player_skips.size());
	if ((int) m_player_skips.size() == m_nr_players)
	{
		// aktuellen Text ueberspringen
		CreatureSpeakText* cst;
		if (!m_speech.empty())
		{
			cst = &(m_speech.front().second);
			// Fragen und Handel kann nicht so uebersprungen werden
			if (m_question !=0 && m_question->m_active || m_trade)
				return;
			
			cst->m_time =0;
		}
	}
}

