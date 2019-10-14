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

#ifndef __SUMWARS_CORE_CREATURESTRUCT_H__
#define __SUMWARS_CORE_CREATURESTRUCT_H__

#include <list>
#include <string>
#include <memory.h>
#include "debug.h"
#include "translatablestring.h"
#include <map>
#include <set>
#define NR_STATUS_MODS 8

#define NR_EFFECTS 1
#include <tinyxml.h>

enum NrTimers
{
	/**
	 * \brief Number of Timers for actions
	 */
	NR_TIMERS = 9
};

struct Item;

/**
 * \struct AbilityInfo
 * \brief Struktur fuer die Faehigkeiten eines Lebewesens
 */
struct AbilityInfo
{
	AbilityInfo()
	{
		m_timer_nr =-1;
		m_time =0;
		m_timer =0;
		m_rating=0;
		m_all_target_rating =0;
		m_all_target_rating_range =0;
		m_random_rating=0;
	}
	
	
	/**
	 * \var int m_timer_nr
	 * \brief Nummer des Timers, der bei benutzen der Aktion gestartet wird. Moegliche Werte sind 0,1,2. Wenn gleich -1, so wird der Standardtimer verwendet
	 */
	int m_timer_nr;

	/**
	 * \var float m_time
	 * \brief Zeit in ms, die die Aktion ohne Modifikatoren in Anspruch nimmt. Wenn gleich 0, wird die Standardzeit verwendet
	 */
	float m_time;
	
	/**
	 * \var float m_timer
	 * \brief Gibt die Zeitdauer in ms an, die der Timer laeuft, der beim Benutzen dieser Aktion gestartet wird.
	 */
	float m_timer;
	
	/**
	 * \var float m_rating
	 * \brief Bewertung der Aktion durch die KI
	 */
	float m_rating;
	
	/**
	 * \var float m_all_target_rating
	 * \brief Bewertung, die mit der Anzahl Ziele Multipliziert wird
	 */
	float m_all_target_rating;
	
	/**
	 * \brief Radius des Umkreises, der fuer all_target_rating verwendet wird. Bei Nahkampf und self Faehigkeiten wird der Umkreis um sich selbst genommen, bei Fernkampf der Umkreis um das Ziel
	 */
	float m_all_target_rating_range;
		
	/**
	 * \var float m_random_rating
	 * \brief Wert, der maximal zufaellig dazu addiert wird
	 */
	float m_random_rating;
};

/**
 * \struct CreatureBaseAttr
 * \brief Struktor mit den Basisattributen einer Kreatur
 */
struct CreatureBaseAttr
{
	/**
	 * \enum Attribute
	 * \brief Aufzaehlung der Attributwerte
	 */
	enum Attribute
	{
		STRENGTH =0,
		DEXTERITY=1,
		MAGIC_POWER =2,
		WILLPOWER =3
	};
	
	/**
	 * \var m_max_health;
	 * \brief maximale Gesundheitspunkte
	 */
	float m_max_health;
	
	/**
	 * \var m_level;
	 * \brief Erfahrungslevel
	 */
	char m_level;
	
	/**
	 * \var m_armor;
	 * \brief der passive R&uuml;stungswert, beeinflusst den Schaden von Angriffen
	 */
	short m_armor;
	
	/**
	 * \var m_block;
	 * \brief der aktive Verteidigungswert, beeinflusst die Chance Angriffe abzuwehren
	 */
	short m_block;
	
	/**
	 * \var m_attack;
	 * \brief Attackewert, beeinflusst die Trefferchance
	 */
	short m_attack;
	
	/**
	 * \var m_power;
	 * \brief Durchschlagskraft, beeinflusst Schaden gegen gepanzerte Feinde
	 */
	short m_power;
	
	
	/**
	 * \var m_strength
	 * \brief Staerke des Lebewesens
	 */
	short m_strength;
	
	/**
	 * \var m_dexterity
	 * \brief Geschicklichkeit des Lebewesens
	 */
	short m_dexterity;
	
	
	/**
	 * \var m_magic_power
	 * \brief Zauberkraft des Lebewesens
	 */
	short m_magic_power;
	
	/**
	 * \var m_willpower;
	 * \brief Willenskraft des Lebewesens
	 */
	short m_willpower;
	
	/**
	 * \var m_resistances[4]
	 * \brief Resistenzen des Lebewesens gegen die vier Schadensarten in Prozent
	 */
	short m_resistances[4];
	
	/**
	 * \var m_resistances_cap[4]
	 * \brief Obergrenzen fuer die Resistenzen des Lebewesens gegen die vier Schadensarten in Prozent
	 */
	short m_resistances_cap[4];
	
	/**
	 * \var m_max_experience
	 * \brief maximaler Erfahrungswert vor Erreichen des n&auml;chsten Level
	 */
	float m_max_experience;
	
	
	/**
	 * \var m_walk_speed
	 * \brief gibt die Geschwindigkeit an, mit der der Spieler laufen kann, 100 entspricht einem Schritt pro Sekunde
	 */
	short m_walk_speed;
	
	/**
	 * \var m_step_length
	 * \brief Schrittlaenge beim laufen
	 */
	float m_step_length;
	
	/**
	 * \var m_attack_speed
	 * \brief gibt die Geschwindigkeit an, mit der der Spieler angreifen kann, 1000 entspricht einem Schlag pro Sekunde 
	 */
	short  m_attack_speed;
	
	/**
	 * \var std::set<std::string, AbilityInfo> m_abilities;
	 * \brief Bitmaske die angibt, welche Fähigkeiten der Spieler benutzen kann.
	 */
	std::map<std::string, AbilityInfo> m_abilities;
	
	/**
	 * \var m_attack_range
	 * \brief Gibt die Reichweite der Waffe an. Fuer Schusswaffen auf  ITEM_BOW_RANGE setzen
	 */
	float m_attack_range;
	
	/**
	 * \var m_special_flags
	 * \brief Bitmaske, die angibt, welche Spezialfähigkeiten aktiviert sind (1 bedeutet aktiviert). Die Bedeutung der verschiedenen Bits ist von genauen Typ des Lebewesens abhängig.
	 */
	int m_special_flags;
	
	/**
	 * \var char m_immunity
	 * \brief Bitmaske, welche angibt gegen welche Statusveraenderungen das Lebewesen immun ist
	 */
	char m_immunity;
	
	/**
	 * \fn int getValue(std::string valname)
	 * \brief Schiebt den gewuenschten Attributwert eines Objektes auf den Lua Stack
	 * \param valname Name des Wertes
	 */
	int getValue(std::string valname);
	
	/**
	 * \fn virtual bool setValue(std::string valname, int& event_mask)
	 * \brief Setzt den gewuenschten Attributwert eines Objektes
	 * \param valname Name des Wertes
	 * \param event_mask Bitmaske, die anzeigt, welche Daten neu uebertragen werden muessen
	 */
	bool setValue(std::string valname, int& event_mask);
	
	/**
	 * \fn void init()
	 * \brief Belegt die Struktur mit Nullen
	 */
	void init();
	
	
	/**
	 * \fn CreatureBaseAttr()
	 * \brief Konstruktor
	 */
	CreatureBaseAttr()
	{
		init();
	}
	
	/**
	 * \fn void operator=(CreatureBaseAttr& other)
	 * \brief Zuweisungsoperator
	 * \param other zugewiesene Struktur
	 */
	void operator=(CreatureBaseAttr other);
	
	/**
	 * \brief Writes the DataStructure to the XML node
	 * \param node XML node
	 * This function also deals with XML trees that are already written. In this case, only the difference is written.
	 */
	void writeToXML(TiXmlNode* node);
	
};

/**
 * \struct CreatureBaseAttrMod
 * \brief Struktur fuer die Modifikationen der Basisattribute einer Kreatur
 */
struct CreatureBaseAttrMod
{
	
	/**
	 * \var m_darmor;
	 * \brief Aenderung des  R&uuml;stungswerts, beeinflusst den Schaden von Angriffen
	 */
	short m_darmor;
	
	/**
	 * \var m_dblock;
	 * \brief Aenderung des Blockwerts, beeinflusst die Chance Angriffe abzuwehren
	 */
	short m_dblock;
	
	/**
	 * \var float m_dmax_health
	 * \brief Aenderung der maximalen Lebenspunkte
	 */
	float m_dmax_health;
	
	/**
	 * \var m_dattack;
	 * \brief Aenderung des Attackewertes, beeinflusst die Chance zu treffen
	 */
	short m_dattack;
	
	/**
	 * \var m_dpower;
	 * \brief Aenderung der Durchschlagskraft, beeinflusst Schaden gegen gepanzerte Feinde
	 */
	short m_dpower;
	
	/**
	 * \var m_dstrength
	 * \brief Aenderung der Staerke des Lebewesens
	 */
	short m_dstrength;
	
	/**
	 * \var m_ddexterity
	 * \brief Aenderung der Geschicklichkeit des Lebewesens
	 */
	short m_ddexterity;
	
	
	/**
	 * \var m_dmagic_power
	 * \brief Aenderung der Zauberkraft des Lebewesens
	 */
	short m_dmagic_power;
	
	/**
	 * \var m_dwillpower;
	 * \brief Aenderung der Willenskraft des Lebewesens
	 */
	short m_dwillpower;
	
	/**
	 * \var m_dresistances[4]
	 * \brief Aenderung der Resistenzen des Lebewesens gegen die vier Schadensarten in Prozent
	 */
	short m_dresistances[4];
	
	/**
	 * \var m_dresistances_cap[4]
	 * \brief Aenderung der Obergrenzen fuer die Resistenzen des Lebewesens gegen die vier Schadensarten in Prozent
	 */
	short m_dresistances_cap[4];
	/**
	 * \var m_dwalk_speed
	 * \brief Aenderung der  Geschwindigkeit, mit der der Spieler laufen kann, 100 entspricht einem Schritt pro Sekunde
	 */
	 
	short m_dwalk_speed;
	
	/**
	 * \var m_dattack_speed
	 * \brief Aenderung der  Geschwindigkeit, mit der der Spieler angreifen kann, 100 entspricht einem Schlag pro Sekunde 
	 */
	short  m_dattack_speed;
	
	/**
	 * \var m_xspecial_flags
	 * \brief Aenderung der Specialflags. Die angegebene Bitmaske wird mit OR zur bestehenden hinzugefügt.
	 */
 	int m_xspecial_flags;
	
	/**
	 * \var m_time
	 * \brief Zeitdauer, die diese Modifikation noch wirksam ist in Millisekunden
	 */
	float m_time;
	
	/**
	 * \var std::set<std::string> m_xabilities
	 * \brief Bitmaske die die Aenderungen der Faehigkeiten angibt. 
	 */
	std::set<std::string> m_xabilities;
	
	/**
	 * \var std::string m_flag
	 * \brief das zugehoerige Flag wird bei Anwenden des Mods gesetzt und am Ende wieder entfernt
	 */
	std::string m_flag;
	
	/**
	 * \var char m_ximmunity
	 * \brief Bitmaske die die Aenderungen der Immunitäten angibt. Die angegebene Bitmaske wird mit OR zur bestehenden hinzugefügt.
	 */
	
	char m_ximmunity;
	
	/**
	 * \fn void init()
	 * \brief Belegt die Struktur mit Nullen
	 */
	void init();
	
	
	/**
	 * \fn CreatureBaseAttrMod()
	 * \brief Konstruktor
	 */
	CreatureBaseAttrMod()
	{
		init();
	}
	
	/**
	 * \fn void operator=(CreatureBaseAttrMod other)
	 * \brief Zuweisungsoperator
	 * \param other zugewiesene Struktur
	 */
	void operator=(CreatureBaseAttrMod other);
	
	/**
	 * \brief Writes the DataStructure to the XML node
	 * \param node XML node
	 * This function also deals with XML trees that are already written. In this case, only the difference is written.
	 */
	void writeToXML(TiXmlNode* node);
	
	/**
	 * \fn int getValue(std::string valname)
	 * \brief Schiebt den gewuenschten Attributwert eines Objektes auf den Lua Stack
	 * \param valname Name des Wertes
	 */
	int getValue(std::string valname);
	
	/**
	 * \fn virtual bool setValue(std::string valname)
	 * \brief Setzt den gewuenschten Attributwert eines Objektes
	 * \param valname Name des Wertes
	 */
	bool setValue(std::string valname);
	
};

/**
 * \struct CreatureDynAttr
 * \brief Struktur mit den sich staendig aendernden Attributen eines Lebewesens
 */
struct CreatureDynAttr
{
	/**
	 * \enum Effect
	 * \brief Auflistung von visuellen Effekten
	 */
	enum Effect
	{
		
		BLEEDING =0,
	};
	
	
	/**
	 * \var m_health;
	 * \brief Anzahl der aktuellen Gesundheitspunkte
	 */
	float m_health;
	
	/**
	 * \var m_experience
	 * \brief Erfahrungswert
	 */
	float m_experience;
	
	/**
	 * \var m_last_attacker_id;
	 * \brief ID des letzten Angreifers, von dem diese Kreatur zuletzt angegriffen wurde
	 */	
	int m_last_attacker_id;
	
	/**
	 * \var float m_effect_time[NR_EFFECTS]
	 * \brief Gibt an wielange verschiedene Effekte noch wirksam sind
	 */
	float m_effect_time[NR_EFFECTS];

	/**
	 * \var m_status_mod_time[NR_STATUS_MODS]
	 * \brief Gibt an wie lange die verschiedenen Status Modifikationen noch wirken. Ein Wert von 0 bedeutet, dass die Statusmodifikation nicht wirksam ist
	 */
	float m_status_mod_time[NR_STATUS_MODS];
	
	/**
	 * \var m_status_mod_immune_time[NR_STATUS_MODS]
	 * \brief Gibt an, wie lange der Spieler gegen verschiedene Statusveränderungen immun ist.
	 */
	float m_status_mod_immune_time[NR_STATUS_MODS];
	
	/**
	 * \var m_temp_mods
	 * \brief Vektor mit alle aktuell wirkenden temporaeren Modifikation auf die Basisattribute
	 */
	std::list<CreatureBaseAttrMod> m_temp_mods;
	
	/**
	 * \var Timer m_timer;
	 * \brief timer fuer verschiedene Zwecke
	 */
	Timer m_timer;
	
	
	/**
	 * \fn int getValue(std::string valname)
	 * \brief Schiebt den gewuenschten Attributwert eines Objektes auf den Lua Stack
	 */
	int getValue(std::string valname);
	
	/**
	 * \fn virtual bool setValue(std::string valname, int& event_mask)
	 * \brief Setzt den gewuenschten Attributwert eines Objektes
	 * \param valname Name des Wertes
	 * \param event_mask Bitmaske, die anzeigt, welche Daten neu uebertragen werden muessen
	 */
	bool setValue(std::string valname, int& event_mask);
	
	/**
	 * \fn void operator=(CreatureDynAttr other)
	 * \brief Zuweisungsoperator
	 */
	void operator=(CreatureDynAttr other);
};

/**
 * \struct CreatureDynAttrMod
 * \brief Struktur fuer die Aenderung der dynamischen Attribute eines Lebewesens
 */
struct CreatureDynAttrMod
{
	/**
	 * \var  m_dhealth
	 * \brief Aenderung der aktuellen Lebenspunkte
	 **/
	float m_dhealth;
	
	/**
	 * \var float m_dstatus_mod_immune_time[NR_STATUS_MODS];
	 * \brief Gibt an, wie lange der Spieler gegen verschiedene Statusveraenderungen immunisiert wird. Heilt
	 **/
	float m_dstatus_mod_immune_time[NR_STATUS_MODS];

	/**
	 * \fn void init()
	 * \brief Belegt die Datenelemente mit Nullen
	 */
	void init()
	{
		DEBUGX("init CreatureDynAttrMod");
		// alles nullen
		m_dhealth =0;
		for (int i=0; i< NR_STATUS_MODS; i++)
		{
			m_dstatus_mod_immune_time[i] =0;
		}
	}
	
	/**
	 * \fn CreatureDynAttrMod()
	 * \brief Konstruktor
	 */
	CreatureDynAttrMod()
	{
		init();
	}
	
	/**
	 * \fn void operator=(CreatureDynAttrMod other)
	 * \brief Zuweisungsoperator
	*/
	void operator=(CreatureDynAttrMod other);
	
	/**
	 * \brief Writes the DataStructure to the XML node
	 * \param node XML node
	 * This function also deals with XML trees that are already written. In this case, only the difference is written.
	 */
	void writeToXML(TiXmlNode* node);
	
	/**
	 * \fn int getValue(std::string valname)
	 * \brief Schiebt den gewuenschten Attributwert eines Objektes auf den Lua Stack
	 * \param valname Name des Wertes
	 */
	int getValue(std::string valname);
	
	/**
	 * \fn virtual bool setValue(std::string valname)
	 * \brief Setzt den gewuenschten Attributwert eines Objektes
	 * \param valname Name des Wertes
	 */
	bool setValue(std::string valname);
	
};

/**
 * \struct CreatureSpeakText
 * \brief Struktur fuer den Text den eine Kreatur spricht
 */
struct CreatureSpeakText
{
	/**
	 * \brief Default constructor
	 */
	CreatureSpeakText()
	:	m_time(0.0),
		m_displayed_time(0.0),
		m_in_dialogue(false)
	{
		
	}
	
	/**
	 * \brief Copy constructor
	 */
	CreatureSpeakText(const CreatureSpeakText& other)
	{
		*this = other;
	}
	
	/**
	 * \var std::string m_text
	 * \brief Der Text
	 */
	TranslatableString m_text;
	
	
	/**
	 * \var std::string m_emotion
	 * \brief Bild das zum Text angezeigt wird
	 */
	std::string m_emotion;
	
	/**
	 * \var float m_time
	 * \brief Gibt an, wie lange der Text zu sehen ist
	 */
	float m_time;
	
	/**
	 * \var float m_displayed_time
	 * \brief Gibt an, wie lange der Text schon zu sehen war
	 */
	float m_displayed_time;
	
	/**
	 * \var std::string m_script_action
	 * \brief Luascript, das parallel ausgefuehrt wird. Dient vor allem dazu, zum Text passende Bewegung zu starten
	 */
	std::string m_script_action;
	
	/**
	 * \var bool m_in_dialogue
	 * \brief ist auf true gesetzt, wenn der Text im Rahmen eines Dialoges gesprochen wird, sonst false
	 */
	bool m_in_dialogue;
		
	/**
	 * \fn void operator=(const CreatureSpeakText& other)
	 * \brief Zuweisungsoperator
	 * \param other zugewiesene Struktur
	 */
	void operator=(const CreatureSpeakText& other);
	
	/**
	 * \fn bool empty()
	 * \brief Gibt aus, ob Text vorhanden ist
	 */
	bool empty();
	
	/**
	 * \fn void clear()
	 * \brief Loescht den Text
	 */
	void clear();
	
};

/**
 * \struct CreatureTradeInfo
 * \brief Informationen ueber den aktuellen Handel
 */
struct CreatureTradeInfo
{
	/**
	 * \var int m_trade_partner
	 * \brief ID des Handelspartners
	 */
	int m_trade_partner;
	
	/**
	 * \var float m_price_factor
	 * \brief Kostenfaktor
	 */
	float m_price_factor;
	
	/**
	 * \var Item* m_last_sold_item
	 * \brief zuletzt verkauftes Item
	 */
	Item* m_last_sold_item;
};

/**
 * \struct FightStatistic
 * \brief einige Statistische Daten zum Kampf
 */
struct FightStatistic
{
	/**
	 * \var std::string m_last_attacker
	 * \brief Typ des letzten Angreifers
	 */
	TranslatableString m_last_attacker;
	
	/**
	 * \var std::string m_last_attacked
	 * \brief Typ des zuletzt angegriffenen
	 */
	TranslatableString m_last_attacked;
	
	/**
	 * \var float m_hit_chance
	 * \brief Chance den Gegner zu treffen
	 */
	float m_hit_chance;
	
	/**
	 * \var float m_damage_dealt_perc
	 * \brief Prozentsatz ausgeteilter Schaden
	 */
	float m_damage_dealt_perc;
	
	/**
	 * \var float m_damage_got_perc
	 * \brief Prozentsatz erhaltener Schaden
	 */
	float m_damage_got_perc;
	
	/**
	 * \var float m_block_chance
	 * \brief Chance zu Blocken
	 */
	float m_block_chance;
};

/**
 * \struct EmotionSet
 * \brief Satz von Emotionsbildern fuer Kreaturen
 */
struct EmotionSet
{
	/**
	 * \fn std::string getEmotionImage(std::string emotion)
	 * \brief Gibt zu einer Emotion das passende Bild aus
	 */
	std::string getEmotionImage(std::string emotion);
	
	/**
	 * \var std::map<std::string, std::string> m_emotion_images
	 * \brief Bildet Name der Emotion auf das Bild ab
	 */
	std::map<std::string, std::string> m_emotion_images;
	
	/**
	 * \var std::string m_default_image
	 * \brief StandardBild, das angezeigt wird, wenn in der Map kein Eintrag vorhanden ist
	 */
	std::string m_default_image;
};

#endif // __SUMWARS_CORE_CREATURESTRUCT_H__
