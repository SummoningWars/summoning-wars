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

#ifndef __SUMWARS_CORE_DIALOGUE_H__
#define __SUMWARS_CORE_DIALOGUE_H__

#include "debug.h"
#include "event.h"
#include "creaturestruct.h"
#include "itemlist.h"
#include "timer.h"

class Region;


/**
 * \class TopicList
 * \brief Klasse fuer die Liste der Themen, ueber die ein NPC reden kann
 */
class TopicList
{
	public:
		
		/**
		 * \fn ~TopicList()
		 * \brief Destruktor
		 */
		~TopicList();
		
		/**
		 * \fn void addTopic(std::string topic, event* speech)
		 * \brief Fuegt der Liste ein Thema hinzu
		 * \param topic das Thema
		 * \param speech das Gespraech zu dem Thema
		 */
		void addTopic(std::string topic, Event* speech);
		
		/**
		 * \brief Fuegt ein Thema als Startthema hinzu
		 * \param topic Thema
		 * \param text Text mit dem das Thema angesprochen wird
		 */
		void addStartTopic(TranslatableString text, std::string topic);
		
		/**
		 * \brief Gibt Gespraech zu einem Thema aus
		 * \param topic Thema
		 */
		Event* getSpeakTopic(std::string topic);
		
		/**
		 * \brief Gibt die Liste der Startthemen aus
		 */
		std::list< std::pair<TranslatableString, std::string> >& getStartTopics()
		{
			return m_start_topics;
		}
				
	private:
		/**
	 	* \var std::map < std::string, Event*> m_topics
		* \brief Liste der Themen
		 */
		std::map < std::string, Event*> m_topics;
		
		/**
		 * \var std::list< std::pair<std::string, std::string> > m_start_topics
		 * \brief Liste der Themen, mit denen ein Gespraech beginnen kann (Text,Thema)
		 */
		std::list< std::pair<TranslatableString, std::string> > m_start_topics;
};

/**
 * \class NPCTrade
 * \brief Klasse die die Objekte speichert mit denen ein NPC handeln kann
 */
class NPCTrade
{
	public:
		/**
	 * \struct TradeObject
	 * \brief Speichert die Daten fuer ein einzelnes Item
		 */
		struct TradeObject
		{
			
			/**
			 * \fn void operator=(TradeObject& other)
			 * \brief Zuweisungsoperator
			 * \param other anderes Objekt
			 */
			void operator=(TradeObject& other);
			
			/**
			 * \var Item::Subtype m_subtype
			 * \brief Subtyp des Items
			 */
			Item::Subtype m_subtype;
			
			/**
			 * \var int m_number
			 * \brief Anzahl, wie oft das Item verkauft werden soll
			 */
			int m_number;
			
			/**
			 * \var int m_number_magical
			 * \brief Anzahl, wie oft das Item mit Verzauberung verkauft werden soll
			 */
			int m_number_magical;
			
			/**
			 * \var float m_min_enchant
			 * \brief minimale Verzauberungsstaerke
			 */
			float m_min_enchant;	
			
			/**
			 * \var float m_max_enchant
			 * \brief maximale Verzauberungsstarke
			 */
			float m_max_enchant;
			
			/**
			 * \brief probability that an item will be generated
			 */
			float m_probability;
			
		};
		
		
		/**
		 * \fn NPCTrade()
		 * \brief Konstruktor
		 */
		NPCTrade();
		
		/**
		 * \fn bool checkRefresh(Equipement& equ)
		 * \brief aktualisiert das Inventar eines Haendlers, wenn noetig
		 * \param equ Inventar
		 * \return Gibt true zurueck, wenn tatsaechlich die Items ausgetauscht wurden
		 */
		bool checkRefresh(Equipement* &equ);
		
				
		/**
		 * \var std::list<TradeObject> m_trade_objects
		 * \brief Liste der Objekte mit denen der Haendler handelt
		 */
		std::list<TradeObject> m_trade_objects;
		
		/**
		 * \var float m_cost_multiplier
		 * \brief Faktor mit dem der Preis fuer eingekaufte Waren multipliziert wird
		 */
		float m_cost_multiplier;
		
		/**
		 * \var float m_pay_multiplier
		 * \brief Faktor mit dem der gezahlte Preis fuer vom Spieler verkaufte Waren multipliziert wird
		 */
		float m_pay_multiplier;
		
		/**
		 * \var float m_refresh_time
		 * \brief Zeit nach der das Angebot erneuert wird in ms
		 */
		float m_refresh_time;
		
		/**
		 * \var Timer m_refresh_timer
		 * \brief Timer fuer den Refresh
		 */
		Timer m_refresh_timer;
};


/**
 * \class Dialogue
 * \brief Klasse fuer einen Gespraech zwischen mehreren Spielern / NPC's
 */
class Dialogue
{
	public:
		
		/**
		 * \struct Question
		 * \brief Klasse fuer eine an die Spieler gestellte Frage
		 */
		struct Question
		{  
			/**
			 * \brief Konstruktor
			 */
			Question()
			{
				m_text="";
				m_active=0;
				m_asked_player = "main_player";
			}
			
			/**
			* \var std::string m_text
			* \brief Der Text der Frage
			*/
			TranslatableString m_text;
			
			/**
			* \var std::list < std::pair<std::string, std::string> > m_answers
			* \brief Liste von moeglichen Antworten und  dem zugehoerigen Thema
			*/
			std::list < std::pair<TranslatableString, std::string> > m_answers;
			
			/**
			 * \brief Gibt an, ob die Frage gerade gestellt wird
			 */
			bool m_active;
			
			/**
			 * \brief Sprecher, der die Frage beantworten soll
			 */
			std::string m_asked_player;
			
			/**
			 * \brief Menge aller Spieler, die geantwortet haben
			 */
			std::set<int> m_players_answered;
			
			/**
			 * \brief Erhaltene Antworten mit Wichtung, Map von Nummer der Antwort auf Wichtung
			 */
			std::map<int, float> m_weighted_answers;
		};

		
		/**
		* \enum Position
		* \brief Position, an der der Spieler angezeigt wird
		*/
		enum Position
		{
			NONE = -2,
			UNKNOWN =-1,
			
			LEFT = 0,
			RIGHT = 1,
			NR_POSITIONS = 2,
			AUTOMATIC = 4
		};
		
		/**
		* \struct SpeakerState
		* \brief Status eines Sprechers im Dialog
		*/
		struct SpeakerState
		{
			/**
			* \var int m_id
			* \brief ID des Sprechers
			*/
			int m_id;
			
			/**
			* \var std::string m_emotion
			* \brief aktuelle Emotion
			*/
			std::string m_emotion;
			
			/**
			* \var Position m_position
			* \brief Position des Sprechers im Dialogfenster (oberer / untere Balken, links/rechts)
			*/
			Position m_position;
			
			/**
			* \var bool m_visible
			* \brief Gibt an, ob der Sprecher gerade sichtbar ist
			*/
			bool m_visible;
			
			/**
			* \var bool m_text_visible
			* \brief Gibt an, ob der Text gerade angezeigt wird
			*/
			bool m_text_visible;
		};
	
	
		/**
		 * \fn Dialogue(Region* region, std::string topic_base="global", int id=0)
		 * \brief Konstruktor
		 * \param region Region, in der das Gespraech stattfindet
		 * \param id ID des Dialogs
		 * \param topic_base Name des NPC, aus dessen Repertoire die Topics genommen werden
		 */
		Dialogue(Region* region, std::string topic_base="global", int id =0);
		
		/**
		 * \fn ~Dialogue()
		 * \brief Destruktor
		 */
		~Dialogue();
		
		/**
		 * \fn void init()
		 * \brief Initialisierung
		 */
		static void init();
		
		/**
		 * \fn static void cleanup()
		 * \brief Gibt Speicher frei
		 */
		static void cleanup();
		
		/**
		 * \fn void addSpeaker(int id, std::string refname, bool force = false)
		 * \brief Fuegt der Gespraech einen Sprecher hinzu
		 * \param id ID des Sprechers
		 * \param refname Name unter dem der Sprecher angesprochen wird
		 * \param force wenn auf true gesetzt, wird der Spieler aus seinem bisherigen Gespraech entfernt, falls er schon einen Dialog hat
		 */
		void addSpeaker(int id, std::string refname="", bool force = false);
		
		/**
		 * \fn void removeSpeaker(int id)
		 * \brief Entfernt einen Sprecher aus dem Gespraech
		 * \param id ID des Sprechers
		 */
		void removeSpeaker(int id);
		
		/**
		 * \fn void speak(std::string refname, std::string text, std::string emotion="", float time = 500)
		 * \brief Fuegt einen neuen gesprochenen Text ein
		 * \param refname Name unter dem der Sprecher referenziert wird
		 * \param text gesprochener Text
		 * \param emotion angezeigte Emotion
		 * \param time Zeit, die der Text angezeigt wird
		 */
		void speak(std::string refname, TranslatableString text ,std::string emotion="", float time = 500);
		
		/**
		 * \brief Fuegt eine Frage ein. Die Frage wird bei dem Spieler mit der ID  m_main_player_id angezeigt
		 * \param text Text der Frage
		 * \param asked_player Spieler dem die Frage gestellt wird
		 */
		void addQuestion(TranslatableString text, std::string asked_player="main_player");
		
		/**
		 * \fn void addAnswer(std::string text, std::string topic)
		 * \brief Fuegt fuer die Frage eine Antwort ein
		 * \param text Text der Antwort
		 * \param topic Thema zu dem gesprungen wird, wenn der Spieler diese Antwort waehlt
		 */
		void addAnswer(TranslatableString text, std::string topic);
		
		
		/**
		 * \fn void setTopicBase(std::string topic_base)
		 * \brief Setzt Repertoire der Topics
		 * \param topic_base global, wenn kein NPC dabei ist, sonst Name des NPC
		 */
		void setTopicBase(std::string topic_base);
		
		/**
		 * \brief Waehlt eine Antwort aus
		 * \param playerid ID des Spielers
		 * \param answer_nr Nummer der ausgewaehlten Antwort
		 */
		void chooseAnswer(int playerid, int answer_nr);
		
		/**
		 * \brief Gibt aus, ob der angegebene Spieler auf die Frage antworten darf
		 * \param playerid ID eines Spielers
		 */
		bool playerCanAnswer(int playerid);
		
		/**
		 * \fn bool checkTopic(std::string topic)
		 * \brief prueft, ob das angegebene Thema angesprochen werden kann
		 */
		bool checkTopic(std::string topic);
		
		/**
		 * \fn void changeTopic(std::string topic)
		 * \brief Setzt das Thema auf das angegebene Thema
		 * Themen mit besonderen Bedeutungen sind 'start' und 'end'
		 * \param selection if the topic was chosen by a player, the ID of the answer is given with this parameter
		 */
		void changeTopic(std::string topic, int answer_id=-1);
		
		/**
		 * \fn void update(float time)
		 * \brief Aktualisiert den Dialog, nachdem eine gewisse Zeitspanne vergangen ist
		 * \param time Zeit in ms
		 */
		void update(float time);
		
		/**
		 * \fn int getSpeaker(std::string refname)
		 * \brief Gibt die ID des Sprechers aus, der ueber den angegebenen Name angesprochen wird
		 * \param refname Referenzname
		 */
		int getSpeaker(std::string refname);
		
		
		/**
		 * \fn bool isFinished()
		 * \brief Gibt aus, ob der Dialog beendet wurde
		 */
		bool isFinished()
		{
			return m_finished;
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
		 * \fn static TopicList& getTopicList(std::string topic_base)
		 * \brief Gibt die Themenliste zu einem NPC aus
		 * \param topic_base Name des NPC
		 */
		static TopicList& getTopicList(std::string topic_base)
		{
			return m_topics[topic_base];
		}
		
		/**
		 * \fn static NPCTrade& getNPCTrade(std::string npcname)
		 * \brief Gibt die Handelsinformationen zu einem NPC aus
		 * \param npcname Name des NPC
		 */
		static NPCTrade& getNPCTrade(std::string npcname)
		{
			return m_npc_trades[npcname];
		}
		
		/**
		 * \fn static bool canTrade(std::string npcname)
		 * \brief prueft, ob ein NPC handeln kann
		 * \param npcname Name des NPC
		 */
		static bool canTrade(std::string npcname)
		{
			return (m_npc_trades.count(npcname) >0);
		}
	
		/**
		 * \fn  SpeakerState* getSpeakerState(Position pos)
		 * \brief Gibt den Zustand des Sprechers auf der Position pos an
		 * \param pos Position eines Sprechers
		 */
		SpeakerState* getSpeakerState(Position pos);
		
		/**
		 * \fn void setSpeakerPosition(int id, Position pos)
		 * \brief Setzt die Position eines Sprechers
		 * \param id ID des Sprechers
		 * \param pos Position
		 */
		void setSpeakerPosition(int id, Position pos);
		
		/**
		 * \fn Position calcSpeakerPosition(int id)
		 * \brief Berechnet die beste Position fuer einen Sprecher
		 */
		Position calcSpeakerPosition(int id);
		
		/**
		 * \fn void toString(CharConv* cv)
		 * \brief Konvertiert das Objekt in einen String und schreibt ihn in der Puffer
		 * \param cv Ausgabepuffer
		 */
		void toString(CharConv* cv);


		/**
		 * \fn void fromString(CharConv* cv)
		 * \brief Erzeugt das Objekt aus einem String
		 * \param cv Eingabepuffer
		 */
		void fromString(CharConv* cv);
		
		/**
		 * \fn void clearNetEventMask()
		 * \brief Setzt die Bitmaske der NetEvents auf 0
		 */
		void clearNetEventMask()
		{
			m_event_mask =0;
		}
		
		/**
		 * \fn int& getEventMaskRef()
		 * \brief Gibt eine Rerenz auf die Eventmaske aus
		 */
		int& getEventMaskRef()
		{
			return m_event_mask;
		}
		
		/**
		 * \brief Player has asked to skip the current text
		 * \param id ID of the player
		 * \param skipAll if true, the player wants to skip all text till the next question
		 * The current text will be skipped, if all players agree on skipping it.
		 */
		void skipText(int id, bool skipAll=false);
		
		/**
		 * \fn void setActive(bool active)
		 * \brief Aktiviert und Deaktiviert den Dialog
		 * \param active neuer Zustand
		 */
		void setActive(bool active)
		{
			m_active = active;
		}
		
		/**
		 * \brief Gibt die Frage aus, die am Ende des aktuellen Dialogabschnittes gestellt wird
		 */
		Question* getQuestion()
		{
			return m_question;
		}
		
		/**
		 * \brief Checks, whether the current text should be skipped and doesso if required
		 * \return if the current text should be skipped
		 */
		bool checkSkipCurrentText();
		
	private:
		
		/**
		 * \var int m_main_player_id
		 * \brief ID des Spielers, dem die Fragen gestellt werden
		 */
		int m_main_player_id;
		
		
		/**
		 * \var std::string m_topic_base
		 * \brief Name des NPC, aus dessen Repertoire die Topics genommen werden
		 */
		std::string m_topic_base;
		
		/**
		 * \var std::map<std::string, int> m_speaker
		 * \brief Liste aller Sprecher, eventuell ein Spieler mehrmals unter verschiedenen Rollen 
		 */
		std::map<std::string, int> m_speaker;
		
		/**
		 * \var std::map<int,SpeakerState> m_speaker_state
		 * \brief Zustand fuer jeden Sprecher
		 */
		std::map<int,SpeakerState> m_speaker_state;
		
		/**
		 * \var int m_active_speaker[4]
		 * \brief Gibt die IDs der aktiven Sprecher an
		 */
		int m_active_speaker[4];
		
		/**
		 * \var  Region* m_region
		 * \brief Region, in der das Gespraech stattfindet
		 */
		Region* m_region;
		
		/**
		 * \var std::list<std::pair< std::string, CreatureSpeakText > > m_speech
		 * \brief Liste von gesprochenen Aussagen
		 */
		std::list<std::pair< std::string, CreatureSpeakText > > m_speech;
		
		/**
		 * \var bool m_finished
		 * \brief auf true gesetzt, wenn das Gespraech beendet ist
		 */
		bool m_finished;
		
		/**
		 * \var bool m_started
		 * \brief true, bevor der erste Text angezeigt wird
		 */
		bool m_started;
		
		/**
		 * \var bool m_trade
		 * \brief auf true gesetzt, wenn ein Handel gestartet wurde
		 */
		bool m_trade;
		
		/**
		 * \var std::string m_topic
		 * \brief aktuelles Thema
		 */
		std::string m_topic;
		
		/**
		 * \var int m_id
		 * \brief ID des Dialogs
		 */
		int m_id;
		
		/**
		 * \var int m_event_mask
		 * \brief Eventmaske des Dialogs
		 */
		int m_event_mask;
		
		/**
		 * \var static std::map<std::string , TopicList > m_topics
		 * \brief Liste der Themen fuer alle NPC's
		 */
		static std::map<std::string , TopicList > m_topics;
		
		/**
		 * \var static std::map<std::string, NPCTrade > m_npc_trades
		 * \brief Liste der Handelsinformationen fuer alle Spieler
		 */
		static std::map<std::string, NPCTrade > m_npc_trades;
		
		
		
		/**
		 * \brief Number of players participating in the dialogue
		 */
		int m_nr_players;
		
		/**
		 * \brief IDs of players, who want to skip the current text
		 */
		std::set<int> m_player_skips;
		
		/**
		 * \brief IDs of player, who want to skip all the text till the next Question or end of dialogue
		 */
		std::set<int> m_player_all_skips;
		
		/**
		 * \brief Question currently asked
		 */
		Question* m_question;
		
		/**
		 * \brief auf true gesetzt, wenn der Dialog aktiv ist
		 */
		bool m_active;
		
};

#endif // __SUMWARS_CORE_DIALOGUE_H__

