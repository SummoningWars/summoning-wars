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

#ifndef __SUMWARS_CORE_WORLDLOADER_H__
#define __SUMWARS_CORE_WORLDLOADER_H__


#include <map>
#include <list>
#include "region.h"
#include "objectfactory.h"
#include "event.h"
#include "dialogue.h"
#include "quest.h"

#include <tinyxml.h>
#include <string>
#include <cstring>
#include "elementattrib.h"




/**
 * \class TemplateLoader
 * \brief Laedt verschiedene Templates aus einer XML-Datei
 */
class WorldLoader
{
	public:
	
	
	
	/**
	 * \fn static bool loadNPCData(const char* pFilename)
	 * \brief Liest die Daten zu NPCs aus einer XML Datei
	 * \param pFilename Name der XML Datei
	 */
	static bool loadNPCData(const char* pFilename);
	
	/**
	 * \fn static loadQuestsData(const char* pFilename)
	 * \brief Liest die Daten zu Quests aus einer XML Datei
	 * \param pFilename Name der XML Datei
	 */
	static bool loadQuestsData(const char* pFilename);
	
	/**
	 * \fn static bool loadRegionData(const char* pFilename)
	 * \brief Laedt die Daten zu den Regionen aus der XML
	 * \param pFilename Name der XML Datei
	 */
	static bool loadRegionData(const char* pFilename);
	
	/**
	 * \fn static void loadEvent( TiXmlNode* node, Event *ev, TriggerType &type)
	 * \brief Laedt die Daten zu einem Event aus einem XML Knoten
	 * \param node Knoten der die Daten enthaelt
	 * \param ev Event in das die Daten geschrieben werden
	 * \param type Typ des Triggers, der das Event ausloesen soll (Ausgabeparameter)
	 */
	static void loadEvent( TiXmlNode* node, Event *ev, TriggerType &type);
	
	private:
	
	/**
	 * \fn static bool loadRegions(TiXmlNode* node)
	 * \brief Lade die Daten zu Regionen aus einer XML Datei
	 * \param node XMl Knoten
	 */
		static bool loadRegions(TiXmlNode* node);
		
	/**
	* \fn static bool loadRegion(TiXmlNode* node, RegionData* rdata)
	 * \brief Laedt die Daten zu einer Region aus einem XML Knoten
	 * \param node Knoten der die Daten enthaelt
	 * \param rdata Datenstruktur in die die Daten geschrieben werden
	 */
	static bool loadRegion(TiXmlNode* node, RegionData* rdata);
	
	/**
	 * \fn static void loadFraction( TiXmlNode* node)
	 * \brief Laedt Daten zu einer Fraktion aus der XML Datei
	 */
	static void loadFraction( TiXmlNode* node);
	
	/**
	 * \fn static void loadNPC( TiXmlNode* node)
	 * \brief Laedt die Daten zu einem NPC aus einem Knoten
	 * \param node XML Knoten
	 */
	static void loadNPC( TiXmlNode* node);
	
	/**
	 * \fn static void loadQuests(TiXmlNode* node)
	 * \brief Liest die Daten zu Quests aus einer XML Datei
	 * \param node XML Knoten
	 */
	static void loadQuests(TiXmlNode* node);
	
	/**
	 * \fn static void loadQuest(TiXmlNode* node, Quest* quest)
	 * \brief Laedt die Daten zu einem Quest
	 * \param node Knoten, der die Daten enthaelt
	 * \param quest Klasse, in die die Daten geschrieben werden
	 */
	static void loadQuest(TiXmlNode* node, Quest* quest);
	
	private:
		/**
		 * \brief Name der Datei die gerade gelesen wird
		 */
		static std::string m_filename;
	

};


#endif // __SUMWARS_CORE_WORLDLOADER_H__

