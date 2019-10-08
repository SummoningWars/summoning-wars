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

#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H

#include <string>
#include "translatablestring.h"
#include <stack>
#include "debug.h"
#include "event.h"
#include "geometry.h"
#include "charconv.h"

extern "C"
{
	
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#define DEBUG_DATABASE

class Region;
class Dialogue;
struct Damage;
struct CreatureBaseAttrMod;
struct CreatureDynAttrMod;
struct Item;

/**
 * \class EventSystem
 * \brief Klasse fuer das Ereignissystem
 */
class EventSystem
{
	public:
	
		/**
		* \fn static void init()
		* \brief initialisiert das Eventsystem
		*/
		static void init();
		
		/**
		* \fn static void cleanup()
		* \brief gibt allokierten Speicher frei
		*/
		static void cleanup();
		
		/**
		 * \fn static void doString(const char* instructions)
		 * \brief Fuehrt die Anweisungen aus, die in dem String stehen
		 * \param instructions String mit Lua Befehlen
		 */
		static void doString(const char* instructions);
		
		/**
		 * \fn static void doFile(const char* file)
		 * \brief Fuehrt die Anweisungen aus, die in der Datei
		 * \param file Datei mit Lua Befehlen
		 */
		static void doFile(const char* file);
		
		/**
		 * \fn static void reportErrors(lua_State *L, int status, const char* instr =0)
		 * \brief Funktion um Fehler bei der Ausfuehrung von Skripten zu melden
		 * \param L Zustandsobjekt von Lua
		 * \param status von Lua gemeldeter Fehlerzustand
		 * \param instr Lua Chunk das den Fehler erzeugt hat (optional)
		 */
		static void reportErrors(lua_State *L, int status, const char* instr =0);
		
		/**
		 * \fn static void clearCodeReference(int& coderef)
		 * \brief Entfernt eine Reference auf ein Lua Fragment
		 * \param coderef Referenz auf Lua Code, wird von der Funktion auf 0 gesetzt
		 */
		static void clearCodeReference(int& coderef);
		
		/**
		 * \fn static bool executeCodeReference(int coderef)
		 * \brief Fuehrt eine CodeReferenz aus
		 * \param coderef Referenz auf Lua Code
		 * \return Gibt an, ob bei der Ausfuehrung Fehler aufgetreten sind
		 */
		static bool executeCodeReference(int coderef);
		
		/**
		 * \fn static int createCodeReference(const char* code)
		 * \brief Compiliert den Lua Code und gibt eine Referenz auf das Fragment aus
		 * \param code Lua Code
		 * \return Referenz, bei einem Fehler 0
		 */
		static int createCodeReference(const char* code);
		
		/**
		 * \brief Pushes a new element to the gettext domain stack
		 * \param domain new topmost element of the stack
		 */
		static void pushGettextDomain(std::string domain)
		{
			m_gettext_domains.push(domain);
		}
		
		/**
		 * \brief Pops the topmoest element from the gettext domain stack
		 */
		static void popGettextDomain()
		{
			m_gettext_domains.pop();
		}
		
		/**
		 * \brief Returns the current Gettext Domain
		 */
		static std::string GetGettextDomain()
		{
			if (m_gettext_domains.empty())
				return "";
			return m_gettext_domains.top();
		}
		
		/**
		 * \fn static void setRegion(Region* region)
		 * \brief Setzt die Region in der aktuell alle Events ablaufen
		 * \param region Region
		 */
		static void setRegion(Region* region)
		{
			m_region = region;
			m_trigger =0;
		}
		
		/**
		 * \fn static void setDialogue(Dialogue* dialogue)
		 * \brief Setzt den Dialog, dem die weiteren Antworten hinzugefuegt werden
		 * \param dialogue Dialog
		 */
		static void setDialogue(Dialogue* dialogue)
		{
			m_dialogue = dialogue;
		}
		
		
		/**
		 * \fn static void setDamage(Damage* damage)
		 * \brief Setzt das aktuelle Schadensobjekt
		 * \param damage Schaden
		 */
		static void setDamage(Damage* damage)
		{
			m_damage = damage;
		}
		
		/**
		 * \fn static void setCreatureBaseAttrMod(CreatureBaseAttrMod* base_mod)
		 * \brief setzt per Lua modifizierbare Basisattribute
		 * \param base_mod Modifikationen der Basisattribute
		 */
		static void setCreatureBaseAttrMod(CreatureBaseAttrMod* base_mod)
		{
			m_base_mod = base_mod;
		}
		
		/**
		 * \fn static void setCreatureDynAttrMod(CreatureDynAttr* dyn_mod)
		 * \brief setzt per Lua modifizierbare dynamische Aattribute
		 * \param dyn_attr Modifikationen der dynamischen Attribute
		 */
		static void setCreatureDynAttrMod(CreatureDynAttrMod* dyn_mod)
		{
			m_dyn_mod = dyn_mod;
		}
		
		/**
		 * \fn 		static void setItem(Item* item)
		 * \brief Setzt das aktuelle Item
		 * \param item Item
		 */
		static void setItem(Item* item);

		/**
		 * \fn static std::string getReturnValue()
		 * \brief Gibt den Rueckgabewert des Luascripts als String aus
		 */
		static std::string getReturnValue();
		
		/**
		 * \fn lua_State* getLuaState()
		 * \brief Gibt das Lua Statusobjekt aus
		 */
		static lua_State* getLuaState()
		{
			return m_lua;
		}
		

		/**
		 * \fn static bool executeEvent(Event* event)
		 * \brief Fuehrt das Event durch.
		 * \return true, wenn das Event erfolgreich ausgefuehrt wurde
		 */
		static bool executeEvent(Event* event);
		
		/**
		 * \fn static int getObjectValue(lua_State *L)
		 * \brief Schiebt den gewuenschten Attributwert eines Objektes auf den Lua Stack
		 * \param L Lua Status
		 */
		static int getObjectValue(lua_State *L);
		
		/**
		 * \fn static int getObjectValue(lua_State *L)
		 * \brief Setzt den gewuenschten Attributwert eines Objektes
		 * \param L Lua Status
		 */
		static int setObjectValue(lua_State *L);
		
		/**
		 * \fn static int moveObject(lua_State *L)
		 * \brief Verschiebt ein Objekt an einen neuen Ort
		 * \param L Lua Status
		 */
		static int moveObject(lua_State *L);
		
		/**
		 * \fn static int objectIsInRegion(lua_State *L)
		 * \brief Gibt aus, ob ein Objekt in der aktuellen Region ist
		 * \param L Lua Status
		 */
		static int objectIsInRegion(lua_State *L);
		
		/**
		 * \fn static int getDamageValue(lua_State *L)
		 * \brief Setzt einen Wert des Schadensobjekts
		 * \param L Lua Status
		 */
		static int getDamageValue(lua_State *L);
		
		/**
		 * \fn static int getDamageValue(lua_State *L)
		 * \brief Schiebt einen Wert des Schadensobjekts auf den Stack
		 * \param L Lua Status
		 */
		static int setDamageValue(lua_State *L);
		
		/**
		 * \fn static int setBaseModValue(lua_State *L)
		 * \brief Setzt ein Attribut der BaseAttr Struktur
		 * \param L Lua Status
		 */
		static int setBaseModValue(lua_State *L);
		
		/**
		 * \fn static int getBaseModValue(lua_State *L)
		 * \brief Gibt ein Attribut der BaseAttr Struktur aus
		 * \param L Lua Status
		 */
		static int getBaseModValue(lua_State *L);
		
		/**
		 * \fn static int setDynModValue(lua_State *L)
		 * \brief Setzt ein Attribut der DynAttr Struktur
		 * \param L Lua Status
		 */
		static int setDynModValue(lua_State *L);
		
		/**
		 * \fn static int getDynModValue(lua_State *L)
		 * \brief Gibt ein Attribut der DynAttr Struktur aus
		 * \param L Lua Status
		 */
		static int getDynModValue(lua_State *L);
		
		/**
		 * \fn static int createProjectile(lua_State *L)
		 * \brief Erzeugt ein Projektil
		 * \param L Lua Status
		 */
		static int createProjectile(lua_State *L);
		
		/**
		 * \fn static int pointIsInArea(lua_State *L)
		 * \brief prueft, ob ein Objekt sich in einem Gebiet befindet
		 * \param L Lua Status
		 */
		static int pointIsInArea(lua_State *L);
		
		/**
		 * \fn static int unitIsInArea(lua_State *L)
		 * \brief prueft, ob ein Objekt sich in einem Gebiet befindet
		 * \param L Lua Status
		 */
		static int unitIsInArea(lua_State *L);
		
		/**
		 * \fn static int createObject(lua_State *L);
		 * \brief erzeugt ein neues Objekt
		 * \param L Lua Status
		 */
		static int createObject(lua_State *L);
		
		/**
		 * \fn static int createObjectGroup(lua_State *L);
		 * \brief erzeugt eine neue Objektgruppe
		 * \param L Lua Status
		 */
		static int createObjectGroup(lua_State *L);
		
		/**
		 * \fn static int createScriptObject(lua_State *L)
		 * \brief erzeugt ein rein scriptgesteuertes Objekt
		 */
		static int createScriptObject(lua_State *L);
		
		/**
		 * \fn static int deleteObject(lua_State *L)
		 * \brief Entfernt ein Objekt
		 * \param L Lua Status
		 */
		static int deleteObject(lua_State *L);
		
		/**
		 * \fn static int createMonsterGroup(lua_State *L)
		 * \brief Erstellt eine neue Monstergruppe
		 * \param L Lua Status
		 */
		static int createMonsterGroup(lua_State *L);
		
		/**
		 * \fn static int addUnitCommand(lua_State *L)
		 * \brief Fuegt zu einer Einheit ein Kommando hinzu
		 * \param L Lua Status
		 */
		static int addUnitCommand(lua_State *L);
		
		/**
		 * \fn static int setUnitAction(lua_State *L)
		 * \brief Setzt die Aktion einer Einheit
		 */
		static int setUnitAction(lua_State *L);
		
		/**
		 * \fn static int clearUnitCommands(lua_State *L)
		 * \brief Loescht alle Kommandos einer Einheit
		 */
		static int clearUnitCommands(lua_State *L);
		
		/**
		 * \fn static int setObjectAnimation(lua_State *L)
		 * \brief Setzt die Animation eines Objekts
		 */
		static int setObjectAnimation(lua_State *L);
		
		/**
		 * \fn  static int setScriptObjectFlag(lua_State *L)
		 * \brief Setzt ein Flag eines Scriptobject
		 */
		static int setScriptObjectFlag(lua_State *L);
		
		/**
		 * \fn static int getScriptObjectFlag(lua_State *L)
		 * \brief Gibt den Status eines Flag eines Scriptobject aus
		 */
		static int getScriptObjectFlag(lua_State *L);
		
		/**
		 * \fn static int createScriptObjectEvent(lua_State *L)
		 * \brief Erzeugt ein Event fuer ein Scriptevent
		 */
		static int createScriptObjectEvent(lua_State *L);
		
		/**
		 * \fn static int getObjectAt(lua_State *L)
		 * \brief Gibt das Objekt an der angegebenen Stelle aus
		 * \param L Lua Status
		 **/
		static int getObjectAt(lua_State *L);
		
		/**
		 * \fn static int getObjectsInArea(lua_State *L)
		 * \brief Gibt die Objekte in der angegebenen Flaeche aus
		 */
		static int getObjectsInArea(lua_State *L);
		
		/**
		 * \fn static int setObjectNameRef(lua_State *L)
		 * \brief Setzt fuer ein Objekt einen Referenzname
		 */
		static int setObjectNameRef(lua_State *L);
		
		/**
		 * \fn static int getObjectByNameRef(lua_State *L)
		 * \brief Gibt das Objekt mit dem angegebenen Referenzname aus
		 */
		static int getObjectByNameRef(lua_State *L);
		
		/**
		 * \fn static int dropItem(lua_State *L)
		 * \brief Laesst einen Gegenstand fallen
		 * \param L Lua Status
		 */
		static int dropItem(lua_State *L);
		
		
		/**
		 * \fn static int createItem(lua_State *L)
		 * \brief Erzeugt einen neuen Gegenstand und speichert ihn in dem aktuellen Gegenstand
		 */
		static int createItem(lua_State *L);
		
		/**
		 * \fn static int createRandomItem(lua_State *L)
		 * \brief Erzeugt einen zufaelligen Gegenstand
		 */
		static int createRandomItem(lua_State *L);
		
		/**
		 * \fn static int getItemValue(lua_State *L)
		 * \brief Gibt einen Wert des aktuellen Items aus
		 */
		static int getItemValue(lua_State *L);
		
		/**
		 * \fn static int setItemValue(lua_State *L)
		 * \brief Setzt einen Wert des aktuellen Items aus
		 */
		static int setItemValue(lua_State *L);
		
		/**
		 * \brief Adds magic mods to an item
		 */
		static int addItemMagicMods(lua_State *L);
		
		/**
		 * \fn static int searchPlayerItem(lua_State *L)
		 * \brief Durchsucht das Inventar eines Spieler nach einem Item
		 */
		static int searchPlayerItem(lua_State *L);
		
		/**
		 * \fn static int getPlayerItem(lua_State *L)
		 * \brief Setzt ein Item des Spielers als aktuelles Item
		 */
		static int getPlayerItem(lua_State *L);
		
		/**
		 * \fn static int removePlayerItem(lua_State *L)
		 * \brief Entfernt ein Item aus dem Inventar des Spielers. Das Item bleibt als aktuelles Item erhalten
		 */
		static int removePlayerItem(lua_State *L);
		
		/**
		 * \fn static int insertPlayerItem(lua_State *L)
		 * \brief Fuegt das aktuelle Item in das Inventar des Spielers ein
		 */
		static int insertPlayerItem(lua_State *L);
		
		/**
		 * \fn static int getInventoryPosition(lua_State *L)
		 * \brief Gibt die Nummer einer Position im Inventar aus
		 */
		static int getInventoryPosition(lua_State *L);
		
		/**
		 * \fn static int deleteItem(lua_State *L)
		 * \brief Loescht das aktuelle Item
		 */
		static int deleteItem(lua_State *L);
		
		/**
		 * \brief creates colored single line of text floating upwards (used for damage numbers and similar)
		 * \param L Lua state
		 */
		static int createFloatingText(lua_State *L);
		
		/**
		 * \fn static int addLocation(lua_State *L)
		 * \brief Fuegt einen neuen Ort hinzu
		 * \param L Lua Status
		 */
		static int addLocation(lua_State *L);
		
		/**
		 * \brief Sets the Position of a location
		 * \param L Lua Status
		 */
		static int setLocation(lua_State *L);
		
		/**
		 * \fn static int getLocation(lua_State *L)
		 * \brief einen Ort aus
		 * \param L Lua Status
		 */
		static int getLocation(lua_State *L);
		
		/**
		 * \fn static int addArea(lua_State *L)
		 * \brief Fuegt einen neuen Bereich hinzu
		 * \param L Lua Status
		 */
		static int addArea(lua_State *L);
		
		/**
		 * \fn static int startTimer(lua_State *L)
		 * \brief startet einen Timer
		 * \param L Lua Status
		 */
		static int startTimer(lua_State *L);
		
		/**
		 * \fn static int insertTrigger(lua_State *L)
		 * \brief Fuegt einen Trigger ein
		 * \param L Lua Status
		 */
		static int insertTrigger(lua_State *L);
		
		/**
		 * \fn static addTriggerVariable(lua_State *L)
		 * \brief Fuegt zum zuletzt erzeugten Trigger eine Variable hinzu
		 * \param L Lua Status
		 */
		static int addTriggerVariable(lua_State *L);
		
		/**
		 * \fn static int setCutsceneMode(lua_State *L)
		 * \brief Setzt den Cutscene Modus der Region
		 * \param L Lua Status
		 */
		static int setCutsceneMode(lua_State *L);
		
		/**
		 * \fn static int addCameraPosition(lua_State *L)
		 * \brief Fuegt der Kamerafahrt eine Position hinzu
		 * \param L Lua Status
		 */
		static int addCameraPosition(lua_State *L);
		
		/**
		* \brief Setzt das Licht in der aktuellen Region
		* \param L Lua Status
		*/
		static int setLight(lua_State *L);
		
		/**
		 * \fn static int speak(lua_State *L)
		 * \brief Fuegt dem aktuellen Dialog einen Text hinzu
		 * \param L Lua Status
		 */
		static int speak(lua_State *L);
		
		/**
		 * \fn static int unitSpeak(lua_State *L)
		 * \brief Fuegt einer Kreatur eine Sprechblase hinzu
		 * \param L Lua Status
		 */
		static int unitSpeak(lua_State *L);
		
		/**
		 * \fn static int addQuestion(lua_State *L)
		 * \brief Fuegt dem aktuellen Dialog eine Frage
		 * \param L Lua Status
		 */
		static int addQuestion(lua_State *L);
		
		/**
		 * \fn static int addAnswer(lua_State *L)
		 * \brief Fuegt dem aktuellen Dialog eine Antwort auf die Frage hinzu
		 * \param L Lua Status
		 */
		static int addAnswer(lua_State *L);
		
		/**
		 * \fn static int setSpeakerEmotion(lua_State *L)
		 * \brief Setzt die Emotion eines Sprechers
		 * \param L Lua Status
		 */
		static int setSpeakerEmotion(lua_State *L);
		
		/**
		 * \fn static int setSpeakerAnimation(lua_State *L)
		 * \brief Setzt die Animation eines Sprechers
		 * \param L Lua Status
		 */
		static int setSpeakerAnimation(lua_State *L);
		
		/**
		 * \fn static int setSpeakerPosition(lua_State *L)
		 * \brief Setzt die Position, auf der ein Sprecher erscheint
		 */
		static int setSpeakerPosition(lua_State *L);
		
		/**
		 * \fn static int changeTopic(lua_State *L)
		 * \brief Wechselt das Thema des aktuelle Dialogs
		 * \param L Lua Status
		 */
		static int changeTopic(lua_State *L);
		
		/**
		 * \fn static int jumpTopic(lua_State *L)
		 * \brief Springt ein Topic eines Dialogs an, anschliessend wird mit dem vorhergehenden Topic fortgesetzt
		 * \param L Lua Status
		 */
		static int jumpTopic(lua_State *L);
		
		/**
		 * \fn static int executeInDialog(lua_State *L)
		 * \brief Fuehrt innerhalb eines Dialogs ein Stueck Script aus
		 */
		static int executeInDialog(lua_State *L);
		
		/**
		 * \fn static int addSpeakerInDialog(lua_State *L)
		 * \brief Fuegt waehrend der Dialog laeuft einen Sprecher hinzu
		 */
		static int addSpeakerInDialog(lua_State *L);
		
		/**
		 * \fn static int createDialogue(lua_State *L)
		 * \brief Erzeugt einen neuen Dialog
		 * \param L Lua Status
		 */
		static int createDialogue(lua_State *L);
		
		/**
		 * \fn static int addSpeaker(lua_State *L)
		 * \brief fuegt dem Dialog einen Sprecher hinzu
		 * \param L Lua Status
		 */
		static int addSpeaker(lua_State *L);
		
		/**
		 * \fn static int removeSpeaker(lua_State *L)
		 * \brief fuegt dem Dialog einen Sprecher hinzu
		 * \param L Lua Status
		 */
		static int removeSpeaker(lua_State *L);
		
		/**
		 * \fn static int getSpeaker(lua_State *L)
		 * \brief Gibt zu einem Sprecher das Objekt aus
		 * \param L Lua Status
		 */
		static int getSpeaker(lua_State *L);
		
		/**
		 * \fn static int setTopicBase(lua_State *L)
		 * \brief Setzt den Basisbereich der Topics
		 * \param L Lua Status
		 */
		static int setTopicBase(lua_State *L);
		
		/**
		 * \fn static int setRefName(lua_State *L)
		 * \brief Setzt den Referenznamen eines Objekts
		 * \param L Lua Status
		 */
		static int setRefName(lua_State *L);
		
		/**
		 * \fn static int setDialogueActive(lua_State *L)
		 * \brief Aktiviert beziehungsweise deaktiviert einen Dialog
		 * \param L Lua Status
		 */
		static int setDialogueActive(lua_State *L);
		
		/**
		 * \fn static int setCurrentDialogue(lua_State *L)
		 * \brief Setzt einen Dialog
		 */
		static int setCurrentDialogue(lua_State *L);
		
		/**
		 * \fn static int createEvent(lua_State *L)
		 * \brief Erzeugt ein neues Event. Auf dieses Event beziehen sich die folgenden addCondition und addEffect Aufrufe
		 *  \param L Lua Status
		 */
		static int createEvent(lua_State *L);
		
		/**
		 * \fn static int addCondition(lua_State *L)
		 * \brief Fuegt dem Event eine Bedingung hinzu
		 * \param  L Lua Status
		 */
		static int addCondition(lua_State *L);
		
		/**
		 * \fn static int addEffect(lua_State *L)
		 * \brief Fuegt dem Event einen Effekt hinzu
		 * \param  L Lua Status
		 **/
		static int addEffect(lua_State *L);
		
		/**
		 * \fn static int timedExecute(lua_State *L)
		 * \brief Fuehrt zeitversetzt ein Script aus
		 */
		static int timedExecute(lua_State *L);
		
		/**
		 * \fn static Vector getVector(lua_State *L, int index)
		 * \brief Liest einen Vector vom Lua Stack
		 * \param L Lua Status
		 * \param index Index des Vektors auf dem Stack
		 */
		static Vector getVector(lua_State *L, int index);
		
		/**
		 * \fn static Shape getArea(lua_State *L, int index)
		 * \brief liest eine Area vom Strack
		 * \param L Lua Status
		 * \param index Index der Area auf dem Stack
		 */
		static Shape getArea(lua_State *L, int index);
		
		/**
		 * \fn static void pushVector(lua_State *L, Vector v)
		 * \brief Schiebt einen Vektor auf den Lua Stack
		 * \param v Vector
		 * \param L Lua Status
		 **/
		static void pushVector(lua_State *L, Vector v);
		
		/**
		 * \brief Retrieves a translatable string from the top of the stack
		 * \param t translatable string
		 * \param L Lua Status
		 * \param index index of the string on the stack
		 * The current top element of the gettext domain stack is used for the string
		 **/
		static void getTranslatableString(lua_State *L, TranslatableString& t, int index);
		
		/**
		 * \fn static int getRolePlayers(lua_State *L)
		 * \brief Gibt eine Liste von Spielerd aus, die eine bestimmte Rolle erfuellen
		 * \param L Lua Status
		 **/
		static int getRolePlayers(lua_State *L);
		
		/**
		 * \fn static int teleportPlayer(lua_State *L)
		 * \brief teleportiert Spieler in eine andere Region
		 * \param L Lua Status
		 */
		static int teleportPlayer(lua_State *L);
		
		/**
		 * \fn static int getRegion(lua_State *L)
		 * \brief Gibt den Namen der aktuellen Region aus
		 * \param L Lua Status
		 */
		static int getRegion(lua_State *L);
		
		/**
		 * \fn static int getRelation(lua_State *L)
		 * \brief gibt das Verhaeltnis zweiter Fraktionen aus
		 * \param L Lua Status
		 */
		static int getRelation(lua_State *L);
		
		/**
		 * \fn static int setRelation(lua_State *L)
		 * \brief Setzt das Verhaeltnis zweier Fraktionen
		 */
		static int setRelation(lua_State *L);
		
		/**
		 * \fn static int printMessage(lua_State *L)
		 * \brief Schreibt eine Nachricht auf den Bildschirm
		 */
		static int printMessage(lua_State *L);
		
		/**
		 * \brief Fuegt ein neues Musikstuecke der Auswahlliste hinzu
		 */
		static int addMusic(lua_State *L);
		
		/**
		 * \brief Loescht die Auswahlliste der Musikstuecke
		 */
		static int clearMusicList(lua_State *L);
		
		/**
		 * \brief Plays a given sound
		 */
		static int playSound(lua_State *L);
		
		/**
		 * \brief Returns a value from \a Options class
		 * \param L Lua Status
		 */
		static int getOption(lua_State *L);
		
		/**
		 * \brief Writes a value to \a Options class
		 * \param L Lua Status
		 */
		static int setOption(lua_State *L);
		
		
		/**
		 * \fn static int writeString(lua_State *L)
		 * \brief schreibt einen String in den Stream
		 * \param L Lua Status
		 */
		static int writeString(lua_State *L);
		
		/**
		 * \brief Schreibt einen Eintrag in sumwars.log
		 * \param L Lua Status
		 */
		static int writeLog(lua_State *L);
		
		/**
		 * \brief Reloads a certain set of game data
		 * \param L Lua Status
		 */
		static int reloadData(lua_State *L);
		
		/**
		 * \brief Setzt die Statistiken von OGRE zurueck
		 * \param L Lua Status
		 */
		static int clearOgreStatistics(lua_State *L);
		
		/**
		 * \brief Gibt die Statistiken von OGRE aus
		 * \param L Lua Status
		 */
		static int getOgreStatistics(lua_State *L);

		/**
		 * \fn static int writeNewline(lua_State *L)
		 * \brief Schreibt ein Newline in den Stream
		 * \param L Lua Status
		 */
		static int writeNewline(lua_State *L);
				
		/**
		 * \fn static void writeSavegame(CharConv* savegame, int playerid, bool local_player)
		 * \brief Schreibt die Daten fuer ein Savegame
		 * \param savegame Streaming Objekt fuer das Savegame
		 */
		static void writeSavegame(CharConv* savegame);
		
		/**
		 * \fn static void readSavegame(CharConv* savegame, int playerid, bool local_player)
		 * \brief Liest die Daten aus einem Savegame
		 * \param savegame Streaming Objekt fuer das Savegame
		 * \param playerid ID des Spielers
		 * \param local_player Gibt an, ob es sich um den lokalen Spieler handelt
		 */
		static void readSavegame(CharConv* savegame, int playerid, bool local_player);
		
		/**
		 * \fn static void clearPlayerVarString(int id)
		 * \brief Loescht den String mit Lua Befehlen
		 * \param id ID des Spielers
		 */
		static void clearPlayerVarString(int id);
		
		/**
		 * \fn static std::string getPlayerVarString(int id)
		 * \brief Gibt den String mit Lua Befehlen zum aktualisieren der Variablen aus
		 * \param id ID des Spielers
		 */
		static std::string getPlayerVarString(int id);
		
		/**
		 * \fn static int writeUpdateString(lua_State *L)
		 * \brief Schreibt einen Updatestring fuer einen Spieler
		 */
		static int writeUpdateString(lua_State *L);
		
	private:
		/**
		 * \var static lua_State * m_lua
		 * \brief Zustandsobjekt von Lua
		 */
		static lua_State * m_lua;
		
		/**
		 * \brief This stack is used to store the text domains used for translatable strings generated from lua.
		 * Each translatable string generated from lua gets the topmost element of the stack as domain
		 */
		static std::stack<std::string> m_gettext_domains;
		
		/**
		 * \var static  Region m_region
		 * \brief Region in der der aktuell alle Events ausgefuehrt werden
		 */
		static Region* m_region;
		
		/**
		 * \var static Dialogue* m_dialogue
		 * \brief aktueller Dialog
		 */
		static Dialogue* m_dialogue;
		
		/**
		 * \var static Trigger* m_trigger
		 * \brief der zuletzt vom Script eingefuegte Trigger
		 */
		static Trigger* m_trigger;
		
		/**
		 * \var static Event* m_event
		 * \brief das zuletzt per Script erzeugte Event
		 */
		static Event* m_event;
		
		/**
		 * \var static Damage* m_damage
		 * \brief aktuelles Schadenobjekt
		 */
		static Damage* m_damage;
		
		/**
		 * \var static CreatureBaseAttrMod* m_base_mod
		 * \brief Objekt mit Basisattributen von Kreaturen
		 */
		static CreatureBaseAttrMod* m_base_mod;
		
		/**
		 * \var static CreatureDynAttrMod* m_dyn_mod
		 * \brief Objekt mit dynamischen Attributen von Kreaturen
		 */
		static CreatureDynAttrMod* m_dyn_mod;
		
		/**
		 * \var static Item* m_item
		 * \brief aktuelles Item
		 */
		static Item* m_item;
		
		/**
		 * \var  static bool m_item_in_game
		 * \brief Gibt an, ob das aktuelle Item sich im Inventar eines Spieler oder auf dem Boden befindet
		 */
		static bool m_item_in_game;
		
		/**
		 * \var static CharConv* m_charconv
		 * \brief Stream Objekt in das die Lua Variablen geschrieben werden
		 */
		static CharConv* m_charconv;
		
		/**
		 * \var static std::map<int, std::string> m_player_varupdates
		 * \brief Liste mit Befehlen mit denen lokale Variablen der Spieler angepasst werden
		 */
		static std::map<int, std::string> m_player_varupdates;
		
#ifdef DEBUG_DATABASE
		static std::map<int, std::string> m_code_fragments;
#endif
};


#endif
